/**
 * 「ORx2」のFILE 処理の実験場です。
 * 気になることをまず挙げてみる。
 * 
 * - トランザクション（ファイルのLock）。
 * - 具体的なクラスとその処理。
 * 
 * @author jack
*/
#include <iostream>
#include "stdio.h"
#include "stdlib.h"

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

int main(void) {
    cout << "START cli_file ===============" << endl;
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Yeah!!! Have Happy Summer Vacation :)",0);
    }
    cout << "=============== cli_file END" << endl;
    return 0;
}
