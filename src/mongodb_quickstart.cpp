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
 * e.g.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc -I/usr/local/include/bson-2.1.2/ mongodb_quickstart.cpp $(pkg-config --cflags --libs libmongocxx) -lbson2 -o ../bin/mongodb_quickstart
 */

#include <cstdint>
#include <iostream>
#include <vector>
#include <cassert>
#include <map>

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

template <class... Args>
void print_debug_v3(Args&&... args)
{
    std::cout << "Debug v3: ";
    auto print_element = [](const auto& element) {
        std::cout << element << '\t';
    };
    // C++17以降の pack expansion で要素を順に処理
    (print_element(std::forward<Args>(args)), ...);
    std::cout << std::endl;
}

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

namespace tmp::mongo {

class ContractorRepository final : public tmp::Repository<std::string, std::map<std::string, std::string>> {
    using Data = std::map<std::string, std::string>;
private:
    mongocxx::client* const client;
public:
    ContractorRepository(mongocxx::client* const _client): client{_client}
    {}
    virtual std::string insert(Data&& data) const override
    {
        print_debug_v3("insert ... ", typeid(*this).name());
        auto db = (*client)["USER"];
        auto collection = db["contractor"];
        // BSON データ形式の登録
        auto doc_builder = bsoncxx::builder::basic::document{};
        doc_builder.append(kvp("company_id", data.at("company_id"))
                            ,kvp("email", data.at("email"))
                            ,kvp("password", data.at("password"))
                            ,kvp("name", data.at("name"))
                            ,kvp("roles", data.at("roles")));
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
    virtual void update(const std::string& id, Data&& data) const override
    {
        print_debug_v3("update ... ", typeid(*this).name());
        auto db = (*client)["USER"];
        auto collection = db["contractor"];

        bsoncxx::oid target_oid(id);
        bsoncxx::types::b_oid b_oid{target_oid};
        auto query_filter = make_document(kvp("_id", b_oid));
        auto update_doc = make_document(kvp("$set", make_document(
            kvp("company_id", data.at("company_id"))
            ,kvp("email", data.at("email"))
            ,kvp("password", data.at("password"))
            ,kvp("name", data.at("name"))
            ,kvp("roles", data.at("roles"))
        )));
        auto result = collection.update_one(query_filter.view(), update_doc.view());
        // 結果の利用法は不明（出力方法がわからない）。
        if (result) {
            std::cout << "Update succeed" << std::endl;
        } else {
            std::cout << "No result found" << std::endl;
        }
    }
    virtual void remove(const std::string& id) const override
    {
        print_debug_v3("remove ... ", typeid(*this).name());
        auto db = (*client)["USER"];
        auto collection = db["contractor"];

        bsoncxx::oid target_oid(id);
        bsoncxx::types::b_oid b_oid{target_oid};
        auto result = collection.delete_one(make_document(kvp("_id", b_oid)));
        if (result) {
            std::cout << "Delete succeed" << std::endl;
        } else {
            std::cout << "No result found" << std::endl;
        }
    }
    virtual std::optional<Data> findById(const std::string& id) const override
    {
        print_debug_v3("findById ... ", typeid(*this).name());
        auto db = (*client)["USER"];
        auto collection = db["contractor"];

        bsoncxx::oid target_oid(id);
        bsoncxx::types::b_oid b_oid{target_oid};
        auto result = collection.find_one(make_document(kvp("_id", b_oid)));
        if (result) {
            std::cout << bsoncxx::to_json(*result) << std::endl;
            std::map<std::string, std::string> data;
            for(bsoncxx::v_noabi::document::element e: *result) {
                // print_debug_v3("type ... ", typeid(e.type()).name());
                std::cout << e.key() << '\t';
                switch (e.type()) {
                    case bsoncxx::type::k_string:
                        data.insert(std::make_pair(e.key(), e.get_string().value));
                        break;
                    default:
                        break;
                }
            }
            std::cout << std::endl;
            data.insert(std::make_pair("_id", id));
            return data;
        } else {
            std::cout << "No result found" << std::endl;
        }
        return std::nullopt;
    }
};

}   // namespace tmp::mongo

int test_insert_one_v2(std::string* sid)
{
    puts("------ test_insert_one_v2");
    try {
        using interface = tmp::Repository<std::string, std::map<std::string, std::string>>;
        using subclazz = tmp::mongo::ContractorRepository;
        std::clock_t start_1 = clock();
        mongocxx::uri uri("mongodb://user:pass@localhost:27017");
        std::clock_t start_2 = clock();
        mongocxx::client client(uri);
        std::unique_ptr<interface> repo = std::make_unique<subclazz>(&client);

        std::map<std::string, std::string> data;
        data.insert(std::make_pair("company_id", "C6_6000"));
        data.insert(std::make_pair("email", "joe@abcd.com"));
        data.insert(std::make_pair("password", "joe1111"));
        data.insert(std::make_pair("name", "Joe Dan"));
        data.insert(std::make_pair("roles", "Admin"));
        *sid = repo->insert(std::move(data));
        std::clock_t end = clock();
        print_debug_v3("id: ", *sid);
        std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

int test_update_one_v2(std::string* sid)
{
    puts("------ test_update_one_v2");
    try {
        using interface = tmp::Repository<std::string, std::map<std::string, std::string>>;
        using subclazz = tmp::mongo::ContractorRepository;
        mongocxx::uri uri("mongodb://user:pass@localhost:27017");
        mongocxx::client client(uri);
        std::unique_ptr<interface> repo = std::make_unique<subclazz>(&client);

        std::map<std::string, std::string> data;
        data.insert(std::make_pair("company_id", "C6_6000"));
        data.insert(std::make_pair("email", "john@abcd.com"));
        data.insert(std::make_pair("password", "john1111"));
        data.insert(std::make_pair("name", "john Dan"));
        data.insert(std::make_pair("roles", "User,Operator"));
        repo->update(*sid, std::move(data));
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

int test_find_v2(std::string* sid)
{
    puts("------ test_find_v2");
    try {
        using interface = tmp::Repository<std::string, std::map<std::string, std::string>>;
        using subclazz = tmp::mongo::ContractorRepository;
        mongocxx::uri uri("mongodb://user:pass@localhost:27017");
        mongocxx::client client(uri);
        std::unique_ptr<interface> repo = std::make_unique<subclazz>(&client);

        std::optional<std::map<std::string, std::string>> ret = repo->findById(*sid);
        if(ret) {
            for(auto f: ret.value()) {
                std::cout << f.first << '\t' << f.second << std::endl;
            }
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

int test_delete_one_v2(std::string* sid)
{
    puts("------ test_delete_one_v2");
    try {
        using interface = tmp::Repository<std::string, std::map<std::string, std::string>>;
        using subclazz = tmp::mongo::ContractorRepository;
        mongocxx::uri uri("mongodb://user:pass@localhost:27017");
        mongocxx::client client(uri);
        std::unique_ptr<interface> repo = std::make_unique<subclazz>(&client);
        repo->remove(*sid);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

int main() {
    puts("START main ===");
    int ret = -1;
    std::clock_t start_1 = clock();
    mongocxx::instance instance;
    std::clock_t end = clock();
    std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
    if(1) {
        printf("Play and Result ... %d\n", ret = test_find());
        assert(ret == 0);
    }
    if(0) {
        printf("Play and Result ... %d\n", ret = test_insert_one());
        assert(ret == 0);
        printf("Play and Result ... %d\n", ret = test_update_one());
        assert(ret == 0);
        printf("Play and Result ... %d\n", ret = test_delete_one());
        assert(ret == 0);
    }
    if(1) {
        std::string sid{""};
        printf("Play and Result ... %d\n", ret = test_insert_one_v2(&sid));
        assert(ret == 0);
        printf("Play and Result ... %d\n", ret = test_update_one_v2(&sid));
        assert(ret == 0);
        printf("Play and Result ... %d\n", ret = test_find_v2(&sid));
        assert(ret == 0);
        printf("Play and Result ... %d\n", ret = test_delete_one_v2(&sid));
        assert(ret == 0);
    }
    puts("=== main END");
    return EXIT_SUCCESS;
}