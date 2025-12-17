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
// あなたの環境に合わせて正しいパスを指定してください
// #include "pqxx_template.cpp"    // AI が最初に指定した方法、本来はヘッダファイルであるべきもの。
#include <pqxx_template.hpp>

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

// --- main関数は不要 ---
// libgtest_main.a をリンクすることで main 関数が提供されます
