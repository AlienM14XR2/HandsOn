#include <stdio.h>
#include <stdlib.h>
#include "h_tree.h"


H_TREE createTree() {
//  puts("--------- createTree");
  struct tree* pt = (struct tree*)malloc(sizeof(struct tree));
  pt->value = NULL;
  pt->next = NULL;
  return (H_TREE)pt;
}

H_TREE hasNextTree(H_TREE _tree) {
  return (H_TREE)((struct tree*)_tree)->next;
}

H_TREE moveLast(H_TREE _tree) {
//  puts("--------- moveLast");
  struct tree* last = (struct tree*)_tree;
  while(last != NULL) {
    if(last->next != NULL) {
      last = last->next;
    } else {
      break;
    }
  }
//  printf("last addr is \t%p\n", (void*)last);
  return (H_TREE)last;
}

size_t countTree(H_TREE _root) {
//  puts("--------- countTree");
  struct tree* current = (struct tree*)_root;
  size_t i = 0;
  while((current = (struct tree*)hasNextTree((H_TREE)current)) != NULL) {
    i++;
  }
  i += 1;     // これは root も含めて返却している。
  return i;
}

void clearTree(H_TREE _root, size_t count) {  // このことは「肝に銘じよ」C 言語では削除する対象のサイズを先に計算しろ。
//  puts("--------- clearTree");
// H_TREE array[count];    // この配列の宣言のやり方は GCC 以外でもできるのかな？ 添字に変数を利用しているんだよね。
// 上記は g++ コンパイラでは NG だった。error: ISO C++ forbids variable length array ‘array’ [-Wvla]
  struct tree** array = (struct tree**)malloc(sizeof(struct tree**)*count);
  struct tree* current = (struct tree*)_root;
  array[0] = current;
  size_t i = 1;
  while((current = (struct tree*)hasNextTree((H_TREE)current)) != NULL) {
    array[i] = current;
    i++;
  }
  for(i=0; i<count; i++ ) {
    // puts("------------ D");
    free((void*)array[i]);
  }
  free((void*)array);
}

H_TREE pushTree(H_TREE _root, void* _value) {
//  puts("--------- pushTree");
  struct tree* pt = (struct tree*)malloc(sizeof(struct tree));
  pt->value = _value;
  pt->next = NULL;
  struct tree* last = (struct tree*)moveLast(_root);
  if(last == (struct tree*)_root) {
//      printf("------------ SAME\n");    
    ((struct tree*)_root)->next = pt;
  } else {
    last->next = pt;
  }
//  printf("pt addr is \t%p\n", (void*)pt);
  return (H_TREE)pt;
}

void* treeValue(H_TREE _current) {
//  puts("--------- treeValue");
  if(_current != NULL) {
    return ((struct tree*)_current)->value;
  } else {
    return NULL;
  }
}

void* popStack(H_TREE _root) {
//  puts("--------- popStack");
  // 前方アドレスの付け替え後、削除する。
  void* value = NULL;
  struct tree* last     = (struct tree*)moveLast(_root);
  struct tree* current  = (struct tree*)_root;
  if(current == last) {
    // ignore
    return NULL;
  } 
  else
  if(current->next == last) {
//    printf("------------ F2 SAME\n");
    value = last->value;
    current->next = NULL;
    free((void*)last);
  } 
  else {
    while((current = (struct tree*)hasNextTree((H_TREE)current)) != NULL) {
      if( ((struct tree*)current)->next == last ) {
//        printf("------------ SAME\n");
        value = last->value;
        ((struct tree*)current)->next = NULL;
        free((void*)last);
        break;
      }
    }
  }
  return value;
}

void* popQueue(H_TREE _root) {
//  puts("--------- popQueue");
  // 先頭の ツリーハンドラは起点なので無視する
  // 2 番目が対象であり、3 番目がある場合はそのアドレスを起点の next に代入する
  void* value = NULL;
  struct tree* second = ((struct tree*)_root)->next;
  if(second != NULL) {
    value                       = second->value;
    ((struct tree*)_root)->next = second->next;
    free((void*)second);
  }
  return value;
}
