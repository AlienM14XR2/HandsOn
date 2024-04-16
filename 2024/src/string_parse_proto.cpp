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
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc/ h_tree.c string_parse_proto.cpp -lcurl -o ../bin/main
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



/**
  指定したファイルサイズを取得する。
*/
size_t getFileSize(const char* fileName);

/**
  指定したメモリの開放を行う。
*/
void removeBuffer(char* _buffer);

/**
  ファイル情報の読み込みを行う。
*/
void readFile(const char* _filePath, char* _buf);

/**
  指定したパターンと区切り文字の間の文字列を取得する。
*/
void searchProto(H_TREE _dest, size_t _destSize, char* _buf, const char* _pattern, const char _limitCh);


/**
  指定したパターンの範囲を取得する。
*/
void setRange(char* _buf, H_TREE _startPositions, H_TREE _endPositions, const char* _startPattern, const char* _endPattern);

/**
  範囲の整合性を確かめる。
*/
bool isValidRange(H_TREE _startPos, H_TREE _endPos);

/**
  開始文字と終了文字のカウントを取得する。
*/
void checkChar(const char* _src, const char _start, const char _end, size_t* _startCount, size_t* _endCount);

/**
  文字の先頭に任意の文字を指定回数分追加する。
*/
void appendTop(H_TREE _dest, const char* _src, const char* _append, const size_t _chCount);

/**
  文字の終端に任意の文字を指定回数分追加する。
*/
void appendBottom(H_TREE _dest, const char* _src, const char* _append, const size_t _chCount);

/**
  指定した範囲の文字列を取得する。
*/
void search2nd(H_TREE _dest, H_TREE _startPos, H_TREE _endPos, char _beforeLimitPos);



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

int test_string_replace() {
  puts("=== test_string_replace");
  try {
    std::string src            = R"(<a class="Q71vJc" href="/search?sca_esv=e7ea9d64b74deda5&amp;ie=UTF-8&amp;q=%E5%A4%B1%E6%A5%BD%E5%9C%92+%E3%83%8D%E3%82%BF%E3%83%90%E3%83%AC&amp;sa=X&amp;ved=2ahUKEwiu5fT42cWFAxWBsVYBHXOLBkAQ1QJ6BAgBEAo" data-ved="2ahUKEwiu5fT42cWFAxWBsVYBHXOLBkAQ1QJ6BAgBEAo"><accordion-entry-search-icon><span class="ieB2Dd"><img class="OEaqif" alt="" src="data:image/gif;base64,R0lGODlhAQABAIAAAP///////yH5BAEKAAEALAAAAAABAAEAAAICTAEAOw==" style="max-width:20px;max-height:20px" id="dimg_15" data-deferred="1"></span></accordion-entry-search-icon><div class="kjGX2"><span class="Xe4YD"><div class="BNeawe s3v9rd AP7Wnd lRVwie">&#22833;&#27005;&#22290; &#12493;&#12479;&#12496;&#12524;</div></span></div></a>)";
    std::string target         = R"(href="/search?)";
    std::string replacement    = R"(href="https://www.google.com/search?)";
    std::string::size_type pos = 0;
    while((pos = src.find(target, pos)) != std::string::npos) {
      src.replace(pos, target.length(), replacement);
      pos += replacement.length();
    }
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
 * <div jsname="xQjRM">
 * ...
 * </div></div></div></div></div></div></div>
*/

bool parseGoogle(std::string& _dest, const std::string& _filePath) {
  puts("--- parseGoogle");
  char* buf       = NULL;
  H_TREE startPos = createTree();
  H_TREE endPos   = createTree();
  H_TREE dest     = createTree();
  _dest           = R"({"gList":[)";
  try {
    size_t size = getFileSize(_filePath.c_str());
    buf = (char*)malloc(size+2);
    memset(buf, '\0', size+2);
    readFile(_filePath.c_str(), buf);

    // 次のパターンで概ねデータの位置の特定は可能だが、もとデータが HTML であるため、startPos endPos の個数の完全一致は不可能だと感じた。
    // char startPattern[]    = "<div><div class=\"Gx5Zad fP1Qef xpd EtOod pkphOe\">";
    // char endPattern[]      = "</div></div></div></div></div></div></div></div>";
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

    size_t scount = countTree(startPos);
    size_t ecount = countTree(endPos);
    printf("scount is %ld\n", scount);
    printf("ecount is %ld\n", ecount);
    H_TREE stmp = startPos;
    H_TREE etmp = endPos;
    printf("\n");
    while((stmp = hasNextTree(stmp)) != NULL) {
      char* s = (char*)treeValue(stmp);
      etmp = hasNextTree(etmp);
      char* e = (char*)treeValue(etmp);
      printf("s : e = %p : %p\n", s, e);
      if( e > s ) {
        printf("s : e = %c : %c\n", *s, *e);
      }
    }
      /**
       * 上記のデバッグから分かったこと、setRange() 関数を新たに用意するか次の要件を満たす必要があるということかな。
       * startPos > endPos の場合は startPos を空ループで無視する必要がある startPos == endPos になるまで。
      */
    // printf("buf is %s\n", buf);
    if(isValidRange(startPos, endPos)) {
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
      H_TREE a_startPos        = createTree();
      H_TREE a_endPos          = createTree();
      H_TREE a_dest            = createTree();
      char   a_startPattern[]  = "<a ";
      char   a_endPattern[]    = "</a>";
      printf("a_startPattern is \t%s\n", a_startPattern);
      printf("a_endPattern   is \t%s\n", a_endPattern);
      setRange((char*)str.c_str(), a_startPos, a_endPos, a_startPattern, a_endPattern);
      printf("scount is %ld\n", countTree(a_startPos));
      printf("ecount is %ld\n", countTree(a_endPos));
      if(isValidRange(a_startPos, a_endPos)) {
        // search2nd(a_dest, a_startPos, a_endPos, (char)((strlen(endPattern)-1))*-1);
        search2nd(a_dest, a_startPos, a_endPos, (char)(3));
        tmp = a_dest;
        while((tmp = hasNextTree(tmp)) != NULL) {
          char* cstr = (char*)treeValue(tmp);
          printf("%s\n", cstr);
          free((void*)cstr);
        }
      }
      clearTree(a_startPos, countTree(a_startPos));
      clearTree(a_endPos, countTree(a_endPos));
      clearTree(a_dest, countTree(a_dest));
      /**
       * 通常の検索エンジンの利用で必要な情報は a タグのみと割り切ればこれでいいと思う。
       * 後はリファクタして、メモリの開放漏れがなければよいかと。
      */
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

int test_parseGoogle() {
  puts("=== test_parseGoogle");
  try {
    std::string dest;
    std::string filePath(WRITE_DIR);
    filePath += "google/source_4.html";
    bool ret = parseGoogle(dest, filePath);
    ptr_lambda_debug<const char*, const bool&>("ret is ", ret);
    return EXIT_SUCCESS;
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    return EXIT_FAILURE;
  }
}

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
    if(0) {        // 1.02
        auto ret = 0;
        std::clock_t start_1 = clock();
        // ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_requestYouTube());
        // assert(ret == 0);
        std::clock_t start_2 = clock();
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_parseYouTube());
        assert(ret == 0);
        std::clock_t end = clock();
        std::cout << "passed: " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        std::cout << "passed: " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
    }
    if(0) {      // 1.03
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_requestGoogle());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_parseGoogle());
        assert(ret == 0);
    }
    puts("===   C/C++ 文字列解析 END");
    return 0;
}






size_t getFileSize(const char* fileName)
{
  puts("--- getFileSize");
  struct stat st;
  if(stat(fileName, &st) != 0) {
    return 0ul;
  }
  printf("S_ISREG(st.st_mode) is \t%d\n", S_ISREG(st.st_mode));
  if(S_ISREG(st.st_mode)) {
    return st.st_size;
  } else {
    return 0ul;
  }
}

void removeBuffer(char* _buffer)
{
  puts("--- removeBuffer");
  free((void*)_buffer);
}

void readFile(const char* _filePath, char* _buf)
{
  puts("--- readFile");
  FILE*  fp          = NULL;
  char   tmp[10242] = "\0";
  size_t readSize    = 0;
    
  fp = fopen(_filePath, "r");
  if(fp != NULL) {    
    // ファイルの読み込みとメモリへの書き込み
    memset(tmp, '\0', 10242);
    while((readSize = fread(tmp, 1, 10240, fp)) != 0) {
      strcat(_buf, tmp);
      memset(tmp, '\0', 10242);  // この一行がなく、初期化できていなかったのが原因だった。
    }
    // printf("\n");
    fclose(fp);
  } else {
    // print_error("not file open.");
    // exit(1);
    throw std::runtime_error("file is not opend.");
  }
}

void searchProto(H_TREE _dest, size_t _destSize, char* _buf, const char* _pattern, const char _limitCh)
{
  puts("------ searchProto");
  if(_buf != NULL) {
    // size_t ptnSize  = strlen(_pattern);
    char* start     = &_buf[0];
    char* hitPos    = NULL;
    H_TREE root     = createTree();
    size_t hitCount = 0;
    // 検索文字（パターン）に該当する箇所（アドレス）の取得    
    do {
      hitPos = strstr(start, _pattern);
      if(hitPos != NULL) {
        // hitPos += ptnSize;
        pushTree(root, hitPos);
        start = hitPos + 1;
        hitCount++;
      }
    } while(hitPos != NULL);
    // debug_long("hitCount is ", &hitCount);
    ptr_lambda_debug<const char*, const decltype(hitCount)&>("hitCount is ", hitCount);
    // 実際の値を _limitCh より前で取得する
    H_TREE tmp     = root;
    size_t i = 0;
    size_t limitSize = ((sizeof(char))*_destSize) - 1;
    while((tmp = hasNextTree(tmp)) != NULL) {
      char* str = (char*)malloc((sizeof(char))*_destSize);  // char は 1 byte だけどね。
      char* searchPos = (char*)treeValue(tmp);
      memset(str, '\0', (sizeof(char))*_destSize);
      i = 0;
      while(1) {
        if(*searchPos != _limitCh) {
          str[i] = *searchPos;
        }
        if(i == limitSize || *searchPos == _limitCh) {
          str[i] = *searchPos;
          pushTree(_dest, str);
          break;
        }
        searchPos++;
        i++;
      }
    }
    printf("\n");    
    clearTree(root, countTree(root));
  } else {
    printf("_buf is null.\n");
  }
}

void setRange(char* _buf, H_TREE _startPositions, H_TREE _endPositions, const char* _startPattern, const char* _endPattern)
{
  puts("------ setRange");
  if(_buf != NULL) {
    char*  start     = &_buf[0];
    char*  hitPos    = NULL;
    char*  firstPos  = NULL;
    // 先頭検索文字（パターン）に該当する箇所（アドレス）の取得
    do {
      hitPos = strstr(start, _startPattern);
      if(hitPos != NULL) {
        if(firstPos == NULL) {
          firstPos = hitPos;
        }
        pushTree(_startPositions, hitPos);
        start = hitPos + 1;
      }
    } while(hitPos != NULL);
    // 終端検索文字（パターン）に該当する箇所（アドレス）の取得 
    start     = &_buf[0];
    // start     = firstPos;
    hitPos    = NULL;
    do {
      hitPos = strstr(start, _endPattern);
      if(hitPos != NULL) {
        pushTree(_endPositions, hitPos);
        start = hitPos + 1;
      }
    } while(hitPos != NULL);
  } else {
    printf("_buf is null.\n");
  }
}

bool isValidRange(H_TREE _startPos, H_TREE _endPos)
{
  puts("------ isValidRange");
  if( countTree(_startPos) == countTree(_endPos) ) {
    puts("start end 個数は同じ");
    H_TREE stmp = _startPos;
    H_TREE etmp = _endPos;
    while((stmp = hasNextTree(stmp)) != NULL) {
      char* scp = (char*)treeValue(stmp);
      printf("%p\t%c", scp, *scp);
      etmp = hasNextTree(etmp);
      char* ecp = (char*)treeValue(etmp);
      printf("%p\t%c", ecp, *ecp);
      if(scp >= ecp) {
        printf("ERROR: addr ecp < scp");
        return false;
      }
    }
    return true;
  }
  return false;
}

void checkChar(const char* _src, const char _start, const char _end, size_t* _startCount, size_t* _endCount)
{
  puts("------ checkChar");
  size_t len = strlen(_src);
  printf("_src size is %ld\n", len);
  for(size_t i=0; i<len ;i++) {
    if(_src[i] == _start) {
      (*_startCount)++;
    }
    else if(_src[i] == _end) {
      (*_endCount)++;
    }
  }
}

void appendTop(H_TREE _dest, const char* _src, const char* _append, const size_t _chCount)
{
  puts("------ appendTop");
  // C 言語での文字列操作、その際のサイズ計算は面倒だし、多少サイズに余裕を持たせるようにする（ヌル文字を忘れる：）。  
  size_t size = strlen(_src) + (strlen(_append) * 2)+1;
  printf("size is %ld\n", size);
  char* str = (char*)malloc(sizeof(char)*size);
  memset(str, '\0', sizeof(char)*size);
  for(size_t i=0; i<_chCount; i++) {
    strcat(str, _append);
  }
  strcat(str, _src);
  pushTree(_dest, str);
}

void appendBottom(H_TREE _dest, const char* _src, const char* _append, const size_t _chCount)
{
  puts("------ appendBottom");
  size_t size = strlen(_src) + (strlen(_append) * 2)+1;
  printf("size is %ld\n", size);
  char* str = (char*)malloc(sizeof(char)*size);
  memset(str, '\0', sizeof(char)*size);
  strcat(str, _src);
  for(size_t i=0; i<_chCount; i++) {
    strcat(str, _append);
  }
  pushTree(_dest, str);
}

void search2nd(H_TREE _dest, H_TREE _startPos, H_TREE _endPos, char _beforeLimitPos)
{
  puts("------ search2nd");
  char* start      = NULL;
  char* end        = NULL;
  size_t sz = countTree(_startPos);       // countTree() は 要素数 + 1 を返却する。 +1 は H_TREE の根（root）。  
  ptr_lambda_debug<const char*, const decltype(sz)&>("sz is ", sz);  
  for(size_t i=0 ; i<(sz-1); i++) {
    start    = (char*)popQueue(_startPos);
    end      = (char*)popQueue(_endPos);
    end += _beforeLimitPos;               // limitCh の直前まで移動
    printf("(end-start) size is %ld\n", end - start);
    size_t msz = (end - start) + 2;       // 余分にバイト数を確保する場合は、必ず 2 の倍数で確保すること。
    char* str = (char*)malloc(sizeof(char)*msz);
    memset(str, '\0', sizeof(char)*msz);
    size_t j = 0;
    while(1) {
      str[j] = *start;
      if(start == end) {
        pushTree(_dest, str);
        break;
      }
      start++;
      j++;
    }
  }
}
