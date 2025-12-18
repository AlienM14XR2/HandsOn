#include <gtest/gtest.h>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <memory>
#include <optional>

// 既存のヘッダー（パスは CMake 設定に準拠）
#include "mongo_template.hpp" 

class VarNodeRepositoryTest : public ::testing::Test {
protected:
    // テストスイート実行前に一度だけドライバを初期化
    static void SetUpTestSuite() {
        static mongocxx::instance instance{};
    }

    void SetUp() override {
        // 各テスト実行前の共通準備（必要に応じて）
    }

    const std::string uri_str = "mongodb://user:pass@localhost:27017";
    const std::string db_name = "USER";
    const std::string coll_name = "contractor";
};

/**
 * @brief Insert 後にその ID で FindById する一連の結合テスト
 */
TEST_F(VarNodeRepositoryTest, InsertAndFindByIdIntegration) {
    using Data = tmp::VarNode;
    using Interface = tmp::Repository<std::string, Data>;
    using Subclazz = tmp::mongo::r1::VarNodeRepository;

    // 1. セットアップ
    mongocxx::uri uri(uri_str);
    mongocxx::client client(uri);
    std::unique_ptr<Interface> repo = std::make_unique<Subclazz>(&client, db_name, coll_name);

    std::string inserted_id;
    const std::string test_name = "Joe Dan";

    try {
        // --- Part 1: Insert ---
        std::clock_t start_1 = std::clock();
        
        Data root{"primaryKey", std::string("id")};
        root.addChild("company_id", std::string("C6_6000"));
        root.addChild("email",      std::string("joe@abcd.org"));
        root.addChild("password",   std::string("joe1111"));
        root.addChild("name",       test_name);
        root.addChild("roles",      std::string("Admin"));

        // insert 実行（移動セマンティクスを適用）
        inserted_id = repo->insert(std::move(root));
        
        std::clock_t end = std::clock();

        // 挿入成功の検証
        ASSERT_FALSE(inserted_id.empty()) << "Insert failed: returned ID is empty";
        
        // パフォーマンス記録
        double elapsed = static_cast<double>(end - start_1) / CLOCKS_PER_SEC;
        std::cout << "[          ] Inserted ID: " << inserted_id << " (Time: " << elapsed << " sec)" << std::endl;

        // --- Part 2: FindById ---
        std::optional<Data> ret = repo->findById(inserted_id);

        // 検索成功の検証
        ASSERT_TRUE(ret.has_value()) << "Could not find document that was just inserted. ID: " << inserted_id;

        // データ内容のダンプ（実績ある関数 tmp::debug_print_varnode を使用）
        tmp::debug_print_varnode(&(ret.value()));

        // データ整合性の検証（例: 名前が一致するか）
        const tmp::VarNode* name_node = ret->getChild("name");
        ASSERT_NE(name_node, nullptr);
        EXPECT_EQ(std::get<std::string>(name_node->data), test_name);

    } catch (const std::exception& e) {
        FAIL() << "Unexpected exception during Insert-Find integration: " << e.what();
    }
}
/**
 * @brief Insert -> Find -> Update -> Find の一連のライフサイクルと整合性を検証する
 */
TEST_F(VarNodeRepositoryTest, FullLifecycleIntegrationTest) {
    using Data = tmp::VarNode;
    using Interface = tmp::Repository<std::string, Data>;
    using Subclazz = tmp::mongo::r1::VarNodeRepository;

    // 1. セットアップ
    mongocxx::uri uri(uri_str);
    mongocxx::client client(uri);
    std::unique_ptr<Interface> repo = std::make_unique<Subclazz>(&client, db_name, coll_name);

    std::string inserted_id;
    const std::string initial_name = "Joe Dan";
    const std::string updated_name = "John Dan";
    const std::string updated_roles = "User,Operator";

    try {
        // --- Part 1: Insert ---
        Data root{"primaryKey", std::string("id")};
        root.addChild("company_id", std::string("C6_6000"));
        root.addChild("email",      std::string("joe@abcd.org"));
        root.addChild("password",   std::string("joe1111"));
        root.addChild("name",       initial_name);
        root.addChild("roles",      std::string("Admin"));

        inserted_id = repo->insert(std::move(root));
        ASSERT_FALSE(inserted_id.empty()) << "Insert failed";
        std::cout << "[          ] Inserted ID: " << inserted_id << std::endl;

        // --- Part 2: Find (Before Update) ---
        std::optional<Data> res1 = repo->findById(inserted_id);
        ASSERT_TRUE(res1.has_value());
        EXPECT_EQ(std::get<std::string>(res1->getChild("name")->data), initial_name);

        // --- Part 3: Update ---
        // 提供された test_update_one_r1 のロジックを適用
        Data update_root{"primaryKey", std::string("id")};
        update_root.addChild("company_id", std::string("C6_6333"));
        update_root.addChild("email",      std::string("john@abcd.org"));
        update_root.addChild("password",   std::string("john1111"));
        update_root.addChild("name",       updated_name);
        update_root.addChild("roles",      updated_roles);

        repo->update(inserted_id, std::move(update_root));
        std::cout << "[          ] Update executed for ID: " << inserted_id << std::endl;

        // --- Part 4: Find (After Update) ---
        // 最重要検証ポイント
        std::optional<Data> res2 = repo->findById(inserted_id);
        ASSERT_TRUE(res2.has_value()) << "Failed to find document after update";

        // 実績ある関数でダンプ出力
        tmp::debug_print_varnode(&(res2.value()));

        // 更新結果の整合性検証
        const tmp::VarNode* name_node = res2->getChild("name");
        const tmp::VarNode* roles_node = res2->getChild("roles");
        const tmp::VarNode* email_node = res2->getChild("email");

        ASSERT_NE(name_node, nullptr);
        ASSERT_NE(roles_node, nullptr);
        
        EXPECT_EQ(std::get<std::string>(name_node->data), updated_name);
        EXPECT_EQ(std::get<std::string>(roles_node->data), updated_roles);
        EXPECT_EQ(std::get<std::string>(email_node->data), std::string("john@abcd.org"));

    } catch (const std::exception& e) {
        FAIL() << "Unexpected exception during FullLifecycle test: " << e.what();
    }
}
/**
 * @brief Insert -> Find -> Delete -> Find (不在確認) の最短ライフサイクルを検証する
 */
TEST_F(VarNodeRepositoryTest, InsertFindDeleteLifecycleTest) {
    using Data = tmp::VarNode;
    using Interface = tmp::Repository<std::string, Data>;
    using Subclazz = tmp::mongo::r1::VarNodeRepository;

    // 1. セットアップ
    mongocxx::uri uri(uri_str);
    mongocxx::client client(uri);
    std::unique_ptr<Interface> repo = std::make_unique<Subclazz>(&client, db_name, coll_name);

    std::string inserted_id;
    const std::string test_name = "Joe Dan";

    try {
        // --- Part 1: Insert ---
        Data root{"primaryKey", std::string("id")};
        root.addChild("name", test_name);
        // 必要最小限の構成で挿入
        inserted_id = repo->insert(std::move(root));
        ASSERT_FALSE(inserted_id.empty()) << "Insert failed";
        std::cout << "[          ] Inserted ID: " << inserted_id << std::endl;

        // --- Part 2: Find (存在確認) ---
        std::optional<Data> res1 = repo->findById(inserted_id);
        ASSERT_TRUE(res1.has_value()) << "Failed to find document after insert";
        EXPECT_EQ(std::get<std::string>(res1->getChild("name")->data), test_name);

        // --- Part 3: Delete (Remove) ---
        // 提供された test_delete_one_r1 のロジックを反映
        repo->remove(inserted_id);
        std::cout << "[          ] Remove executed for ID: " << inserted_id << std::endl;

        // --- Part 4: Find (不在確認) ---
        // 削除後に検索を行い、nullopt であることを確認する
        std::optional<Data> res2 = repo->findById(inserted_id);
        
        // 検証: 結果が空であること
        EXPECT_FALSE(res2.has_value()) << "Document still exists after deletion. ID: " << inserted_id;
        
        if (!res2.has_value()) {
            std::cout << "[          ] Success: Document confirmed as deleted." << std::endl;
        }

    } catch (const std::exception& e) {
        FAIL() << "Unexpected exception during Lifecycle test: " << e.what();
    }
}
