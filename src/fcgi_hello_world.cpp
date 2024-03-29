/**
 * fcgi サンプル
 * 
 * 残念、最後まで上手く確認できなかった。
 * 502 Bad Gateway orz
 * 
 * 現時点では Web Server (Nginx) と fcgi の知識が足りなすぎると判断した。
 * より単純なコードに変更した。
 * 
 * うん、一応の確認は出来た：）以下は手順の羅列になる。
 * 
 * Nginx は事前に別途インストールしていたので割愛する（@see https://www.digitalocean.com/community/tutorials/how-to-install-nginx-on-ubuntu-20-04-ja）。
 * 
 * sudo apt install libfcgi-dev spawn-fcgi
 * 
 * /etc/nginx/sites-available/default
 * 
 * を編集した。
 * 
 * server ディレクティブの中に次を追加した
 * 
       location /hello_world
        {
                # spawn-fcgi でプロセスを動作させる host:port
                fastcgi_pass   127.0.0.1:9000;
                # nginx の FCGI 各種パラメーター群の設定
                fastcgi_param  GATEWAY_INTERFACE  CGI/1.1;
                fastcgi_param  SERVER_SOFTWARE    nginx;
                fastcgi_param  QUERY_STRING       $query_string;
                fastcgi_param  REQUEST_METHOD     $request_method;
                fastcgi_param  CONTENT_TYPE       $content_type;
                fastcgi_param  CONTENT_LENGTH     $content_length;
                fastcgi_param  SCRIPT_FILENAME    $document_root$fastcgi_script_name;
                fastcgi_param  SCRIPT_NAME        $fastcgi_script_name;
                fastcgi_param  REQUEST_URI        $request_uri;
                fastcgi_param  DOCUMENT_URI       $document_uri;
                fastcgi_param  DOCUMENT_ROOT      $document_root;
                fastcgi_param  SERVER_PROTOCOL    $server_protocol;
                fastcgi_param  REMOTE_ADDR        $remote_addr;
                fastcgi_param  REMOTE_PORT        $remote_port;
                fastcgi_param  SERVER_ADDR        $server_addr;
                fastcgi_param  SERVER_PORT        $server_port;
                fastcgi_param  SERVER_NAME        $server_name;
        }

sudo systemctl restart nginx

g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror fcgi_hello_world.cpp -lfcgi++ -lfcgi -o ../bin/hello_world

// プロセス起動
spawn-fcgi -p 9000 -n hello_world

curl -i -s -X POST -d 'this is the post content.' http://localhost/hello_world

// エラーの確認
sudo tail -f /var/log/nginx/error.log

 * curl 及び ブラウザでの確認はひとまず出来た、今回はここまで：）
 * 
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror fcgi_hello_world.cpp -lfcgi++ -lfcgi -o ../bin/hello_world
*/
#include <iostream>
#include <memory>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcgi_config.h>
#include <fcgi_stdio.h>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << "\t" << debug << std::endl;
};

template <class T>
class Controller {
public:
    virtual ~Controller() = default;
    virtual T execute() const = 0;
};

class CreatePersonCtr final : public Controller<std::string> {
public:
    static Controller* factory(const std::string& uri) {
        printf("\nDEBUG: uri is %s\n", uri.c_str());
        if(uri == "/hello_world/create/person/" || uri == "/hello_world/create/person") {
            return new CreatePersonCtr();
        }
        return nullptr;
    }
    virtual std::string execute() const override {
        puts("------ CreatePersonCtr::execute()");
        return "CreatePersonCtr";
    }
};

class ReadPersonCtr final : public Controller<std::string> {
public:
    static Controller* factory(const std::string& uri) {
        if(uri == "/hello_world/read/person/" || uri == "/hello_world/read/person") {
            return new ReadPersonCtr();
        }
        return nullptr;
    }
    virtual std::string execute() const override {
        puts("------ ReadPersonCtr::execute()");
        return "ReadPersonCtr";
    }
};

class UpdatePersonCtr final : public Controller<std::string> {
public:
    static Controller* factory(const std::string& uri) {
        if(uri == "/hello_world/update/person/" || uri == "/hello_world/update/person") {
            return new UpdatePersonCtr();
        }
        return nullptr;
    }
    virtual std::string execute() const override {
        puts("------ UpdatePersonCtr::execute()");
        return "UpdatePersonCtr";
    }
};

class DeletePersonCtr final : public Controller<std::string> {
public:
    static Controller* factory(const std::string& uri) {
        if(uri == "/hello_world/delete/person/" || uri == "/hello_world/delete/person") {
            return new DeletePersonCtr();
        }
        return nullptr;
    }
    virtual std::string execute() const override {
        puts("------ DeletePersonCtr::execute()");
        return "DeletePersonCtr";
    }
};

void action(const Controller<std::string>* ctr) 
{
    std::string ret = ctr->execute();
    printf("\nret is %s\n", ret.c_str());
    ptr_lambda_debug<const char*, const std::string&>("ret is ", ret);  // これは標準出力としてコンソールに出力される
    delete ctr;
    ctr = nullptr;
}

int main()
{
    int count = 0;      // これが答えだったか、何らかのオブジェクトを Pool するならここで行い
    while(FCGI_Accept() >= 0)
    {
        printf("content-type:text/html\r\n");
        printf("\r\n");
        printf("<title>Fast CGI Hello</title>");
        printf("<h1>fast CGI hello</h1>");

        printf("request uri is %s\n", getenv("REQUEST_URI"));   // これをもとに各処理に分岐できる、REST API のエンドポイントとして充分使えそう。
        printf("Request number %d running on host<i>%s</i>\n",++count,getenv("SERVER_NAME"));

        Controller<std::string>* ctr = nullptr;
        ctr = CreatePersonCtr::factory(getenv("REQUEST_URI"));
        if(ctr) {   // このブロック内は関数化できるよね（DRY：）。
            action(ctr);
        }
        ctr = ReadPersonCtr::factory(getenv("REQUEST_URI"));
        if(ctr) {
            action(ctr);
        }
        ctr = UpdatePersonCtr::factory(getenv("REQUEST_URI"));
        if(ctr) {
            action(ctr);
        }
        ctr = DeletePersonCtr::factory(getenv("REQUEST_URI"));
        if(ctr) {
            action(ctr);
        }
        /**
        * URI の重複、コーディング・ミスがなければ、上記の実装でいいと思う。
        */
        
    }
    // while を抜けた際に、取得したメモリは解放する
    return 0;
}

/**
 * URI で if 文書きたくない。
 * 
 * 単純に考えれば
 * 
 * if(uir == "/api/insert/person/") {
 * } 
 * else if(...) {
 * }
 * else if(...) {
 * }
 * ...
 * のようになるが、必要な処理を行うオブジェクトを生成する側で勝手に判断してほしいということ。
 * 例えば、必要な処理を定義するクラスの static に factory を用意して、その factory 内でその
 * オブジェクトを生成するか否かの判断ができるはず。
*/