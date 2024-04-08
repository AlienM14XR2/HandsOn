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


int main(void) {
    puts("START C/C++ 文字列解析 ===");
    if(0.01) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_H_TREE());
        assert(ret == 0);
    }
    puts("===   C/C++ 文字列解析 END");
    return 0;
}
