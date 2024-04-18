#ifndef _STRINGPARSE_H_
#define _STRINGPARSE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <cstring>
#include "h_tree.h"


/**
  指定したファイルサイズを取得する。

  fileName: ファイルのフルパス
*/
size_t getFileSize(const char* fileName);

/**
  指定したメモリの開放を行う。

  _buffer: 開放するメモリ
*/
void removeBuffer(char* _buffer);

/**
  テキストファイルの読み込みを行う。

  _filePath: ファイルのフルパス
  _buf: 読み込んだ情報を保持する
*/
void readFile(const char* _filePath, char* _buf);

/**
  指定したパターンと区切り文字の間の文字列を取得する。

  _dest: 検索結果を保持するリスト
  _destSize: 一度に走査するサイズ
  _buf: もどデータ
  _pattern: 検索する文字列
  _limitCh: 終端文字
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

  _dest: 開始、終了位置で切り出した文字列を保存するリスト
  _startPos: 抽出開始位置のリスト
  _endPos: 抽出終了位置のリスト
  _beforeLimitPos: 終端位置
*/
void search2nd(H_TREE _dest, H_TREE _startPos, H_TREE _endPos, char _beforeLimitPos);

#ifdef __cplusplus
}
#endif

#endif
