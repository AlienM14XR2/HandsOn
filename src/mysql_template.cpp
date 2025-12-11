/**
 * mysql_template.cpp
 * 
 * 概要としては builder_template_m.cpp からの引き継ぎ。
 * リポジトリとエンティティに関する考察とサンプリングに始まり、現在はいかに複製コードを
 * 減らせるかについて考えている。コンパイル時間が長くなったのでサンプリング用のソース
 * ファイルを分割した。
 * 
 * 具体的な問題点は以下。
 * - テーブル単位でリポジトリのサブクラスが作られるのを防ぎたい。
 * 
 * 具体的な改善案は以下。
 * - エンティティ（テーブルクラス or 構造体）毎に必要なメタ情報を取得可能なメンバ関数を
 *   設ける。この仕組みが巨大化、複雑化したら問題だと思う。
 * - リポジトリのサブクラスをひとつとする（汎用クラスで共通の仕組みを持つこと）。
 * 
 * 懸念点は ...
 * その実現性に関して現時点で、あまりに不透明であること。
 * 私の知識・技術不足と汎用クラスを用いた処理が複雑化しないかということ。
 * 似通ったリポジトリは減らせるが、エンティティは無くせない。
 * 
 * e.g. compile
 * g++ -O3 -std=c++20 -DDEBUG -pedantic-errors -Wall -Werror -I../inc/ -I/usr/include/mysql-cppconn/ -L/usr/lib/x86_64-linux-gnu/ mysql_template.cpp -lmysqlcppconn -lmysqlcppconnx -o ../bin/mysql_template
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
#include <sstream>

#include <Repository.hpp>
#include <mysql/jdbc.h>
#include <mysqlx/xdevapi.h>

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
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cerr << "ERROR: " << e.what() << std::endl;
};

template <class... Args>
void print_debug(Args&&... args)
{
    std::cout << "Debug: ";
    auto print_element = [](const auto& element) {
        std::cout << element << '\t';
    };
    // C++17以降の pack expansion で要素を順に処理
    (print_element(std::forward<Args>(args)), ...);
    std::cout << std::endl;
}



namespace tmp::mysql::r2
{

// フィールド情報を保持する構造体
struct FieldMeta {
    std::string db_column_name;
    // 型安全なアクセサを関数オブジェクトとして保持
    // std::function<std::any()> getter;
    // std::function<void(std::any)> setter;
    std::function<std::any(const std::any&)> getter;
    std::function<void(std::any&, const std::any&)> setter;
};

// Data型 (Entity) からメタデータを取得するためのコンセプト
template <typename T>
concept EntityWithMeta = requires {
    { T::get_metadata() } -> std::same_as<const std::map<std::string, FieldMeta>&>;
};

// Contractor Entity
struct Contractor final {
    uint64_t id;
    std::string companyId;
    std::string email;
    std::string password;
    std::string name;
    std::optional<std::string> roles;

    explicit Contractor():id{0}
    {}
    explicit Contractor(const uint64_t& _id, const std::string& _companyId, const std::string& _email, const std::string& _password, const std::string& _name, const std::optional<std::string>& _roles)
    : id{_id}, companyId{_companyId}, email{_email}, password{_password}, name{_name}, roles{_roles}
    {}

    // uint64_t getId()           noexcept { return id; }
    // std::string getCompanyId() noexcept { return companyId; }
    // std::string getEmail()     noexcept { return email; }
    // std::string getPassword()  noexcept { return password; }
    // std::string getName()      noexcept { return name; }
    // std::optional<std::string> getRoles() noexcept {return roles;}

    // EntityWithMeta コンセプトを満たす静的メソッド
    static const std::map<std::string, FieldMeta>& get_metadata() {
        static const std::map<std::string, FieldMeta> metadata = {
            {"primaryKey", {"id", // DBカラム名
                [](const std::any& e) -> std::any { return std::any_cast<const Contractor&>(e).id; }, // Getter
                [](std::any& e, const std::any& v) { std::any_cast<Contractor&>(e).id = std::any_cast<uint64_t>(v); }  // Setter
            }},
            {"companyId", {"company_id",
                [](const std::any& e) -> std::any { return std::any_cast<const Contractor&>(e).companyId; },
                [](std::any& e, const std::any& v) { std::any_cast<Contractor&>(e).companyId = std::any_cast<std::string>(v); }
            }},
            {"email", {"email",
                [](const std::any& e) -> std::any { return std::any_cast<const Contractor&>(e).email; },
                [](std::any& e, const std::any& v) { std::any_cast<Contractor&>(e).email = std::any_cast<std::string>(v); }
            }},
            {"password", {"password",
                [](const std::any& e) -> std::any { return std::any_cast<const Contractor&>(e).password; },
                [](std::any& e, const std::any& v) { std::any_cast<Contractor&>(e).password = std::any_cast<std::string>(v); }
            }},
            {"name", {"name",
                [](const std::any& e) -> std::any { return std::any_cast<const Contractor&>(e).name; },
                [](std::any& e, const std::any& v) { std::any_cast<Contractor&>(e).name = std::any_cast<std::string>(v); }
            }},
            {"roles", {"roles",
                [](const std::any& e) -> std::any { return std::any_cast<const Contractor&>(e).roles; },
                [](std::any& e, const std::any& v) { std::any_cast<Contractor&>(e).roles = std::any_cast<std::optional<std::string>>(v); }
            }}
        };
        return metadata;
    }
    void print()
    {
        puts("Contractor::print");
        std::cout << "id: " << id << '\t';
        std::cout << "companyId: " << companyId << '\t';
        std::cout << "email: " << email << '\t';
        std::cout << "password: " << password << '\t';
        std::cout << "name: " << name << '\t';
        if(roles) {
            std::cout << "roles: " << roles.value();
        }
        std::cout << std::endl;
    }
};


// 一旦この静的な型安全性の高いパターンは保留する。
template <class ID, class Data>
requires EntityWithMeta<Data>
class GenericRepository : public tmp::Repository<ID, Data> {
private:
    mysqlx::Session* const session;
    std::string db;
    std::string table;

public:
    GenericRepository(mysqlx::Session* const _session, const std::string& _db, const std::string& _table)
    : session{_session}, db{_db}, table{_table}
    {}

    ID insert(Data&& data) const override {
        const auto& meta = Data::get_metadata();
        std::stringstream sql_cols;
        std::stringstream sql_vals;

        for (auto const& [field_name, field_meta] : meta) {
            // Getterを使って動的に値を取得
            std::any value = field_meta.getter(std::any(data));
            print_debug("field_name: ", field_name);
            // valueを使ってSQLパラメータをバインドする処理が必要 (ここがDB依存)
            // std::cout << "Inserting " << field_meta.db_column_name << " with value: " << /* cast value */ << std::endl;
            sql_cols << field_meta.db_column_name << ",";
            sql_vals << "?,"; // プリペアドステートメントのプレースホルダ

            // 実際にはここでDBバインディングAPIを呼び出す
        }
        
        // 構築したSQL (INSERT INTO table (cols) VALUES (vals)) を実行
        // DBから返されたIDを返す
        std::cout << "DEBUG SQL: INSERT INTO ... (" << sql_cols.str() << ") VALUES (" << sql_vals.str() << ")" << std::endl;
        return ID{1}; // 仮の戻り値
    }

    // findByIdの実装
    std::optional<Data> findById(const ID& id) const override {
        // SELECT * FROM table WHERE id = ? を実行し、結果セットを取得
        // ResultSet res = db_connection->execute("SELECT ...")

        Data result_entity;
        // const auto& meta = Data::get_metadata();
        // 結果セットの各カラム名に対応する Setter を呼び出し、Dataオブジェクトを動的に構築
        // field_meta.setter(std::any(&result_entity), retrieved_db_value);

        return result_entity;
    }

    void update(const ID&, Data&&) const override { /* ... */ }
    void remove(const ID&) const override { /* ... */ }
};


}   // namespace tmp::mysql::r2


namespace tmp::mysql::r3
{

// @see https://dev.mysql.com/doc/dev/connector-cpp/latest/classmysqlx_1_1abi2_1_1r0_1_1Value.html
// このサンプルでは上記の型のみに対応しておけば良さそう。
// VarNode が保持できる型のリストを定義する
using ValueType = std::variant<
    std::monostate, // 値がない状態を表す
    int64_t,
    uint64_t,
    float,
    double,
    bool,
    std::string
>;

struct VarNode
{
    std::string key;
    ValueType data;
    VarNode* parent = nullptr;
    std::vector<std::unique_ptr<VarNode>> children;

    // コンストラクタを ValueType を受け取るように変更
    VarNode(const std::string& _key, ValueType _data, VarNode* _parent = nullptr)
        : key(_key), data(_data), parent(_parent)
    {}

    // 値の取得は std::get<T> を使う（型が違えば std::bad_variant_access を投げる）
    template <class T>
    T get() const
    {
        return std::get<T>(data);
        // std::get_if を使って安全なポインタ取得を試みる
        // return std::get_if<T>(data);
    }
    template <class T>
    bool is_type() const
    {
        return std::holds_alternative<T>(data);
    }
    VarNode* addChild(const std::string& _key, ValueType _data)
    {
        children.push_back(std::make_unique<VarNode>(_key, _data, this));
        return children.back().get();
    }
    // debug関数は std::visit を使うと大幅に簡潔化できる
    static void debug(const VarNode* const _node, int indent = 0)
    {
        if (_node == nullptr) {
            std::cerr << "node is nil." << std::endl;
            return;
        }
        // インデント表示
        std::cout << std::string(indent * 2, ' ');
        std::cout << "key: " << _node->key << "\tdata: ";

        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (!std::is_same_v<T, std::monostate>) {
                if constexpr (std::is_same_v<T, bool>) {
                    std::cout << (arg ? "true" : "false");
                } else {
                    std::cout << arg;
                }
            } else {
                std::cout << "(null)";
            }
        }, _node->data);
        std::cout << std::endl;

        // 子要素を再帰的に呼び出す
        for (const auto& child : _node->children) {
            debug(child.get(), indent + 1);
        }
    }
};  // VarNode

template <typename T>
std::unique_ptr<T> get_value_safely(const VarNode* const node)
{
    if (node == nullptr) {
        // node 自体が nullptr の場合は runtime_error を投げる
        throw std::runtime_error("VarNode* node is null.");
    }
    // std::get_if<T>(&node->data) が、VarNodeTypeFixer の役割を果たす
    // - T 型が格納されていれば T* を返す
    // - T 型でなければ nullptr を返す
    if (const T* value_ptr = std::get_if<T>(&node->data)) {
        // 値が見つかったので、unique_ptr でラップして返す
        return std::make_unique<T>(*value_ptr);
    } else {
        // 型が一致しない場合は nullptr を返す
        return nullptr;
    }
}

template <typename T>
std::optional<T> get_value_safely_optional(const VarNode* const node)
{
    if (node == nullptr) {
        throw std::runtime_error("VarNode* node is null.");
    }
    if (const T* value_ptr = std::get_if<T>(&node->data)) {
        return *value_ptr; // 値そのものを optional でラップして返す
    } else {
        return std::nullopt; // 値がないことを示す
    }
}

std::vector<mysqlx::Value> convertVarNodeToSqlValues(const VarNode& data_tree)
{
    std::vector<mysqlx::Value> values_to_bind;
    for (const auto& child : data_tree.children) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                // mysqlx::Value() は内部的に NULL を保持するオブジェクトを生成する
                values_to_bind.push_back(mysqlx::Value()); 
            } else {
                // その他の型は mysqlx::Value のコンストラクタが自動的に処理する
                values_to_bind.push_back(mysqlx::Value(arg));
            }
        }, child->data);
    }
    return values_to_bind;
}

// `convertVarNodeToSqlValues` の逆を行うヘルパー関数
ValueType convertSqlValueToVarNode_Corrected(const mysqlx::Value& sql_val) {
    using Type = mysqlx::abi2::r0::Value::Type;
    switch (sql_val.getType()) {
        case Type::VNULL:
            return std::monostate{};
        case Type::UINT64:
            return sql_val.get<uint64_t>();
        case Type::INT64:
            return sql_val.get<int64_t>();
        case Type::FLOAT:
            return sql_val.get<float>();
        case Type::DOUBLE:
            return sql_val.get<double>();
        case Type::BOOL:
            return sql_val.get<bool>();
        case Type::STRING:
            return std::string{sql_val.get<mysqlx::string>()};
        default:
            throw std::runtime_error("Unsupported mysqlx::Value type encountered.");
    }
}

template <class ID>
class VarNodeRepository : public tmp::Repository<ID, VarNode> {
private:
    mysqlx::Session* const session;
    std::string dbName;
    std::string tableName;
    const std::string primaryKeyName;

public:
    VarNodeRepository(
        mysqlx::Session* const _session
        , const std::string&   _db
        , const std::string&   _table
        , const std::string&   _primaryKeyName = "id"
    )
    : session{_session}, dbName{_db}, tableName{_table}, primaryKeyName{_primaryKeyName}
    {}

    ID insert(VarNode&& data) const override
    {
        int status;
        print_debug("insert ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        std::vector<std::string> cols;
        for(size_t i=0; i < data.children.size(); i++) {
            cols.push_back(data.children[i]->key);
            // std::optional<std::string> str = get_value_safely_optional<std::string>(data.children[i].get());
            // if(str) print_debug("key: ", data.children[i]->key, "data: ", str.value());
            // std::optional<int> integer = get_value_safely_optional<int>(data.children[i].get());
            // if(integer) print_debug("key: ", data.children[i]->key, "data: ", integer.value());
            // std::optional<long> l = get_value_safely_optional<long>(data.children[i].get());
            // if(l) print_debug("key: ", data.children[i]->key, "data: ", l.value());
            // std::optional<unsigned int> ui = get_value_safely_optional<unsigned int>(data.children[i].get());
            // if(ui) print_debug("key: ", data.children[i]->key, "data: ", ui.value());
            // std::optional<uint64_t> ui64 = get_value_safely_optional<uint64_t>(data.children[i].get());
            // if(ui64) print_debug("key: ", data.children[i]->key, "data: ", ui64.value());
            // std::optional<double> d = get_value_safely_optional<double>(data.children[i].get());
            // if(d) print_debug("key: ", data.children[i]->key, "data: ", d.value());
            // std::optional<bool> b = get_value_safely_optional<bool>(data.children[i].get());
            // if(b) print_debug("key: ", data.children[i]->key, "data: ", b.value());
        }
        std::vector<mysqlx::Value> vals = convertVarNodeToSqlValues(data);
        mysqlx::Schema db{session->getSchema(dbName)};
        mysqlx::Table table{db.getTable(tableName)};
        mysqlx::Result res = table.insert(cols)
                .values(vals)
                .execute();
        std::cout << "affected items count: " << res.getAffectedItemsCount() << std::endl; // 上記で Insert されたレコード件数
        return res.getAutoIncrementValue();
    }

    // findByIdの実装
    std::optional<VarNode> findById(const ID& id) const override
    {
        mysqlx::Schema db{session->getSchema(dbName)};
        mysqlx::Table table{db.getTable(tableName)};
        std::string condition = primaryKeyName + " = :id";
        mysqlx::RowResult rows = table.select()
                                      .where(condition)
                                      .bind("id", id)
                                      .execute();
        
        if (rows.count() == 0) {
            return std::nullopt;
        }
        mysqlx::Row row = rows.fetchOne();
        // Columns オブジェクトを参照で受け取る (size() 等は使えない)
        const auto& meta = rows.getColumns();
        
        VarNode result_entity("result_row", std::monostate{});
        // インデックスではなく、範囲ベース for ループとカウンタを併用する
        size_t i = 0;
        for (const auto& col_meta : meta) {
            const std::string& col_name = col_meta.getColumnName();
            // 行オブジェクトからはインデックスで値を取得する
            mysqlx::Value db_value = row.get(i);
            // 正しい変換関数を使用
            ValueType variant_value = convertSqlValueToVarNode_Corrected(db_value);
            result_entity.addChild(col_name, variant_value);
            i++;
        }
        return result_entity;
    }

    void update(const ID& id, VarNode&& data) const override
    {
        int status;
        print_debug("update ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        mysqlx::Schema db{session->getSchema(dbName)};
        mysqlx::Table table{db.getTable(tableName)};
        // 1. まず update ステートメントを開始する
        auto tableUpdate = table.update();

        // 2. VarNode の子ノード（更新対象カラム）をループ処理し、動的に set() を呼び出す
        for (const auto& child : data.children) {
            // child->key がカラム名
            // child->data が更新する値 (ValueType)
            // std::visit を使用して ValueType の中身を取り出し、mysqlx::Value に変換してセットする
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!std::is_same_v<T, std::monostate>) {
                    // mysqlx::Value のコンストラクタが arg の型を適切に処理する
                    mysqlx::Value sql_val(arg);
                    // set(カラム名, 値)
                    tableUpdate.set(child->key, sql_val);
                }
                // monostate の場合は更新しない（NULLをセットしたい場合は別途ロジックが必要）
            }, child->data);
        }
        // 3. WHERE句を設定して実行する
        // primaryKeyName はコンストラクタで受け取ったメンバ変数
        std::string condition = primaryKeyName + " = :id";
        mysqlx::Result res = tableUpdate.where(condition)
                                        .bind("id", id)
                                        .execute();
        std::cout << "affected items count: " << res.getAffectedItemsCount() << std::endl;
    }
    virtual void remove(const ID& id) const override
    {
        int status;
        print_debug("remove ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        mysqlx::Schema db{session->getSchema(dbName)};
        mysqlx::Table table{db.getTable(tableName)};
        std::string condition = primaryKeyName + " = :id";
        mysqlx::Result res = table.remove()
                            .where(condition)
                            .bind("id", id)
                            .execute();
        std::cout << "affected items count: " << res.getAffectedItemsCount() << std::endl;
	}
};

}   // namespace tmp::mysql::r3


int test_VarNodeRepository_Remove(uint64_t* id)
{
    puts("------ test_VarNodeRepository_Remove");
    using Data = tmp::mysql::r3::VarNode;
    std::clock_t start_1 = clock();
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    try {
        std::clock_t start_2 = clock();
        std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
            = std::make_unique<tmp::mysql::r3::VarNodeRepository<uint64_t>>(&sess, "test", "contractor", "id");

        sess.startTransaction();
        irepo->remove(*id);
        sess.commit();
        std::clock_t end = clock();
        std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        sess.rollback();
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_VarNodeRepository_Update(uint64_t* id)
{
    puts("------ test_VarNodeRepository_Update");
    using Data = tmp::mysql::r3::VarNode;
    std::clock_t start_1 = clock();
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    try {
        std::clock_t start_2 = clock();
        std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
            = std::make_unique<tmp::mysql::r3::VarNodeRepository<uint64_t>>(&sess, "test", "contractor", "id");

        // e.g. Data sample
        // Data data{"B3_3333", "foo@loki.org", "foo1111", "Foo", roles};
        // ポリシの問題だが、更新時は別途、仮引数で値を渡しているため、ここでの設定はいらない（プライマリキの更新は不可が正しい）。
        // データベース上でNULL を許可しているカラムは設定しなくてよい（contractor table では roles にあたる）。
        // root ノードのKey とValue は空でなければ何でもよい（現状は）。
        std::string pkcol{"id"};
        Data root{"primaryKey", pkcol};
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
        sess.startTransaction();
        irepo->update(*id, std::move(root));
        sess.commit();
        std::clock_t end = clock();
        std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        sess.rollback();
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_VarNodeRepository_FindById(uint64_t* id)
{
    puts("------ test_VarNodeRepository_FindById");
    using Data = tmp::mysql::r3::VarNode;
    std::clock_t start_1 = clock();
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    try {
        std::clock_t start_2 = clock();
        std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
            = std::make_unique<tmp::mysql::r3::VarNodeRepository<uint64_t>>(&sess, "test", "contractor");

        sess.startTransaction();
        std::optional<Data> resultVNode = irepo->findById(*id);
        sess.commit();
        std::clock_t end = clock();
        if(resultVNode) Data::debug(&(resultVNode.value()));
        else throw std::runtime_error("test_VarNodeRepository_FindById() is failed.");
        std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        sess.rollback();
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_VarNodeRepository_Insert(uint64_t* id)
{
    puts("------ test_VarNodeRepository_Insert");
    using Data = tmp::mysql::r3::VarNode;
    std::clock_t start_1 = clock();
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    try {
        std::clock_t start_2 = clock();
        std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
            = std::make_unique<tmp::mysql::r3::VarNodeRepository<uint64_t>>(&sess, "test", "contractor");

        // e.g. Data sample
        // Data data{0, "B3_1000", "alice@loki.org", "alice1111", "Alice", std::nullopt};
        // 
        // Data の作り方にひと工夫必要なはず。
        // primaryKey is xxx という情報が最低限必要（VarNoderepository のコンストラクタの仮引数として解決した：）
        // データベース上でNULL を許可しているカラムは設定しなくてよい（contractor table では roles にあたる）。
        // root ノードのKey とValue は空でなければ何でもよい（現状は）。
        std::string pkcol{"id"};
        Data root{"primaryKey", pkcol};
        std::string companyId{"B3_1000"};
        root.addChild("company_id", companyId);
        std::string email{"alice@loki.org"};
        root.addChild("email", email);
        std::string password{"alice1111"};
        root.addChild("password", password);
        std::string name{"Alice"};
        root.addChild("name", name);
        sess.startTransaction();
        *id = irepo->insert(std::move(root));
        sess.commit();
        std::clock_t end = clock();
        print_debug("id: ", *id);
        std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        sess.rollback();
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_GenericRepository()
{
    puts("------ test_GenericRepository");
    // using namespace tmp::mysql::r2;
    // using Repo = tmp::mysql::r2::GenericRepository;
    using Data = tmp::mysql::r2::Contractor;
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    try {
        std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
            = std::make_unique<tmp::mysql::r2::GenericRepository<uint64_t, Data>>(&sess, "test", "contractor");
        Data data{0, "B3_1000", "alice@loki.org", "alice1111", "Alice", std::nullopt};
        sess.startTransaction();
        uint64_t id = irepo->insert(std::move(data));
        sess.commit();
        print_debug("id: ", id);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        sess.rollback();
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main()
{
    puts("START main ===");
    int ret = -1;
    if(0) {
        print_debug("Play and Result ...", ret = test_GenericRepository());
        assert(ret == 0);
    }
    if(1) {
        uint64_t id = 0ul;
        print_debug("Play and Result ...", ret = test_VarNodeRepository_Insert(&id));
        assert(ret == 0);
        print_debug("Play and Result ...", ret = test_VarNodeRepository_FindById(&id));
        assert(ret == 0);
        print_debug("Play and Result ...", ret = test_VarNodeRepository_Update(&id));
        assert(ret == 0);
        print_debug("Play and Result ...", ret = test_VarNodeRepository_Remove(&id));
        assert(ret == 0);
    }
    puts("=== main END");
    return EXIT_SUCCESS;
}
