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
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc -L/usr/lib/x86_64-linux-gnu/ rest_template.cpp -lcurl -o ../bin/rest_template
 */
#include <iostream>
#include <cassert>
#include <cstring>
#include <curl/curl.h>

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
    ptr_print_debug<const char*, const decltype(res)&>("res code is ", res);
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
        ptr_print_debug<const char*, const decltype(res)&>("res code is ", res);
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
        ptr_print_debug<const char*, const decltype(res)&>("res code is ", res);
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
        ptr_print_debug<const char*, const decltype(res)&>("res code is ", res);
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
        std::string res = curl_get(_uri.c_str());
        return res;
    }
    virtual std::string post(const std::string& _uri, const std::string& _req) const
    {
        std::string res = curl_post(_uri.c_str(), _req.c_str());
        return res;
    }
    virtual std::string put(const std::string& _uri, const std::string& _req)  const
    {
        std::string res = curl_put(_uri.c_str(), _req.c_str());
        return res;
    }
    virtual std::string del(const std::string& _uri, const std::string& _req)  const
    {
        std::string res = curl_delete(_uri.c_str(), _req.c_str());
        return res;
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

int main(void)
{
    puts("START main ===");
    int ret = -1;
    if(1) {
        ptr_print_debug<const char*, int&>("Play and Result ... test_HelloApi", ret = test_HelloApi());
        assert(ret == 0);
    }
    puts("=== main END");
    return EXIT_SUCCESS;
}