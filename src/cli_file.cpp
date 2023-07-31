/**
 * 「ORx2」のFILE 処理の実験場です。
 * 気になることをまず挙げてみる。
 * 
 * - トランザクション（ファイルのLock）。
 * - 具体的なクラスとその処理。
 * 
 * 手始めにファイルのOpen Close をクラスで表現してみる。
 * コンストラクタの引数でファイルのフルパスあるいはシステムの相対パスを受付てファイルをOpen する。
 * デストラクタで確保したリソースの解放とファイルのClose を行う、こんなものをイメージしている。
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

class ORx2File final {
private:
    string filePath;
    ORx2File():filePath{""}{}
public:
    ORx2File(const string& fpath) {
        filePath = fpath;
    }
    ORx2File(const ORx2File& own) {
        (*this) = own;
    }
    ~ORx2File() {}
};

int main(void) {
    cout << "START cli_file ===============" << endl;
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Yeah!!! Have Happy Summer Vacation :)",0);
    }
    cout << "=============== cli_file END" << endl;
    return 0;
}
