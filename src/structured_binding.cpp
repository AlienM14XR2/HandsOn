/**
 * 構造化束縛（C++17）========================================================
 * 
 * structured_binding
 * 
 * 英語圏でこう呼ばれてるかは知らん。Google翻訳だから。
 * 
 * うん、聞いたことない。
 * C++17 からね、それなら知ってるはずないか。
 * はじめての出会いは、写経から。
*/
#include <iostream>
#include <cassert>

#define NDEBUG
using namespace std;

//
// ここから、昨日の復習がてら、ウォーミングアップが少し続く。
//
template<class T=const char*> 
struct Print {
    void comment(T t) {
        cout << "Message: " << t << endl;
    }
};
template<class T=const char*>
void (*ptr_lambda_comment)(T) = [](T message) -> void {
    cout << "Message: " << message << endl;
};
// やった、これで少しはまともなDebugができるよ。
template<class D>
void (*ptr_lambda_debug)(D) = [](auto a) -> void {
    cout << "DEBUG: " << a << endl;
};
// メッセージも受け付けるようにした。
template<class M, class D>
void (*ptr_lambda_message_debug)(M,D) = [](auto m, auto d) -> void {
    cout << "DEBUG: " << m << '\t' << d << endl;
};

//
// ここから、本題の構造化束縛をやります。（写経
//
pair<int,string> fa() {
    return {3, "Alice"};    // オブジェクトを返すのか。オレも目が悪くなって、最初 () 演算子でコンパイルできなかった。
}

struct Point {
    float x = 0;
    float y = 0;
};
Point getPoint() {
    return {3.0f, 5.0f};    // これは単なるコンストラクタだね。
}
void test_Get_Point() {
    cout << "test_Get_Point --------------------- " << endl;
    auto[x,y] = getPoint();     // これが構造化束縛だよ。
    ptr_lambda_message_debug<string,int>("x is ",x);
    assert(x == 3.0f);
    ptr_lambda_message_debug<string,int>("y is ",y);
    assert(y == 5.0f);
}

// C++20 以降では、構造化束縛の先頭に記憶クラスを指定できます。
/*
void fb() {
    // 静的記憶クラスの変数に分解した値を代入する
    static const auto[id,name] fb();
}
*/
int main() {
    cout << "START 構造化束縛（C++17） ===========================" << endl;
    Print<> print;
    // template は違う場所で同じものを複数利用できるのか。
    print.comment("こんなのできるのかなと、試したくなった。");
    ptr_lambda_comment<>("できるね。本題の構造化束縛とは何も関係ないけど。");
    // ではね。こっちはこんな感じでDEBUGに利用できるかな。
    int x = 333;
    ptr_lambda_debug<int>(x);
    double y = 0.666;
    ptr_lambda_debug<double>(y);
    const char* z = "これは少し使えるかも。";
    ptr_lambda_debug<const char*>(z);
    //
    // ここから、本題の構造化束縛をやります。（写経
    //
    auto[id,name] = fa();
    ptr_lambda_debug<int>(id);
    assert(id == 3);
    ptr_lambda_debug<string>(name);
    assert(name == "Alice");
    // 参照もいけると、無論オレのDebugもできるぞ。
    pair<int,string> p = fa();
    auto&[id2,name2] = p;
    ptr_lambda_debug<int>(id2);
    ptr_lambda_debug<string>(name2);
    id2 = 1;
    ptr_lambda_message_debug<string,int>("p.first is ",p.first);    // 気に入った。
    // assert(p.first == 2);
    assert(p.first == 1);

    test_Get_Point();
    // 便利だなとはおもった。もっとイメージを定着させるには、具体的なものを
    // 作らないといけないかな、今はまだ、ここ。
    cout << "=========================== 構造化束縛（C++17） END " << endl;
    return 0;
}