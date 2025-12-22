/**
 * MySQL mysql_template.cpp のテスト (X Dev API / テストフィクスチャ単位トランザクション戦略)
 * 
 * 
 * Google Test を利用する場合は、その取り込まれるソースファイル、今回であれば、
 * mysql_template.cpp の main 関数はいらない。
 * 
 * テスト対象のmain 関数の扱いについて。
 * 以下、AIの見解。
 * 
 * 例外：独自の main 関数が必要なケース
 * 標準的な Google Test の利用において main 関数が不要なのは、コンパイル時に
 *  -lgtest_main というライブラリをリンクしているためです。このライブラリが、
 * テスト実行に必要な main 関数を提供してくれます。
 * ただし、以下のようないくつかの例外的なシナリオでは、独自の main 関数を記述
 * する必要があります。
 * 
 *  環境リスナー (Environment Listeners) の登録:
 *  今回のセッションで一時的に採用した「グローバルトランザクション」戦略のように、
 *  テストスイート全体の実行前後に特別なセットアップやクリーンアップが必要な場合 (::testing::AddGlobalTestEnvironment() を呼び出すため)。
 *  高度なコマンドライン引数のパース:
 *  Google Test 標準以外の独自のコマンドライン引数を処理したい場合。
 *  特定の初期化処理の実行:
 *  Google Test 以外の別のライブラリの初期化を、テスト実行前に確実に行いたい場合
 *  これらの例外ケースを除けば、常に -lgtest_main をリンクし、テスト対象ファイルから 
 *  main 関数を排除するのが、最もクリーンで標準的なアプローチです。
 * 
 * e.g. compile
 * g++ -O3 -std=c++20 -DDEBUG -pedantic-errors -Wall -Werror mysql_template_test.cpp -o ../bin/mysql_template_test_runner -I../inc/ -I/usr/include/mysql-cppconn/ -I/usr/local/include -L/usr/lib/x86_64-linux-gnu/ -L/usr/local/lib -lmysqlcppconn -lmysqlcppconnx -lgtest -lgtest_main -lpthread
 * g++ -O3 -std=c++20 -DNDEBUG -pedantic-errors -Wall -Werror mysql_template_test.cpp -o ../bin/mysql_template_test_runner -I../inc/ -I/usr/include/mysql-cppconn/ -I/usr/local/include -L/usr/lib/x86_64-linux-gnu/ -L/usr/local/lib -lmysqlcppconn -lmysqlcppconnx -lgtest -lgtest_main -lpthread
 */
#include "gtest/gtest.h"
#include <mysqlx/xdevapi.h> // X Dev API のヘッダー
#include <memory>
#include <mysql_template.hpp>
#include <ObjectPool.hpp>
#include <sql_helper.hpp>

tmp::VarNode create_test_var_node(
    const std::string& company_id,
    const std::string& email,
    const std::string& password,
    const std::string& name,
    const std::optional<std::string>& roles = std::nullopt
) {
    tmp::VarNode root("root", std::monostate{});
    root.addChild("company_id", company_id);
    root.addChild("email"     , email);
    root.addChild("password"  , password);
    root.addChild("name"      , name);
    if (roles.has_value()) {
        root.addChild("roles", roles.value());
    }
    return root;
}

// ユーザーが実装する具体的なサービス
// template<typename... Repos>
// concept AllRepos = (tmp::mysql::r3::VarNodeRepository<std::uint64_t><Repos> && ...);
// template<AllRepos... Args>
template<typename... Repos>
class MyBusinessService : public tmp::ServiceExecutor {
    using Data = tmp::VarNode;
private:
    std::tuple<Repos...> repos; // 複数のリポジトリを保持

public:
    // 可変引数でリポジトリを受け取る
    explicit MyBusinessService(Repos&&... args) : repos(args...) {}

    void execute() override {
        // 1. リポジトリの取り出し
        // 2. 複数のリポジトリを「串刺し」にしたビジネスロジック
        // 例: std::get<0>(repos)->insert(...);
        // Insert
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
        int64_t id = static_cast<tmp::mysql::r3::VarNodeRepository<uint64_t>>(std::get<0>(repos)).insert(std::move(root));
        tmp::print_debug("id: ", id);
        // Update
        std::string pkcol_u{"id"};
        Data updateData{"primaryKey", pkcol_u};
        std::string companyId_u{"B3_3333"};
        updateData.addChild("company_id", companyId_u);
        std::string email_u{"alice_uloki.org"};
        updateData.addChild("email", email_u);
        std::string password_u{"alice3333"};
        updateData.addChild("password", password_u);
        std::string name_u{"Alice_U"};
        updateData.addChild("name", name_u);
        std::string role_u{"Admin,User"};
        updateData.addChild("roles", role_u);
        static_cast<tmp::mysql::r3::VarNodeRepository<uint64_t>>(std::get<1>(repos)).update(id, std::move(updateData));
        // Find
        std::optional<Data> fnode = static_cast<tmp::mysql::r3::VarNodeRepository<uint64_t>>(std::get<2>(repos)).findById(id);
        if(!fnode) throw std::runtime_error("Unexpected case.");
        else tmp::debug_print_varnode(&(fnode.value()));
        // Remove
        static_cast<tmp::mysql::r3::VarNodeRepository<uint64_t>>(std::get<3>(repos)).remove(id);
    }
};


class MySqlRepositoryTest : public ::testing::Test
{
protected:
    void SetUp() override 
    {
        // !!! ここをあなたの正しい接続情報に修正してください !!!
        // 手動テストログより推測される情報を使用
        const std::string conn_string = "root:root1234@127.0.0.1:33060/test";

        try {
            sess.reset(new mysqlx::Session(conn_string));
            sess->startTransaction();
            ASSERT_NE(sess.get(), nullptr) << "DB session failed in SetUp (session is null).";

        } catch (const mysqlx::Error &e) {
            FAIL() << "DB接続またはトランザクション開始に失敗しました: " << e.what();
        }
    }

    void TearDown() override {
        if (sess) { 
            sess->rollback(); // テストデータはDBに残さない
        }
    }

    // pool->push(std::make_unique<mysqlx::Session>(mysqlx::Session("localhost", 33060, "root", "root1234")));
    const std::string HOST = "localhost";
    const int64_t     PORT = 33060;
    const std::string USER = "root";
    const std::string PASSWORD = "root1234";
    // プールマネージャーの unique_ptr を保持
    std::unique_ptr<mysqlx::Session> sess; 
    const std::string TEST_SCHEMA = "test";
    const std::string TEST_TABLE = "contractor";
    const std::string TEST_PK = "id";
};
// --- TEST_F マクロを使用したテストケース ---

TEST_F(MySqlRepositoryTest, InsertAndFindById)
{
    std::clock_t start_1 = clock();
    // 1. テスト対象のリポジトリをインスタンス化
    // Done... ObjectPool にする必要がある。
    tmp::mysql::r3::VarNodeRepository<unsigned long> repo(
        sess.get(), TEST_SCHEMA, TEST_TABLE, TEST_PK // 4引数で呼び出す
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
    ASSERT_NE(new_id, 0UL);
    EXPECT_GT(new_id, 0UL); 

    // 5. findById で検索 
    std::optional<tmp::VarNode> found_node = repo.findById(new_id);

    // 6. 検索結果が存在することを確認
    ASSERT_TRUE(found_node.has_value());
    
    // 7. 取得したデータの検証
    auto email_node = found_node->getChild("email");
    ASSERT_NE(email_node, nullptr);
    EXPECT_EQ(email_node->get<std::string>(), "test@example.com");
}
TEST_F(MySqlRepositoryTest, Insert_Update_FindById)
{
    // 1. テスト対象のリポジトリをインスタンス化
    // Done... ObjectPool にする必要がある。
    tmp::mysql::r3::VarNodeRepository<unsigned long> repo(
        sess.get(), TEST_SCHEMA, TEST_TABLE, TEST_PK // 4引数で呼び出す
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

    // 5. 更新する VarNode データを準備 (ヘルパー関数を使用)
    // 期待値もヘルパー関数の引数から取得可能
    std::string expect_company_id{"B3_TEST_U"};
    std::string expect_email{"test_U@example.com"};
    std::string expect_password{"testpass_U"};
    std::string expect_name{"Test User_U"};
    std::string expect_roles{"ADMIN_USER_U"};
    tmp::VarNode data_to_update = create_test_var_node(
        expect_company_id, expect_email, expect_password, expect_name, expect_roles
    );
    // 6. 挿入実行
    repo.update(new_id, std::move(data_to_update));
    // 7. findById で検索
    std::optional<tmp::VarNode> found_node = repo.findById(new_id);
    // 8. 検索結果が存在することを確認
    ASSERT_TRUE(found_node.has_value());
    
    // 9. 取得したデータの検証
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
TEST_F(MySqlRepositoryTest, Insert_Remove_FindById)
{
    // 1. テスト対象のリポジトリをインスタンス化
    tmp::mysql::r3::VarNodeRepository<unsigned long> repo(
        sess.get(), TEST_SCHEMA, TEST_TABLE, TEST_PK // 4引数で呼び出す
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

    // 5. 削除実行
    repo.remove(new_id);
    // 6. findById で検索
    std::optional<tmp::VarNode> found_node = repo.findById(new_id);
    // 7. 検索結果が存在しないことを確認
    ASSERT_FALSE(found_node.has_value());
}
TEST_F(MySqlRepositoryTest, ServiceLayer_CRUD_cycle) {
    // Poolオブジェクトの作成
    auto pool = ObjectPool<mysqlx::Session>::create("mysql");
    // コネクション（セッション）をひとつPool に確保。
    pool->push(std::make_unique<mysqlx::Session>(mysqlx::Session(HOST, PORT, USER, PASSWORD)));
    // Poolから取り出す
    auto sess = pool->pop();
    tmp::mysql::r3::MySqlTransaction tx(sess.get());
    tmp::mysql::r3::VarNodeRepository<uint64_t> insert_repo{sess.get(), TEST_SCHEMA, TEST_TABLE};
    tmp::mysql::r3::VarNodeRepository<uint64_t> update_repo{sess.get(), TEST_SCHEMA, TEST_TABLE};
    tmp::mysql::r3::VarNodeRepository<uint64_t> find_repo{sess.get(), TEST_SCHEMA, TEST_TABLE};
    tmp::mysql::r3::VarNodeRepository<uint64_t> remove_repo{sess.get(), TEST_SCHEMA, TEST_TABLE};
    MyBusinessService service(std::move(insert_repo), std::move(update_repo), std::move(find_repo), std::move(remove_repo));
    // 6. 共通プロトコルによる実行
    EXPECT_NO_THROW({
        tmp::mysql::r3::execute_service_with_tx(tx, service);
    });

    // sess_ptr のスコープを抜ける際、デストラクタによりセッションはPoolに返却、
    // またはPoolと共に安全に破棄される（ObjectPoolの仕様に依存）
}
TEST_F(MySqlRepositoryTest, SimplePrepare_SqlExecutor_LifecycleTest) {
    using namespace tmp::mysql::r3;
    using Data = std::map<std::string, std::string>;

    // SqlExecutor はフィクスチャの sess を借用
    tmp::mysql::r3::SqlExecutor exec{this->sess.get(), TEST_SCHEMA};

    // 1. Insert
    tmp::mysql::r3::SimplePrepare insPrepare{"stmt1"};
    insPrepare.setQuery(tmp::mysql::helper::insert_sql(TEST_TABLE, "company_id", "email", "password", "name"));
    
    std::vector<std::string> ins_syntax = insPrepare
        .set("C3_3000")
        .set("derek@loki.org")
        .set("derek1111")
        .set("DEREK")
        .build();

    // 実行（例外が発生しないことを確認）
    ASSERT_NO_THROW(exec.cudProc_v1(std::move(ins_syntax)));

    // 2. Find (Insertしたデータの確認とID取得)
    std::string select_sql = "SELECT * FROM " + TEST_TABLE + " ORDER BY " + TEST_PK + " DESC LIMIT 1;";
    std::vector<std::string> find_vec{select_sql};
    
    std::vector<Data> r1 = exec.findProc_v1(std::move(find_vec));
    
    ASSERT_FALSE(r1.empty()) << "挿入したデータが取得できませんでした。";
    std::string current_id = r1[0].at(TEST_PK);
    EXPECT_EQ(r1[0].at("name"), "DEREK");

    // 3. Update
    SimplePrepare upPrepare{"stmt2"};
    upPrepare.setQuery(tmp::mysql::helper::update_by_pkey_sql(TEST_TABLE, TEST_PK, "company_id", "email", "password", "name"));
    
    std::vector<std::string> up_syntax = upPrepare
        .set("C3_3333")
        .set("jack@loki.org")
        .set("jack2222")
        .set("JACK")
        .set(std::stoi(current_id))
        .build();

    ASSERT_NO_THROW(exec.cudProc_v1(std::move(up_syntax)));

    // 更新後の再確認
    std::vector<Data> r2 = exec.findProc_v1({"SELECT name FROM " + TEST_TABLE + " WHERE " + TEST_PK + " = " + current_id + ";"});
    ASSERT_FALSE(r2.empty());
    EXPECT_EQ(r2[0].at("name"), "JACK");

    // 4. Delete
    SimplePrepare delPrepare{"stmt3"};
    delPrepare.setQuery(tmp::mysql::helper::delete_by_pkey_sql(TEST_TABLE, TEST_PK));
    
    std::vector<std::string> del_syntax = delPrepare
        .set(std::stoi(current_id))
        .build();

    ASSERT_NO_THROW(exec.cudProc_v1(std::move(del_syntax)));

    // 削除確認
    std::vector<Data> r3 = exec.findProc_v1({"SELECT * FROM " + TEST_TABLE + " WHERE " + TEST_PK + " = " + current_id + ";"});
    EXPECT_TRUE(r3.empty()) << "データが削除されていません。ID: " << current_id;
}

// --- main関数 ---
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
