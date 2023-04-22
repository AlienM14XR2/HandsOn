/**
    GoF Bridge

ハンドルと実装を分離して、それぞれを独立して拡張できる。

これは、使ったことないし。何かのライブラリで使ったことがあるかもしれないが。
うん、意識したことはないかな。

- 基本的な形から。
    - interface Implementer
    - Implementer の派生クラス（複数可）
    - Abstraction（ハンドル） メンバ変数に Impementer 型のオブジェクトを持つ
    - Abstraction の派生クラス

- いいイメージ、アイデアがあれば、自分で応用してみる。
*/
#include <iostream>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message,auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

int main() {
    cout << "START Bridge ===============" << endl;
    cout << "=============== Bridge END" << endl;
}