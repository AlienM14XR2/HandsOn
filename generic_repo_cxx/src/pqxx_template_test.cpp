/**
 * PostgreSQL pqxx_template.cpp のテスト (テストフィクスチャ単位トランザクション戦略)
 * 
 * 
 * ```
# Google Test (Debian/Ubuntu)
sudo apt update
# google test の開発パッケージをインストール
sudo apt install libgtest-dev
# CMake を使ってビルドするために必要
sudo apt install cmake 

# インストールされたソースがあるディレクトリに移動（Ubuntuのデフォルトパス）
cd /usr/src/googletest/googletest
# ビルドディレクトリを作成して移動
sudo cmake CMakeLists.txt
sudo make
# ライブラリをシステムライブラリのパスにコピー
sudo cp lib/libgtest.a lib/libgtest_main.a /usr/local/lib/
find /usr/local/lib -name 'libgtest.a' 2>/dev/null
```
 *  
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror pqxx_template_test.cpp -o ../bin/pqxx_template_test_runner -I../inc -I/usr/local/include -L/usr/local/lib -lpqxx -lpq -lgtest -lgtest_main -lpthread
 * g++ -O3 -DNDEBUG -std=c++20 -pedantic-errors -Wall -Werror pqxx_template_test.cpp -o ../bin/pqxx_template_test_runner -I../inc -I/usr/local/include -L/usr/local/lib -lpqxx -lpq -lgtest -lgtest_main -lpthread
 */
#include "gtest/gtest.h"
#include <pqxx/pqxx>
#include <pqxx_template.hpp>
#include <ObjectPool.hpp>
#include <memory>

tmp::VarNode create_test_var_node(
    const std::string& company_id,
    const std::string& email,
    const std::string& password,
    const std::string& name,
    const std::optional<std::string>& roles = std::nullopt
) {
    tmp::VarNode root("root", std::monostate{});
    root.addChild("id", 0ul);// IDは自動生成されるので仮の値
    root.addChild("company_id", company_id);
    root.addChild("email"     , email);
    root.addChild("password"  , password);
    root.addChild("name"      , name);
    if (roles.has_value()) {
        root.addChild("roles", roles.value());
    }
    return root;
}

// サービス層のサンプル
// サンプルでは一つのテーブルで表現しているが、複数の異なるテーブルに
// 対応したリポジトリが扱える。
template<typename... Repos>
class MyBusinessService : public tmp::ServiceExecutor {
    using Data = tmp::VarNode;
private:
    std::tuple<Repos...> repos; // 複数のリポジトリを保持

public:
    // 可変引数でリポジトリを受け取る
    explicit MyBusinessService(Repos&&... args) : repos(args...) {}

    void execute() override {
        using Data = tmp::VarNode;
        // Insert
        const std::string pkey  = "id";
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
        int64_t id = static_cast<tmp::postgres::r3::VarNodeRepository<uint64_t>>(std::get<0>(repos)).insert(std::move(root));
        tmp::print_debug("id: ", id);
        // Update
        Data updateNode{"__DATA__KEY__", std::monostate{}};
        uint64_t u_nid{0};
        updateNode.addChild(pkey, u_nid);
        std::string u_companyId{"B3_3333"};
        updateNode.addChild("company_id", u_companyId);
        std::string u_email{"foo@loki.org"};
        updateNode.addChild("email", u_email);
        std::string u_password{"foo1111"};
        updateNode.addChild("password", u_password);
        std::string u_name{"Foo"};
        updateNode.addChild("name", u_name);
        std::string u_roles{"Admin,User"};
        updateNode.addChild("roles", u_roles);
        static_cast<tmp::postgres::r3::VarNodeRepository<uint64_t>>(std::get<1>(repos)).update(id, std::move(updateNode));
        // Find
        std::optional<Data> f_result = static_cast<tmp::postgres::r3::VarNodeRepository<uint64_t>>(std::get<2>(repos)).findById(id);
        if(!(f_result)) throw std::runtime_error("Unexpected case.");
        else tmp::debug_print_varnode(&(f_result.value()));
        // 4. Remove
        static_cast<tmp::postgres::r3::VarNodeRepository<uint64_t>>(std::get<3>(repos)).remove(id);
    }
};

// --- テストフィクスチャークラス ---
class VarNodeRepositoryTest : public ::testing::Test
{
protected:
    // 各テストケース実行前に呼ばれるセットアップメソッド
    void SetUp() override {
        // !!! ここをあなたの正しい接続情報に修正してください !!!
        const std::string conn_string = "postgresql://derek:derek1234@localhost:5432/derek"; 
        try {
            // テストケースごとに接続とトランザクションを開始
            conn.reset(new pqxx::connection(conn_string));
            tx.reset(new pqxx::work(*conn)); 
            ASSERT_TRUE(conn->is_open()) << "DB connection failed in SetUp.";
        } catch (const std::exception &e) {
            FAIL() << "DB接続またはトランザクション開始に失敗しました: " << e.what();
        }
    }

    // 各テストケース実行後に呼ばれるティアダウンメソッド
    void TearDown() override {
        // テスト終了時に、すべての変更をロールバック（アボート）する
        if (tx) { 
            tx->abort(); // テストデータはDBに残さない
        }
        // connection は unique_ptr のデストラクタで自動的に閉じられる
    }

    // pool->push(std::make_unique<pqxx::connection>(pqxx::connection("hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234")));
    const std::string CONN_STR = "hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234";
    // フィクスチャで共有するメンバ変数
    std::unique_ptr<pqxx::connection> conn;
    std::unique_ptr<pqxx::work> tx;
    const std::string TEST_TABLE = "contractor";
    const std::string TEST_SEQ = "contractor_id_seq";
    const std::string TEST_PK = "id";
};

// --- TEST_F マクロを使用したテストケース ---
TEST_F(VarNodeRepositoryTest, InsertAndFindById)
{
    std::clock_t start_1 = clock();
    // 1. テスト対象のリポジトリをインスタンス化
    // tx.get() でトランザクションのポインタを渡す
    tmp::postgres::r3::VarNodeRepository<unsigned long> repo(
        tx.get(), TEST_TABLE, TEST_SEQ, TEST_PK
    );

    // 2. 挿入する VarNode データを準備
    tmp::VarNode data_to_insert = create_test_var_node(
        "B3_TEST", "test@example.com", "testpass", "Test User"
    );
    tmp::debug_print_varnode(&data_to_insert);

    // 3. 挿入実行
    unsigned long new_id = repo.insert(std::move(data_to_insert));
    std::clock_t end = clock();
    std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;

    // 4. IDが有効な値であることを確認
    ASSERT_NE(new_id, 0UL);     // assert not equals... 0 でないことの表明。
    EXPECT_GT(new_id, 0UL);     // expect greater than... 0 より大きい値であることを期待。

    // 5. findById で検索
    tmp::postgres::r3::VarNodeRepository<unsigned long> repo_find(
        tx.get(), TEST_TABLE, TEST_SEQ, // tx.get() でトランザクションのポインタを渡す
        [](tmp::VarNode& root, const pqxx::row& row){
            // テスト用のシンプルなマッパー
            root.addChild("id", row["id"].as<uint64_t>());
            root.addChild("email", row["email"].as<std::string>());
        }, 
        TEST_PK
    );

    std::optional<tmp::VarNode> found_node = repo_find.findById(new_id);

    // 6. 検索結果が存在することを確認
    ASSERT_TRUE(found_node.has_value());
    
    // 7. 取得したデータの検証
    auto email_node = found_node->getChild("email");
    ASSERT_NE(email_node, nullptr);
    EXPECT_EQ(email_node->get<std::string>(), "test@example.com");

    // TearDownでabort()が呼ばれるため、ここではtx->commit()は不要
}
TEST_F(VarNodeRepositoryTest, Insert_Update_FindById)
{
    // 1. テスト対象のリポジトリをインスタンス化
    // リポジトリの作成
    tmp::postgres::r3::VarNodeRepository<unsigned long> repo(
        tx.get(), TEST_TABLE, TEST_SEQ, TEST_PK
    );

    // 2. 挿入する VarNode データを準備
    tmp::VarNode data_to_insert = create_test_var_node(
        "B3_TEST", "test@example.com", "testpass", "Test User"
    );

    // 3. 挿入実行
    unsigned long new_id = repo.insert(std::move(data_to_insert));
    // 4. IDが有効な値であることを確認
    ASSERT_NE(new_id, 0UL);
    EXPECT_GT(new_id, 0UL);

    // 5. 更新する VarNode データを準備
    std::string expect_company_id{"B3_TEST_U"};
    std::string expect_email{"test_U@example.com"};
    std::string expect_password{"testpass_U"};
    std::string expect_name{"Test User_U"};
    std::string expect_roles{"ADMIN_USER_U"};
    tmp::VarNode data_to_update = create_test_var_node(
        expect_company_id, expect_email, expect_password, expect_name, expect_roles
    );
    repo.update(new_id, std::move(data_to_update));
    // 6. findById で検索
    tmp::postgres::r3::VarNodeRepository<unsigned long> repo_find(
        tx.get(), TEST_TABLE, TEST_SEQ, // tx.get() でトランザクションのポインタを渡す
        [](tmp::VarNode& root, const pqxx::row& row){
            // テスト用のシンプルなマッパー
            root.addChild("id",         row["id"].as<uint64_t>());
            root.addChild("company_id", row["company_id"].as<std::string>());
            root.addChild("email",      row["email"].as<std::string>());
            root.addChild("password",   row["password"].as<std::string>());
            root.addChild("name",       row["name"].as<std::string>());
            root.addChild("roles",      row["roles"].as<std::string>());
        }, 
        TEST_PK
    );
    std::optional<tmp::VarNode> found_node = repo_find.findById(new_id);
    // 7. 検索結果が存在することを確認
    ASSERT_TRUE(found_node.has_value());
    // 8. 取得したデータの検証
    auto company_id_node = found_node->getChild("company_id");
    ASSERT_NE(company_id_node, nullptr);
    EXPECT_EQ(company_id_node->get<std::string>(), expect_company_id);
    auto email_node = found_node->getChild("email");
    ASSERT_NE(email_node, nullptr);
    EXPECT_EQ(email_node->get<std::string>(), expect_email);
    auto password_node = found_node->getChild("password");
    ASSERT_NE(password_node, nullptr);
    EXPECT_EQ(password_node->get<std::string>(), expect_password);
    auto name_node = found_node->getChild("name");
    ASSERT_NE(name_node, nullptr);
    EXPECT_EQ(name_node->get<std::string>(), expect_name);
    auto roles_node = found_node->getChild("roles");
    ASSERT_NE(roles_node, nullptr);
    EXPECT_EQ(roles_node->get<std::string>(), expect_roles);
}
TEST_F(VarNodeRepositoryTest, Insert_Remove_FindById)
{
    // 1. テスト対象のリポジトリをインスタンス化
    // リポジトリの作成
    tmp::postgres::r3::VarNodeRepository<unsigned long> repo(
        tx.get(), TEST_TABLE, TEST_SEQ, TEST_PK
    );

    // 2. 挿入する VarNode データを準備
    tmp::VarNode data_to_insert = create_test_var_node(
        "B3_TEST", "test@example.com", "testpass", "Test User"
    );

    // 3. 挿入実行
    unsigned long new_id = repo.insert(std::move(data_to_insert));
    // 4. IDが有効な値であることを確認
    ASSERT_NE(new_id, 0UL);
    EXPECT_GT(new_id, 0UL);

    // 5. 削除する VarNode データを準備（IDのみ）
    repo.remove(new_id);
    // 6. findById で検索
    tmp::postgres::r3::VarNodeRepository<unsigned long> repo_find(
        tx.get(), TEST_TABLE, TEST_SEQ, // tx.get() でトランザクションのポインタを渡す
        [](tmp::VarNode& root, const pqxx::row& row){
            // テスト用のシンプルなマッパー
            root.addChild("id",         row["id"].as<uint64_t>());
            root.addChild("company_id", row["company_id"].as<std::string>());
            root.addChild("email",      row["email"].as<std::string>());
            root.addChild("password",   row["password"].as<std::string>());
            root.addChild("name",       row["name"].as<std::string>());
            root.addChild("roles",      row["roles"].as<std::string>());
        }, 
        TEST_PK
    );
    std::optional<tmp::VarNode> found_node = repo_find.findById(new_id);
    // 7. 検索結果が存在しないことを確認
    ASSERT_FALSE(found_node.has_value());
}
TEST_F(VarNodeRepositoryTest, ServiceLayer_CRUD_cycle)
{
    using Data = tmp::VarNode;
    // Poolオブジェクトの作成
    auto pool = ObjectPool<pqxx::connection>::create("postgres");
    // コネクション（セッション）をひとつPool に確保。
    pool->push(std::make_unique<pqxx::connection>(pqxx::connection(CONN_STR)));
    // Poolから取り出す
    auto conn = pool->pop();
    pqxx::work tx(*(conn.get()));   // これが実質 Tx BEGIN
    const std::string table = "contractor";
    const std::string idseq = "contractor_id_seq";
    const std::string pkey  = "id";
    tmp::postgres::r3::VarNodeRepository<uint64_t> insert_repo{&tx, table, idseq, pkey};
    tmp::postgres::r3::VarNodeRepository<uint64_t> update_repo{&tx, table, idseq, pkey};
    auto dataMapper = [](Data& root, const pqxx::row& row) -> void {
        // テーブルで、null を許可している場合は const char* を使わざるを得ない。
        auto [id, companyId, email, password, name, roles] = row.as<uint64_t, std::string, std::string, std::string, std::string, const char*>();
        tmp::print_debug(id, companyId, email, password, name, roles);
        root.addChild("id", id);
        root.addChild("companyId", companyId);
        root.addChild("email", email);
        root.addChild("password", password);
        root.addChild("name", name);
        if(roles) root.addChild("roles", std::string(roles));
        else root.addChild("roles", std::monostate{});
    };
    tmp::postgres::r3::VarNodeRepository<uint64_t> find_repo{&tx, table, idseq, dataMapper, pkey};
    tmp::postgres::r3::VarNodeRepository<uint64_t> remove_repo{&tx, table, idseq, pkey};
    MyBusinessService service(
        std::move(insert_repo), 
        std::move(update_repo), 
        std::move(find_repo),
        std::move(remove_repo)
    );
	EXPECT_NO_THROW({
        tmp::postgres::r3::execute_service_with_tx(tx, service);
    });
}
TEST_F(VarNodeRepositoryTest, SimplePrepare_SqlExecutor_LifecycleTest) {
    using namespace tmp::postgres::r3;
    using Data = std::map<std::string, std::string>;

    // SqlExecutor はフィクスチャの tx (pqxx::work) を借用
    SqlExecutor exec{this->tx.get()};

    // 1. Insert
    SimplePrepare insPrepare{"insert_"};
    insPrepare.setQuery(tmp::postgres::helper::insert_sql(TEST_TABLE, "company_id", "email", "password", "name", "roles"));
    
    // Postgres版の形式: 型指定と値指定
    insPrepare.type("text").type("text").type("text").type("text").type("text");
    std::vector<std::string> ins_stmt = insPrepare
        .value("D1_1000")
        .value("foo@loki.org")
        .value("foo2222")
        .value("Foo")
        .value("Admin")
        .build();

    ASSERT_NO_THROW(exec.cudProc_v1(std::move(ins_stmt)));

    // 2. Find (Insertしたデータの確認とID取得)
    // 自動パースの findProc_v2 を使用
    std::string s1 = "PREPARE select_(int) AS SELECT id, company_id, email FROM " + TEST_TABLE + " ORDER BY " + TEST_PK + " DESC LIMIT $1;";
    std::string s2 = "EXECUTE select_(1);";
    
    std::vector<Data> r1 = exec.findProc_v2({s1, s2});
    
    ASSERT_FALSE(r1.empty()) << "挿入したデータが取得できませんでした。";
    std::string current_id = r1[0].at(TEST_PK);
    EXPECT_EQ(r1[0].at("company_id"), "D1_1000");

    // 3. Delete (取得したIDを使用して削除)
    SimplePrepare delPrepare{"delete_"};
    delPrepare.setQuery(tmp::postgres::helper::delete_by_pkey_sql(TEST_TABLE, TEST_PK));
    
    std::vector<std::string> del_stmt = delPrepare
        .type("int")
        .value(current_id) // stringを渡しても内部で to_string(int) 相当が動くか確認
        .build();

    ASSERT_NO_THROW(exec.cudProc_v1(std::move(del_stmt)));

    // 4. Final Check (削除確認)
    // 再度 PREPARE する際は DEALLOCATE が必要になる可能性があるが、
    // 同一セッションでの別名指定か、findProc_v2 で直接実行
    std::string check_sql = "SELECT id FROM " + TEST_TABLE + " WHERE " + TEST_PK + " = " + current_id + ";";
    std::vector<Data> r2 = exec.findProc_v2({check_sql});
    EXPECT_TRUE(r2.empty()) << "データが削除されていません。ID: " << current_id;
}
// --- main関数は不要 ---
// libgtest_main.a をリンクすることで main 関数が提供されます
