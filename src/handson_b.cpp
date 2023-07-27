/**
    継承に関する疑問をはっきりとさせることが第一課題。
    インタフェース（Pure Virtual）、基底クラス、派生クラスにおける protected public の各メンバ関数の
    呼び出しを確認してみる。
*/
#include <iostream>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

int main(void) {
    cout << "START handson_b ===============" << endl;
    cout << "=============== handson_b END" << endl;
    return 0;
}

