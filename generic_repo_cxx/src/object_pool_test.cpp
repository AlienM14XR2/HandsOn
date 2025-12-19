#include <gtest/gtest.h>
#include <ObjectPool.hpp>

// テスト用の軽量ダミークラス
struct Dummy {
    int id;
    explicit Dummy(int i) : id(i) {}
};

/**
 * 特記事項
 * ObjectPool のプールを実現しているメンバ変数は std::queue<std::unique_ptr<T>>
 * 
 */
class ObjectPoolTest : public ::testing::Test {
protected:
    // ヘルパー：初期状態で3つオブジェクトを入れたプールを作成
    std::shared_ptr<ObjectPool<Dummy>> create_filled_pool() {
        auto pool = ObjectPool<Dummy>::create("test_credit");
        pool->push(std::make_unique<Dummy>(1));
        pool->push(std::make_unique<Dummy>(2));
        pool->push(std::make_unique<Dummy>(3));
        return pool;
    }
};

// 1. 正常系：基本サイクル (push / pop / 自動返却)
TEST_F(ObjectPoolTest, BasicPushPopCycle) {
    auto pool = create_filled_pool();

    {
        auto obj = pool->pop();
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(obj->id, 1);  // first-in-first-out を理解、そのため 1 と判定している。
        // この時点でプール内は 2, 3
    } 
    // スコープを抜けて obj がプールに戻る

    // 戻った 1 を再度取得できるか
    auto obj1 = pool->pop(); // 2
    auto obj2 = pool->pop(); // 3
    auto obj3 = pool->pop(); // 1 (戻ったもの)
    
    EXPECT_EQ(obj3->id, 1);     // queue を正しく理解し、返却の確認を行った。
}

// 2. 正常系：リソース枯渇 (nullptr 返却)
TEST_F(ObjectPoolTest, ReturnsNullptrWhenEmpty) {
    // 次はObjectPoolを作っているが、push していない。
    auto pool = ObjectPool<Dummy>::create("empty_pool");

    auto obj = pool->pop();
    EXPECT_EQ(obj, nullptr) << "空のプールは nullptr を返すべき";
}

// 3. 例外系・興味：プール本体が先に破棄された場合の挙動
TEST_F(ObjectPoolTest, SafeDestructionWhenPoolDiesFirst) {
    // 宣言のみのインスタンス化
    ObjectPool<Dummy>::Ptr obj;
    
    {
        auto pool = ObjectPool<Dummy>::create("short_lived_pool");
        pool->push(std::make_unique<Dummy>(99));
        obj = pool->pop();
        // ここで pool がスコープを抜け破棄される
    }

    // pool は消滅しているが、obj を解体してもクラッシュしないかを確認
    // weak_ptr::lock() が失敗し、通常の delete が行われるはず
    EXPECT_NO_THROW({
        obj.reset(); 
    });
}
