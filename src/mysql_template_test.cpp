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
 * g++ -O3 -std=c++20 -DNDEBUG -pedantic-errors -Wall -Wextra -Werror mysql_template_test.cpp -o ../bin/mysql_template_test_runner -I../inc/ -I/usr/include/mysql-cppconn/ -I/usr/local/include -L/usr/lib/x86_64-linux-gnu/ -L/usr/local/lib -lmysqlcppconn -lmysqlcppconnx -lgtest -lgtest_main -lpthread
 */
#include "gtest/gtest.h"
#include <mysqlx/xdevapi.h> // X Dev API のヘッダー
// あなたの環境に合わせて正しいパスを指定してください
#include "mysql_template.cpp" 

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

// フィクスチャでセッションを管理する
class MySqlRepositoryTest : public ::testing::Test
{
protected:
    void SetUp() override {
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
        // isOpen() メンバは存在しないためチェックを削除
        if (sess) { 
            sess->rollback(); // テストデータはDBに残さない
        }
    }

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
    // sess.get() で mysqlx::Session* のポインタを渡す
    tmp::mysql::r3::VarNodeRepository<unsigned long> repo(
        sess.get(), TEST_SCHEMA, TEST_TABLE, TEST_PK // 4引数で呼び出す
    );

    // 2. 挿入する VarNode データを準備
    tmp::VarNode data_to_insert = create_test_var_node(
        "B3_TEST", "test@example.com", "testpass", "Test User"
    );

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
    // sess.get() で mysqlx::Session* のポインタを渡す
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
    // sess.get() で mysqlx::Session* のポインタを渡す
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

// --- main関数 ---
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
