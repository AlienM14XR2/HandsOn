/**
 * 多重継承だ。
 * 
 * これは、適当なデザインパターンが浮かばなかったのと
 * 長らく、多重継承は良くないという文化にいたので
 * あっさり済ませる。
 * 
 * 次のようなものを作ろうと思う。
 * 四則演算を持つクラス、それらを多重継承した派生クラス。
 * 
*/
#include <iostream>

using namespace std;

class Addition {
public:
    int add(const int& n1, const int& n2) {
        return (n1+n2);
    }
};
class Multiplication {
public:
    int multi(const int& n1, const int& n2) {
        return (n1*n2);
    }
};
// リファレンスはクラスの説明でも頑なに struct を使ってるから、
// 次の書き方ではない。
// class の場合は public のアクセス指定子を忘れないように。
class Calculator : public Addition, public Multiplication {
};

int main() {
    cout << "START ========== " << endl;
    Calculator calc;
    int ret = calc.add(3,6);
    cout << "ret is " << ret << endl;
    ret = calc.multi(3,6);
    cout << "ret is " << ret << endl;
    cout << "========== END " << endl;
    return 0;
}
// はい、終わり。
// class Calculator : public Addition, public Multiplication {}
// この文法だけ分かればいい。