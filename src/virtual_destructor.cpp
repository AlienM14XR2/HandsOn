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
    string& getName() {
        return name;
    }
    Component(const string& name) {
        this->name = name;
    }
    virtual ~Component() {
        cout << "Debug. Component Destructor." << endl;
    }
};
class Leaf:public Component {
public:
    Leaf(){}
    Leaf(const string& name) {
        this->name = name;
    }
};
class Composite:public Component {
    int index = -1;
    Leaf* pl;
public:
    Leaf pop(const int& i) {
        return (pl[i]);
    }
    void push(const Leaf& lf) {
        pl[index] = lf;
        ++index;
    }
    Composite(const string& name, const int& lfSize) {
        index = 0;
        this->name = name;
        pl = new Leaf[lfSize];
    }
    // Composite(const string& name, const Leaf& lef) {
    //     this->name = name;
    //     leaf = lef;
    // }
    ~Composite() {
        cout << "Debug. == Composite Destructor." << endl;
        delete [] pl;
    }
};

void test_string();
int main() {
    cout << "START virtual_destructor ========== " << endl;
    // string name("Alice");
    // string cheshire("Cheshire cat");
    // Leaf cat(cheshire);
    // Composite alice(name,cat);
    // cout << "alice.getName() is " << alice.getName() << endl;

    string name2("Alice 2");
    Composite alice2(name2,3);
    cout << "alice2.getName() is " << alice2.getName() << endl;
    string humptyName("Humptydumpty");
    Leaf humpty(humptyName);
    alice2.push(humpty);

    string jabberName("Jabberwocky");
    Leaf jabber(jabberName);
    alice2.push(jabber);

    string lionName("Lion");
    Leaf lion(lionName);
    alice2.push(lion);

    cout << "alice2.pop(0).getName() is " << alice2.pop(0).getName() << endl;
    cout << "alice2.pop(1).getName() is " << alice2.pop(1).getName() << endl;
    cout << "alice2.pop(2).getName() is " << alice2.pop(2).getName() << endl;
    
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


