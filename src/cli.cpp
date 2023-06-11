/**
    CLI その学習と実践。

    cli.c で始めたORx2 開発の足がかり、その発展と考えている。
    今後の開発はC++ も利用するが、ベースはC で行う予定。まずは、いつものテンプレソースファイルを用意し
    その後cli.c で有効だった関数を移植する予定。

*/
#include <iostream>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};
// C のprintf が優秀なのでほしい、そのデバッグも利用したい：）

int main(void) {
    cout << "START CLI ===============" << endl;
    if(1.0) {
        ptr_lambda_debug<const string&,const int&>("Yeah here we go.",0);
    }
    cout << "=============== CLI END" << endl;
    return 0;
}