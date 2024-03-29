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
    
    ## Insert 時のトランザクション、ファイルで行う際に必要なこと。（これは採用していない。）
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

    # 「登録」「更新」「削除」は PK.bin PK.loc のファイルで管理する仕組みにした。
     所謂登録・更新系の処理はこれが一番単純で分かりやすいと思ったから。
     問題は「検索」今は 3 パターンある。
     - 1. 現状の仕組みのまま、ファイルが増えた場合 100万件のファイルの Open Close が発生する（これはNG だろうな：）
     - 2. PK の仕組みを拡張して、UK などファイル名を値にする。ファイルの開閉の必要はなくなる。
     - 3. 検索専用のファイルを用意する。この場合は登録・更新系の処理に連動させるのが面倒になる。
     - 4. N-gram ? などの仕組みを導入する。
     1 以外はどれも複雑な仕組みになりそう。
     個人的な興味・学習という観点で言えば 2 をやってみたい。

     プログラム、ストリーム、ファイル。
     fflush. seek. カレントカーソル。

 * @author jack
*/
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "time.h"

using namespace std;
#define FILE_PATH_SIZE                32
#define TRANSACTION_TIMEOUT            5     // sec.

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
/*
    ファイルの存在を確認する。

    path:   ファイルパス。
    戻り値: 存在したら 0以外、存在しなければ 0
*/
int exist_file(const char* path) {  // 簡易テストで利用してるからこれは消せなかった。
    struct stat st;
    // これは Linux でしか使えない、Win は別のヘッダファイルの関数を利用する必要がある（https://programming-place.net/ppp/contents/c/rev_res/file000.html#way2）
    if (stat(path, &st) != 0) { 
        return 0;
    }
    // ファイルかどうか
    // S_ISREG(st.st_mode); の方がシンプルだが、Visual Studio では使えない。
    return (st.st_mode & S_IFMT) == S_IFREG;
}
int initArray(char* array, const int& size) {
    for(int i=0; i<size; i++) {
        array[i] = '\0';
    }
    return 0;
}

/**
    サンプル・テスト用データ。
*/
typedef struct {
    unsigned int id;
    char email[256];
} SAMPLE_DATA;
/**
    サンプル・テスト用データB。
*/
typedef struct {
    unsigned int id;
    char email[256];
    char end;
} SAMPLE_B_DATA;
/**
    テーブルカラム情報。
*/
typedef struct {
    char column[256] = {"\0"};
    char type[32] = {"\0"};
    char option[512] = {"\0"};
} TABLE_COLUMN;
/**
    テーブルの値
*/
typedef struct {
    unsigned int id = 0;
    char email[256] = {"\0"};
    char name[256] = {"\0"};
} TABLE_VALUE;

/**
    プライマリキの重複例外クラス。
*/
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
/**
    トランザクション・インタフェースクラス。
*/
class ITransaction {
protected:
    virtual int exist_file(const char* path) const = 0;
    virtual int delete_file(const char* file_name) const = 0;
    virtual int makeFilePath(const unsigned int& pk, char* filePath) const = 0;
    virtual int makeLFilePath(const unsigned int& pk, char* lfilePath) const = 0;
    virtual void checkFilePathSize(const unsigned int& pk,char* filePath,char* lfilePath) const = 0;
    virtual int checkFiles(const char* filePath, const char* lfilePath) const = 0;
public:
    virtual int begin() const = 0;
    virtual int commit() const = 0;
    virtual int rollback() const = 0;
    virtual ~ITransaction() {}
};
/**
    トランザクション・抽象クラス。
    共通処理の具象化を行っている。
*/
class ATransaction : public virtual ITransaction {
protected:
    /*
        ファイルの存在を確認する。

        path:   ファイルパス。
        戻り値: 存在したら 0以外、存在しなければ 0
    */
    virtual int exist_file(const char* path) const override {
        struct stat st;

        // これは Linux でしか使えない、Win は別のヘッダファイルの関数を利用する必要がある（https://programming-place.net/ppp/contents/c/rev_res/file000.html#way2）
        if (stat(path, &st) != 0) { 
            return 0;
        }

        // ファイルかどうか
        // S_ISREG(st.st_mode); の方がシンプルだが、Visual Studio では使えない。
        return (st.st_mode & S_IFMT) == S_IFREG;
    }
    /*
        ファイルを削除する。

        file_name: 削除するファイルの名前
        戻り値:   成功したら 0以外、失敗したら 0
    */
    virtual int delete_file(const char* file_name) const override {
        return !(remove(file_name));
    }
    virtual int makeFilePath(const unsigned int& pk, char* filePath) const override {
        string spk = to_string(pk);
        spk = spk + ".bin";
        char cpk[16] = {"\0"};
        memcpy(cpk,spk.data(),spk.size());
        cpk[spk.size()] = '\0';
        int size = strlen(filePath) + strlen(cpk);
        if( size <= FILE_PATH_SIZE ) {
            strcat(filePath,cpk);  // 第一引数のサイズが第二引数と連結されたサイズ以下だとエラー。。。らしい。
        }
        return size;
    }
    virtual int makeLFilePath(const unsigned int& pk, char* lfilePath) const override {
        string spk = to_string(pk);
        spk = spk + ".loc";
        char cpk[16] = {"\0"};
        memcpy(cpk,spk.data(),spk.size());
        cpk[spk.size()] = '\0';
        int size = strlen(lfilePath) + strlen(cpk);
        if( size <= FILE_PATH_SIZE ) {
            strcat(lfilePath,cpk);  // 第一引数のサイズが第二引数と連結されたサイズ以下だとエラー。。。らしい。
        }
        return size;
    }
    virtual void checkFilePathSize(const unsigned int& pk,char* filePath,char* lfilePath) const override {
        try {
            int size = makeFilePath(pk,filePath);
            int lsize = makeLFilePath(pk,lfilePath);
            if(size <= FILE_PATH_SIZE && lsize <= FILE_PATH_SIZE) {
                printf("DEBUG: filePath is %s\n",filePath);
                printf("DEBUG: lfilePath is %s\n",lfilePath);
    //            pd = pdata;
            } else {
                // 例外は、これが一般的で一番簡単かもしれない。
                throw runtime_error("Error: file path size 32 but over.");
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
            exit(1);
        }
    }
    virtual int checkFiles(const char* filePath, const char* lfilePath) const override {
        try {
            // [PK].bin があり、[PK].lock がない場合に正常処理ができる。
            if(exist_file(filePath)) {
                printf("DEBUG: %s exist.\n",filePath);
                if(exist_file(lfilePath) == 0) {    // 現在 Lock している処理がない場合
                    printf("DEBUG: no lock file. \n");
                    // [PK].lock ファイルを作成する。
    //                lfp = fopen(lfilePath,"w");
                } else {    // 問題はこっちをどうするか、処理を止めるか一定時間待機するのか
                    clock_t start = clock();
                    while(1) {
                        clock_t end = clock();
                        double elapsed = (double)(end-start)/CLOCKS_PER_SEC;
                        if(elapsed >= TRANSACTION_TIMEOUT) {
                            throw runtime_error("Error: Transaction timeout.");
                        } else {
                            // 他の処理が終了して、Lock ファイルがないか確認する。ない場合は Lock ファイルを作成しループを終了する。
                            // ただし、Delete が先行していた場合はその後の Update は無効にすること。
                            if(exist_file(lfilePath) == 0) {
                                if(exist_file(filePath)) {  // 対象のデータファイルがあるかチェック。
                                    // [PK].lock ファイルを作成する。
    //                                lfp = fopen(lfilePath,"w");
                                    break;
                                } else {
                                    throw runtime_error("Error: Data has already deleted.");
                                }
                            }
                        }
                    }
                }
                return 0;
            } else {
                throw runtime_error("Error: Data file don't exist.");
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
};
/**
    新規登録のトランザクション処理クラス。
*/
class InsertTx final : public virtual ATransaction {
private:
    mutable FILE* fp = NULL;
    char filePath[64] = {"../tmp/"};
    char tablePath[64] = {"../tmp/"};
    // データ用の struct 構造体をメンバ変数に持ち、コンストラクタの引数で代入すること。
    SAMPLE_DATA* pd = nullptr;
    unsigned int pkey = 0;
    int colCount = 0;   // TABLE_COLUMN* の参照先になる TABLE_COLUMN 配列の要素数。
    int valCount = 0;   // TABLE_VALUE* の参照先になる TABLE_VALUE 配列の要素数。
    TABLE_COLUMN* ptblColumn = nullptr;
    TABLE_VALUE* ptblValue = nullptr;
    InsertTx() {}
public:
    /**
        コンストラクタ

        - fileName 数値+.bin、e.g., 1,2,3... 100.bin
        - pdata 登録されるデータ。
    */
    InsertTx(const char* fileName,SAMPLE_DATA* pdata) {
        try {
            int size = strlen(filePath) + strlen(fileName);
            if(size <= 64) {
                strcat(filePath,fileName);  // 第一引数のサイズが第二引数と連結されたサイズ以下だとエラー。。。らしい。
                pd = pdata;
            } else {
                // 例外は、これが一般的で一番簡単かもしれない。
                throw runtime_error("Error: file path size 64 but over.");
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
            exit(1);
        }
    }
    /**
        コンストラクタ

        - fileName 文字列、e.g., [db_name]/[tbl_name]/[pkey].bin
        - key 数値 プライマリキ。
        - ccount カラム情報配列の要素数。
        - cdata カラム情報。[column_name]
        - vdata カラムに対応した値。
    */
    InsertTx(const char* fileName,const char* tblName,unsigned int key,TABLE_COLUMN* cdata,int ccount,TABLE_VALUE* vdata,int vcount) {
        try {
            // ../tmp/[db_name]/[tbl_name]/[column_name]/[pkey]
            // これがこのコンストラクタで管理するディレクトリ階層になる。
            // データを登録する際は、[column_name] を動的に切り替えて [column_value.bin] ファイルを [pkey] ディレクトリ
            // の配下に作成する。
            int size = strlen(filePath) + strlen(fileName);
            if( size <= 64 ) {
                strcat(filePath,fileName);  // ../tmp/[db_name]/[tbl_name]/ になるようにする。
                printf("filePath is %s\n",filePath);
                strcat(tablePath,tblName);
                printf("tablePath is %s\n",tablePath);
                pkey = key;
                printf("pkey is %d\n",pkey);
                ptblColumn = cdata;
                colCount = ccount;
                ptblValue = vdata;
                valCount = vcount;
            } else {
                // カラム名を動的に切り替える必要があるのでこれが決して発生しない仕組みが必要かな。
                // ../tmp/[db_name]/[tbl_name]/ までは、外部で担保して、[column_name]/[pkey] のみを管理する。
                // 根本的な解決ではないかもしれないが、問題を切り分けられるようにはなる。
                throw runtime_error("Error: file path size 64 but over.");
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
            exit(1);
        }
    }
    InsertTx(const InsertTx& own) {
        (*this) = own;
    }
    ~InsertTx() {
        if(fp != NULL) {
            fclose(fp);
        }
    }
    /**
        データの仮登録を行う。

        問題、仮登録が必要なのか。
        例えば、データの先頭3桁で仮登録、登録済み、Lock 等のデータ情報を用いて管理すること。
        こうすることで、Rollback が意味を成すのかもしれない。
        2023-08-10 現在は上記をイメージして実装と考察を続ける予定。
        上記はまだ、すべてのレコードを 1 ファイルで管理することを考えていた。
        現在（2023-08-17）は 1 レコード 1 ファイルで管理しようと思っている。

        - データを保存するためのファイルを確定する。
        - ファイル名はプライマリキとする。
        - すでに同一のファイルが存在する場合は、PK の重複例外を発生させる。
    */
    virtual int begin() const override {
        try {
            ptr_lambda_debug<const string&,const int&>("InsertTx ... begin.",0);
            if(exist_file(filePath)) {
                printf("DEBUG: %s exist.\n",filePath);
                throw PrimaryKeyDuplicateException();
            }
            fp = fopen(filePath,"wb+");
            if(fp != NULL) {
                printf("DEBUG: It's open file. mode is \"wb+\" file path is %s \n",filePath);
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
        データ登録を完了させる。

        結局処理は早いもの勝ちというルールでいいが、この仕組みで完全に重複データを防げるのかは正直不明。
        マルチスレッドで確認してみるのもいいかもしれない。
        ファイルのOpen と Close（ファイルのLock） とともに要検証と確認が必要だと思う。

        begin で ファイル名によるPK の重複チェックは済んでいるのでファイルにデータを書き込む。
    */
    virtual int commit() const override {
        try {
            // データの保存を行う、プライマリキの重複の最終確認を行う。
            ptr_lambda_debug<const string&,const int&>("InsertTx ... commit.",0);
            if(fp != NULL) {
                if(pd != nullptr) {
                    printf("DEBUG: id is %d, email is %s.\n",pd[0].id,pd[0].email);
                    printf("DEBUG: sizeof(SAMPLE_DATA) is %ld.\n",sizeof(SAMPLE_DATA));
                    printf("DEBUG: sizeof(pd) is %ld.\n",sizeof(pd));
                    fwrite(pd,sizeof(SAMPLE_DATA),1,fp);
                    fclose(fp);
                    fp = NULL;
                    return 0;
                }
                // 各カラム・ディレクトリ配下にファイルを新規作成する。
                if(ptblColumn != nullptr && ptblValue != nullptr) {
                    fwrite(ptblValue,sizeof(TABLE_VALUE),1,fp);
                    fclose(fp);
                    fp = NULL;
                    // ファイルポインタを使いまわして、カラム・ファイルを作る。
                    // ../tmp/test/address_book/
                    // 引数なり、動的な仕組みで [system_current_dir]/[db_name]/[table_name]/ を取得できないといけない。
                    FILE* cfp = NULL;
                    char tmpDir[64] = {"\0"};
                    char buf[16];
                    snprintf(buf, 16, "%d", pkey);
                    for(int i=0; i<colCount; i++) {
                        printf("DEBUG: column is %s\n",ptblColumn[i].column);
                        strcat(tmpDir,tablePath);
                        strcat(tmpDir,ptblColumn[i].column);
                        strcat(tmpDir,"/");
                        strcat(tmpDir,buf);
                        printf("DEBUG: tmpDir is %s\n",tmpDir); // この配下に[value.bin]（空ファイル）を作成する。
                        initArray(tmpDir,64);
                    }

                    return 0;
                }
                return -1;
            } else {
                throw runtime_error("Error: Data file pointer is NULL.");
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
    /**
        問題が生じた際、Begin 以前の状態を担保する。        
        現状は何も行っていない。
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
/**
    更新のトランザクション処理クラス。
    Fix しないといけない問題、PK の更新を許可するかということ。
    アプリケーションで考えた場合は NG だが（今回のPK はシステム管理でありユーザ定義ではないため）
    一般的なシステムとういう観点では OK 。
    PK の更新とは現状の作りでは、ファイルの 削除と新規作成になる。
    これは少し特殊なので、更新処理に直接組み込むかは保留する。
*/
class UpdateTx final : public virtual ATransaction {
private:
    mutable FILE* fp = NULL;
    mutable FILE* lfp = NULL;
    char filePath[FILE_PATH_SIZE] = {"../tmp/"};
    char lfilePath[FILE_PATH_SIZE] = {"../tmp/"};
    SAMPLE_DATA* pd = nullptr;

    UpdateTx() {}

public:
    // この作り、DRY の原則からはずれてる、リファクタ対象だろうな。
    // InsertTx との共通処理の割り出しが必要という意味。
    // コンストラクタとしてはプライマリキを仮引数に取る方が良さそう。
    UpdateTx(const unsigned int& pk,SAMPLE_DATA* pdata) {
        checkFilePathSize(pk,filePath,lfilePath);
        pd = pdata;
    }
    UpdateTx(const UpdateTx& own) {
        (*this) = own;
    }
    ~UpdateTx() {
        if(fp != NULL) {
            fclose(fp);
        }
        if(lfp != NULL) {
            fclose(lfp);
        }
    }
    /**
        更新対象のファイルを Lock する。
        [PK].lock ファイルを作成する。
        すでに作成されていた場合は処理を待機する。
        Lock ファイルが削除処理の場合もあるので、その場合は [PK].bin ファイルの有無に注意すること。
    */
    virtual int begin() const override {
        if(checkFiles(filePath,lfilePath) == 0) {
            lfp = fopen(lfilePath,"w");
            return 0;
        } else {
            return -1;
        }
    }
    /**
        更新処理の実行。
        ファイルの内容の書き換えを行う。
        "wb+"
    */
    virtual int commit() const override {
        try {
            if(lfp != NULL) {
                fp = fopen(filePath,"wb+");
                if(fp != NULL) {
                    fwrite(pd,sizeof(SAMPLE_DATA),1,fp);
                    fclose(fp);
                    fp = NULL;
                    // 最後に Lock ファイルを削除する。
                    fclose(lfp);
                    lfp = NULL;
                    if(delete_file(lfilePath)) {
                        return 0;
                    } else {
                        throw runtime_error("Error: Failed ... remove loc file.");
                    }
                } else {
                    throw runtime_error("Error: Data file pointer is NULL.");
                }
            } else {
                throw runtime_error("Error: Lock file pointer is NULL.");
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
    /**
        Lock ファイルがある場合は削除する。
    */
    virtual int rollback() const override {
        try {
            if(lfp != NULL) {   // 自分でOpen したLock ファイルを削除しないといけない。
                fclose(lfp);
                lfp = NULL;
                if(!delete_file(lfilePath)){
                    throw runtime_error("Error: Failed ... remove loc file.");
                }
            }
            return 0;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
};
class DeleteTx final : public virtual ATransaction {
private:
    mutable FILE* fp = NULL;
    mutable FILE* lfp = NULL;
    char filePath[FILE_PATH_SIZE] = {"../tmp/"};
    char lfilePath[FILE_PATH_SIZE] = {"../tmp/"};
    DeleteTx() {}
public:
    DeleteTx(const unsigned int& pk) {
        checkFilePathSize(pk,filePath,lfilePath);
    }
    DeleteTx(const DeleteTx& own) {
        (*this) = own;
    }
    ~DeleteTx() {
        if(fp != NULL) {
            fclose(fp);
        }
        if(lfp != NULL) {
            fclose(lfp);
        }
    }
    /**
        Lock ファイルの作成を行う。
    */
    virtual int begin() const override {
        if(checkFiles(filePath,lfilePath) == 0) {
            lfp = fopen(lfilePath,"w");
            return 0;
        } else {
            return -1;
        }
    }
    /**
        データファイルの削除を行う。
    */
    virtual int commit() const override {
        try {
            if(lfp != NULL) {
                // データファイルを削除する。
                if(!delete_file(filePath)) {
                    throw runtime_error("Error: Failed ... remove data file.");
                }
                // 最後に Lock ファイルを削除する。
                fclose(lfp);
                lfp = NULL;
                if(!delete_file(lfilePath)) {
                    throw runtime_error("Error: Failed ... remove loc file.");
                }
                return 0;
            } else {
                throw runtime_error("Error: Lock file pointer is NULL.");
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
    /**
        Lock ファイルがある場合は削除する。
    */
    virtual int rollback() const override {
        try {
            if(lfp != NULL) {   // 自分でOpen したLock ファイルを削除しないといけない。
                fclose(lfp);
                lfp = NULL;
                if(!delete_file(lfilePath)) {
                    throw runtime_error("Error: Failed ... remove loc file.");
                }
            }
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
            return -1;
        }
    }
};
int testInsertTransaction(const unsigned int& pk, const char* fname) {
    try {
        cout << "--------- testInsertTransaction" << endl;
        SAMPLE_DATA sample[] = {{pk,"jack@loki.org"}};
        InsertTx* insertTx = new InsertTx(fname,sample);
        ITransaction* tx = static_cast<ITransaction*>(insertTx);
        Transaction transaction(tx);
        return transaction.proc();
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
}
// const char* fileName,unsigned int key,int ccount,TABLE_COLUMN* cdata,TABLE_VALUE* vdata
int testInsertTransaction_v2(const unsigned int& key, const char* fileName, const char* tblName) {
    try {
        TABLE_COLUMN tblCols[3] = {
            {"ID","INT","NOT NULL PRIMARY KEY"},
            {"EMAIL","VARCHAR(256)","NOT NULL"},
            {"NAME","VARCHAR(256)",""},
        };
        TABLE_VALUE tblVals[3] = {
            {1,"jack@loki.org","jack"},
            {2,"alice@loki.org","alice"},
            {3,"alice@loki.co.jp","alice"},
        };  // 今気がついたけど、こればBatch Insert になるからNG だな。通常は一行Insert になる。
        InsertTx* insertTx = new InsertTx(fileName,tblName,key,tblCols,3,tblVals,3);
        ITransaction* tx = static_cast<ITransaction*>(insertTx);
        Transaction transaction(tx);
        return transaction.proc();
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
}
int testUpdateTransaction(const unsigned int& pk) {
        cout << "--------- testUpdateTransaction" << endl;
        SAMPLE_DATA sample[] = {{pk,"delek@loki.org"}};
        UpdateTx* updateTx = new UpdateTx(pk,sample);
        ITransaction* tx = static_cast<ITransaction*>(updateTx);
        Transaction transaction(tx);
        return transaction.proc();
}
int testDeleteTransaction(const unsigned int& pk) {
    cout << "--------- testDeleteTransaction" << endl;
    DeleteTx* deleteTx = new DeleteTx(pk);
    ITransaction* tx = static_cast<ITransaction*>(deleteTx);
    Transaction transaction(tx);
    return transaction.proc();
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

/**
    分からないことをどこまで無くせるのか。
    まずはバイナリファイルの一行インサートを行う。
    扱うデータは SAMPLE_B_DATA 。

    分かったこと、バイナリにおける改行コード、それもバイナリに過ぎないという当たり前すぎる事実。
    つまりたいした価値がないとも言える。
*/
int test_insert_sample_b() {
    try {
        char filePath[] = {"../tmp/sample_b.bin"};
        FILE* fp = NULL;
        fp = fopen(filePath,"wb+");
        if(fp != NULL) {
            SAMPLE_B_DATA samples[] = {
                {1,"jack@loki.org",'A'},
                {2,"alice@loki.org",'B'},
                {3,"jabberwock@loki.org",'C'},
                {4,"cheshire@loki.org",'D'},
                {5,"rabbit@loki.org",'E'},
                {6,"derek@loki.org",'F'},
            };
            fwrite(samples,sizeof(SAMPLE_B_DATA),sizeof(samples)/sizeof(samples[0]),fp);
            fclose(fp);            
        } else {
            throw runtime_error("Error: File pointer is null.");
        }
        return 0;
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
}
/**
    # 検索に関する考察

    fseek ? これは都度読み込みには利用できるが、C/C++ でできること、私が調べた限りでは
    全読み込み、全書き換え。この考え方が基本になるようだ。
    現状の作りでは言えば 100 万件のレコードが存在した場合はファイルの開閉が 100 万回実行される。
    やはり、検索用にワン・ファイルにレコードをまとめるのは現実的ではないのかもしれない。
    ビッグデータの考えのように緩やかにデータをまとめるのはありかもしれないが、即効性のものは現実的ではない。
    であれば、データを分割して保存する仕組みしか残ってない気がする。
    つまり、[PK].bin のように [データカラム番号]フォルダ内に[email_value].bin のようなディレクト構造に
    することである。 このようなデータ管理であれば、ファイルの開閉をせずに済むはずだから。

    Like 検索を考えるとさらにややこしくなるな：）あるいは、コメントやメモなどの長文テキストの扱いをどうするのか。
    という問題もある、この辺は現状は保留する。

    システムの具体的なデータの持ち方、ディレクトリ構造を考えてみる。
    例えば、[PK].bin と同一階層に「email」「address」「phone_1」などのシステムカラムのディレクトリを置き
    その中に[email_value].bin ファイルを作る。これは、どの項目に索引（Index）をつけるのかということに似ているかもしれない。
    つまり、検索対象を予め決めておく。
    Key Value Store これをもう少し掘り下げて調べるのも参考になるかもしれない。
*/

/**
    # CREATE DATABASE に関する考察

    任意のディレクトリ、仮に sys をシステムのトップ階層とした場合、その下にデータベース名のディレクトリを作る。
    そして、各カラム毎にデータベースディレクトリ配下にカラム・ディレクトリを作る。
    こんなイメージを持っている。
    検索を行う前に、まずこれを実現してみたくなった。
    ```
    CREATE TABLE db_name
    ```
    - コマンドを半角スペースで分割する。
    - データベース名を取得する。
    - データベース名のディレクトリを作成する。
    - db_definition.bin を作成する。（現状空ファイルなのでいらないかもしれない）
    
    # CREATE TABLE に関する考察

    PK とは別に システムキ（SK）が必要かもしれない。
    だだし、両方はいらない。
    PK が明示されている場合はファイル内にもその値を記載するが、明示されない場合は値の記載はない。
    現状はそう結論する。
    PK は NOT NULL で重複不可のカラム・オプションをさす。
    UK は NULL を許可した重複不可のカラム・オプションをさす。（NULL 値は重複可とみなす）

    重複可能なデータをただしく検索できるようにするためには、カラム・ディレクトリの配下に PK ディレクトリ
    あるいは、ファイル名が重複しない仕組みが必要と考える。
    具体的な方法に関しては保留する。

    テーブルディレクトリ配下にカラム・ディレクトリを持つ。
    テーブルディレクトリの直下に テーブル定義ファイル、tbl_definition.bin を作る。
    テーブル定義ファイルにそのテーブルに関するデータ構造を記載する（カラム名、データ型、オプション）。
    テーブル定義ファイルは CLI システム起動時に随時読み込まれ、メモリに展開されるものになる。
    これで、異なるテーブルに対する処理が可能になるはず。
    テーブル定義ファイルはシステムの初期データとも言える。
    ```
    CREATE TABLE テーブル名 (
        カラム名 データ型 オプション,
        ～～～
    );
    ```
    ```
    CREATE TABLE PointList (
        ID       INT(3)       NOT NULL PRIMARY KEY,
        Name     VARCHAR(50)  NOT NULL,
        Class    VARCHAR(1),
        Japanese INT(3)       DEFAULT '0',
        Math     INT(3)       DEFAULT '0', 
        English  INT(3)       DEFAULT '0'
    );
    クリエイト・テーブル構文のパースも厄介だな、これはこれで時間をとってじっくりやりたい。
    最大で 3 のデータ構造で表現可能だと思う。

    // 出来上がったテーブルがこちら
    +----------+-------------+------+-----+---------+-------+
    | Field    | Type        | Null | Key | Default | Extra |
    +----------+-------------+------+-----+---------+-------+
    | ID       | int(3)      | NO   | PRI | NULL    |       |
    | Name     | varchar(50) | NO   |     | NULL    |       |
    | Class    | varchar(1)  | YES  |     | NULL    |       |
    | Japanese | int(3)      | YES  |     | 0       |       |←デフォルト値0が設定されている
    | Math     | int(3)      | YES  |     | 0       |       |←デフォルト値0が設定されている
    | English  | int(3)      | YES  |     | 0       |       |←デフォルト値0が設定されている
    +----------+-------------+------+-----+---------+-------+
```
*/
/**
    DB ディレクトリの作成を行う。

    - コマンドを半角スペースで分割する。
    - データベース名を取得する。
    - データベース名のディレクトリを作成する。
    - db_definition.bin を作成する。（現状空ファイルなのでいらないかもしれない）
*/
int test_create_database(const char* dbName) {
    printf("DEBUG: dbName is %s\n",dbName);
    if(mkdir(dbName, 0777)==0) {    // すでに同一名であってもエラーにはならない。
        printf("DEBUG: succeed mkdir. it's %s\n",dbName);
    }
    return 0;
}


/**
    Table ディレクトリの作成を行う。
    Table ディレクトリの上位はDB ディレクトリになる。

    - コマンドのパースは無視する（ここでは行わない）。
    - struct の構造体にコマンドのカラム名、データ型、オプションを入力（設定）する。
        - データ型に関しては厳密に定義する必要があるが今回は省く。
        - オプションに関しても同様に省く。
    - tbl_definition.bin（構造体のI/O） と tbl_definition.txt（ユーザ入力コマンドのコピ） を作成する。
    - 構造体を元にディレクトリを作成する。

    e.g. 次のコマンドのパースが成功したものとして、ディレクトリ階層を作ること。 
    ```
        - ID        システム自動設定、  INT             NOT NULL PRIMARY KEY        0。
        - EMAIL     ユーザ入力、        VARCHAR(256)    NOT NULL                    1。
        - NAME      ユーザ入力、        VARCHAR(128)    NOT NULL                    2。
        - PHONE_1   ユーザ入力、        VARCHAR(16)     NOT NULL                    3。
    ```
    非常に些細なことだが、マルチバイト文字と半角文字におけるLength が気になる。
    C/C++ ではこれはどのようにカウントされるのだろうか。
    これがはっきりしないとValidation も VARCHAR() 設定も正しく理解できない。
    という訳でまずはこれを確認したい。
*/
int test_create_table(const char* dbName, const char* tblName) {    // 前提条件として、DB名 と Tbl名 が定義されていて、DB ディレクトリがあること。
    printf("DEBUG: dbName is %s\n",dbName);
    printf("DEBUG: tblName is %s\n",tblName);

    char cname[] = {"鈴木 一郎"};
    string sname = "鈴木 一郎";
    int len = strlen(cname);
    printf("len is %d\n",len);
    printf("size is %ld\n",sname.size());
    printf("length is %ld\n",sname.length());
    // つまりマルチバイトでは 2 bytes 消費している。
    // 入力チェック、結局指定したバイト数を超えてなければOK でいいのかな。

    // テーブル・ディレクトリを作成する。
    char tblDir[256] = {"\0"};
    strcat(tblDir,dbName);
    strcat(tblDir,"/");
    strcat(tblDir,tblName);
    printf("tblDir is %s\n",tblDir);
    if(mkdir(tblDir, 0777)==0) {
        printf("DEBUG: succeed mkdir. it's %s\n",tblDir);
    }
    TABLE_COLUMN tblCols[4] = {
        {"ID","INT","NOT NULL PRIMARY KEY"},
        {"EMAIL","VARCHAR(256)","NOT NULL"},
        {"NAME","VARCHAR(128)","NOT NULL"},
        {"PHONE_1","VARCHAR(128)","NOT NULL"},
    };
    // 配列で要素数が明確な場合は次の計算ができる、まぁする必要もないけど。
    printf("there are %ld cols.\n",sizeof(tblCols)/sizeof(tblCols[0]));
    tblDir[strlen(tblDir)] = '/';
    tblDir[strlen(tblDir)+1] = '\0';    // これがカラムのベースになるディレクトリ階層。
    char tmp[256] = {"\0"};
    for(int i=0; i<sizeof(tblCols)/sizeof(tblCols[0]); i++) {
        // カラム・ディレクトリを作成する。
        printf("i is %d\n",i);
        memcpy(tmp,tblDir,strlen(tblDir));
        strcat(tmp,tblCols[i].column);
        printf("tmp is %s\n",tmp);
        if(mkdir(tmp, 0777)==0) {
            printf("DEBUG: succeed mkdir. it's %s\n",tmp);    
        }
        initArray(tmp,256);
    }
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
    if(0) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... test_insert_system_data",test_insert_system_data("1.bin"));
        ptr_lambda_debug<const string&,const int&>("Play and Result ... test_insert_system_data",test_insert_system_data("2.bin"));
    }
    // 自分が知らないという事実を正しく理解し、それを理解できるようにする取り組み。
    // 次の新たな課題だな、ファイルの中身の操作、この無知をどれだけ無くせるのか、そんなところ：）
    if(0) {
        ptr_lambda_debug<const string&,const int&>("Let's try file operations.",0);
        ptr_lambda_debug<const string&,const int&>("Play and Result ... test_insert_sample_b",test_insert_sample_b());
    }
    if(3.0) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... test_create_database",test_create_database("../tmp/test"));
        ptr_lambda_debug<const string&,const int&>("Play and Result ... test_create_table",test_create_table("../tmp/test","address_book"));
    }
    if(3.1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... testInsertTransaction",testInsertTransaction(100,"100.bin"));
        // 例外確認を行っている、exit(1) で終了することを期待している。
        // ptr_lambda_debug<const string&,const int&>("Play and Result ... testInsertTransaction",testInsertTransaction(1,"100111111111111111111111111111111111111111111111111111111111.bin"));
        ptr_lambda_debug<const string&,const int&>("Play and Result ... testUpdateTransaction",testUpdateTransaction(100));
        ptr_lambda_debug<const string&,const int&>("Play and Result ... testDeleteTransaction",testDeleteTransaction(100));
    }
    // カラム・ディレクトの作成はできた。
    // Create Database と Create Table のコマンドは保留した。
    // 現状できていること 、登録・更新系の修正だろう。
    // つまり、カラム・ディレクト内にプライマリキ・ディレクトを作成して、そこにカラム・ファイル（email_val.bin）を
    // 作成すること、中身は空で構わない。
    // 3.2 で上記の修正を行う予定。
    if(3.2) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... testInsertTransaction_v2",testInsertTransaction_v2(200,"test/address_book/200.bin","test/address_book/"));
        // CREATE DATABASE
        // CREATE TABLE 
        // cli.cpp ... この2つを次はやるかもしれない。
    }
    cout << "=============== cli_file END" << endl;
    return 0;
}
