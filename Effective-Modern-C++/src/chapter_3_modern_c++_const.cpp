/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 16 ：const メンバ関数はスレッドセーフにする
 * 
 * 重要ポイント
 * - 並行実行はあり得ないことが『確実でもない限り』、const メンバ関数はスレッドセーフにする。
 * 
 * - std::atomic 変数を用いれば mutex よりも性能を改善できる可能性がある。しかし、適しているのは
 *   単独の変数やメモリ領域を操作する場合のみである。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_const.cpp -o ../bin/main
*/
#include <iostream>
#include <vector>
#include <mutex>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int test_debug() {
    puts("--- test_debug");
    try {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

class Prime {
public:
    using PrimeNumbers = std::vector<int>;
    /**
     * n までの素数を求める。
     * 
     * 書籍に沿ったスレッドセーフの学習なので無駄のある実装
     * （メンバ変数を const 修飾したメンバ関数で変更）をしている。
     * 私が考える一番単純なスレッドセーフの考え方は、スタック（関数内宣言された変数）のみを利用することだが。
     * それも本当に合っているのか自信はない（少なくともコンパイラに !readable、変更を行っている旨のエラーはない：）。
    */
    PrimeNumbers compute(const int n) const {       // メンバ関数を const 修飾するということは、メンバ変数は read-only であるべき、これが基本的な考え方のはず。
        puts("------ Prime::compute");
        std::lock_guard<std::mutex> guard(m);           // lock mutex
        if(n <= 1) { return primeNumbers; }
        int counter = 0;
        for(int i=2 ; i<=n; i++) {
            for(int j=2; j<=i; j++) {
                if( i%j == 0){
                    counter++;
                    if(counter >= 2) {
                        break;
                    }
                }
            }
            if(counter == 1) {
                primeNumbers.push_back(i);
            }
            counter = 0;
        } 
        return primeNumbers;
    }                                               // unlock mutex
    PrimeNumbers getPrimeNumbers() noexcept { return primeNumbers; }
private:
    mutable std::mutex m;
    mutable PrimeNumbers primeNumbers{};
};

int test_Prime() {
    puts("--- test_Prime");
    try {
        Prime prime;
        auto primeNumbers = prime.compute(100);
        ptr_lambda_debug<const char*,const size_t&>("sum is ", primeNumbers.size());
        for(auto i: primeNumbers) {
            ptr_lambda_debug<const char*,const decltype(i)&>("i is prime number ... ", i);
        }
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 項目 16 ：const メンバ関数はスレッドセーフにする ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug());
    }
    if(1.00) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_Prime());
    }
    puts("=== 項目 16 ：const メンバ関数はスレッドセーフにする END");
    return 0;
}