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
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc/ h_tree.c string_parse_proto.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <cstring>
#include <chrono>
#include <nlohmann/json.hpp>
#include <sys/stat.h>

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
  puts("test_Result_List_JSON");
  try {
    std::string p1 = R"({"personData": {"name": "Jojo","email": "jojo@loki.org","age": 24}})";
    std::string p2 = R"({"personData": {"name": "Alpha","email": "alpha@loki.org","age": 30}})";
    nlohmann::json res;
    res["list"] = {};
    res["list"][0] = p1;
    res["list"][1] = p2;
    ptr_lambda_debug<const char*, const std::string&>("res is ", res.dump());
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

int parseYouTube(nlohmann::json* _dest, const std::string& _filePath) {
  puts("--- parseYouTube");
  char* buf       = NULL;
  H_TREE startPos = createTree();
  H_TREE endPos   = createTree();
  H_TREE dest     = createTree();
  H_TREE fix      = createTree();
  try {
    size_t size = getFileSize(_filePath.c_str());
    buf = (char*)malloc(size+1);
    memset(buf, '\0', size+1);
    readFile(_filePath.c_str(), buf);

    char startPattern[]    = "{\"videoRenderer\":";
    char endPattern[]      = "\"}}}]},\"shortBylineText\"";   // endPattern の中に必ず終端を表現する文字があること。この場合は ','
    printf("startPattern is \t%s\n", startPattern);
    printf("endPattern   is \t%s\n", endPattern);
    char appendT[] = "{";
    char appendB[] = "}";
    setRange(buf, startPos, endPos, startPattern, endPattern);

    if(isValidRange(startPos, endPos)) {
      search2nd(dest, startPos, endPos, 5);
      H_TREE tmp = dest;
      while((tmp = hasNextTree(tmp)) != NULL) {
        char* str = (char*)treeValue(tmp);          // BAD KNOW-HOW ここで pop してはいけない（理由が知りたければ試してみてくれ：）
        size_t s = 0;
        size_t e = 0;
        checkChar(str, '{', '}', &s, &e);
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
        // TODO ここで最終的に返却する JSON にする必要がある。
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
    return EXIT_SUCCESS;
  } catch(std::exception& e) {
    ptr_print_error<const decltype(e)&>(e);
    clearTree(startPos, countTree(startPos));
    clearTree(endPos, countTree(endPos));
    clearTree(dest, countTree(dest));
    clearTree(fix, countTree(fix));
    removeBuffer(buf);
    return EXIT_FAILURE;
  }
}

int test_parseYouTube() {
  puts("=== test_parseYouTube");
  try {
    nlohmann::json dest;
    std::string filePath("/home/jack/tmp/sample.html");
    return parseYouTube(&dest, filePath);
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
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_Result_List_JSON());
        assert(ret == 0);
        // ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_parseYouTube());
        // assert(ret == 0);
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
  char   tmp[10241] = "\0";
  size_t readSize    = 0;
    
  fp = fopen(_filePath, "r");
  if(fp != NULL) {    
    // ファイルの読み込みとメモリへの書き込み
    memset(tmp, '\0', 10241);
    while((readSize = fread(tmp, 1, 10240, fp)) != 0) {
      strcat(_buf, tmp);
      memset(tmp, '\0', 10241);  // この一行がなく、初期化できていなかったのが原因だった。
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
    size_t ptnSize  = strlen(_pattern);
    char* start     = &_buf[0];
    char* hitPos    = NULL;
    H_TREE root     = createTree();
    size_t hitCount = 0;
    // 検索文字（パターン）に該当する箇所（アドレス）の取得    
    do {
      hitPos = strstr(start, _pattern);
      if(hitPos != NULL) {
        hitPos += ptnSize;
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
    // 先頭検索文字（パターン）に該当する箇所（アドレス）の取得
    do {
      hitPos = strstr(start, _startPattern);
      if(hitPos != NULL) {
        pushTree(_startPositions, hitPos);
        start = hitPos + 1;
      }
    } while(hitPos != NULL);
    // 終端検索文字（パターン）に該当する箇所（アドレス）の取得 
    start     = &_buf[0];
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
//   debug_long("sz is ", &sz);
  ptr_lambda_debug<const char*, const decltype(sz)&>("sz is ", sz);  
  for(size_t i=0 ; i<(sz-1); i++) {
    start    = (char*)popQueue(_startPos);
    end      = (char*)popQueue(_endPos);
    end += _beforeLimitPos;               // limitCh の直前まで移動
    printf("(end-start) size is %ld\n", end - start);
    size_t msz = (end - start) + 1;
    char* str = (char*)malloc(sizeof(char)*msz);
    memset(str, '\0', sizeof(char)*msz);
    size_t j = 0;
    size_t jlimit = msz-1;
    while(1) {
      str[j] = *start;
      if(j >= jlimit || start == end) {
        pushTree(_dest, str);
        break;
      }
      start++;
      j++;
    }
  }
}
