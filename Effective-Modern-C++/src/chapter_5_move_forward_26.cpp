/**
 * 5 章 右辺値参照、ムーブセマンティクス、完全転送
 * 
 * 項目 26 ：転送参照をとるオーバーロードは避ける
 * - 転送参照をとるオーバーロードを加えると、予想以上に多くの場面で、ほぼ常に転送参照をとるオーバーロードが呼び出される。
 * - 完全転送コンストラクタは特に問題になる。非 const な左辺値をとるコピーコンストラクタよりも一致度が高くなるのが通例であり、派生クラス
 *   からの基底クラスのコピー／ムーブコンストラクタ呼び出しを乗っ取ってしまうためである。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_move_forward_26.cpp -o ../bin/main
*/
#include <iostream>
#include <chrono>
#include <set>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << "\t" << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {            // このコンセプトはやはりあまり意味がないのかもな。
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) {
    std::cout << "ERROR: " << e.what() << std::endl;
};

void (*ptr_print_now)(void) = [](void) -> void {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::cout << std::ctime(&t) << std::endl;
};

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        ptr_print_now();
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        throw std::runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        printf("e type is ... %s\n", typeid(e).name());
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * 仮引数に任意の名前をとり、日付と時刻をログ出力し、名前をグローバルデータに追加する関数を開発
 * するとしましょう。まず浮かぶのは次のような関数でしょう。
*/

std::multiset<std::string> names;           // グローバル変数

void logAndAdd(const std::string& name) {
    puts("--- logAndAdd");
    auto now = std::chrono::system_clock::now();
    // log(now, "logAndAdd");
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    ptr_lambda_debug<const char*,const char*>("time is ", std::ctime(&t));
    names.emplace(name);
}

/**
 * 上例は悪いコードではありませんが、最高性能でもありません。呼び出し側として次の 3 通りを考えてみましょう。
 * 
 * std::string petName("Derla");
 * logAndAdd(petName);                      // 左辺値の std::string
 * 
 * logAndAdd(std::string("Persephone"));    // 右辺値の std::string
 * 
 * logAndAdd("Patty Dog");                  // 文字リテラル
 * 
 * 最初の呼び出しでは、logAndAdd の仮引数 name は変数 petName にバインドされ、logAndAdd 内で name は最終的に
 * names.emplace に渡されます。この name は左辺値のため、names にコピーされます。logAndAdd に渡されたものが
 * 左辺値である以上（petName）、このコピーを回避する方法はありません。
 * 
 * 2 番目の呼び出し例では、仮引数 name は右辺値にバインドされます（"Persephone" から明示的に作成された一時オブジェクトの std::string）。
 * name 自身は左辺値のため names にコピーされますが、原則としてムーブ可能であるとも分かっています。この呼び出し例ではコピーのコストが
 * かかりますが、ムーブするだけで避けられるコストです。
 * 
 * 最後の呼び出し例でも仮引数 name は右辺値にバインドされますが、今度は "Patty Dog" から暗黙に作成された std::string の一時オブジェクト
 * です。2 番目の呼び出し例同様に name は names にコピーされますが、logAndAdd に元々渡された実引数は文字列リテラルです。仮に文字列リテラル
 * を直接 emplace へ与えたとすると、std::string の一時オブジェクトを作成する必要は一切ありません。代わりに emplace が std::mutiset 内に
 * 文字列リテラルから std::string オブジェクトを直接作成します。この 3 番目の呼び出し例でも std::string をコピーするコストがかかりますが、
 * ずっとコスト安なムーブコストですら実は『払う必要はありません』。
 * 
 * logAndAdd を変更し転送参照をとるようにすると、上記 2 番目、3 番目の呼び出し例にある非効率性を改善できます（項目 24 参照）。項目 25 に
 * 従い、この転送参照を emplace へ std::forward するのです。
*/

int test_logAndAdd() {
    puts("=== test_logAndAdd");
    try {
        logAndAdd("2024-02-14-A");
        for(auto n: names) {
            ptr_lambda_debug<const char*,const decltype(n)&>("n is ", n);
        }

        std::string petName("Derla");
        logAndAdd(petName);

        logAndAdd(std::string("Persephone"));

        logAndAdd("Patty Dog");

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

template <class T>
void logAndAdd_V2(T&& name) {
    puts("--- logAndAdd_V2");
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    ptr_lambda_debug<const char*,const char*>("time is ", std::ctime(&t));
    names.emplace(std::forward<T>(name));    
}

int test_logAndAdd_V2() {
    puts("=== test_logAndAdd_V2");
    try {
        std::string petName("Derla");
        logAndAdd_V2(petName);                      // 先の例と同様に multiset へ左辺値をコピー

        logAndAdd_V2(std::string("Persephone"));    // コピーではなく 右辺値をムーブ

        logAndAdd_V2("Patty Dog");                  // 一時オブジェクトの std::string をコピーではなく
                                                    // multiset 内で std::string を作成
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

// std::string nameFromIdx(int idx)       // idx に対応する名前を返す
// {
//     // ... TODO data access
//     std::string result;
//     result = "Derek";
//     return result;
// }
// void logAndAdd_V2(int idx) {
//     puts("--- logAndAdd_V2");
//     auto now = std::chrono::system_clock::now();
//     std::time_t t = std::chrono::system_clock::to_time_t(now);
//     ptr_lambda_debug<const char*,const char*>("time is ", std::ctime(&t));
//     names.emplace(nameFromIdx);
// }

/**
 * 私の環境では、上記のサンプルはコンパイルエラーになった。
 * 書籍が言いたいことは、転送参照をとる関数のオーバーロードは避けるという説明のためサンプルだった。
 * 転送参照とオーバーロードを組み合わせて使用するのは、ほぼ常に悪手ということだった。
 * 理由は、転送参照を用いた関数はプログラマが通常期待する以上にずっと多くの実引数を吸収してしまうため。
*/

/**
 * Coffee Break
 * 
 * O/R Mapping (ORM) を考えてみる
 * まずは最低限必要と思われる情報をクラスにする。
 * - 型情報
 * - 名前（カラム名）
 * - 値
 * 
 * 課題
 * - ヌル値の扱い
 * - ID の AutoSequence
*/

template <class T>
class DataField {
public:
    DataField(const std::string& _name, const T& _value): name(_name), value(_value)
    {}

    // ...
    std::pair<std::string,T> bind() {
        return {name, value};
    }
    // void setValue(T&& _value) {
    //     value = std::forward<T>(_value);
    // }

private:
    std::string name;
    T value;
};

/**
 * 上例のクラスを std::vector なりのコンテナに詰めたものが、レコードの 1 行を表現するのではないのか？
*/

int test_DataField() {
    puts("=== test_DataField");
    try {
        DataField<int> d1("id",1);
        auto[name, value] = d1.bind();
        ptr_lambda_debug<const char*,const decltype(value)&>("value is ", value);
        ptr_lambda_debug<const char*,const std::string&>("value type is ", typeid(value).name());   // Boost のライブラリの方が正確との情報があった。
        ptr_lambda_debug<const char*,const decltype(name)&>("name is ", name);

        DataField<std::string> d2("name", "Alice");
        auto[name2, value2] = d2.bind();
        ptr_lambda_debug<const char*,const decltype(value2)&>("value2 is ", value2);
        ptr_lambda_debug<const char*,const std::string&>("value2 type is ", typeid(value2).name());   // Boost のライブラリの方が正確との情報があった。
        ptr_lambda_debug<const char*,const decltype(name2)&>("name2 is ", name2);
        /**
         * TODO Jack
         * 時間を見て、MySQL にある型と一致する C++ の型を網羅させる。
        */
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * ORM の本質を、或いは自分が必要と思う機能を列挙してみる。
 * - ORM オブジェクトからの動的 SQL の生成、最低限のもので良い（CRUD）
 * - RDBMS から取得したレコードの ORM オブジェクトへの代入、あるいは初期化。
 * 
 * テーブルを表現するクラスを考えてみる。
*/

class PersonData final {
public:
    PersonData(const DataField<std::string>& _name, const DataField<std::string>& _email): TABLE_NAME{"PERSON"},
    id{std::move(DataField<std::size_t>("id",0))}, name{_name}, email{_email}
    {
        // 必要ならここで Validation を行う、妥当性検証のオブジェクトをコンポジションして利用するのもあり。
    }

    // ..
    const std::string getTableName() noexcept { return TABLE_NAME; }
    DataField<std::size_t> getId() noexcept { return id; }
    DataField<std::string> getName() noexcept { return name; }
    DataField<std::string> getEmail() noexcept { return email; }
private:
    const std::string TABLE_NAME;
    DataField<std::size_t> id;
    DataField<std::string> name;
    DataField<std::string> email;
};

int test_PersonData() {
    puts("=== test_PersonData");
    try {
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        PersonData derek(name,email);

        ptr_lambda_debug<const char*,const std::string&>("table is ", derek.getTableName());
        auto[nam, val] = derek.getName().bind();
        ptr_lambda_debug<const char*, const decltype(nam)&>("name is ", nam);        
        ptr_lambda_debug<const char*, const decltype(val)&>("value is ", val);        
        auto[nam2, val2] = derek.getEmail().bind();
        ptr_lambda_debug<const char*, const decltype(nam2)&>("name is ", nam2);        
        ptr_lambda_debug<const char*, const decltype(val2)&>("value is ", val2);

        /**
         * 次はこのテーブル情報を持ったクラスから、動的に SQL 生成ができればよい、具体的には以下。
         * - INSERT 文
         * - UPDATE 文
         * - DELETE 文
         * - SELECT 文 これは、findOne() のみでよい。
         * これらは定型文なので自動で SQL を生成する仕組みがあった方が絶対にいい。
         * 
        */
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

template <class Data, class PKEY>
class Repository {
    bool insert(const Data&) = 0;
    bool update(const Data&) = 0;
    bool remove(const PKEY&) = 0;
    Data findOne(const PKEY&) = 0;
};

/**
 * ここからまた、書籍に戻る。
 * 
 * ムーブコンストラクタやムーブ代入演算子ではなく 完全転送コンストラクタも転送参照のオーバーロードと
 * 同じ問題をはらむので止める。
*/



int main(void) {
    puts("START 項目 26 ：転送参照をとるオーバーロードは避ける ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
    }
    if(1.00) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_logAndAdd());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_logAndAdd_V2());
    }
    if(1.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_DataField());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_PersonData());
    }
    puts("=== 項目 26 ：転送参照をとるオーバーロードは避ける END");
    return 0;
}