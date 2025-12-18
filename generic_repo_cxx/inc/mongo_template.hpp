/**
 * mongodb_quickstart.cpp
 * 
 * MongoDB C++ ドライバのインストール
 * ```
 * curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r4.1.4/mongo-cxx-driver-r4.1.4.tar.gz
 * tar -xzf mongo-cxx-driver-r4.1.4.tar.gz
 * cd mongo-cxx-driver-r4.1.4/build
 * 
 * cmake ..                                \
 *     -DCMAKE_BUILD_TYPE=Release          \
 *     -DCMAKE_CXX_STANDARD=17
 * 
 * cmake --build .
 * sudo cmake --build . --target install
 * ```
 * 
 * ユーザをdocker グループにする（必要なら）
 * ```
 * echo $USER
 * sudo usermod -aG docker $USER
 * cat /etc/group
 * ```
 * 
 * Docker にMongoDB を配置する
 * ```
 * docker pull mongodb/mongodb-community-server:latest
 * docker run --name mongodb -d -p 27017:27017 \
 * -e MONGODB_INITDB_ROOT_USERNAME=user \
 * -e MONGODB_INITDB_ROOT_PASSWORD=pass \
 * -v $(pwd)/mongo-data:/data/db \
 * mongodb/mongodb-community-server:latest
 * ```
 * 
 * 個人的な事柄、備忘録として、'-v' パラメータが原因でコンテナ起動に失敗した場合
 * 次のコマンドを打て。
 * ```
 * kvm-ok
 * // 正常動作していたら、次のようになる。
 * INFO: /dev/kvm exists
 * KVM acceleration can be used
 * ```
 * 
 * mongosh のインストールは次のURL を参照
 * ```
 * https://www.mongodb.com/ja-jp/docs/mongodb-shell/install/?operating-system=linux&linux-distribution=tgz
 * 
 * // コンテナ上のMongoDB にログインする
 * mongosh -u user -p pass
 * ```
 * 
 * 事前に次のコマンドをMongoDB にて実行する。
 * ```
 * use USER
 * db.createCollection('Company')
 * db.Company.insertOne( { name:'ABC.com', address:'Tokyo Japan' } )
 * db.Company.findOne( { name:'ABC.com'} )
 * db.createCollection('contractor')
 * ```
 * 
 * MongoDB コマンド
 * ```
 * help
 * show dbs
 * show collections
 * cls
 * db.Company.find()
 * db.Company.find({name:'Foo.com'})
 * db.Company.deleteMany({name:'Foo.com'})
 * db.collection.updateOne({ name: "Buzz" },{ $set: { name: "Buuuuuzzzz" } })
 * db.collection.updateOne({ name: "Buuuuuzzzz" },{ $set: {name:"Buzz",address:"Osaka Japan"} })
 * ```
 * 
 * 必要なら、ライブラリパスを環境変数に指定する。
 * ```
 * export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
 * ```
 * 
 * e.g. compile
 * 非常に厳格。
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Wextra -Werror -I../inc -I/usr/local/include/bson-2.1.2/ mongodb_quickstart.cpp $(pkg-config --cflags --libs libmongocxx) -lbson2 -o ../bin/mongodb_quickstart
 * 実験、サンプリングでは以下を推奨（-Wextra があると簡単に関数内をすべてコメントとかできないから。
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc -I/usr/local/include/bson-2.1.2/ mongodb_quickstart.cpp $(pkg-config --cflags --libs libmongocxx) -lbson2 -o ../bin/mongodb_quickstart
 */
#include <iostream>
#include <cstdint>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <algorithm> // for find_if, if you keep using vector
#include <cxxabi.h>

#include <Repository.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bson/bson.h>
#include <bsoncxx/builder/stream/document.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

int test_find()
{
    puts("------ test_find");
    try {
        // mongocxx::instance instance;     // 処理を分割した場合は、メインで一度コールすればよい（複数回コールすると実行時エラー：）。
        mongocxx::uri uri("mongodb://user:pass@localhost:27017");
        mongocxx::client client(uri);
        auto db = client["USER"];
        auto collection = db["Company"];
        auto result = collection.find_one(make_document(kvp("name", "ABC.com")));
        if (result) {
            std::cout << bsoncxx::to_json(*result) << std::endl;
        } else {
            std::cout << "No result found" << std::endl;
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

int test_insert_one()
{
    puts("------ test_insert_one");
    try {
        std::clock_t start_1 = clock();
        mongocxx::uri uri("mongodb://user:pass@localhost:27017");
        std::clock_t start_2 = clock();
        mongocxx::client client(uri);
        auto db = client["USER"];
        auto collection = db["Company"];
        auto result = collection.insert_one(make_document(kvp("name", "Foo.com"), kvp("address", "Tokyo Japan")));
        std::clock_t end = clock();
        if (result) {
            bsoncxx::v_noabi::types::bson_value::view id = result->inserted_id();
            // if (id.type() == bsoncxx::types::b_oid::types) {
                std::cout << "_id ... ObjectId is " << id.get_oid().value.to_string() << std::endl;
            // } else {
            //     std::cout << "Inserted document ID type: " << static_cast<int>(id.type()) << std::endl;
            // }
        } else {
            std::cout << "No result found" << std::endl;
        }
        std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
        // BSON データ形式
        puts("insert ... BSON Data Type sample.");
        auto doc_builder = bsoncxx::builder::basic::document{};
        doc_builder.append(kvp("name", "XYZ.com")
                            ,kvp("address", "Fukuoka Japan")
                            ,kvp("comment", "Book store"));
        bsoncxx::document::value doc_value{doc_builder.extract()};
        auto result2 = collection.insert_one(std::forward<bsoncxx::document::value>(doc_value));
        if (result2) {
            bsoncxx::v_noabi::types::bson_value::view id = result2->inserted_id();
            std::cout << "_id ... ObjectId is " << id.get_oid().value.to_string() << std::endl;
            auto ret = collection.find_one(make_document(kvp("_id", id)));
            if (ret) std::cout << "find by ObjectId ... \n" << bsoncxx::to_json(*ret) << std::endl;
            puts("find by oid to_string() ...");
            bsoncxx::oid target_oid(std::string(id.get_oid().value.to_string()));
            bsoncxx::types::b_oid b_oid{target_oid};
            auto filter = bsoncxx::builder::stream::document{}
                << "_id" << b_oid
                << bsoncxx::builder::stream::finalize;
            auto cursor = collection.find(filter.view());
            for (auto&& doc : cursor) {
                puts("... Hit");
                std::cout << bsoncxx::to_json(doc) << std::endl;
            }
            // bson_oid_t oid;
            // bson_oid_init_from_string (&oid, "012345678901234567890123");
            // char str[25]{0};
            // bson_oid_to_string(&oid, str);
            // std::cout << "oid is " << str << std::endl;
        } else {
            std::cout << "No result found" << std::endl;
        }

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

int test_update_one()
{
    puts("------ test_update_one");
    try {
        mongocxx::uri uri("mongodb://user:pass@localhost:27017");
        mongocxx::client client(uri);
        auto db = client["USER"];
        auto collection = db["Company"];

        auto query_filter = make_document(kvp("name", "Foo.com"));
        auto update_doc = make_document(kvp("$set", make_document(kvp("name", "Bar.com"), kvp("address", "Osaka Japan"))));
        auto result = collection.update_one(query_filter.view(), update_doc.view());
        // 結果の利用法は不明（出力方法がわからない）。
        if (result) {
            std::cout << "Update succeed" << std::endl;
        } else {
            std::cout << "No result found" << std::endl;
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

int test_delete_one()
{
    puts("------ test_delete_one");
    try {
        mongocxx::uri uri("mongodb://user:pass@localhost:27017");
        mongocxx::client client(uri);
        auto db = client["USER"];
        auto collection = db["Company"];

        auto result = collection.delete_one(make_document(kvp("name", "Bar.com")));
        if (result) {
            std::cout << "Delete succeed" << std::endl;
        } else {
            std::cout << "No result found" << std::endl;
        }
        auto result2 = collection.delete_one(make_document(kvp("name", "XYZ.com")));
        if (result2) {
            std::cout << "Delete succeed" << std::endl;
        } else {
            std::cout << "No result found" << std::endl;
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}


namespace tmp::mongo::r1 {
/**
 * VarNodeRepository r1
 * 
 */
class VarNodeRepository final : public tmp::Repository<std::string, tmp::VarNode>
{
    using Data = tmp::VarNode;
private:
    mongocxx::client* const client;
    const std::string dbName;
    const std::string collectionName;
    const std::string primaryKeyName;
    public:
    VarNodeRepository(mongocxx::client* const _client, const std::string& _dbName, const std::string& _collectionName, const std::string& _primaryKeyName="_id")
        : client{_client}, dbName{_dbName}, collectionName{_collectionName}, primaryKeyName{_primaryKeyName}
    {}
    virtual std::string insert(Data&& data) override
    {
        int status;
        print_debug("insert ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        // DONE ... db "USER"のハードコードこれは、コンストラクタの引数にする。
        auto db = (*client)[dbName];
        // DONE ... collection "contractor"のハードコードこれは、コンストラクタの引数にする。
        auto collection = db[collectionName];
        // BSON データ形式の登録
        auto doc_builder = bsoncxx::builder::basic::document{};
        for (const auto& child : data.children) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::monostate>) {
                    // 1. std::monostate の場合：BSON null として登録
                    doc_builder.append(bsoncxx::builder::basic::kvp(child->key, bsoncxx::types::b_null{}));
                } else if constexpr (std::is_same_v<T, uint64_t>) {
                    // 2. uint64_t の場合：int64_t にキャストして型を固定（BSON規格に合わせる）
                    doc_builder.append(bsoncxx::builder::basic::kvp(child->key, static_cast<int64_t>(arg)));
                } else {
                    // 3. その他の型 (int64_t, float, double, bool, std::string)
                    // これらは bsoncxx が標準でオーバーロードを持っているため直接 append 可能
                    doc_builder.append(bsoncxx::builder::basic::kvp(child->key, arg));
                }
            }, child->data);
        }
        bsoncxx::document::value doc_value{doc_builder.extract()};
        auto result = collection.insert_one(std::forward<bsoncxx::document::value>(doc_value));
        if (result) {
            bsoncxx::v_noabi::types::bson_value::view id = result->inserted_id();
            // std::cout << "_id ... ObjectId is " << id.get_oid().value.to_string() << std::endl;
            return id.get_oid().value.to_string();
        } else {
            std::cout << "No result found" << std::endl;
        }
        return std::string{""};
    }
    virtual void update(const std::string& id, Data&& data) override
    {
        int status;
        print_debug("update ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        // DONE ... db "USER"のハードコードこれは、コンストラクタの引数にする。
        auto db = (*client)[dbName];
        // DONE ... collection "contractor"のハードコードこれは、コンストラクタの引数にする。
        auto collection = db[collectionName];

        bsoncxx::oid target_oid(id);
        bsoncxx::types::b_oid b_oid{target_oid};
        // DONE ... この"_id"は、他のリポジトリの primaryKeyName に相当する。したがって、コンストラクタ引数が妥当。
        auto query_filter = make_document(kvp(primaryKeyName, b_oid));
        // 1. ルートドキュメントのビルダー
        auto doc_builder = bsoncxx::builder::basic::document{};
        // 2. $set セクションをサブドキュメントとして構築
        doc_builder.append(bsoncxx::builder::basic::kvp("$set", [&](bsoncxx::builder::basic::sub_document sub_doc) {
            
            // Insert と同様のループ処理
            for (const auto& child : data.children) {
                // シグネチャの意図に基づき、VarNode内のID要素をスキップする
                if (child->key == "_id") continue;
                std::visit([&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::monostate>) {
                        sub_doc.append(bsoncxx::builder::basic::kvp(child->key, bsoncxx::types::b_null{}));
                    } else if constexpr (std::is_same_v<T, uint64_t>) {
                        sub_doc.append(bsoncxx::builder::basic::kvp(child->key, static_cast<int64_t>(arg)));
                    } else {
                        // その他の型をサブドキュメントに追加
                        sub_doc.append(bsoncxx::builder::basic::kvp(child->key, arg));
                    }
                }, child->data);
            }
        }));
        // 3. 更新の実行
        auto result = collection.update_one(query_filter.view(), doc_builder.view());
        // 結果の利用法は不明（出力方法がわからない）。
        if (result) {
            std::cout << "Update succeed" << std::endl;
        } else {
            std::cout << "No result found" << std::endl;
        }
    }
    virtual void remove(const std::string& id) override
    {
        int status;
        print_debug("remove ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        // DONE ... db "USER"のハードコードこれは、コンストラクタの引数にする。
        auto db = (*client)[dbName];
        // DONE ... collection "contractor"のハードコードこれは、コンストラクタの引数にする。
        auto collection = db[collectionName];

        bsoncxx::oid target_oid(id);
        bsoncxx::types::b_oid b_oid{target_oid};
        auto result = collection.delete_one(make_document(kvp(primaryKeyName, b_oid)));
        if (result) {
            std::cout << "Delete succeed" << std::endl;
        } else {
            std::cout << "No result found" << std::endl;
        }
    }
    virtual std::optional<Data> findById(const std::string& id) const override
    {
        int status;
        print_debug("find ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        // DONE ... db "USER"のハードコードこれは、コンストラクタの引数にする。
        auto db = (*client)[dbName];
        // DONE ... collection "contractor"のハードコードこれは、コンストラクタの引数にする。
        auto collection = db[collectionName];
        bsoncxx::oid target_oid(id);
        bsoncxx::types::b_oid b_oid{target_oid};
        auto result = collection.find_one(make_document(kvp(primaryKeyName, b_oid)));
        if (result) {
            std::cout << bsoncxx::to_json(*result) << std::endl;
            // std::map<std::string, std::string> data;     // 以前の戻値 Data 目安としてコメントアウト
            tmp::VarNode data("result_row_mongo", std::monostate{});   // ルートノードのkey は何でもいいが、MySQL と同じ... ハードコードで同じはよくないから、個別をすすめる、サフィックスを付けた。
            // VarNode data にid を詰める、現状の作りに準拠した。
            data.addChild(primaryKeyName, id);
            for(bsoncxx::v_noabi::document::element e: *result) {
                switch (e.type()) {
                    case bsoncxx::type::k_string:
                        // ValueType = std::string
                        data.addChild(std::string(e.key()), std::string(e.get_string().value));
                        break;
                    case bsoncxx::type::k_int64:
                        // ValueType = int64_t
                        data.addChild(std::string(e.key()), static_cast<int64_t>(e.get_int64().value));
                        break;
                    case bsoncxx::type::k_int32:
                        // ValueType = int64_t に格上げ
                        data.addChild(std::string(e.key()), static_cast<int64_t>(e.get_int32().value));
                        break;
                    case bsoncxx::type::k_bool:
                        // ValueType = bool
                        data.addChild(std::string(e.key()), e.get_bool().value);
                        break;
                    case bsoncxx::type::k_oid:
                        // OIDを文字列として VarNode に格納
                        data.addChild(std::string(e.key()), e.get_oid().value.to_string());
                        break;
                    default:
                        // サポート外の型は std::monostate もしくは無視
                        break;
                }
            }
            // std::cout << std::endl;  // bsoncxx::v_noabi::document::element e の型チェックの名残。
            return data;
        } else {
            std::cout << "No result found" << std::endl;
        }
        return std::nullopt;
    }
};  // ContractorRepository

}   // namespace tmp::mongo::r1


// int test_delete_one_r1(std::string* sid)
// {
//     puts("------ test_delete_one_r1");
//     try {
//         using Data = tmp::VarNode;
//         using interface = tmp::Repository<std::string, Data>;
//         using subclazz = tmp::mongo::r1::VarNodeRepository;
//         mongocxx::uri uri("mongodb://user:pass@localhost:27017");
//         mongocxx::client client(uri);
//         std::unique_ptr<interface> repo = std::make_unique<subclazz>(&client, "USER", "contractor");
//         repo->remove(*sid);
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
// }

// int test_update_one_r1(std::string* sid)
// {
//     puts("------ test_update_one_r1");
//     try {
//         using Data = tmp::VarNode;
//         using interface = tmp::Repository<std::string, Data>;
//         using subclazz = tmp::mongo::r1::VarNodeRepository;
//         mongocxx::uri uri("mongodb://user:pass@localhost:27017");
//         mongocxx::client client(uri);
//         std::unique_ptr<interface> repo = std::make_unique<subclazz>(&client, "USER", "contractor");

//         // std::map<std::string, std::string> data;
//         // data.insert(std::make_pair("company_id", "C6_6000"));
//         // data.insert(std::make_pair("email", "john@abcd.com"));
//         // data.insert(std::make_pair("password", "john1111"));
//         // data.insert(std::make_pair("name", "john Dan"));
//         // data.insert(std::make_pair("roles", "User,Operator"));
//         std::string pkcol{"id"};
//         Data root{"primaryKey", pkcol};
//         std::string companyId{"C6_6333"};
//         root.addChild("company_id", companyId);
//         std::string email{"john@abcd.org"};
//         root.addChild("email", email);
//         std::string password{"john1111"};
//         root.addChild("password", password);
//         std::string name{"John Dan"};
//         root.addChild("name", name);
//         std::string roles{"User,Operator"};
//         root.addChild("roles", roles);
//         repo->update(*sid, std::move(root));
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
// }

// int test_find_r1(std::string* sid)
// {
//     puts("------ test_find_r1");
//     try {
//         using Data = tmp::VarNode;
//         using interface = tmp::Repository<std::string, Data>;
//         using subclazz = tmp::mongo::r1::VarNodeRepository;
//         mongocxx::uri uri("mongodb://user:pass@localhost:27017");
//         mongocxx::client client(uri);
//         std::unique_ptr<interface> repo = std::make_unique<subclazz>(&client, "USER", "contractor");

//         std::optional<Data> ret = repo->findById(*sid);
//         if(ret) {
//             // for(auto f: ret.value()) {
//             //     std::cout << f.first << '\t' << f.second << std::endl;
//             // }
//             tmp::VarNode::debug(&(ret.value()));
//         }
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
// }

// int test_insert_one_r1(std::string* sid)
// {
//     puts("------ test_insert_one_r1");
//     try {
//         using Data = tmp::VarNode;
//         using interface = tmp::Repository<std::string, Data>;
//         using subclazz = tmp::mongo::r1::VarNodeRepository;
//         std::clock_t start_1 = clock();
//         mongocxx::uri uri("mongodb://user:pass@localhost:27017");
//         std::clock_t start_2 = clock();
//         mongocxx::client client(uri);
//         std::unique_ptr<interface> repo = std::make_unique<subclazz>(&client, "USER", "contractor");

//         // std::map<std::string, std::string> data;
//         // data.insert(std::make_pair("company_id", "C6_6000"));
//         // data.insert(std::make_pair("email", "joe@abcd.org"));
//         // data.insert(std::make_pair("password", "joe1111"));
//         // data.insert(std::make_pair("name", "Joe Dan"));
//         // data.insert(std::make_pair("roles", "Admin"));
//         std::string pkcol{"id"};
//         Data root{"primaryKey", pkcol};
//         std::string companyId{"C6_6000"};
//         root.addChild("company_id", companyId);
//         std::string email{"joe@abcd.org"};
//         root.addChild("email", email);
//         std::string password{"joe1111"};
//         root.addChild("password", password);
//         std::string name{"Joe Dan"};
//         root.addChild("name", name);
//         std::string roles{"Admin"};
//         root.addChild("roles", roles);
//         *sid = repo->insert(std::move(root));
//         std::clock_t end = clock();
//         print_debug_v3("id: ", *sid);
//         std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
//         std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
// }

// int test_insert_one_v2(std::string* sid)
// {
//     puts("------ test_insert_one_v2");
//     try {
//         using interface = tmp::Repository<std::string, std::map<std::string, std::string>>;
//         using subclazz = tmp::mongo::ContractorRepository;
//         std::clock_t start_1 = clock();
//         mongocxx::uri uri("mongodb://user:pass@localhost:27017");
//         std::clock_t start_2 = clock();
//         mongocxx::client client(uri);
//         std::unique_ptr<interface> repo = std::make_unique<subclazz>(&client);

//         std::map<std::string, std::string> data;
//         data.insert(std::make_pair("company_id", "C6_6000"));
//         data.insert(std::make_pair("email", "joe@abcd.com"));
//         data.insert(std::make_pair("password", "joe1111"));
//         data.insert(std::make_pair("name", "Joe Dan"));
//         data.insert(std::make_pair("roles", "Admin"));
//         *sid = repo->insert(std::move(data));
//         std::clock_t end = clock();
//         print_debug_v3("id: ", *sid);
//         std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
//         std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
// }

// int test_update_one_v2(std::string* sid)
// {
//     puts("------ test_update_one_v2");
//     try {
//         using interface = tmp::Repository<std::string, std::map<std::string, std::string>>;
//         using subclazz = tmp::mongo::ContractorRepository;
//         mongocxx::uri uri("mongodb://user:pass@localhost:27017");
//         mongocxx::client client(uri);
//         std::unique_ptr<interface> repo = std::make_unique<subclazz>(&client);

//         std::map<std::string, std::string> data;
//         data.insert(std::make_pair("company_id", "C6_6000"));
//         data.insert(std::make_pair("email", "john@abcd.com"));
//         data.insert(std::make_pair("password", "john1111"));
//         data.insert(std::make_pair("name", "john Dan"));
//         data.insert(std::make_pair("roles", "User,Operator"));
//         repo->update(*sid, std::move(data));
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
// }

// int test_find_v2(std::string* sid)
// {
//     puts("------ test_find_v2");
//     try {
//         using interface = tmp::Repository<std::string, std::map<std::string, std::string>>;
//         using subclazz = tmp::mongo::ContractorRepository;
//         mongocxx::uri uri("mongodb://user:pass@localhost:27017");
//         mongocxx::client client(uri);
//         std::unique_ptr<interface> repo = std::make_unique<subclazz>(&client);

//         std::optional<std::map<std::string, std::string>> ret = repo->findById(*sid);
//         if(ret) {
//             for(auto f: ret.value()) {
//                 std::cout << f.first << '\t' << f.second << std::endl;
//             }
//         }
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
// }

// int test_delete_one_v2(std::string* sid)
// {
//     puts("------ test_delete_one_v2");
//     try {
//         using interface = tmp::Repository<std::string, std::map<std::string, std::string>>;
//         using subclazz = tmp::mongo::ContractorRepository;
//         mongocxx::uri uri("mongodb://user:pass@localhost:27017");
//         mongocxx::client client(uri);
//         std::unique_ptr<interface> repo = std::make_unique<subclazz>(&client);
//         repo->remove(*sid);
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
// }


// int main() {
//     puts("START main ===");
//     int ret = -1;
//     std::clock_t start_1 = clock();
//     mongocxx::instance instance;
//     std::clock_t end = clock();
//     std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
//     if(0) {
//         printf("Play and Result ... %d\n", ret = test_find());
//         assert(ret == 0);
//     }
//     if(0) {
//         printf("Play and Result ... %d\n", ret = test_insert_one());
//         assert(ret == 0);
//         printf("Play and Result ... %d\n", ret = test_update_one());
//         assert(ret == 0);
//         printf("Play and Result ... %d\n", ret = test_delete_one());
//         assert(ret == 0);
//     }
//     if(0) {
//         std::string sid{""};
//         printf("Play and Result ... %d\n", ret = test_insert_one_v2(&sid));
//         assert(ret == 0);
//         printf("Play and Result ... %d\n", ret = test_update_one_v2(&sid));
//         assert(ret == 0);
//         printf("Play and Result ... %d\n", ret = test_find_v2(&sid));
//         assert(ret == 0);
//         printf("Play and Result ... %d\n", ret = test_delete_one_v2(&sid));
//         assert(ret == 0);
//     }
//     if(1) {
//         std::string sid{""};
//         printf("Play and Result ... %d\n", ret = test_insert_one_r1(&sid));
//         assert(ret == 0);
//         printf("Play and Result ... %d\n", ret = test_update_one_r1(&sid));
//         assert(ret == 0);
//         // TODO ユニットテストでも構わないから、存在しないKey（カラム）を含めた検索はやろうね。
//         printf("Play and Result ... %d\n", ret = test_find_r1(&sid));
//         assert(ret == 0);
//         printf("Play and Result ... %d\n", ret = test_delete_one_r1(&sid));
//         assert(ret == 0);
//     }
//     puts("=== main END");
//     return EXIT_SUCCESS;
// }
