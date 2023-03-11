/**
 　 ここにC++ virtual に関する考察が述べられている、読むべし。
    https://www.yunabe.jp/docs/cpp_virtual_destructor.html

Virtual、C++の仮想関数がそのクラスに定義されているということは、そのクラスは
継承した子クラスを作成し、その子クラスのインスタンスは親クラスの
ポインタに格納して利用するはず、といっているが。

うん、確かにそうゆうパターンもあるだろうね。

もし、そのクラスが、継承されないのであれば、Virtualにするのは、
vtableへのポインタの分だけインスタンスのサイズが増え、vtableを利用する
分だけ遅くなるといっている。

オレの結論。
だから、必ず、デストラクタをVirtualにしろとは、リファレンス先生は
言っていないと。

備考ね。
暗黙的にデストラクタが作られると、ここのサイトは、言っている。

サンプルはなんにしようかね。
コンポジットでいいか。

*/

#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

/**
 * コンポジットの基底クラス
*/
class Component {
    // Component(){}
protected:
    string name;
    Component(){}
public:
    string getName() {
        return name;
    }
    Component(const string& name) {
        this->name = name;
    }
    virtual ~Component() {
        cout << "Debug. Component Destructor." << endl;
    }
};
class Leaf:Component {
public:
    Leaf(){}
    Leaf(const string& name) {
        this->name = name;
    }
};
class Composite:public Component {
    // これをポインタで管理したい、配列でもいいけど。
    Leaf leaf;
public:
    Composite(const string& name, const Leaf& lef) {
        this->name = name;
        leaf = lef;
    }
};

void test_string();
int main() {
    cout << "START virtual_destructor ========== " << endl;
    string name("Alice");
    string cheshire("Cheshire cat");
    Leaf cat(cheshire);
    Composite alice(name,cat);
    cout << "alice.getName() is " << alice.getName() << endl;
    // test_string();
    cout << "virtual_destructor ========== END " << endl;
    return 0;
}
class Person {
    string name;
public:
    Person(const string& name) {
        this->name;
    }
};
void test_string() {
    cout << "test_string ----------- " << endl;
    string name("derek");
    cout << "name is " << name << endl;
    Person derek(name);
}


