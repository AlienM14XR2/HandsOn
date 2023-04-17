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
	RedPen(const RedPen& own){}
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
	BluePen(const BluePen& own){}
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
	RedPenFactory(const RedPenFactory& own){}
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
	BluePenFactory(const BluePenFactory& own){}
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

 本来はここに、Point(const int& x, const int& y)
 みたいな、座標管理するクラスが必要なはずだけど、本題から逸れるので端折った：）
 まぁ気になるのであれば次に実装してみたらいいよね。

 IProduct の派生クラスである、RedPen BluePen を上手く適用させたい。
 どうする、オレ：）理想とするものが、浮かんでくるな。
 疑似描画ツールは IProduct のみを操作する、その中身を知る必要はない。 

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
	//
	// Point を作って、以下のような管理の仕方にすると
	// 各メンバ関数は Point のファクトリという見方もできるね。
	// 勿論、FactoryMethod パターンではない。
	//
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
	Point mouseUp(const int& x, const int& y, const IProduct& pen) {
		cout << "start drop, append pen." << endl;
		draw(pen);
		return Point(x,y);

	}
	virtual int draw() const = 0;
	virtual int draw(const IProduct& pen) const = 0;
};
class CircleEvent final : public virtual DragAndDropEvent {
public:
	virtual int draw() const override {
		cout << "\tCircle === " << endl;
		return 0;
	}
	virtual int draw(const IProduct& pen) const override {
		cout << "\tCircle with pen === " << endl;
		pen.draw();
		return 0;
	}
};
class SquareEvent final : public virtual DragAndDropEvent {
public:
	virtual int draw() const override {
		cout << "\tSquare === " << endl;
		return 0;
	}
	virtual int draw(const IProduct& pen) const override {
		cout << "\tSquare with pen ===" << endl;
		pen.draw();
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

//
// IProduct の派生クラス、RedPen BluePen を利用した、疑似描画ツールのテスト。
//
int test_CircleEvent_With_Pen() {
	cout << "----------------------------- test_CircleEvent_With_Pen" << endl;
	IProduct* pen = nullptr;
	try {
		RedPenFactory redPenFac;
		pen = redPenFac.factoryMethod();
		CircleEvent event;
		event.mouseDown(3,9);
		event.mouseMove(9,9);
		event.mouseUp(9,9,*pen);
		delete pen;
	} catch(exception& e) {
		if(pen != nullptr) {
			delete pen;
		}
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}
int test_SquareEvent_With_Pen() {
	cout << "----------------------------- test_SquareEvent_With_Pen" << endl;
	IProduct* pen = nullptr;
	try {
		BluePenFactory bluePenFac;
		pen = bluePenFac.factoryMethod();
		SquareEvent event;
		event.mouseDown(10,10);
		event.mouseMove(40,40);
		event.mouseUp(40,40,*pen);
		delete pen;
	} catch(exception& e) {
		if(pen != nullptr) {
			delete pen;
		}
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}
/**
 至って、ここまでは素直な設計をしたつもり。でも最初に言ったように関数ポインタでもできないのかと、
 少し、思考を巡らし遊んでみたい。IProduct の派生クラス、そのオブジェクトのメンバ関数を複数利用するなら
 絶対にオブジェクトのポインタや参照を渡すべき、そうじゃない場合でも上記のような戦略をとるべきだと思う。
 上記の一文は大切なこと。以下、行うことはあくまで自分の学習の一環であり、邪道だと考えてはいる。

 現状だけを眺めるとオブジェクトをもらう必要はないよねって理由だけ。（便利だから、結局オブジェクトはもらう形にはした：）

*/
class DragAndDropEvent_Sandbox : public virtual DragAndDropEvent {
public:
	virtual int draw(const IProduct& pen) const override {
		cout << "\tDragAndDropEvent_Sandbox with pen ===" << endl;
		pen.draw();
		return 0;
	}
};
// とってもイレギュラーなケースを想定してみる。
int oepn_draw(const IProduct& pen1, const IProduct& pen2) {
	cout << "---------------------- oepn_draw" << endl;
	pen1.draw();
	pen2.draw();
	return 0;
}
int (*ptr_draw)(const IProduct&,const IProduct&) = oepn_draw;

/**
 三角形を描画するクラス。

 このクラスで少し邪道なコーディングを試してみる。

 この場合だと、基底クラスに純粋仮想関数の定義を増やすことなく、実装できるよね。
 いままでのは、全ての派生クラスにその影響が及ぶ。
*/
class TryangleEvent final : public virtual DragAndDropEvent_Sandbox {
public:
	virtual int draw() const override {
		cout << "\tTryangle === " << endl;
		RedPenFactory rfactory;
		BluePenFactory bfactory;
		IProduct* redPen = rfactory.factoryMethod();
		IProduct* bluePen = bfactory.factoryMethod();
		ptr_draw(*redPen,*bluePen);
		delete redPen;
		delete bluePen;
		return 0;
	}
};
int test_TryangleEvent() {
	cout << "---------------------------- test_TryangleEvent" << endl;
	try {
		TryangleEvent event;
		event.mouseDown(0,0);
		event.mouseMove(33,33);
		event.mouseUp(33,33);
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
	if(3) {
		ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_CircleEvent_With_Pen());
		ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_SquareEvent_With_Pen());
	}
	if(4) {
		ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_TryangleEvent());
	}
	// なるほど、面白い。FactoryMethod の使い方はこのぐらいでいいかな。
	// さっき、うちのリファレンス先生を読み返してたら、ムーブセマンティクスをちゃんとやったか不安になってきた
	// なので、それを次はやるかな。今日はこの後はJavaかな：）
    cout << "=============== FactoryMethod END" << endl;
    return 0;
}
