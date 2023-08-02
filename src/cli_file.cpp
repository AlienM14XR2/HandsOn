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
public:
    /**
     * デフォルトコンストラクタの場合はメンバ変数 filePath にシステムのデフォルト値が設定される。
    */
    ORx2File():filePath{nullptr},fp{NULL} {
        fp = fopen(SYSTEM_DEFAULT_PATH,"a+");
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
        fp = fopen(filePath,"a+");
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
};
int testOpenClose() {
    try {
        string data_a = "SampleA.";
        ORx2File x2File;
        ptr_lambda_debug<const string&,const int&>("Play and Result ... x2File.write is",x2File.write(data_a));
        
        const char* data_b = "SampleB.";
        ORx2File x2File_b("../tmp/ORx2File_b.bin");
        ptr_lambda_debug<const string&,const int&>("Play and Result ... x2File_b.write is",x2File_b.write(data_b));
        // ここまでで各ファイルへの文字列の書き込みはできた、その確認は GHex で行った。
        return 0;
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
}

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
