/**
 * 課題
 * 
 * C/C++ 文字列解析
 * 
 * C 言語で書かれたソースの利用に際し gcc と g++ では異なるという点がやはりある。
 * 今回は、以前 C で実装した文字列解析を C++ に組み込むことを目的にする。
 * 勿論 C を使わないという選択を最初に行えばよいが、それでは面白くないではないか。
 * 
 * また、この場を検索サービスの結果を解析し必要な情報を取り出す実験の場としたい。
 * その過程で C を使わなくなるのは止む終えないと考える。前置きは以上だ。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc/ h_tree.c string_parse.c string_parse_proto.cpp -lcurl -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <cstring>
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sys/stat.h>
#include <curl/curl.h>

#include "h_tree.h"
#include "string_parse.h"


template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class ERR>
concept ErrReasonable = requires(ERR& e) {
    e.what();
};
template <class ERR> requires ErrReasonable<ERR>
void (*ptr_print_error)(ERR) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

int test_debug_and_error() {
    puts("====== test_debug_and_error");
    try {
        double pi = 3.141592;
        ptr_lambda_debug<const char*, const double&>("pi is ", pi);
        throw std::runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_H_TREE() {
    puts("====== test_H_TREE");
    try {
        H_TREE root = createTree();
        int n1 = 3;
        int n2 = 6;
        int n3 = 9;
        int n4 = 12;
        int n5 = 15;
        int n6 = 18;
        pushTree(root, &n1);
        pushTree(root, &n2);
        pushTree(root, &n3);
        pushTree(root, &n4);
        pushTree(root, &n5);
        pushTree(root, &n6);
        int* pn = nullptr;
        pn = (int*)popStack(root);
        assert(*pn == 18);
        ptr_lambda_debug<const char*, const int&>("pn is ", *pn);
        H_TREE tmp = root;
        while((tmp = hasNextTree(tmp)) != NULL) {
            ptr_lambda_debug<const char*, const int&>("value is ", *(int*)treeValue(tmp));
        }
        pn = (int*)popStack(root);
        assert(*pn == 15);
        ptr_lambda_debug<const char*, const int&>("pn is ", *pn);
        pn = (int*)popStack(root);
        assert(*pn == 12);
        ptr_lambda_debug<const char*, const int&>("pn is ", *pn);
        pn = (int*)popStack(root);
        assert(*pn == 9);
        ptr_lambda_debug<const char*, const int&>("pn is ", *pn);
        pn = (int*)popStack(root);
        assert(*pn == 6);
        ptr_lambda_debug<const char*, const int&>("pn is ", *pn);
        pn = (int*)popStack(root);
        assert(*pn == 3);
        ptr_lambda_debug<const char*, const int&>("pn is ", *pn);
        pn = (int*)popStack(root);
        assert(pn == NULL);
        size_t count = countTree(root);
        ptr_lambda_debug<const char*, const size_t&>("count is ", count);
        assert(count == 1);
        clearTree(root, count);

        H_TREE root2 = createTree();
        pushTree(root2, &n1);
        pushTree(root2, &n2);
        pushTree(root2, &n3);
        pushTree(root2, &n4);
        pushTree(root2, &n5);
        pushTree(root2, &n6);
        tmp = root2;
        while((tmp = hasNextTree(tmp)) != NULL) {
            ptr_lambda_debug<const char*, const int&>("value is ", *(int*)treeValue(tmp));
        }
        pn = (int*)popQueue(root2);
        assert(*pn == 3);
        ptr_lambda_debug<const char*, const int&>("pn is ", *pn);
        pn = (int*)popQueue(root2);
        assert(*pn == 6);
        ptr_lambda_debug<const char*, const int&>("pn is ", *pn);
        pn = (int*)popQueue(root2);
        assert(*pn == 9);
        ptr_lambda_debug<const char*, const int&>("pn is ", *pn);
        pn = (int*)popQueue(root2);
        assert(*pn == 12);
        ptr_lambda_debug<const char*, const int&>("pn is ", *pn);
        pn = (int*)popQueue(root2);
        assert(*pn == 15);
        ptr_lambda_debug<const char*, const int&>("pn is ", *pn);
        pn = (int*)popQueue(root2);
        assert(*pn == 18);
        ptr_lambda_debug<const char*, const int&>("pn is ", *pn);
        pn = (int*)popQueue(root2);
        assert(pn == NULL);
        count = countTree(root2);
        ptr_lambda_debug<const char*, const size_t&>("count is ", count);
        clearTree(root2, count);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}


std::string url_encode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}


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
    ptr_lambda_debug<const char*, const decltype(res)&>("res is ", res);
    curl_easy_cleanup(curl);
  }
  if(res != CURLE_OK) {
    std::string errMsg;
    errMsg.append("curl error. CURLcode is ").append(std::to_string(res));
    throw std::runtime_error(errMsg);
  }
  return chunk;
}






static const char* FILE_PATH = "/home/jack/tmp/sample.html";

int test_searchProto() {
  puts("=== test_searchProto");
  size_t size = getFileSize(FILE_PATH);
  char* buf = (char*)malloc(size+1);
  memset(buf, '\0', size+1);
  readFile(FILE_PATH, buf);
  
  size_t strSize    = 1025;
  H_TREE tmp        = NULL;
  char pattern[]    = "href=\"";
  H_TREE t1 = createTree();
  searchProto(t1, strSize, buf, pattern, '"');
  tmp = t1;
  while((tmp = hasNextTree(tmp)) != NULL) {
    char* str = (char*)treeValue(tmp);
    printf("%s\n", str);
    free((void*)str);
  }
  char pattern_2[]  = "videoId\":\"";
  H_TREE t2 = createTree();
  searchProto(t2, strSize, buf, pattern_2, '"');
  tmp = t2;
  while((tmp = hasNextTree(tmp)) != NULL) {
    char* str = (char*)treeValue(tmp);
    printf("%s\n", str);
    free((void*)str);
  }
  
  clearTree(t1, countTree(t1));
  clearTree(t2, countTree(t2));
  removeBuffer(buf);
  return EXIT_SUCCESS;
}

/**
    少しだけターゲットの解析をしてみた。取得するデータの終端も文字列で検索できないと意味
    がない、あるいは不自由を来すおそれがある。
    e.g. 先頭の文字列 "{\"videoRenderer\"" 終端の文字列 "\"}}}]},\"shortBylineText\""
  上例のような形が JSON であるため。
  これを先に考えてみよう。
  次のテスト関数が YouTube の必要情報のとり方のサンプルになる。
*/

int test_search2nd() {
  puts("=== test_search2nd");
  clock_t start_clock = clock();
  size_t size = getFileSize(FILE_PATH);
  char* buf = (char*)malloc(size+1);
  memset(buf, '\0', size+1);
  readFile(FILE_PATH, buf);
  
  char startPattern[]    = "{\"videoRenderer\":";
  char endPattern[]      = "\"}}}]},\"shortBylineText\"";   // endPattern の中に必ず終端を表現する文字があること。この場合は ','
  printf("startPattern is \t%s\n", startPattern);
  printf("endPattern   is \t%s\n", endPattern);
    
  /**
    search2nd() を実装するにあたり、最初に validation を行う必要がこれはある。
      そもそもが「ヤマカン」であるため、設定するパラメータで正しく機能する保証がないから（JSON として正しいこととは別の話）。
      - 最低限、_startPattern と _endPattern でヒットする数が同じこと。
      - 各 start と end の組み合わせにおいて、アドレスが start < end であること（start >= end は NG）。
  */
  
  H_TREE startPos = createTree();
  H_TREE endPos   = createTree();
  H_TREE dest     = createTree();
  H_TREE fix      = createTree();
  char appendT[] = "{";
  char appendB[] = "}";
  setRange(buf, startPos, endPos, startPattern, endPattern);
  if(isValidRange(startPos, endPos)) {
    search2nd(dest, startPos, endPos, 5);
    H_TREE tmp = dest;
    while((tmp = hasNextTree(tmp)) != NULL) {
      char* str = (char*)treeValue(tmp);          // BAD KNOW-HOW ここで pop してはいけない（理由が知りたければ試してみてくれ：）
      /**
        簡易 valid JSON を考えてみる。
        {} の数が同じ、[] の数が同じ、それぞれの位置の問題。
        endPattern から、{} の数だけに注目すればいいと思う、各括弧の位置については考慮しない（それは元データが間違っている）。
        
        単一責務の原則と処理速度の問題。
        これは正直悩ましいが原則に従うならば、特定の文字列の Parse とそれが任意のフォーマットに適合するか否かとは分けて考えるべきだ
        と思う。したがってここでフォーマット適合に関する処理を行う。
      */
      size_t s = 0;
      size_t e = 0;
      checkChar(str, '{', '}', &s, &e);
    //   debug_long("s is ", &s);
    //   debug_long("e is ", &e);
      ptr_lambda_debug<const char*, const decltype(s)&>("s is ", s);
      ptr_lambda_debug<const char*, const decltype(e)&>("e is ", e);
      if(s > e) {
        appendBottom(fix, str, appendB, s-e);
      } else {
        appendTop(fix, str, appendT, e-s);
      }
      free((void*)str);
    }
    tmp = fix;
    while((tmp = hasNextTree(tmp)) != NULL) {
      char* str = (char*)treeValue(tmp);
      printf("str is \t%s\n", str);
      free((void*)str);
    }
    printf("dest count is \t%ld\n", countTree(dest));   // H_TREE は 根（root）分余計にある。実際の要素数 + 1 になる。
    printf("fix count is \t%ld\n", countTree(fix));     // H_TREE は 根（root）分余計にある。実際の要素数 + 1 になる。
  }
  clearTree(startPos, countTree(startPos));
  clearTree(endPos, countTree(endPos));
  clearTree(dest, countTree(dest));
  clearTree(fix, countTree(fix));
  removeBuffer(buf);
  clock_t end_clock = clock();
  printf("clock: %lf (sec)\n", (double)(end_clock-start_clock)/CLOCKS_PER_SEC);
  return EXIT_SUCCESS;
}

int test_Result_List_JSON() {
  puts("=== test_Result_List_JSON");
  try {
    std::string p1 = R"({"personData": {"name": "Jojo","email": "jojo@loki.org","age": 24}})";
    std::string p2 = R"({"personData": {"name": "Alpha","email": "alpha@loki.org","age": 30}})";
    nlohmann::json res;
    res["list"] = {};
    res["list"][0] = p1;
    res["list"][1] = p2;
    // 次の出力ができれば OK
    ptr_lambda_debug<const char*, const std::string&>("res is ", res.dump());
    return EXIT_SUCCESS;
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    return EXIT_FAILURE;
  }
}

int test_Result_List_JSON_innerHtml() {
  puts("=== test_Result_List_JSON_innerHtml");
  try {
    std::string p1 = R"({"innerHtml": "<div jsname="xQjRM"><div class="sATSHe"><div><div jscontroller="SC7lYd" class="g Ww4FFb vt6azd tF2Cxc asEBEc" lang="ja" style="width:652px" jsaction="QyLbLe:OMITjf;ewaord:qsYrDe;xd28Mb:A6j43c" data-hveid="CD0QAA" data-ved="2ahUKEwjBtfzKnr6FAxVeiVYBHXgyA40QFSgAegQIPRAA">"})";
    std::string p2 = R"({"innerHtml": "<div jsname="xQjRM"><div class="sATSHe"><div><div jscontroller="SC7lYd" class="g Ww4FFb vt6azd tF2Cxc asEBEc" lang="ja" style="width:600px" jsaction="QyLbLe:OMITjf;ewaord:qsYrDe;xd28Mb:A6j43c" data-hveid="CDkQAA" data-ved="2ahUKEwjBtfzKnr6FAxVeiVYBHXgyA40QFSgAegQIORAA">"})";
    nlohmann::json res;
    res["list"] = {};
    res["list"][0] = p1;
    res["list"][1] = p2;
    // 次の出力ができれば OK
    ptr_lambda_debug<const char*, const std::string&>("res is ", res.dump());
    return EXIT_SUCCESS;
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    return EXIT_FAILURE;
  }
}

void replaceAll(std::string& _src, const std::string& _target, const std::string& _replacement) {
  puts("--- replaceAll");
  try {
    std::string::size_type pos = 0;
    while((pos = _src.find(_target, pos)) != std::string::npos) {
      _src.replace(pos, _target.length(), _replacement);
      pos += _replacement.length();
    }
  } catch(std::exception& e) {
    throw std::runtime_error(e.what());
  }
}

int test_string_replace() {
  puts("=== test_string_replace");
  try {
    std::string src            = R"(<a class="Q71vJc" href="/search?sca_esv=e7ea9d64b74deda5&amp;ie=UTF-8&amp;q=%E5%A4%B1%E6%A5%BD%E5%9C%92+%E3%83%8D%E3%82%BF%E3%83%90%E3%83%AC&amp;sa=X&amp;ved=2ahUKEwiu5fT42cWFAxWBsVYBHXOLBkAQ1QJ6BAgBEAo" data-ved="2ahUKEwiu5fT42cWFAxWBsVYBHXOLBkAQ1QJ6BAgBEAo"><accordion-entry-search-icon><span class="ieB2Dd"><img class="OEaqif" alt="" src="data:image/gif;base64,R0lGODlhAQABAIAAAP///////yH5BAEKAAEALAAAAAABAAEAAAICTAEAOw==" style="max-width:20px;max-height:20px" id="dimg_15" data-deferred="1"></span></accordion-entry-search-icon><div class="kjGX2"><span class="Xe4YD"><div class="BNeawe s3v9rd AP7Wnd lRVwie">&#22833;&#27005;&#22290; &#12493;&#12479;&#12496;&#12524;</div></span></div></a>)";
    std::string target         = R"(href="/search?)";
    std::string replacement    = R"(href="https://www.google.com/search?)";
    replaceAll(src, target, replacement);
    // std::string::size_type pos = 0;
    // while((pos = src.find(target, pos)) != std::string::npos) {
    //   src.replace(pos, target.length(), replacement);
    //   pos += replacement.length();
    // }
    ptr_lambda_debug<const char*, const std::string&>("src is ", src);
    return EXIT_SUCCESS;
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    return EXIT_FAILURE;
  }
}

/**
 * YouTube
 * 
 * 外部サービス、ユーチューブへのリクエストとパース。
*/

#define WRITE_DIR   "/home/jack/tmp/string_parse_proto/"

void writeYouTube(const std::string& source) {
  puts("--- writeYouTube");
  try {
    std::string fileName(WRITE_DIR);
    fileName += "youtube/source.html";
    std::ofstream writer;
    writer.open(fileName, std::ios::out);
    writer << source << std::endl;
    writer.close();
  } catch(std::exception& e) {
    throw std::runtime_error(e.what());
  }
}

std::string requestYouTube(const std::string& keyword) {
  puts("--- requestYouTube");
  try {
    std::string url = "https://www.youtube.com/results?search_query=";
    url.append(keyword);
    std::string res = curl_get(url.c_str());
    return res;
  } catch(std::exception& e) {
    throw std::runtime_error(e.what());
  }
}

int test_requestYouTube() {
  puts("=== test_requestYouTube");
  try {
    std::string keyword = "メタルギア";
    std::string res = requestYouTube(url_encode(keyword));
    // ptr_lambda_debug<const char*, const std::string&>("res is ", res);
    writeYouTube(res);
    return EXIT_SUCCESS;
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    return EXIT_FAILURE;
  }
}

/**
 * YouTube の任意の検索結果の解析を行う。
 * 必要情報、この場合は JSON を返却する。
*/

bool parseYouTube(std::string& _dest, const std::string& _filePath) {
  puts("--- parseYouTube");
  char* buf       = NULL;
  H_TREE startPos = createTree();
  H_TREE endPos   = createTree();
  H_TREE dest     = createTree();
  _dest           = R"({"ytList":[)";
  try {
    size_t size = getFileSize(_filePath.c_str());
    buf = (char*)malloc(size+2);
    memset(buf, '\0', size+2);
    readFile(_filePath.c_str(), buf);

    char startPattern[]    = "{\"videoRenderer\":";
    char endPattern[]      = "\"}}}]},\"shortBylineText\"";   // endPattern の中に必ず終端を表現する文字があること。この場合は ','
    printf("startPattern is \t%s\n", startPattern);
    printf("endPattern   is \t%s\n", endPattern);
    setRange(buf, startPos, endPos, startPattern, endPattern);

    if(isValidRange(startPos, endPos)) {
      search2nd(dest, startPos, endPos, 5);
      H_TREE tmp = dest;
      size_t i = 0;
      while((tmp = hasNextTree(tmp)) != NULL) {
        char* cstr = (char*)treeValue(tmp);
        std::string str(cstr);
        str.append("}}");
        // printf("str is %s\n", str.c_str());
        if(i == 0) {
          _dest.append(str);
        } else {
          _dest.append(", ").append(str);
        }
        i++;
        free((void*)cstr);
      }
      printf("dest count is \t%ld\n", countTree(dest));   // H_TREE は 根（root）分余計にある。実際の要素数 + 1 になる。
    }
    _dest.append("]}");
    clearTree(startPos, countTree(startPos));
    clearTree(endPos, countTree(endPos));
    clearTree(dest, countTree(dest));
    removeBuffer(buf);
    return true;
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    clearTree(startPos, countTree(startPos));
    clearTree(endPos, countTree(endPos));
    clearTree(dest, countTree(dest));
    removeBuffer(buf);
    return false;
  }
}

int test_parseYouTube() {
  puts("=== test_parseYouTube");
  try {
    std::string dest;
    // std::string filePath("/home/jack/tmp/sample.html");
    std::string filePath(WRITE_DIR);
    filePath += "youtube/source.html";
    bool ret = parseYouTube(dest, filePath);
    ptr_lambda_debug<const char*, const bool&>("ret is ", ret);
    nlohmann::json j(dest);
    ptr_lambda_debug<const char*, const std::string&>("j is ", j.dump());   // これで問題なく JSON 成形されていれば OK。問題があれば exception になる：）
    return EXIT_SUCCESS;
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    return EXIT_FAILURE;
  }
}



/**
 * Google
 * 
 * 外部サービス、グーグルへのリクエストとパース。
*/

void writeGoogle(const std::string& source) {
  puts("--- writeGoogle");
  try {
    std::string fileName(WRITE_DIR);
    fileName += "google/source_4.html";
    std::ofstream writer;
    writer.open(fileName, std::ios::out);
    writer << source << std::endl;
    writer.close();
  } catch(std::exception& e) {
    throw std::runtime_error(e.what());
  }
}

std::string requestGoogle(const std::string& keyword) {
  puts("--- requestGoogle");
  try {
    std::string url = "https://www.google.com/search?q=";
    url.append(keyword);
    std::string res = curl_get(url.c_str());
    return res;
  } catch(std::exception& e) {
    throw std::runtime_error(e.what());
  }
}

int test_requestGoogle() {
  puts("=== test_requestGoogle");
  try {
    std::string keyword = "失楽園";
    // std::string keyword = "DMC 4";
    // std::string keyword = "レジデントイービル";
    // std::string keyword = "レジデントエビル";
    std::string res = requestGoogle(url_encode(keyword));
    // ptr_lambda_debug<const char*, const std::string&>("res is ", res);
    writeGoogle(res);
    return EXIT_SUCCESS;
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    return EXIT_FAILURE;
  }
}

/**
 * Google
 * 検索結果の文字列解析。
 * 
 * <div></div>
 * ...
 * <footer>
*/

void parseGoogle(std::string& _dest, const std::string& _filePath) {
  puts("--- parseGoogle");
  char* buf         = NULL;
  H_TREE startPos   = createTree();
  H_TREE endPos     = createTree();
  H_TREE dest       = createTree();
  H_TREE a_startPos = createTree();
  H_TREE a_endPos   = createTree();
  H_TREE a_dest     = createTree();
  _dest             = R"({"gList":[)";      // JSON リスト構造のはじまり
  try {
    size_t size = getFileSize(_filePath.c_str());
    buf = (char*)malloc(size+2);
    memset(buf, '\0', size+2);
    readFile(_filePath.c_str(), buf);

    /**
     * 色々パターンを調べた結果、確実な規則性が得られなかった。
     * 始点と終点の 2 点のみで、全ブロックを返却しようかと思う：）
     * e.g. <div id="main"> から </footer> まで
    */
    char startPattern[]  = "<div></div>";
    char endPattern[]    = "<footer>";
    printf("startPattern is \t%s\n", startPattern);
    printf("endPattern   is \t%s\n", endPattern);
    setRange(buf, startPos, endPos, startPattern, endPattern);
    printf("\n");
      /**
       * 上記のデバッグから分かったこと、setRange() 関数を新たに用意するか次の要件を満たす必要があるということかな。
       * startPos > endPos の場合は startPos を空ループで無視する必要がある startPos == endPos になるまで。
      */
    if(countTree(startPos)>1 && countTree(endPos)>1 && isValidRange(startPos, endPos)) {
      search2nd(dest, startPos, endPos, (char)((strlen(endPattern)-1)*-1));
      H_TREE tmp = dest;
      tmp = hasNextTree(tmp);
      char* cstr = (char*)treeValue(tmp);
      std::string str(cstr);
      printf("%s\n", str.c_str());
      free((void*)cstr);
      /**
       * ここで、a タグだけを抜き取りたい。
       * e.g. "<a href=" から "</a>" まで。
      */
      char a_startPattern[]  = "<a ";
      char a_endPattern[]    = "</a>";
      printf("a_startPattern is \t%s\n", a_startPattern);
      printf("a_endPattern   is \t%s\n", a_endPattern);
      setRange((char*)str.c_str(), a_startPos, a_endPos, a_startPattern, a_endPattern);
      printf("scount is %ld\n", countTree(a_startPos));
      printf("ecount is %ld\n", countTree(a_endPos));
      if(isValidRange(a_startPos, a_endPos)) {
        search2nd(a_dest, a_startPos, a_endPos, (char)(3));
        tmp = a_dest;
        size_t i = 0;
        while((tmp = hasNextTree(tmp)) != NULL) {
          char* cstr = (char*)treeValue(tmp);
          std::string str;
          if(i == 0) {
            str = R"({"item":")";
          } else {
            str = R"(,{"item":")";
          }
          str.append(cstr);
          str.append(R"("})");
          printf("%s\n", str.c_str());
          _dest.append(str);
          free((void*)cstr);
          i++;
        }
      }
      /**
       * 通常の検索エンジンの利用で必要な情報は a タグのみと割り切ればこれでいいと思う。
       * 後はリファクタして、メモリの開放漏れがなければよいかと。
      */
    }
    _dest.append("]}");     // JSON リスト構造の終わり
    clearTree(startPos, countTree(startPos));
    clearTree(endPos, countTree(endPos));
    clearTree(dest, countTree(dest));
    clearTree(a_startPos, countTree(a_startPos));
    clearTree(a_endPos, countTree(a_endPos));
    clearTree(a_dest, countTree(a_dest));
    removeBuffer(buf);
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    clearTree(startPos, countTree(startPos));
    clearTree(endPos, countTree(endPos));
    clearTree(dest, countTree(dest));
    clearTree(a_startPos, countTree(a_startPos));
    clearTree(a_endPos, countTree(a_endPos));
    clearTree(a_dest, countTree(a_dest));
    removeBuffer(buf);
    throw std::runtime_error(e.what());
  }
}

int test_parseGoogle() {
  puts("=== test_parseGoogle");
  try {
    std::string dest;
    std::string filePath(WRITE_DIR);
    filePath += "google/source_4.html"; 
    parseGoogle(dest, filePath);

    // 文字列解析後に a タグのドメイン相対に対する置換処理を行う
    std::string target         = R"(href="/search?)";
    std::string replacement    = R"(target="_blank" href="https://www.google.com/search?)";
    replaceAll(dest, target, replacement);
    ptr_lambda_debug<const char*, const std::string&>("dest is ", dest);

    nlohmann::json j(dest);
    ptr_lambda_debug<const char*, const std::string&>("j is ", j.dump());   // これで問題なく JSON 成形されていれば OK。問題があれば exception になる：）
    return EXIT_SUCCESS;
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    return EXIT_FAILURE;
  }
}

/**
 * Yahoo JAPAN
 * 
 * 外部サービス、Yahoo JAPAN へのリクエストとパース。
*/

void writeYahoo(const std::string& source) {
  puts("--- writeYahoo");
  try {
    std::string fileName(WRITE_DIR);
    fileName += "yahoo/source.html";
    std::ofstream writer;
    writer.open(fileName, std::ios::out);
    writer << source << std::endl;
    writer.close();
  } catch(std::exception& e) {
    throw std::runtime_error(e.what());
  }
}

std::string requestYahoo(const std::string& keyword) {
  puts("--- requestYahoo");
  try {
    std::string url = "https://search.yahoo.co.jp/search?p=";
    url.append(keyword);
    std::string res = curl_get(url.c_str());
    return res;
  } catch(std::exception& e) {
    throw std::runtime_error(e.what());
  }
}

int test_requestYahoo() {
  puts("=== test_requestYahoo");
  try {
    std::string keyword = "哲学";
    std::string res = requestYahoo(url_encode(keyword));
    // ptr_lambda_debug<const char*, const std::string&>("res is ", res);
    writeYahoo(res);
    return EXIT_SUCCESS;
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    return EXIT_FAILURE;
  }
}

/**
 * Yahoo
 * 検索結果の文字列解析。
*/

void parseYahoo(std::string& _dest, const std::string& _filePath) {
  puts("--- parseYahoo");
  char* buf = NULL;
  H_TREE startPos   = createTree();
  H_TREE endPos     = createTree();
  H_TREE dest       = createTree();
  _dest             = R"({"yList":[)";      // JSON リスト構造のはじまり
  try {
    size_t size = getFileSize(_filePath.c_str());
    buf = (char*)malloc(size+2);
    memset(buf, '\0', size+2);
    readFile(_filePath.c_str(), buf);

    // ptr_lambda_debug<const char*, const char*>("buf is ", buf);
    /**
     * 考え方は Google の場合と同じで、大まかな範囲を絞り込み、a タグを取り出す。
     * <ot>
     * <li>... 必要な情報 ...</li>
     * </ot>
     * 実際のもとデータ上に <ot></ot> は発見できなかったため、<li></li> を直接抽出した。
    */
    char startPattern[]  = "<li>";
    char endPattern[]    = "</li>";
    printf("startPattern is \t%s\n", startPattern);
    printf("endPattern   is \t%s\n", endPattern);
    setRange(buf, startPos, endPos, startPattern, endPattern);
    printf("\n");
    printf("startPos count is \t%ld\n", countTree(startPos));
    printf("endPos count is \t%ld\n", countTree(endPos));

    if(countTree(startPos)>1 && countTree(endPos)>1 && isValidRange(startPos, endPos)) {
      search2nd(dest, startPos, endPos, (char)((strlen(endPattern)-1)));
      H_TREE tmp = dest;
      size_t i = 0;
      while((tmp = hasNextTree(tmp)) != NULL) {
        char* cstr = (char*)treeValue(tmp);
        std::string str;
        if(i == 0) {
          str = R"({"item":")";
        } else {
          str = R"(,{"item":")";
        }
        str.append(cstr);
        str.append(R"("})");
        printf("%s\n", str.c_str());
        _dest.append(str);
        free((void*)cstr);
        i++;
      }
    }
    _dest.append("]}");     // JSON リスト構造の終わり
    clearTree(startPos, countTree(startPos));
    clearTree(endPos, countTree(endPos));
    clearTree(dest, countTree(dest));
    removeBuffer(buf);
  } catch(std::exception& e) {
    clearTree(startPos, countTree(startPos));
    clearTree(endPos, countTree(endPos));
    clearTree(dest, countTree(dest));
    removeBuffer(buf);
    throw std::runtime_error(e.what());
  }
}

int test_parseYahoo() {
  puts("=== test_parseYahoo");
  try {
    std::string dest;
    std::string filePath(WRITE_DIR);
    filePath += "yahoo/source.html"; 
    parseYahoo(dest, filePath);
    std::string target         = R"(href=)";
    std::string replacement    = R"(target="_blank" href=)";
    replaceAll(dest, target, replacement);
    nlohmann::json j(dest);
    ptr_lambda_debug<const char*, const std::string&>("j is ", j.dump());   // これで問題なく JSON 成形されていれば OK。問題があれば exception になる：）
    return EXIT_SUCCESS;
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    return EXIT_FAILURE;
  }
}

/**
 * ここまでで文字列解析のソースを宣言と定義に分けて、必要ならリファクタする。
 * 最終的な形は 1 つの JSON で 各外部サービスの検索結果をまとめるもの。
 * 現状は 1 プロセス 1 スレッドでよいが、各外部サービスは別スレッドにしたい。
 * 上記は、Paradise-Lost/REST-Jabberwocky で進めたほうがいいかもしれない。
*/

int main(void) {
    puts("START C/C++ 文字列解析 ===");
    if(0.01) {
      auto ret = 0;
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_debug_and_error());
      assert(ret == 1);
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_H_TREE());
      assert(ret == 0);
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_Result_List_JSON());
      assert(ret == 0);
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_Result_List_JSON_innerHtml());
      assert(ret == 0);
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_string_replace());
      assert(ret == 0);
    }
    if(0) {        // 1.00
      auto ret = 0;
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_searchProto());
      assert(ret == 0);
    }
    if(0) {        // 1.01
      auto ret = 0;
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_search2nd());
      assert(ret == 0);
    }
    if(1.02) {        // 1.02
      auto ret = 0;
      std::clock_t start_1 = clock();
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_requestYouTube());
      assert(ret == 0);
      std::clock_t start_2 = clock();
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_parseYouTube());
      assert(ret == 0);
      std::clock_t end = clock();
      std::cout << "passed: " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
      std::cout << "passed: " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
    }
    if(1.03) {      // 1.03
      auto ret = 0;
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_requestGoogle());
      assert(ret == 0);
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_parseGoogle());
      assert(ret == 0);
    }
    if(1.04) {      // 1.04
      auto ret = 0;
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_requestYahoo());
      assert(ret == 0);
      ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_parseYahoo());
      assert(ret == 0);
    }
    puts("===   C/C++ 文字列解析 END");
    return 0;
}
