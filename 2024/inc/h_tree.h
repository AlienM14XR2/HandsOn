#ifndef HTREE_H_
#define HTREE_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
  Linked List 
  
  void* を利用したものを考えてみる。
    データ構造は  value と next で表現できるはず。
  push() で追加、pop() で削除する。
*/

struct tree {
  void* value;
  struct tree* next;
};

typedef void* H_TREE;

/**
    ツリーハンドラの作成。
    内部で struct tree を動的作成（malloc）している。
    利用後は必ず、clearTree() を使用すること。
    tree.value に設定する値は自由で、動的に取得したメモリでも構わないが
    その解放は、ツリーハンドラでは行わないので注意が必要。  
*/
H_TREE createTree();

/**
    ツリーハンドラの次の値を返却する。
  
  _tree： カレントツリー
*/
H_TREE hasNextTree(H_TREE _tree);

/**
    ツリーハンドラのポインタを最後尾に移動する。
  
  _tree： カレントツリー
*/
H_TREE moveLast(H_TREE _tree);

/**
    ツリーハンドラの要素数と根の合計数を返却する。
    根（root）は要素を持たないツリーだが、countTree() は根も
    含めてそのサイズを返却する。つまり、その値は根 + 要素数と
    なる。
  
  _root： ルートツリー
*/
size_t countTree(H_TREE _root);

/**
    ツリーハンドラの全要素の削除を行う。
    根も削除する。
    ※ もし、H_TREE が管理する value が動的にメモリ確保されたもので、H_TREE 以外そのアドレスを知り得ない場合
    この関数を value 解放前に行うとメモリリークを引き起こす：）
  
  _root： ルートツリー
  count： ルートツリーを含めた、全要素数
*/
void clearTree(H_TREE _root, size_t count);

/**
    ツリーハンドラに値を追加する。
  
  _root： ルートツリー
  value： ツリーに追加する値
*/
H_TREE pushTree(H_TREE _root, void* value);

/**
    ツリーハンドラに設定した値の取得。
  
  _current： カレントツリー
*/
void* treeValue(H_TREE _current);

/**
    最後尾にある値の取り出しとそのツリーハンドラの削除を行う。
  Stack と同じ動作、最初にツリーハンドラに登録した値は、最後に取り出される（First-in Last-out）。
  
  _root： ルートツリー
*/
void* popStack(H_TREE _root);

/**
    起点の次、仮の先頭の値の取り出しとそのツリーハンドラの削除を行う。
  queue と同じ動作（First-in First-out）。
    
  _root： ルートツリー
*/
void* popQueue(H_TREE _root);

#ifdef __cplusplus
}
#endif

#endif
