/**
 * 課題 curl
 * 
 * そのサンプリング。
 * 
 * curl の開発環境を整備する、必要なヘッダファイルやライブラリのインストール。
 * sudo apt install libcurl4-openssl-dev
 * 
 * @see /usr/include/x86_64-linux-gnu/curl/curl.h
 * dpkg -L libcurl4
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -L/usr/lib/x86_64-linux-gnu/ std_curl.cpp -lcurl -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <curl/curl.h>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <typename ERR>
concept ErrReasonable = requires(ERR& e) {
    e.what();
};
template <typename ERR>
requires ErrReasonable<ERR>
void (*ptr_print_error)(ERR) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        double pi = 3.141592;
        printf("pi is \t%lf\n", pi);
        ptr_lambda_debug<const char*, const double&>("pi is ", pi);
        throw std::runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

size_t curl_write_func(char* cp, size_t size, size_t nmemb, std::string* stream) 
{
    size_t realSize = size * nmemb;
    stream->append(cp, realSize);
    return realSize;
}

std::string curl_get(const char* url) 
{
  CURL *curl;
  CURLcode res = CURLE_UNSUPPORTED_PROTOCOL;
  curl = curl_easy_init();
  std::string chunk;
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
    curl_easy_setopt(curl, CURLOPT_PROXY, "");
    res = curl_easy_perform(curl);
    ptr_lambda_debug<const char*, const decltype(res)&>("res is ", res);
    curl_easy_cleanup(curl);
  }
  if(res != CURLE_OK) {
    std::cout << "ERROR: curl error." << std::endl;
    std::exit(1);
  }
  return chunk;
}

int test_curl_get() {
    puts("=== test_curl_get");
    try {
        char url[] = "http://www.loki.org/";
        std::string res = curl_get(url);
        ptr_lambda_debug<const char*, const std::string&>("res is ", res);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 課題 curl ===");
    if(0.01) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);
    }
    if(1.00) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_curl_get());
        assert(ret == 0);
    }
    puts("===   課題 curl END");
    return 0;
}
