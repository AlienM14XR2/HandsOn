#include "string_parse.h"

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
    printf("ERROR: not file open.\n");
    exit(1);
    // throw std::runtime_error("file is not opend.");
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
    // ptr_lambda_debug<const char*, const decltype(hitCount)&>("hitCount is ", hitCount);
    printf("hitCount is %lu\n", hitCount);
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
  // ptr_lambda_debug<const char*, const decltype(sz)&>("sz is ", sz);
  printf("sz is %lu\n", sz);
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
