/**
 * 「ORx2」のFILE 処理の実験場です。
 * 気になることをまず挙げてみる。
 * 
 * - トランザクション（ファイルのLock）。
 * - 具体的なクラスとその処理。
 * 
 * 手始めにファイルのOpen Close をクラスで表現してみる。
 * コンストラクタの引数でファイルのフルパスあるいはシステムの相対パスを受付てファイルをOpen する。
 * デストラクタで確保したリソースの解放とファイルのClose を行う、こんなものをイメージしている。
 * 
 * cli.cpp （2023-08-01）現在の進捗のデータを具体的に書き込む処理を行う。
 * その際にトランザクションについて考えてみる。マルチスレッドでの振る舞いを考慮しないとダメなのかな。
 * その点も踏まえてテストできたらいいかな。
 * 
 * @author jack
*/
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
using namespace std;
//#define DEFAULT_FILE_PATH           "../tmp/ORx2.bin"

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

class ORx2File final {
private:
    const char* SYSTEM_DEFAULT_PATH = {"../tmp/ORx2.bin"};
    char* filePath = nullptr;
    FILE* fp = NULL;

    void checkOpenFile() {
        if(fp == NULL){
            cerr << "Error. Can't open ORx2.bin file." << endl;
            exit(1);
        }
    }
    void checkOpenFile(const char* fpath) {
        if(fp == NULL){
            cerr << "Error. Can't open " << fpath << endl;
            exit(1);
        }
    }
    void readClose(FILE* file, const int& size) {
        if( file != NULL ){
            char buf[size] = {"\0"};
            fread(buf,sizeof(char),sizeof(buf)/sizeof(buf[0]),file);
            printf("buf is %s\n",buf);
            fclose(file);
        }
    }
public:
    /**
     * デフォルトコンストラクタの場合はメンバ変数 filePath にシステムのデフォルト値が設定される。
    */
    ORx2File():filePath{nullptr},fp{NULL} {
        fp = fopen(SYSTEM_DEFAULT_PATH,"ab+");  // このオプションで指定しても同じファイルポインタでW/R を同時にはできない。
        checkOpenFile();
    }
    /**
     * コンストラクタ、仮引数で任意のファイルパス（ファイル）の指定ができる。
    */
    ORx2File(const char* fpath) {
        int len = strlen(fpath);
        filePath = new char[len+1];
        int i=0;
        for(; i<len; i++) {
            filePath[i] = fpath[i];
        }
        filePath[i] = '\0';
        ptr_lambda_debug<const char*,const int&>(filePath,0);
        fp = fopen(filePath,"ab+");     // このオプションで指定しても同じファイルポインタでW/R を同時にはできない。
        checkOpenFile(filePath);
    }
    ORx2File(const ORx2File& own) {
        (*this) = own;
    }
    ~ORx2File() {
        ptr_lambda_debug<const string&,const int&>("Destructor delete and close ... ORx2File.",0);
        if(filePath != nullptr) {
            delete [] filePath;
        }
        if(fp != NULL) {
            fclose(fp);
        }
    }
    /**
     * ファイル書き込みを行う。
     * 
     * 敗北感が凄まじいな。
     * Generics ... template でやりたかったけど。
     * cast がうまくできなかった。
     * typeid で型のチェックまではできるのだけれど。
    */
    int write(const string& data) {
        try {
            printf("write string ... \n");
            char tmp[1024];
            memcpy(tmp,data.data(),data.size());
            tmp[data.size()] = '\0';
            printf("tmp is %s\n",tmp);
            return write(tmp);
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
    int write(const char* data) {
        try {
            printf("write char ... \n");
            fwrite(data,sizeof(char),sizeof(data)/sizeof(data[0]),fp);
            return 0;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
    int read(const int& size) {
        try {
            if(filePath == nullptr){
                FILE* file = NULL;
                file = fopen(SYSTEM_DEFAULT_PATH,"rb");
                readClose(file,size);
            } else {
                FILE* file = NULL;
                file = fopen(filePath,"rb");
                readClose(file,size);
            }
            return 0;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
};
/**
 * このサンプルで色々修正したい箇所が出てきた。
 * 少し考えをまとめる。
 * - Insert に関して言えばトランザクションはいらないかもしれない、一意制約を気にしなければ：）
 * - Commit 単位、処理単位でOpen Close するべきかもしれない。
 * - Begin Commit Rollback の仕組みを考えてみる。
 * 
    ファイル全体のLock と 行Lock の二種類があると思う。
    何も行わない場合は更新系はすべて前者かな。
    ユニークキ制約も考えると別のファイルがやはり必要だと思う。
    主キ（一意制約）も別ファイルで管理すべきなのか？
    主キの管理方法も色々考えられる、例えばファイルヘッダに置くことだって可能だろう。
    ユニークキも登録、更新時にすべて走査して確認できる、問題は複数人で操作している場合だけど
    その時の振る舞いがどうなるのか、結局早いもの勝ちで、Commit 時に再度判定するのか。
    この辺の調査とテストができることがベストかな。

    - STEP 1
    Begin で行 Lock 。
    Commit 時に最終判定を行う。
    問題があれば Rollback の流れ。
    この仕組みを考えて実装してみたい。
        - テストデータが必要。
        - 行 Lock の仕組み（e.g. 先頭行にフラグ。

*/
int testOpenClose() {
    try {
        string data_a = "SampleA.\n";
        ORx2File x2File;
        ptr_lambda_debug<const string&,const int&>("Play and Result ... x2File.write is",x2File.write(data_a));
        ptr_lambda_debug<const string&,const int&>("Play and Result ... x2File.read is ",x2File.read(1024));

        const char* data_b = "SampleB.\n";
        ORx2File x2File_b("../tmp/ORx2File_b.bin");
        ptr_lambda_debug<const string&,const int&>("Play and Result ... x2File_b.write is",x2File_b.write(data_b));
        ptr_lambda_debug<const string&,const int&>("Play and Result ... x2File_b.read is ",x2File_b.read(1024));
        // ここまでで各ファイルへの文字列の書き込みはできた、その確認は GHex で行った。
        return 0;
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
}
class ITransaction {
public:
    virtual int begin() const = 0;
    virtual int commit() const = 0;
    virtual int rollback() const = 0;
    virtual ~ITransaction() {}
};
class Transaction final {
private:
    ITransaction* tx = nullptr;
public:
    /**
        トランザクションの処理。

        return -1 is error.
        -2 is begin error.
        -3 is rollback error.
    */
    int proc() {
        try {
            if(tx->begin() == 0) {
                if(tx->commit() != 0) {
                    if(tx->rollback() != 0) {
                        return -3;
                    }
                    return -1;
                } else {
                    return 0;
                }
            } else {
                return -2;
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
            if(tx->rollback() != 0) {
                return -3;
            }
            return -1;
        }
    }
};

int main(void) {
    cout << "START cli_file ===============" << endl;
    if(0.1) {
        ptr_lambda_debug<const string&,const int&>("Yeah!!! Have Happy Summer Vacation :)",0);
    }
    if(1.0) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... testOpenClose",testOpenClose());
    }
    cout << "=============== cli_file END" << endl;
    return 0;
}
