/**
 * 11 章 Proxy パターン（保護プロキシ）
 * 
 * RMI は私には C++ で書けない（@see chapter_11_proxy.cpp）。
 * ここでは、サンプルの「保護プロキシ」について書いてみる。
 * 
 * GEEK matchmaking ... これが課題のサンプル。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_11b_proxy.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug)-> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * Person インタフェース
*/
class Person {
public:
    virtual ~Person() {}

    virtual string getName() const = 0;
    virtual string getGender() const = 0;
    virtual string getInterests() const = 0;
    virtual int getGeekRating() const = 0;

    virtual void setName(const string& n) const = 0;
    virtual void setGender(const string& g) const = 0;
    virtual void setInterests(const string& i) const = 0;
    virtual void setGeekRating(const int& r) const = 0;
};

class PersonImpl final : public virtual Person {
private:
    mutable string name = "";
    mutable string gender = "";
    mutable string interests = "";
    mutable int geekRating = 0;         // 1 - 10
    mutable int ratingCount = 0;
public:
    PersonImpl() {}
    PersonImpl(const PersonImpl& own) {*this = own;}
    ~PersonImpl() {}

    virtual string getName() const override {
        return name;
    }
    virtual string getGender() const override {
        return gender;
    }
    virtual string getInterests() const override {
        return interests;
    }
    virtual int getGeekRating() const override {
        if(ratingCount == 0) return 0;              // ギーク度を ratingCount で割ってギーク度の平均を計算します。
        return (geekRating/ratingCount); 
    }

    virtual void setName(const string& n) const override {
        name = n;
    }
    virtual void setGender(const string& g) const override {
        gender = g;
    }
    virtual void setInterests(const string& i) const override {
        interests = i;
    }
    virtual void setGeekRating(const int& r) const override {
        geekRating = r;
        ratingCount++;
    }

};
// TODO test

/**
 * 呼び出しハンドラの作成
 * - 所有者 geekRating 以外の変更が可能な者。
 * - 非所有者 geekRating のみ変更が可能な者。
 * 
 * 上記 2 つを作成する必要がある。
 * それらが、保護プロキシと呼ばれるもの。
 * 
 * あぁ、Java だからね、サンプルは。
 * java.lang.reflect.* パッケージを利用するのか。
 * これは、関数ポインタでどうにかなるかな？
 * これも保留だな：）
*/

int main(void) {
    puts("START 11 章 Proxy パターン（保護プロキシ）===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double>("pi is ",pi);
    }
    puts("=== 11 章 Proxy パターン（保護プロキシ）END");
    return 0;
}