/**
 * 2023-03-11
 * @author alienm14xr2
 * 
 * 古い本を引っ張り出してメモリの扱いを復習するのが目的です。
 * 
*/

#include <iostream>
#include <cstdlib>

using namespace std;

/**
 * コピーコンストラクタがない場合の確認用クラス。
*/
class Heap_No_Copy_Constructor {
    int *p=nullptr;
    int size=-1;
    int index=-1;
    Heap_No_Copy_Constructor():p{nullptr},size{-1},index{-1} {}
    void heapError() {
        cerr << "NO alloc Heap." << endl;
        exit(1);
    }
public:
    int get(const int& i) {
        return p[i];
    }
    int put(const int& num) {
        p[index] = num;
        ++index;
        return p[index-1];
    }
    Heap_No_Copy_Constructor(const int& size) {
        cout << "DEBUG. Heap Constructor." << endl;
        index = 0;
        this->size = size;
        p = new int[this->size];
        if(!p) {
            heapError();
        }
    }
    ~Heap_No_Copy_Constructor() {
        cout << "DEBUG. Heap Destructor." << endl;;
        delete [] p;
    }
};

/**
 * メモリの制御を行うクラス。
 * 
 * メモリの確保とその解放の訓練だよ。
*/
class Heap {
    // 例えば次のメンバ変数の初期化を確実なもの。
    // クラスオブジェクトの共通の初期化にしたい場合。
    // デフォルトコンストラクタの初期化構文を用いる。
    // 今回はデフォルトコンストラクタはprivateにしているため
    // 意味はないが。外から利用できないため。
    int *p=nullptr;
    int size=-1;
    int index=-1;
    Heap():p{nullptr},size{-1},index{-1} {}
    void heapError() {
        cerr << "NO alloc Heap." << endl;
        exit(1);
    }
public:
    int get(const int& i) {
        return p[i];
    }
    int put(const int& num) {
        p[index] = num;
        ++index;
        return p[index-1];
    }
    Heap(const int& size) {
        cout << "DEBUG. Heap Constructor." << endl;
        index = 0;
        this->size = size;
        p = new int[this->size];
        if(!p) {
            heapError();
        }
    }
    // これがコピーコンストラクタ。
    Heap(const Heap& heap) {
        cout << "DEBUG. Heap Copy Constructor." << endl;;
        size = heap.size;
        p = new int[size];
        for(int i = 0; i< size; i++) {
            p[i] = heap.p[i];
        }
    }
    ~Heap() {
        cout << "DEBUG. Heap Destructor." << endl;;
        delete [] p;
    }

};
void test_Heap_Class();
void test_heap();
void test_Heap_No_Copy_Constructor();

// 本当はここにTry Catchを入れたい。
// TODO @alienm14xr2 Try Catch と例外処理。
int main() {
    cout << "START memory ========== " << endl;
    test_Heap_Class();
    test_Heap_No_Copy_Constructor();
 //   test_heap();
    cout << "========== memory END " << endl;
    return 0;
}

// そもそもクラスオブジェクトをBy Val で受け付けるのが良くないよ。
void bad_company(Heap heap) {
    cout << "bad_company -----" << endl;
    cout << "----- end bad_company" << endl;
}

// そもそもクラスオブジェクトをBy Val で受け付けるのが良くないよ。
void bad_company(Heap_No_Copy_Constructor heap) {
    cout << "bad_company -----" << endl;
    cout << "----- end bad_company" << endl;
}


void test_Heap_No_Copy_Constructor() {
    cout << "test_Heap_No_Copy_Constructor -----" << endl;
    Heap_No_Copy_Constructor heap(3);
    int num = heap.put(2);
    cout << "num is " << num << endl;
    num = heap.put(4);
    cout << "num is " << num << endl;
    num = heap.put(6);
    cout << "num is " << num << endl;
    bad_company(heap);
    for(int i=0; i<3; i++) {
        cout << "heap.get(i) " << heap.get(i) << endl;
    }
}

void test_Heap_Class() {
    cout << "test_Heap_Class -----" << endl;
    Heap heap(3);
    int num = heap.put(2);
    cout << "num is " << num << endl;
    num = heap.put(4);
    cout << "num is " << num << endl;
    num = heap.put(6);
    cout << "num is " << num << endl;
    bad_company(heap);
    for(int i=0; i<3; i++) {
        cout << "heap.get(i) " << heap.get(i) << endl;
    }
}

void test_heap() {
    cout << "----- test_heap" << endl;
    int* pa = nullptr;
    int* pb;
    cout << "sizeof(pa) " << sizeof(pa) << endl;
    if(!pa) {
        cout << "nullptr is false." << endl;
    }
    if(!pb) {
        cout << "no initialized is false." << endl;
    }
    // OK. 理解したよ。
    // e.g. pp = new int(3) とした場合は値が3となる。ただし、new しているので delete する必要がある。
    // e.g. delete pp
    // e.g. int a = 3 と値は同じ。
    pa = new int(3);
    cout << "*pa is " << *pa << endl;
    unsigned long size = sizeof(pa);
    cout << "size is " << size << endl;
    delete pa;
    cout << "sizeof(pa) " << sizeof(pa) << endl;
    cout << "*pa is " << *pa << endl;
    // delete 後のpaは不明な値になるのね。実行時に問題にはならないが、注意は必要と。

    // これはまた、意味が違うよね。
    pb = new int[3];
    cout << "sizeof(pb) " << sizeof(pb) << endl;    // これはpb[0]を見てるのかな。
    for(int i=0; i<3; i++) {
        pb[i] = i+1;
        cout << "pb is " << pb[i] << endl;
    }
    delete [] pb;
    for(int i=0; i<3; i++) {
        cout << "pb is " << pb[i] << endl;
    }
    /*
        これは、意図した動きになる。
        delete [] pb 以降でのForループないの値は不明なものになる。
        問題は次ができてしまう。
    */
    for(int i=0; i<4; i++) {
        pb[i] = i+1;
        cout << "pb is " << pb[i] << endl;
    }
    delete [] pb;

}
