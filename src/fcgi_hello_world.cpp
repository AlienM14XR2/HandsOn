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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcgi_config.h>
#include <fcgi_stdio.h>

int main()
{
    int count = 0;      // これが答えだったか、何らかのオブジェクトを Pool するならここで行い
    while(FCGI_Accept() >= 0)
    {
        printf("content-type:text/html\r\n");
        printf("\r\n");
        printf("<title>Fast CGI Hello</title>");
        printf("<h1>fast CGI hello</h1>");

        printf("Request number %d running on host<i>%s</i>\n",++count,getenv("SERVER_NAME"));
    }
    // while を抜けた際に、取得したメモリは解放する
    return 0;
}

// EXTERNAL
// #include "fcgio.h"
// // STD
// #include <iostream>
// #include <memory>
// #include <functional>
// #include <string>
// #include <cstring>
// #include <fcgi_config.h>
// #include <fcgi_stdio.h>

// auto main() -> int
// {
//   // 元の標準入出力系の streambuf 群をバックアップ＆カスタムデリーターによるリストア処理の予約
//   using streambuf_restorer_signature_type = auto ( std::streambuf* ) -> void;
//   using streambuf_restorer_functor_type = std::function< streambuf_restorer_signature_type >;
//   using streambuf_restorer_type = std::unique_ptr< std::streambuf, streambuf_restorer_functor_type >;

//   const auto cin_restorer  = streambuf_restorer_type( std::cin.rdbuf(), []( auto in ) { std::cin.rdbuf( in ); } );
//   const auto cout_restorer = streambuf_restorer_type( std::cout.rdbuf(), []( auto in ) { std::cout.rdbuf( in ); } );
//   const auto cerr_restorer = streambuf_restorer_type( std::cerr.rdbuf(), []( auto in ) { std::cerr.rdbuf( in ); } );

//   FCGX_Request request;

//   FCGX_Init();
//   FCGX_InitRequest( &request, 0, 0 );

//   while ( FCGX_Accept_r( &request ) == 0 )
//   {
//     auto cin_buffer  = fcgi_streambuf( request.in );
//     auto cout_buffer = fcgi_streambuf( request.out );
//     auto cerr_buffer = fcgi_streambuf( request.err );

//     std::cin.rdbuf( &cin_buffer );
//     std::cout.rdbuf( &cout_buffer );
//     std::cerr.rdbuf( &cerr_buffer );

//     std::string content_header_part = R"(Content-type: text/html
// <html>
//   <head>
//     <title>Hello, World!</title>
//   </head>
//   <body>
// )";

//     std::string content_footer_part = R"(  </body>
// </html>
// )";

//     // リクエストにいわゆる POST で投げられたデータがあるか CONTENT_LENGTH で確認
//     const auto in_content_length_string = FCGX_GetParam( "CONTENT_LENGTH", request.envp );
//     const auto in_content_length = std::strlen( in_content_length_string )
//       ? std::stoull( std::string( in_content_length_string ) )
//       : 0ull
//       ;

//     // リクエストの CONTENT_LENGTH だけリクエストボディーを読み出す
//     std::string in_content;
//     in_content.resize( in_content_length );
//     std::cin.read( &in_content[0], in_content_length );

//     // レスポンスを出力する
//     std::cout
//       << content_header_part
//       << "    <h1>Hello, World!</h>\n"
//          "    <p>REQUEST_URI: " << FCGX_GetParam( "REQUEST_URI", request.envp ) << "</p>\n"
//          "    <p>REQUEST CONTENT_LENGTH: " << in_content_length << "</p>\n"
//          "    <p>REQUEST CONTENT: " << in_content << "</p>\n"
//       << content_footer_part
//       ;

//     // スコープの終了により明示的に std::flush して居なくても fcgi_streambuf のデストラクターでフラッシュされる
//   }

//   // スコープの終了により標準入出力系の streambuf 群は自動的にバックアップからリストアされる
// }
