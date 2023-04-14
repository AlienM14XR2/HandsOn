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

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

class IProduct {
public:
	virtual void draw() const = 0;
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
	virtual void draw() const override {
	    cout << "draw red line." << endl;
	}
	~RedPen() {
	    ptr_lambda_debug<const char*,const int&>("Called RedPen Destructor.",0);
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

int testFactoryMethod() {
	cout << "------------------- testFactoryMethod" << endl;
	try {
		RedPenFactory factory;
		IProduct* pen = factory.factoryMethod();
		pen->draw();
		delete pen;
	} catch(exception& e) {
		cerr << e.what() << endl;
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
