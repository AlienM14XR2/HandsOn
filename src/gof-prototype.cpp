/**
    Prototype パターン

    まず、基本となるクラス群を用意してみる。
*/
#include <iostream>
#include <cassert>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};


template <class T>
class Prototype {
public:
    virtual ~Prototype() {} 
    virtual T clone() const = 0;
};

class Screw final : public virtual Prototype<Screw> {
    string type;
    int size;
public:
    Screw():type{""},size{-1} {}
    Screw(const string& t,const int& s) {
        type = t;
        size = s;
    }
    Screw(const Screw& own) {}
    ~Screw() {
        ptr_lambda_debug<const char*,const int&>("Called Screw Destructor.",0);
    }
    string getType() noexcept {
        return type;
    }
    int getSize() noexcept {
        return size;
    }
    virtual Screw clone() const override {
        Screw screw = Screw(type,size);
        return screw;
    }

};
int test_Screw() {
    cout << "--------------------------- test_Screw" << endl;
    Screw screw = Screw("test",10);
    Screw clone = screw.clone();
    cout << "screw is " << &screw << endl;
    cout << "clone is " << &clone << endl;
    assert( screw.getSize() == clone.getSize() );

    // Screw* screws = new Screw[5];
    // cout << "sizeof(*screws) is " << sizeof(*screws) << endl;
    Screw screws[5];
    cout << "sizeof(screws) is " << sizeof(screws) << endl;
    for(int i=0; i < 5 ; i++ ) {
        screws[i] = clone.clone();
    }
    cout << "screws[4].getSize() is " << screws[4].getSize() << endl;
    // delete [] screws;
    return 0;
}

int main() {
    cout << "START Prototype ===============" << endl;
    ptr_lambda_debug<const char*,const int&>("Here we go.",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ", test_Screw()); 
    }
    cout << "=============== Prototype END" << endl;
    return 0;
}