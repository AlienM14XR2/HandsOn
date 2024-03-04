# Start at 2024-02-XX

# Lost Chapter Cheshire

O/R Mapping を題材に、現在の自分の C++ のスキル向上及び、不明点、疑問点を見つけることを主目的にしているプロジェクトである。

Ubuntu を開発 OS としているが、相変わらず Linux に関しては不慣れで知識も少ない。当然、開発環境の構築には色々と間違うことが多々ある。したがって、まず始めに、外部依存ライブラリの導入方法を簡単に記載する（URL を貼って終わりな気がしなくもない：）。

# 依存ライブラリ

## MySQL Connector/C++

本家サイト

https://dev.mysql.com/doc/mysql-shell/8.0/ja/mysql-shell-install-linux-quick.html

ここにある「MySQL APT リポジトリ」から リポジトリを DL する。

「MySQL APT リポジトリの追加」、次の URL の情報が一番確実で早かったと記憶している。

https://dev.mysql.com/doc/mysql-apt-repo-quick-guide/en/#apt-repo-setup

これで、旧来のライブラリとMySQL Shell が利用できるライブラリ及びそれらのヘッダファイルがインストールされる（はず：）。

次のコマンドでライブラリとヘッダファイルの場所がわかる。
```
$ dpkg -L libmysqlcppconn8-2
```

```
$ dpkg -L libmysqlcppconn-dev
```

当分先になるのか、はたまた、興味がなくなるのか。それは分からないが、次は PostgreSQL にも対応したい。現状、MySQL への対応がまだ終わっていないことを考えれば、やはり後回しになるのだろう。

The C++ connector for PostgreSQL

https://pqxx.org/development/libpqxx/

## OSS ORM

この課題が終わったら、次を見てみる（やはり既にあるよね：）。

https://codesynthesis.com/products/odb/