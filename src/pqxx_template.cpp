/**
 * pqxx_template.cpp
 * 
 * @see builder_template.cpp
 * 実現したいことは、mysql_template.cpp と同じで、VarNode を利用した。汎用リポジトリの実装
 * である。純粋仮想関数を持つインタフェースのクラスがひとつ、そのサブクラスもひとつ。エン
 * ティティというクラスや構造体は持たずに、VarNodeのみでエンティティは表現する。
 * 
 * 静的で型安全のメタデータによるSQL の動的発行という目標もあったが、それはひとまず忘れる。
 * この場合は、必ずEntity クラスか構造体が必要になる（それが駄目だというつもりもない、ただ
 * 楽がしたい：）では進めてみよう。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc -I/usr/local/include -I/usr/include/postgresql -L/usr/local/lib pqxx_template.cpp -lpqxx -lpq -o ../bin/pqxx_template
 */
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <cxxabi.h>
#include <cassert>
#include <any>
#include <functional>
#include <optional>
#include <random>
#include <ctime>
#include <chrono>
#include <string_view> // C++17以降 @see void safe_print(const char* p)
#include <unordered_map>
#include <algorithm> // for find_if, if you keep using vector

#include <Repository.hpp>
#include <sql_helper.hpp>
#include <pqxx/pqxx>
#include <pqxx/version>

template <class M, class D>
void (*ptr_print_debug)(M, D) = [](const auto message, const auto debug) -> void
{
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void
{
    std::cerr << "ERROR: " << e.what() << std::endl;
};

// ポインタ安全な出力のためのヘルパー関数オーバーロード
void safe_print(const char* p)
{
    if (p == nullptr) {
        std::cout << "(nullptr)";
    } else {
        std::cout << p;
    }
}
// 他の型はそのまま出力する汎用テンプレート
template <typename T>
void safe_print(const T& element)
{
    std::cout << element;
}
template <class... Args>
void print_debug(Args&&... args)
{
    std::cout << "Debug: ";
    auto print_element = [](const auto& element) {
        safe_print(element);
        std::cout << '\t';
    };
    // C++17以降の pack expansion で要素を順に処理
    (print_element(std::forward<Args>(args)), ...);
    std::cout << std::endl;
}

/**
 * 乱数生成関数（C++）
 * 
 */
float random_cxx()
{
    std::random_device seed_gen;                            // 非決定的な乱数生成器
    std::mt19937 engine(seed_gen());                        // 擬似乱数製正規。ランダムなシードを設定する。
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);        // 分布方法
    return dist(engine);                                    // 乱数生成
}

/**
 * システム固有のユニークID を生成する。
 * 正直次のロジックで本当にシステム固有になるのかは自信はない：）
 * 奇跡的に以下 3 つの値（t, nanoseconds_since_epoch, b）がそろったらアウト。
 * 
 */
std::string generate_uid()
{
    const float t = random_cxx();
    auto now = std::chrono::system_clock::now();
    // エポック (起点) からの経過時間をナノ秒単位のdurationに変換
    auto nanoseconds_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()
    );
    // ナノ秒のカウントを取得 (整数値)
    const long long nanosecond_count = nanoseconds_since_epoch.count();
    const float b = random_cxx();
    return std::to_string(t) + std::to_string(nanosecond_count) + std::to_string(b);
}



namespace tmp::postgres::r3
{


class SqlBuilder final
{
private:
    // 内部で保持するデータを不変にすることで、SQL 単位の処理ということを担保している。
    // 別なSQL を利用したい場合は、本オブジェクトを作り直せということ。
    // 多少記憶は古いが、コネクションをプーリングしている際は次のsqlName は何らかの方法
    // でその一意性を確保しないとDB 側でエラーになる（はず）。
    const std::string sqlName;  // postgres では発行PreparedStatement 単位で何らかの名前が必要。
    const std::string sql;
public:
    SqlBuilder(const std::string& _sqlName, const std::string& _sql):
     sqlName{_sqlName}, sql{_sql}
    {}
    pqxx::connection& makePrepare(pqxx::connection& conn) const
    {
        conn.prepare(
            sqlName,
            sql
        );
        return conn;
    }
    pqxx::prepped makePrepped() const
    {
        return pqxx::prepped{sqlName};
    }
    template <class... Args>
    requires (!std::is_same_v<std::decay_t<std::tuple_element_t<0, std::tuple<Args...>>>, VarNode>)
    pqxx::params makeParams(Args&&... args) const
    {
        return pqxx::params{args...};
    }
    pqxx::params makeParams(const VarNode& node) const
    {
        pqxx::params params;
		for (const auto& child : node.children) {
            // child->key がカラム名
            // child->data が更新する値 (ValueType)
            // std::visit を使用して ValueType の中身を取り出し、pqxx::params に変換してセットする
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;  // これカッコいいよね。
                if constexpr (!std::is_same_v<T, std::monostate>) {
                    // pqxx::params のコンストラクタが適切な型にしてくれる
                    pqxx::params sql_val(arg);
                    params.append(sql_val);
                }
                // monostate の場合は処理しない
            }, child->data);
        }
        return params;
    }
    static std::string nextvalSql(const std::string& seqName)
    {
        std::string sql{"SELECT nextval('"};
        sql.append(seqName).append("')");
        return sql;
    }
    static std::string insertSql(const std::string& table, const VarNode& node)
    {
        // R"(INSERT INTO contractor (id, company_id, email, password, name, roles) VALUES ($1, $2, $3, $4, $5, $6))"
        std::string sql = "INSERT INTO " + table + '\n';
        const std::string dollar = "$";
        std::string cols;
        std::string vals;
        size_t i = 0;
		for (const auto& child : node.children) {
            // child->key がカラム名、このカラム名を正しくSQL に組み込む。
            if(i == 0) {
                cols.append("\"").append(child->key).append("\"");
                vals.append(dollar + std::to_string(i+1));
            } else {
                cols.append(", ").append("\"").append(child->key).append("\"");
                vals.append(", ").append(dollar + std::to_string(i+1));
            }
            i++;
        }
        sql.append("( ");
        sql.append(cols);
        sql.append(" )");
        sql.append("\nVALUES\n");
        sql.append("( ");
        sql.append(vals);
        sql.append(" )\n");
        return sql;
    }
    static std::string updateSql(const std::string& table, const VarNode& node)
    {
        // e.g.
        // UPDATE contractor
        // SET id=$1, company_id=$2, email=$3, password=$4, name=$5, roles=$6
        // WHERE id=$1
        // 「IDの整合性: i=0 の最初の要素がプライマリキーであることを前提としています。」
        std::string sql = "UPDATE " + table + '\n';
        const std::string dollar = "$";
        std::string cols;
        std::string pkey;
        size_t i = 0;
		for (const auto& child : node.children) {
            // child->key がカラム名、このカラム名を正しくSQL に組み込む。
            if(i == 0) {
                pkey.append("\"").append(child->key).append("\"=").append(dollar+std::to_string(1));
                cols.append("\"").append(child->key).append("\"=").append(dollar+std::to_string(1));
            } else {
                cols.append(", \"").append(child->key).append("\"=").append(dollar+std::to_string(i+1));
            }
            i++;
        }
        sql.append("SET ").append(cols);
        sql.append("\nWHERE ").append(pkey).append("\n");
        return sql;
    }

};


using VarNodeMapper = std::function<void(tmp::VarNode& root, const pqxx::row& row)>;
template <class ID>
class VarNodeRepository : public tmp::Repository<ID, tmp::VarNode>
{
private:
    pqxx::work* const tx;
    std::string tableName;
    std::string idSeqName;
    const VarNodeMapper mapper; // 関数オブジェクト、現状 findById() 内で利用するもの。
    const std::string primaryKeyName;
public:
    VarNodeRepository(
        pqxx::work* const    _tx
        , const std::string& _table
        , const std::string& _idseq
        , const std::string& _primaryKeyName = "id"
    )
    : tx{_tx}, tableName{_table}, idSeqName{_idseq}, primaryKeyName{_primaryKeyName}
    {}
    VarNodeRepository(
        pqxx::work* const    _tx
        , const std::string& _table
        , const std::string& _idseq
        , VarNodeMapper _mapper
        , const std::string& _primaryKeyName = "id"
    )
    : tx{_tx}, tableName{_table}, idSeqName{_idseq}, mapper{std::move(_mapper)}, primaryKeyName{_primaryKeyName}
    {}
    virtual ID insert(tmp::VarNode&& data) const override
    {
        int status;
        print_debug("insert ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        ID id = tx->query_value<ID>(SqlBuilder::nextvalSql(idSeqName));
        print_debug("nextval: ", id);
        // 悩ましい問題だが、id（プラマリキ）はリポジトリ内部で値が決定され、data 上の値を更新するものとする。
        auto pkeyNode = data.getChild(primaryKeyName);
        if(pkeyNode) pkeyNode->data = id;
        else throw std::runtime_error("primary key node is not found.");
        // DONE... 動的なSQL の発行が必要。
        std::string sql = SqlBuilder::insertSql(tableName, data);
        print_debug("sql: ", sql);
        SqlBuilder builder{generate_uid(), sql};
        builder.makePrepare(tx->conn());
        pqxx::params params = builder.makeParams(data); // 一時オブジェクト
        pqxx::result result = tx->exec(builder.makePrepped(), params);
        print_debug("affected rows: ", result.affected_rows());
        return id;
    }
    virtual void update(const ID& id, tmp::VarNode&& data) const override
    {
        int status;
        print_debug("update ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        // DONE ... VarNode から動的にUpdate SQL を作る。
        std::string sql = SqlBuilder::updateSql(tableName, data);
        print_debug("sql: ", sql);
        SqlBuilder builder{generate_uid(), sql};
        builder.makePrepare(tx->conn());
        // id の値で data のプライマリキを先に更新する。その後、builder.makeParams(VarNode&&) を利用する。
        auto pkeyNode = data.getChild(primaryKeyName);
        if(pkeyNode) pkeyNode->data = id;
        else throw std::runtime_error("primary key node is not found.");
        pqxx::params params = builder.makeParams(data); // 一時オブジェクト
        pqxx::result result = tx->exec(builder.makePrepped(), params);
        print_debug("affected rows: ", result.affected_rows());
    }
    virtual void remove(const ID& id) const override
    {
        int status;
        print_debug("remove ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        std::string sql = tmp::postgres::helper::delete_by_pkey_sql(tableName, primaryKeyName);
        print_debug("sql: ", sql);
        SqlBuilder builder{generate_uid(), sql};
        builder.makePrepare(tx->conn());
        pqxx::result result = tx->exec(builder.makePrepped(), builder.makeParams(id));
        print_debug("affected rows: ", result.affected_rows());
    }
    virtual std::optional<tmp::VarNode> findById(const ID& id) const override
    {
        int status;
        print_debug("findById ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        // Step 1 データベースへの問い合わせを行う。
        std::string sql = tmp::postgres::helper::select_by_pkey_sql(tableName, primaryKeyName);
        print_debug("sql: ", sql);
        SqlBuilder builder{generate_uid(), sql};
        builder.makePrepare(tx->conn());
        pqxx::result result = tx->exec(builder.makePrepped(), builder.makeParams(id));
        print_debug("affected rows: ", result.affected_rows());
        // Step 2 pqxx::result からVarNode の作成。
        // 結果は最大1行なので、ループではなく if (!result.empty()) で処理する
        if(!result.empty()) {
            VarNode result_entity("result_row", std::monostate{});
            const auto& row = result[0];
            mapper(result_entity, row);
            return result_entity;
        }
        return std::nullopt;
    }
};


}   // namespace tmp::postgres::r3


int test_VarNodeRepository_Delete(uint64_t* id)
{
    puts("------ test_VarNodeRepository_Delete");
    using Data = tmp::VarNode;
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        const std::string table = "contractor";
        const std::string idseq = "contractor_id_seq";
        const std::string pkey  = "id";
        std::unique_ptr<tmp::Repository<uint64_t, Data>>
            repo = std::make_unique<tmp::postgres::r3::VarNodeRepository<uint64_t>>(&tx, table, idseq, pkey);

        repo->remove(*id);
        tx.commit();
        conn.close();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_VarNodeRepository_Update(uint64_t* id)
{
    puts("------ test_VarNodeRepository_Update");
    using Data = tmp::VarNode;
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        const std::string table = "contractor";
        const std::string idseq = "contractor_id_seq";
        const std::string pkey  = "id";
        std::unique_ptr<tmp::Repository<uint64_t, Data>>
            repo = std::make_unique<tmp::postgres::r3::VarNodeRepository<uint64_t>>(&tx, table, idseq, pkey);

        // Data 作成
        // e.g. Data sample
        // Data data{"B3_3333", "foo@loki.org", "foo1111", "Foo", roles};
        // データベース上でNULL を許可しているカラムは設定しなくてよい（contractor table では roles にあたる）。
        // root ノードのKey とValue は空でなければ何でもよい（現状は）。
        // MySQL の場合との決定的な違いは、プライマリキの設定が必須になる点（data の値はなんでもよい）。
        // リポジトリの更新用関数のシグネチャには、その仮引数が明確に分かれているから。
        Data root{"__DATA__KEY__", std::monostate{}};
        uint64_t nid{0};
        root.addChild(pkey, nid);
        std::string companyId{"B3_3333"};
        root.addChild("company_id", companyId);
        std::string email{"foo@loki.org"};
        root.addChild("email", email);
        std::string password{"foo1111"};
        root.addChild("password", password);
        std::string name{"Foo"};
        root.addChild("name", name);
        std::string roles{"Admin,User"};
        root.addChild("roles", roles);

        repo->update(*id, std::move(root));
        tx.commit();
        conn.close();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_VarNodeRepository_Find(uint64_t* id)
{
    puts("------ test_VarNodeRepository_Find");
    using Data = tmp::VarNode;
    try {
        auto dataMapper = [](Data& root, const pqxx::row& row) -> void {
            // テーブルで、null を許可している場合は const char* を使わざるを得ない。
            auto [id, companyId, email, password, name, roles] = row.as<uint64_t, std::string, std::string, std::string, std::string, const char*>();
            print_debug(id, companyId, email, password, name, roles);
            root.addChild("id", id);
            root.addChild("companyId", companyId);
            root.addChild("email", email);
            root.addChild("password", password);
            root.addChild("name", name);
            if(roles) root.addChild("roles", std::string(roles));
            else root.addChild("roles", std::monostate{});
        };
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        const std::string table = "contractor";
        const std::string idseq = "contractor_id_seq";
        const std::string pkey  = "id";
        std::unique_ptr<tmp::Repository<uint64_t, Data>>
            repo = std::make_unique<tmp::postgres::r3::VarNodeRepository<uint64_t>>(&tx, table, idseq, dataMapper, pkey);

        std::optional<Data> result = repo->findById(*id);
        tx.commit();
        conn.close();
        if(result) {
            // check...
            Data::debug(&(result.value()));
        } else {
            throw std::runtime_error("UnExpects Data.");
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_VarNodeRepository_Insert(uint64_t* id)
{
    puts("------ test_VarNodeRepository_Insert");
    using Data = tmp::VarNode;
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        const std::string table = "contractor";
        const std::string idseq = "contractor_id_seq";
        const std::string pkey  = "id";
        std::unique_ptr<tmp::Repository<uint64_t, Data>>
            repo = std::make_unique<tmp::postgres::r3::VarNodeRepository<uint64_t>>(&tx, table, idseq, pkey);

        // Data 作成
        // e.g. Data sample
        // Data data{0, "B3_1000", "alice@loki.org", "alice1111", "Alice", std::nullopt};
        // データベース上でNULL を許可しているカラムは設定しなくてよい（contractor table では roles にあたる）。
        // root ノードのKey とValue は空でなければ何でもよい（現状は）。
        // MySQL の場合との決定的な違いは、プライマリキの設定が必須になる点（data の値はなんでもよい）。
        Data root{"__DATA__KEY__", std::monostate{}};
        uint64_t nid{0};
        root.addChild(pkey, nid);
        std::string companyId{"B3_1000"};
        root.addChild("company_id", companyId);
        std::string email{"alice@loki.org"};
        root.addChild("email", email);
        std::string password{"alice1111"};
        root.addChild("password", password);
        std::string name{"Alice"};
        root.addChild("name", name);

        *id = repo->insert(std::move(root));
        print_debug("id: ", *id);
        tx.commit();
        conn.close();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main()
{
    puts("START main ===");
    // const int version_check = pqxx::internal::check_pqxx_version_7_10();
    // print_debug("check_pqxx_version_7_10: ", version_check);
    int ret = -1;
    if(1) {
        uint64_t id = 0;
        print_debug("Play and Result ...", ret = test_VarNodeRepository_Insert(&id));
        assert(ret == 0);
        print_debug("Play and Result ...", ret = test_VarNodeRepository_Find(&id));
        assert(ret == 0);
        print_debug("Play and Result ...", ret = test_VarNodeRepository_Update(&id));
        assert(ret == 0);
        print_debug("Play and Result ...", ret = test_VarNodeRepository_Delete(&id));
        assert(ret == 0);
    }
    puts("=== main END");
    return EXIT_SUCCESS;
}