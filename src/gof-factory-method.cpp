/**
 * GoF FactoryMethod
 *
 * 今日から再開したC++ の学習、その一環でクラス設計と実装が学べるので再びGoF を
 * 的にしたコーディングをやっていこうと思う。前回のC++ の学習の途上で利用できな
 * かったものを順次、実装していきたい。
 *
 * その第一弾がFactoryMethod だ。
 * では、早速忘れかけているであろうC++ を書いてみるかな：）
 * まずは、やはり暖機運転からかな。
 * */
#include <iostream>
#include <cassert>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

class IProduct {
public:
	virtual int draw() const = 0;
	virtual ~IProduct() {
	    ptr_lambda_debug<const char*,const int&>("Called IProduct Destructor.",0);
	}
};
class IFactory {
public:
	virtual IProduct* factoryMethod() const = 0;
	virtual ~IFactory() {
	    ptr_lambda_debug<const char*,const int&>("Called IFactory Destructor.",0);
	}
};
class RedPen final : public virtual IProduct {
public:
	RedPen(){}
	RedPen(const RedPen& copy){}
	virtual int draw() const override {
	    cout << "draw red line." << endl;
		return 0;
	}
	~RedPen() {
	    ptr_lambda_debug<const char*,const int&>("Called RedPen Destructor.",0);
	}
};
class BluePen final : public virtual IProduct {
public:
	BluePen(){}
	BluePen(const BluePen& copy){}
	virtual int draw() const override {
	    cout << "draw blue line." << endl;
		return 0;
	}
	~BluePen() {
	    ptr_lambda_debug<const char*,const int&>("Called BluePen Destructor.",0);
	}
};
class RedPenFactory final : public IFactory {
public:
	RedPenFactory(){}
	RedPenFactory(const RedPenFactory& copy){}
	virtual IProduct* factoryMethod() const override {
	    return new RedPen();
	}
	~RedPenFactory() {
	    ptr_lambda_debug<const char*,const int&>("Called RedPenFactory Destructor.",0);
	}
};
class BluePenFactory final : public IFactory {
public:
	BluePenFactory(){}
	BluePenFactory(const BluePenFactory& copy){}
	virtual IProduct* factoryMethod() const override {
	    return new BluePen();
	}
	~BluePenFactory() {
	    ptr_lambda_debug<const char*,const int&>("Called BluePenFactory Destructor.",0);
	}
};

//
// ここまでのテストね。
//
int testFactoryMethod() {
	cout << "------------------- testFactoryMethod" << endl;
	// インタフェースで操作できるか確認している。
	IProduct* pen = nullptr;
	try {
		RedPenFactory redPenFactory;
		// これは意味合いととしては、new RedPen() を行っていることと同じ。
		// つまり、それを利用したものがメモリ解放する責務を負う。
		pen = redPenFactory.factoryMethod();
		assert(pen->draw() == 0);
		delete pen;

		BluePenFactory bluePenFactory;
		pen = bluePenFactory.factoryMethod();
		assert(pen->draw() == 0);
		delete pen;
		
	} catch(exception& e) {
		cerr << e.what() << endl;
		if(pen != nullptr) {
			delete pen;
		}
		return 1;
	}
	return 0;
}


int main() {
    cout << "START FactoryMethod ===============" << endl;
    if(1) {
        ptr_lambda_debug<const char*,const int&>("It's my way :)",0);
		ptr_lambda_debug<const char*,const int&>("Play and Result ... ",testFactoryMethod());
    }
    cout << "=============== FactoryMethod END" << endl;
    return 0;
}
