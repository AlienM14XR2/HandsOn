/**
 * 関数オブジェクト
 * 
 * 私には耳馴染みのない言葉。
 * したがって、GoFを使うもなにも、まずは写経というのが正しいアプローチ
 * に思われる。
 * 
 * リファレンスに記載されている内容を次にそのまま転記する。
 * 関数オブジェクトとは、オブジェクトの後ろに() 演算子を記述できる、関数
 * のように振る舞うオブジェクトのことで、以下のいずれかになります。
 * 
 * - 関数ポインタ
 * - operator() メンバ関数を持つクラスのオブジェクト
 * - 関数ポインタへの変換メンバ関数を持つクラスのオブジェクト
 * 
 * うん、関数ポインタ（普通の関数とクラスのメンバ関数）とoperator()
 * どれも関数な訳だよね、Ok、ここは写経してみる。
*/
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

using namespace std;

struct Nums {
    int num1;
    int num2;
    Nums():num1{-1},num2{-1} {}
    Nums(const int& n1, const int& n2) {
        num1 = n1;
        num2 = n2;
    }
};

// いいじゃないか、リファレンス先生、突然でてきた vector コンテナ。
// ただし、先生は厳しくてこんなに丁寧な書き方はしていない。
// さらに言えば、ヘッダの include についても言及していないというサドッ。
// 分かるよねっ　じゃないから、お願いしますよ。（C++11）
std::vector<Nums> nums = {Nums(6,5),Nums(4,3),Nums(2,1)};

// 奇しくも比較をやるのか
// 先生も芸がないな。
// うん、そして、いついかなる時でさえ、struct を使い続ける徹底的ぶり。
// 未来のオレ、気づいているとは思うが、この書き方は先生を真似てはいない。
// Java like に読みやすさに重きをおいた。先生はその本の通り、C 文化のヒト。
class Comp1stNum {
public:
    // 参考になるとすれば、この、l r ぐらいだろう。（書き方のね。
    bool operator()(const Nums& l, const Nums& r) const {   
        return l.num1 < r.num1;
    }
};
class Comp2ndNum {
public:
    bool operator()(const Nums& l, const Nums& r) const {
        return l.num2 < r.num2;
    }
};

// うん、operator() をoverride した関数をもってるね。
Comp1stNum c1;      // c1 は関数オブジェクト
Comp2ndNum c2;      // c2 は関数オブジェクト

// 嫌な予感しかなかったが、投げっぱなしか。
// 嫌いなタイプの先生だったか。では、久しぶりに「死ね!!」。
// 受けてやる、奥歯ガタガタいわしたる。
// 絶対動かしたる。
// リファレンスの「関数オブジェクト」の担当者、ぶん殴りてー。
// 確かにな、「基本文法」の章だからそれでいいのかもな、ってオレは思わないの。
// オレは概念的な説明だけで納得できるほど、人間ができちゃいねーんだよ。
// std::sort(nums.begin(),nums.end(),c1);
// std::sort(nums.begin(),nums.end(),c2);

int main() {
    cout << "START 関数オブジェクト ========== " << endl;    
    // ソート前ね
    cout << "Before sort --- " << endl;
    std::for_each(nums.begin(),nums.end(),[](Nums x) {
        cout << "x.num1 is " << x.num1 << endl;
        cout << "x.num2 is " << x.num2 << endl;
    });
    std::sort(nums.begin(),nums.end(),c1);
    std::sort(nums.begin(),nums.end(),c2);
    // なにが、どお変化しのか
    cout << "After sort --- " << endl;
    std::for_each(nums.begin(),nums.end(),[](Nums x) {
        cout << "x.num1 is " << x.num1 << endl;
        cout << "x.num2 is " << x.num2 << endl;
    });
    // リファレンスよ、舐めるなよ、ネット検索の力を。
    // いや、リファレンス先生、あんたは分かってたんだな。
    // 「ぐぐるよね、ぐぐれば、すぐにでも動くようには、情報はおいたよ」
    // そういうことなんだろ。「はい、すぐに動かせました :)」
    cout << "========== 関数オブジェクト END " << endl;
    return 0;
}