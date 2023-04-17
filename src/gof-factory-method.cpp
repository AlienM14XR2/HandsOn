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
class RedPenFactory final : public virtual IFactory {
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
class BluePenFactory final : public virtual IFactory {
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
// Factory Method ここまでのテストね。
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

/**
 ここからの展開。
 次のような課題を考えてみる。
 Template Methodのような固定されたテンプレートの 一部を、どうしたら
 調和の取れた動的置き換えができるのか。

 そのために、そのテンプレートを作る必要がある。
 関数ポインタを利用したいと思っているがどうかな：）　上手くいくだろうか。

 本来はここに、Point(int& x, int& y)
 みたいな、座標管理するクラスが必要なはずだけど、本題から逸れるので端折った：）
 まぁ気になるのであれば次に実装してみたらいいよね。

*/

// typedef unsigned int UINT;
class Point final {
	int x,y;
	Point():x{-1},y{-1}{}
public:
	Point(const int& x_, const int& y_) noexcept {
		x = x_;
		y = y_;
	}
	Point(const Point& own) noexcept {
		x = own.x;
		y = own.y;
	}
	~Point(){}
	int getX() noexcept {return x;}
	int getY() noexcept {return y;}
};
int test_Point() {
	cout << "------------------------- test_Point" << endl;
	try {
		Point point(100,20);
		ptr_lambda_debug<const char*,const int&>("x is ",point.getX());
		ptr_lambda_debug<const char*,const int&>("y is ",point.getY());
		assert(point.getX() == 100);
		assert(point.getY() == 20);
	} catch(exception& e) {
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}
class DragAndDropEvent {
public:
	Point mouseDown(const int& x, const int& y) {
		cout << "start drag." << endl;
		return Point(x,y);
	}
	Point mouseMove(const int& x, const int& y) {
		cout << "start move" << endl;
		return Point(x,y);
	}
	Point mouseUp(const int& x, const int& y) {
		cout << "start drop." << endl;
		draw();
		return Point(x,y);
	}
	virtual int draw() const = 0;
};
class CircleEvent final : public virtual DragAndDropEvent {
public:
	virtual int draw() const override {
		cout << "\tCircle === " << endl;
		return 0;
	}
};
class SquareEvent final : public virtual DragAndDropEvent {
public:
	virtual int draw() const override {
		cout << "\tSquare === " << endl;
		return 0;
	}
};
//
// Template Method ここまでのテストね。
//
int test_CircleEvent() {
	cout << "--------------- test_CircleEvent" << endl;
	try {
		// 細かいことを言ったらきりがないが、EventHandler が必要なんだろうね。
		// やはり、本題から逸れるが。
		CircleEvent circleEvent;
		circleEvent.mouseDown(0,0);
		circleEvent.mouseMove(10,30);
		circleEvent.mouseUp(10,30);
	} catch(exception& e) {
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}
int test_SquareEvent() {
	cout << "--------------- test_SquareEvent" << endl;
	try {
		SquareEvent square;
		square.mouseDown(300,333);
		square.mouseMove(330,363);
		square.mouseUp(330,363);
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
	if(2) {
		ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Point());
		ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_CircleEvent());
		ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_SquareEvent());
	}
    cout << "=============== FactoryMethod END" << endl;
    return 0;
}
