/**
 * REST API クライアントを考える
 * 
 * 概要としては次の点に留意する。
 * - エンドポイント、REST API URI の設定ができる。
 * - REST に対応したリクエストが可能( GET, POST, PUT, DELETE )。
 * 
 * 
 * 次のような REST API のエンドポイントを用意した。
 * http://localhost:8080/api/
 * 
 * curl -i -s -X GET http://localhost:8080/api/get/hello?id=1000
 * curl -i -s -X POST -d 'post content.' http://localhost:8080/api/post/hello
 * curl -i -s -X PUT -d 'put content.' http://localhost:8080/api/put/hello
 * curl -i -s -X DELETE -d 'delete content.' http://localhost:8080/api/delete/hello
 * 
 * 次のライブラリが必要
 * sudo apt install libcurl4-openssl-dev
 * sudo apt install -y curl
 * sudo apt install nlohmann-json3-dev
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc -L/usr/lib/x86_64-linux-gnu/ rest_template.cpp -lcurl -o ../bin/rest_template
 */
#include <iostream>
#include <cassert>
#include <cstring>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

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

size_t curl_write_func(char* cp, size_t size, size_t nmemb, std::string* stream) 
{
    size_t realSize = size * nmemb;
    stream->append(cp, realSize);
    return realSize;
}

std::string curl_get(const char* url) 
{
  CURL*    curl;
  CURLcode res = CURLE_OK;
  curl = curl_easy_init();
  std::string chunk;
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    // サーバのSSL証明書の検証をしない
    // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
    curl_easy_setopt(curl, CURLOPT_PROXY, "");
    res = curl_easy_perform(curl);
    ptr_print_debug<const char*, const decltype(res)&>("curl res code is ", res);
    curl_easy_cleanup(curl);
  }
  if(res != CURLE_OK) {
    std::string errMsg;
    errMsg.append("curl error. CURLcode is ").append(std::to_string(res));
    throw std::runtime_error(errMsg);
  }
  return chunk;
}

std::string curl_post(const char* url, const char* postData) 
{
    CURL*    curl;
    CURLcode res;
    curl = curl_easy_init();
    std::string chunk;
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(postData));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        res = curl_easy_perform(curl);
        ptr_print_debug<const char*, const decltype(res)&>("curl res code is ", res);
        curl_easy_cleanup(curl);
    }
    if(res != CURLE_OK) {
        std::string errMsg;
        errMsg.append("curl error. CURLcode is ").append(std::to_string(res));
        throw std::runtime_error(errMsg);
    }
    return chunk;
}

std::string curl_put(const char* url, const char* postData) 
{
    CURL*    curl;
    CURLcode res;
    curl = curl_easy_init();
    std::string chunk;
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(postData));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        res = curl_easy_perform(curl);
        ptr_print_debug<const char*, const decltype(res)&>("curl res code is ", res);
        curl_easy_cleanup(curl);
    }
    if(res != CURLE_OK) {
        std::string errMsg;
        errMsg.append("curl error. CURLcode is ").append(std::to_string(res));
        throw std::runtime_error(errMsg);
    }
    return chunk;
}

std::string curl_delete(const char* url, const char* postData) 
{
    CURL*    curl;
    CURLcode res;
    curl = curl_easy_init();
    std::string chunk;
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(postData));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        res = curl_easy_perform(curl);
        ptr_print_debug<const char*, const decltype(res)&>("curl res code is ", res);
        curl_easy_cleanup(curl);
    }
    if(res != CURLE_OK) {
        std::string errMsg;
        errMsg.append("curl error. CURLcode is ").append(std::to_string(res));
        throw std::runtime_error(errMsg);
    }
    return chunk;
}

std::string curl_post_json(const char* url, const char* postData) 
{
    CURL*    curl;
    CURLcode res;
    curl = curl_easy_init();
    std::string chunk;
    struct curl_slist *slist = nullptr;
    if(curl) {
        slist = curl_slist_append(slist, "Content-Type: application/json;charset=UTF-8");
        slist = curl_slist_append(slist, "Accept: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(postData));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        res = curl_easy_perform(curl);
        ptr_print_debug<const char*, const decltype(res)&>("curl res code is ", res);
        curl_slist_free_all(slist);
        curl_easy_cleanup(curl);
    }
    if(res != CURLE_OK) {
        std::string errMsg;
        errMsg.append("curl error. CURLcode is ").append(std::to_string(res));
        throw std::runtime_error(errMsg);
    }
    return chunk;
}

std::string curl_put_json(const char* url, const char* postData) 
{
    CURL*    curl;
    CURLcode res;
    curl = curl_easy_init();
    std::string chunk;
    struct curl_slist *slist = nullptr;
    if(curl) {
        slist = curl_slist_append(slist, "Content-Type: application/json;charset=UTF-8");
        slist = curl_slist_append(slist, "Accept: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(postData));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        res = curl_easy_perform(curl);
        ptr_print_debug<const char*, const decltype(res)&>("curl res code is ", res);
        curl_slist_free_all(slist);
        curl_easy_cleanup(curl);
    }
    if(res != CURLE_OK) {
        std::string errMsg;
        errMsg.append("curl error. CURLcode is ").append(std::to_string(res));
        throw std::runtime_error(errMsg);
    }
    return chunk;
}

std::string curl_delete_json(const char* url, const char* postData) 
{
    CURL*    curl;
    CURLcode res;
    curl = curl_easy_init();
    std::string chunk;
    struct curl_slist *slist = nullptr;
    if(curl) {
        slist = curl_slist_append(slist, "Content-Type: application/json;charset=UTF-8");
        slist = curl_slist_append(slist, "Accept: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(postData));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        res = curl_easy_perform(curl);
        ptr_print_debug<const char*, const decltype(res)&>("curl res code is ", res);
        curl_slist_free_all(slist);
        curl_easy_cleanup(curl);
    }
    if(res != CURLE_OK) {
        std::string errMsg;
        errMsg.append("curl error. CURLcode is ").append(std::to_string(res));
        throw std::runtime_error(errMsg);
    }
    return chunk;
}

template <class REQUEST, class RESPONSE>
class RestClient
{
public:
    virtual ~RestClient() = default;
    virtual RESPONSE get(const std::string& _uri) const = 0;
    virtual RESPONSE post(const std::string& _uri, const REQUEST& _req) const = 0;
    virtual RESPONSE put(const std::string& _uri, const REQUEST& _req)  const = 0;
    virtual RESPONSE del(const std::string& _uri, const REQUEST& _req)  const = 0; 
};

class CRestClient final : public RestClient<std::string, std::string>
{
private:
public:
    virtual std::string get(const std::string& _uri) const
    {
        return curl_get(_uri.c_str());
    }
    virtual std::string post(const std::string& _uri, const std::string& _req) const
    {
        return curl_post(_uri.c_str(), _req.c_str());
    }
    virtual std::string put(const std::string& _uri, const std::string& _req)  const
    {
        return curl_put(_uri.c_str(), _req.c_str());
    }
    virtual std::string del(const std::string& _uri, const std::string& _req)  const
    {
        return curl_delete(_uri.c_str(), _req.c_str());
    }
};

class CRestClientJ final : public RestClient<std::string, std::string>
{
public:
    virtual std::string get(const std::string& _uri) const
    {
        return curl_get(_uri.c_str());
    }
    virtual std::string post(const std::string& _uri, const std::string& _req) const
    {
        return curl_post_json(_uri.c_str(), _req.c_str());
    }
    virtual std::string put(const std::string& _uri, const std::string& _req)  const
    {
        return curl_put_json(_uri.c_str(), _req.c_str());
    }
    virtual std::string del(const std::string& _uri, const std::string& _req)  const
    {
        return curl_delete_json(_uri.c_str(), _req.c_str());
    }
};

/**
 * TODO
 * 
 * 現状、RestApi は CRestClient のジェネレータに過ぎないが
 * CRestClient は隠蔽して、REST API 単位の具体的処理にまとめる方が
 * 望ましいかな。
 */

class RestApi final
{
private:
    const CRestClient restClient;
public:
    const CRestClient& client() const {
        return restClient;
    }
};

namespace res::data {

struct Contractor {
    int id = 0;
    std::string companyId;
    std::string email;
    std::string password;
    std::string name;
    std::string roles;
};

struct ContractorResponse {
    std::string code;
    std::string message;
    Contractor contractor;
};

void to_json(nlohmann::json& j, const Contractor& c) {
    j = nlohmann::json{
        {"id", c.id}
        , {"companyId", c.companyId}
        , {"email", c.email}
        , {"password", c.password}
        , {"name", c.name}
        , {"roles", c.roles}
    };
}

void to_json(nlohmann::json& j, const ContractorResponse& res) {
    j = nlohmann::json{
        {"code", res.code}
        , {"message", res.message}
        , {"contractor", res.contractor}
    };
}

void from_json(const nlohmann::json& j, Contractor& c) {
    j.at("id").get_to(c.id);       // get_to(T& arg) は arg = get<T>() と同じ
    j.at("companyId").get_to(c.companyId);
    j.at("email").get_to(c.email);
    j.at("password").get_to(c.password);
    j.at("name").get_to(c.name);
    if( !(j.at("roles").is_null()) ) j.at("roles").get_to(c.roles);
}

void from_json(const nlohmann::json& j, ContractorResponse& res) {
    j.at("code").get_to(res.code);       // get_to(T& arg) は arg = get<T>() と同じ
    j.at("message").get_to(res.message);
    if( !(j.at("contractor").is_null()) ) j.at("contractor").get_to(res.contractor);
}

}   // namespace res::data

class RestContractorApi final
{
private:
    const CRestClientJ restClientJ;
public:
    nlohmann::json insert(const nlohmann::json& req) const
    {
        std::string res = restClientJ.post("http://localhost:8080/api/contractor/insert", req.dump());
        return nlohmann::json::parse(res);
    }
    nlohmann::json login(const nlohmann::json& req) const
    {
        std::string res = restClientJ.post("http://localhost:8080/api/contractor/login", req.dump());
        return nlohmann::json::parse(res);
    }
    nlohmann::json update(const nlohmann::json& req) const
    {
        std::string res = restClientJ.put("http://localhost:8080/api/contractor/update", req.dump());
        return nlohmann::json::parse(res);
    }
    nlohmann::json del(const nlohmann::json& req) const
    {
        std::string res = restClientJ.del("http://localhost:8080/api/contractor/delete", req.dump());
        return nlohmann::json::parse(res);
    }
    // res::data::ContractorResponse makeData(const nlohmann::json& json) const
    // {
    //     res::data::ContractorResponse res = json;
    //     return res;
    // }
};


int test_HelloApi()
{
    puts("------ test_HelloApi");
    try {
        RestApi api;
        std::string getRes = api.client().get("http://localhost:8080/api/get/hello?id=1000");
        ptr_print_debug<const char*, std::string&>("reponse: ", getRes);

        std::string postRes = api.client().post("http://localhost:8080/api/post/hello", "post content");
        ptr_print_debug<const char*, std::string&>("reponse: ", postRes);

        std::string putRes = api.client().put("http://localhost:8080/api/put/hello", "put content");
        ptr_print_debug<const char*, std::string&>("reponse: ", putRes);

        std::string delRes = api.client().del("http://localhost:8080/api/delete/hello", "delete content");
        ptr_print_debug<const char*, std::string&>("reponse: ", delRes);

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_RestContractorApi_insert()
{
    puts("------ test_RestContractorApi_insert");
    try {
        std::string s = R"({
        "companyId": "foo_1000A",
        "email": "joe@foo.com",
        "password": "joe1234",
        "name": "Joe"
        })";

        // REST API Server は Java, Spring Web と Spring Data JPA で実装している。
        // そのリポジトリの仕組み上、プライマリキーの有無で Insert と Update を
        // 区別しており、次の構造体を引数に設定するとエラーになる（id 0 の更新を実行しようとするため：）。
        res::data::Contractor c;
        c.companyId = "foo_1000A";
        c.email = "joe@foo.com";
        c.password = "joe1234";
        c.name = "Joe";
        nlohmann::json testj = c;
        ptr_print_debug<const char*, decltype(testj)&>("testj: ", testj);

        RestContractorApi api;
        nlohmann::json resj = api.insert(nlohmann::json::parse(s));
        ptr_print_debug<const char*, decltype(resj)&>("response: ", resj);
        ptr_print_debug<const char*, const std::string&>("code: ", resj.at("code").dump());
        
        res::data::ContractorResponse cres = resj;
        ptr_print_debug<const char*, int&>("cres.contractor.id: ", cres.contractor.id);
        ptr_print_debug<const char*, std::string&>("cres.contractor.companyId: ", cres.contractor.companyId);
        ptr_print_debug<const char*, std::string&>("cres.contractor.email: ", cres.contractor.email);
        assert(resj.at("code").dump().compare("\"ok\"") == 0);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_RestContractorApi_login(int* id)
{
    puts("------ test_RestContractorApi_login");
    try {
        std::string s = R"({
        "companyId": "foo_1000A",
        "email": "joe@foo.com",
        "password": "joe1234"
        })";
        RestContractorApi api;
        nlohmann::json resj = api.login(nlohmann::json::parse(s));
        ptr_print_debug<const char*, decltype(resj)&>("response: ", resj);
        ptr_print_debug<const char*, const std::string&>("code: ", resj.at("code").dump());
        assert(resj.at("code").dump().compare("\"ok\"") == 0);
        res::data::ContractorResponse resc = resj; 
        *id = resc.contractor.id;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_RestContractorApi_update(const int* const id)
{
    puts("------ test_RestContractorApi_update");
    try {
        printf("id is %d\n", *id);
        std::string s = R"({
        "id": 0,
        "companyId": "foo_1000A",
        "email": "derek@foo.com",
        "password": "derek1234",
        "name": "Derek"
        })";
        RestContractorApi api;
        nlohmann::json reqj = nlohmann::json::parse(s);
        reqj["id"] = *id;
        nlohmann::json resj = api.update(reqj);
        ptr_print_debug<const char*, decltype(resj)&>("response: ", resj);
        ptr_print_debug<const char*, const std::string&>("code: ", resj.at("code").dump());
        assert(resj.at("code").dump().compare("\"ok\"") == 0);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_RestContractorApi_del(const int* const id)
{
    puts("------ test_RestContractorApi_del");
    try {
        std::string s = R"({
        "id": 0,
        "companyId": "foo_1000A",
        "email": "derek@foo.com",
        "password": "derek1234",
        "name": "Derek"
        })";
        RestContractorApi api;
        nlohmann::json reqj = nlohmann::json::parse(s);
        reqj["id"] = *id;
        nlohmann::json resj = api.del(reqj);
        ptr_print_debug<const char*, decltype(resj)&>("response: ", resj);
        ptr_print_debug<const char*, const std::string&>("code: ", resj.at("code").dump());
        assert(resj.at("code").dump().compare("\"ok\"") == 0);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void)
{
    puts("START main ===");
    int ret = -1;
    if(1) {
        ptr_print_debug<const char*, int&>("Play and Result ... test_HelloApi", ret = test_HelloApi());
        assert(ret == 0);
    }
    if(1) {
        ptr_print_debug<const char*, int&>("Play and Result ... test_RestContractorApi_insert", ret = test_RestContractorApi_insert());
        assert(ret == 0);
        int id;
        ptr_print_debug<const char*, int&>("Play and Result ... test_RestContractorApi_login", ret = test_RestContractorApi_login(&id));
        assert(ret == 0);
        ptr_print_debug<const char*, int&>("Play and Result ... test_RestContractorApi_update", ret = test_RestContractorApi_update(&id));
        assert(ret == 0);
        ptr_print_debug<const char*, int&>("Play and Result ... test_RestContractorApi_del", ret = test_RestContractorApi_del(&id));
        assert(ret == 0);
    }
    puts("=== main END");
    return EXIT_SUCCESS;
}