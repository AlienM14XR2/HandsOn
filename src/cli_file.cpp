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
    
    ## Insert 時のトランザクション、ファイルで行う際に必要なこと。
        - データの型（struct）、ヘッダにデータの状態を表現する変数が必要。
        - "a+" でファイルをOpen して、正しくRead できるのか確認する必要がある。
        - Read 機能が必要。begin で仮登録、commit で本登録する際に重複がないか調べる必要があるため。
    
    ## システムカラム
        - ID        システム自動設定、プライマリキ 0 。
        - EMAIL     ユーザ入力、必須               1 。
        - NAME      ユーザ入力、必須               2 。
        - PHONE_1   ユーザ入力、必須               3 。
        - PHONE_2   ユーザ入力                     4 。
        - PHONE_3   ユーザ入力                     5 。
        - ADDRESS   ユーザ入力                     6 。
        - MEMO      ユーザ入力                     7 。
        - CREATE_AT システム自動設定               8 。
        - UPDATE_AT システム自動設定               9 。
    ここの先頭にデータの状態を表現する変数を設ける、今考えているのはchar 配列で要素数は 3 。
    現状考えられるものは、「仮登録」「登録完了（本登録）」「Lock」。
    「TMP」「REG」「LOC」。


 * @author jack
*/
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"

using namespace std;
//#define DEFAULT_FILE_PATH           "../tmp/ORx2.bin"

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

typedef struct {
    char dstat[8] = {"\0"};
    unsigned int id = 0;
    char email[512] = {"\0"};
    char name[256] = {"\0"};
    char phone_1[16] = {"\0"};
    char phone_2[16] = {"\0"};
    char phone_3[16] = {"\0"};
    char address[512] = {"\0"};
    char memo[512] = {"\0"};
    char createAt[32] = {"\0"};
    char updateAt[32] = {"\0"};
} SYSTEM_DATA;

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
class PrimaryKeyDuplicateException final {
private:
    const char* defaultErrMsg = "Error: Primary Key is Duplicated.";
public:
    PrimaryKeyDuplicateException() {
    }
    PrimaryKeyDuplicateException(const PrimaryKeyDuplicateException& own) {
        (*this) = own;
    }
    ~PrimaryKeyDuplicateException() {
    }
    const char* what() { return defaultErrMsg; }

};
class ITransaction {
public:
    virtual int begin() const = 0;
    virtual int commit() const = 0;
    virtual int rollback() const = 0;
    virtual ~ITransaction() {}
};
class InsertTx final : public virtual ITransaction {
public:
    InsertTx() {}
    InsertTx(const InsertTx& own) {
        (*this) = own;
    }
    ~InsertTx() {}
    /**
        データの仮登録を行う。

        問題、仮登録が必要なのか。
        例えば、データの先頭3桁で仮登録、登録済み、Lock 等のデータ情報を用いて管理すること。
        こうすることで、Rollback が意味を成すのかもしれない。
        2023-08-10 現在は上記をイメージして実装と考察を続ける予定。
    */
    virtual int begin() const override {
        try {
            ptr_lambda_debug<const string&,const int&>("InsertTx ... begin.",0);
            // プライマリキを発行して、重複がないか確認する。
            // if(1) {     // 例外を強制的に発生させている。
            //     throw PrimaryKeyDuplicateException();
            // }
            return 0;
        } catch(PrimaryKeyDuplicateException& e) {
            cerr << e.what() << endl;
            return -1;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
    /**
        データ登録を完了させる。

        結局処理は早いもの勝ちというルールでいいが、この仕組みで完全に重複データを防げるのかは正直不明。
        マルチスレッドで確認してみるのもいいかもしれない。
        ファイルのOpen と Close（ファイルのLock） とともに要検証と確認が必要だと思う。
    */
    virtual int commit() const override {
        try {
            // データの保存を行う、プライマリキの重複の最終確認を行う。
            ptr_lambda_debug<const string&,const int&>("InsertTx ... commit.",0);
            if(1) {     // 例外を強制的に発生させている。
                throw PrimaryKeyDuplicateException();
            }
            return 0;
        } catch(PrimaryKeyDuplicateException& e) {
            cerr << e.what() << endl;
            return -1;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
    /**
        問題が生じた際、Begin 以前の状態を担保する。
        Insert に関して言えば仮登録されたデータを削除する。
    */
    virtual int rollback() const override {
        try {
            ptr_lambda_debug<const string&,const int&>("InsertTx ... rollback.",0);
            return 0;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
};
class Transaction final {
private:
    ITransaction* tx = nullptr;
    Transaction():tx{nullptr}{}
public:
    Transaction(ITransaction* iface) {
        tx = iface;
    }
    Transaction(const Transaction& own) {
        (*this) = own;
    }
    ~Transaction() {
        if(tx != nullptr) {
            delete tx;
            ptr_lambda_debug<const string&,const int&>("Done Transaction Destructor... delete tx.",0);
        }
    }
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
            // これは無駄かな、ちょっと考えてみる。
            // if(tx->rollback() != 0) {
            //     return -3;
            // }
            return -1;
        }
    }
};
int testInsertTransaction() {
    try {
        cout << "--------- testInsertTransaction" << endl;
        InsertTx* insertTx = new InsertTx();
        ITransaction* tx = static_cast<ITransaction*>(insertTx);
        Transaction transaction(tx);
        return transaction.proc();
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
}
/**
    トランザクション。。。Commit Rollback をInsert 時に実現しようとすると
        - 一行登録あるいは改行なしの登録。（どちらが効率的なのか判然としない：）
        - 任意（主キ）のデータを検索取得。
        - 任意（主キ）のデータの削除。
        - つまりUpdate 以外の処理がすべて必要ということ：）
        - 登録、検索、削除を順序立てて調査・実装していく必要があるのだね。
*/

/**
    Insert について今は次のように考えている。
    主キをファイル名にして 1 レコード 1 ファイル。
    こうすることで削除や Update の更新系処理も考え方が単純化される。
    ただし、この場合は Read が少し厄介になる。
    更新タイミングで Read 用ファイルをすべて書き換える仕組みが必要なため。

    FILE *fp = fopen(filename, "w");
    ファイルのオープンモードを "w" にすることで中身がクリアされる。
    つまり、Insert と Update はファイル名を新規作成するかしないかの違いがあるだけで同じと言えるのか。

    Insert は主キ（プライマリキ）の重複を調べる。（ユニークキも別ファイルで管理すればいいのか？）
    - ファイル名はその主キ（プライマリキ）にする。
    - ファイル Open モードは "w+"
    - Read してデータの有無をチェック、既に同一のプライマリキがあれば Primary Key Duplicate Exception で終了する。
    現状ではこんなことしか言えない。

*/

/*
    ファイルの存在を確認する。

    path:   ファイルパス。
    戻り値: 存在したら 0以外、存在しなければ 0
*/
int exist_file(const char* path) {
    struct stat st;

    // これは Linux でしか使えない、Win は別のヘッダファイルの関数を利用する必要がある（https://programming-place.net/ppp/contents/c/rev_res/file000.html#way2）
    if (stat(path, &st) != 0) { 
        return 0;
    }

    // ファイルかどうか
    // S_ISREG(st.st_mode); の方がシンプルだが、Visual Studio では使えない。
    return (st.st_mode & S_IFMT) == S_IFREG;
}

/**
    Insert は概ね考えは固まった。
    その他の更新系、Update Delete は lock ファイルが必要なのか、それをはっきりさせたい。
    例えば、削除が先に行われて、そのあとに更新が実行されるのはやはりおかしいと思う。
    これをしっかりと考えておく。
*/
int test_insert_system_data(const char* pkey) {
    try {
        FILE* fp = NULL;
        char filePath[32] = {"../tmp/"};
        strcat(filePath,pkey);
        ptr_lambda_debug<const string&,const char*>("filePath is ",filePath);
        // これをやる前にそのファイルの有無をチェックしないといけない。
        // それがつまり、すでにそのプライマリキが利用されていることになる。
        if(exist_file(pkey) == 0) { // フルパス、あるいは相対パスとしてこのファイルがないことを期待してのこと。
            printf("DEBUG: %s no exist\n",pkey);
        }
        if(exist_file(filePath)) {
            printf("DEBUG: %s exist\n",filePath);
            throw PrimaryKeyDuplicateException();
        }
        // 上記のファイルチェックがプライマリキの重複チェックになる。（こんなかたちでユニークキもできるはず：）
        fp = fopen(filePath,"w+");
        if(fp != NULL) {
            printf("DEBUG: It's open file. mode is \"w+\"\n");
        }
        return 0;
    } catch(PrimaryKeyDuplicateException& e) {
        cerr << e.what() << endl;
        return -1;
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
}
int test_read_system_data() {
    return 0;
}
int test_delete_system_data() {
    return 0;
}

int main(void) {
    cout << "START cli_file ===============" << endl;
    if(0.1) {
        ptr_lambda_debug<const string&,const int&>("Yeah!!! Have Happy Summer Vacation :)",0);
    }
    if(0) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... testOpenClose",testOpenClose());
    }
    if(1.1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... testInsertTransaction",testInsertTransaction());
    }
    if(1.2) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... test_insert_system_data",test_insert_system_data("1.bin"));
        ptr_lambda_debug<const string&,const int&>("Play and Result ... test_insert_system_data",test_insert_system_data("2.bin"));
    }
    cout << "=============== cli_file END" << endl;
    return 0;
}
