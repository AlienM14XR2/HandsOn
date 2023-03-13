/**
 * 関数テンプレート
 * 
 * クラステンプレートはなんとなく使ってしまったが、関数テンプレートはまだ、
 * 未実装だね。
   なんか、GoF使ってみたいな。

   読むべし。
   https://debimate.jp/2021/01/07/%E3%80%90%E3%82%AA%E3%83%96%E3%82%B8%E3%82%A7%E3%82%AF%E3%83%88%E6%8C%87%E5%90%91%E3%80%91%E5%85%A823%E7%A8%AE%E9%A1%9E%E3%81%AEgof%E3%83%87%E3%82%B6%E3%82%A4%E3%83%B3%E3%83%91%E3%82%BF%E3%83%BC/

   Template Method　そのままのがあるね。無論今回の関数テンプレートとGoFの
   それは違うけど。
   
   これにしようかな。
   少し写経が続いたから、気分転換にもなる。
   自由に作れる楽しさと責任を味わおう。

   Cooking でもしてみるか。
   Soup の種類により、出来上がる品物が変化する。
   Curry, WhiteSource, water こんなんでどうか。

   // TODO 関数テンプレート
   template <class T>
   T foo(const T& t) {return t * t;}
   こんなんが、今回のメインですからね。
   クドいけど GoF はメインではないよ。
*/
#include <iostream>
#include <cassert>

using namespace std;

// スコープ付き列挙型。
enum struct Element {
    Water,
    WhiteSource,
    Curry
};

struct Soup {
  Element element;
  Soup(const Element& elem) {
    element = elem;
  }
  void toString() {
    // ストラテジーか、いいだろここは、飽きたろ、いいかげんにしなさいよ:)
    switch(element) {
      case Element::Curry:
        cout << "Yeah I'm Curry Soup." << endl;break;
      case Element::WhiteSource:
        cout << "Yeah I'm White Crean Soup." << endl;break;
      case Element::Water:
        cout << "orz I'm Hot Water." << endl;break;
    }
  }
};

template <class S=Soup>
class Cooking {
public:
  virtual void pureVirCut() const = 0;
  virtual S pureVirSimmer() const = 0;
  virtual void cook() final {   // これで、派生クラスからの override ができないはず。
    cout << "start cook. ---------- " << endl;
    pureVirCut();
    S soup = pureVirSimmer();
    soup.toString();
  }
  virtual ~Cooking(){}
};

class CurryCock : public Cooking<Soup> {
public:
  void pureVirCut() const override {
    cout << "ニンジン、玉ねぎ、ジャガイモをカットします。" << endl;
  }
  Soup pureVirSimmer() const override {
    return Soup(Element::Curry);
  }
};

class WhiteCreamSoupCock : public Cooking<Soup> {
public:
  void pureVirCut() const override {
    cout << "マカロニ、タラ（魚）、ニンジン、ジャガイモをカットします。" << endl;
  }
  Soup pureVirSimmer() const override {
    return Soup(Element::WhiteSource);
  }
};

class Wife : public Cooking<Soup> {
public:
  void pureVirCut() const override {
    cout << "あら大変、指きっちゃいました。" << endl;
  }
  Soup pureVirSimmer() const override {
    return Soup(Element::Water);
  }
};

void test_Curry_Cock() {
  cout << "---------------- test_Curry_Cock " << endl;
  CurryCock cock;
  cock.cook();
}

void test_White_Cream_Soup_Cock() {
  cout << "---------------- test_White_Cream_Soup_Cock " << endl;
  WhiteCreamSoupCock cock;
  cock.cook();
}

void test_Wife() {
  cout << "---------------- test_Wife " << endl;
  Wife wife;
  wife.cook();
  // うん、かわいい、ならいいだろ。
}

int main() {
  cout << "START 関数テンプレート ========== " << endl;
  test_Curry_Cock();
  test_White_Cream_Soup_Cock();
  test_Wife();
  cout << "関数テンプレート END ========== " << endl;
  return 0;
}