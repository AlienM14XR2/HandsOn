/**
 * 9 章 Iterator パターンと Composite パターン
 * 
 * 適切に管理されたコレクション
 * 
 * Iterator パターンとの出会い
 * 反復処理のカプセル化計画は、実際にうまくいきそうです。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_9_iterator.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

template <class T>
class Iterator {
public:
    virtual ~Iterator() {}
    virtual bool hasNext() const = 0;
    virtual T next() const = 0;
    virtual void remove() const = 0;    // 必要がない場合は、サブクラスでは Exception を Throw すること。
};

template <class T>
class DinerMenuIterator final : public virtual Iterator<T> {
private:
    mutable T* array;
    mutable size_t position = 0;
    mutable size_t length = 0;
    DinerMenuIterator():position{0},length{0},array{nullptr} {}
public:
    DinerMenuIterator(T* a, const size_t& l) {
        position = 0;
        length = l;
        array = a;
    }
    DinerMenuIterator(const DinerMenuIterator& own) {*this = own;}
    ~DinerMenuIterator() {}

    virtual bool hasNext() const override {
        if(position >= length) {
            return false; 
        } else {
            return true;
        }
    }
    virtual T next() const override {
            T nextValue = array[position];
            position++;
            return nextValue;
    }
    virtual void remove() const override {
        // このクラスで扱っているいるのは総称型のポインタであり、開放の責務はないものとする。
        // したがって、position と length には 0 を代入して、array には nullptr を代入する。
        position = 0;
        length = 0;
        array = nullptr;
    }
};

class Menu final {
private:
    int price;
    string name;
    Menu():price{-1},name{""} {}
public:
    Menu(const int& p, const string& n) {
        price = p;
        name = n;
    }
    Menu(const Menu& own) {*this = own;}
    ~Menu() {}

    int& getPrice() {
        return price;
    }
    string& getName() {
        return name;
    }
};

int test_DinerMenuIterator() {
    puts("--- test_DinerMenuIterator");
    try {
        string menu[] = {"ビーフ","チキン","魚","パスタ","ライス"};
        size_t length = sizeof(menu)/sizeof(menu[0]);
        DinerMenuIterator<string> iterator(menu,length);
        while(iterator.hasNext()) {
            ptr_lambda_debug<const char*, const string&>("menu is ",iterator.next());
        }
        iterator.remove();
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int test_DinerMenuIterator_V2() {
    puts("--- test_DinerMenuIterator_V2");
    try {
        Menu menu[] = {{2500,"ビーフ"},{2000,"チキン"},{1500,"魚"},{1250,"パスタ"},{1000,"ライス"}};
        size_t length = sizeof(menu)/sizeof(menu[0]);
        DinerMenuIterator<Menu> iterator(menu,length);
        while(iterator.hasNext()) {
            Menu m = iterator.next();
            ptr_lambda_debug<const char*,const int&>("price is ",m.getPrice());
            ptr_lambda_debug<const char*,const string&>("name is ",m.getName());
        }
        iterator.remove();
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

// int test_array_length(int* arr) {
//     puts("--- test_array_length");
//     try {
//         size_t length = sizeof(arr) / sizeof(arr[0]);
//         ptr_lambda_debug<const char*,const size_t&>("length is ",length);
//         return 0;
//     } catch(exception& e) {
//         cout << e.what() << endl;
//         return -1;
//     }
// }

int main(void) {
    puts("START 9 章 Iterator パターン ===");
    if(0.1) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(0.11) {
        int t[] = {0,1,2,3,4};
        size_t length = sizeof(t) / sizeof(t[0]);
        ptr_lambda_debug<const char*,const size_t&>("length is ",length);
        // ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_array_length(t));
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_DinerMenuIterator());
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_DinerMenuIterator_V2());
    }
    puts("=== 9 章 Iterator パターン END");
    return 0;
}