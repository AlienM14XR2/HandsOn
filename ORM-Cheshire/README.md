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

これで、旧来の API と MySQL X DevAPI が利用できるライブラリ及びそれらのヘッダファイルがインストールされる（はず：）。

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

## libpqxx

私が行ったことを羅列する、時間があれば整理する。

### CMake

libpqxx のソースをビルドするのに必要だった。

https://ja.linux-console.net/?p=15565

sudo apt -y install build-essential libssl-dev

cd ~/downloads

tar -zxvf cmake-3.28.3.tar.gz

cd cmake-3.28.3/

sudo ./bootstrap

sudo make

sudo make install

cmake --version

```
sudo make uninstall
```

### PostgreSQL インストール

https://www.digitalocean.com/community/tutorials/how-to-install-and-use-postgresql-on-ubuntu-20-04-ja

sudo apt install postgresql postgresql-contrib

sudo -i -u postgres

psql

\q

// sudo -i -u postgres ... postgres で Ubuntu login すること。
createuser --interactive

createdb derek

// sudo 権限を持つユーザで実行すること
sudo adduser derek

sudo -i -u derek
psql

\conninfo

postgres=# ALTER ROLE derek WITH PASSWORD 'derek1234';
ALTER ROLE
postgres=# /q

$ psql -U derek -W

derek=# CREATE DATABASE jabberwocky;
CREATE DATABASE
derek=# \d
Did not find any relations.
derek=# \l
derek=# \c jabberwocky
Password: 
You are now connected to database "jabberwocky" as user "derek".
jabberwocky=# 


CREATE TABLE animal (
    id SERIAL NOT NULL PRIMARY KEY
    , name VARCHAR(128) NOT NULL
);

INSERT INTO animal (name) values ('Lion');

// e.g. CREATE TABLE
CREATE TABLE playground (
    equip_id serial PRIMARY KEY,
    type varchar (50) NOT NULL,
    color varchar (25) NOT NULL,
    location varchar(25) check (location in ('north', 'south', 'west', 'east', 'northeast', 'southeast', 'southwest', 'northwest')),
    install_date date
);

### C++ client API for PostgreSQL

https://pqxx.org/development/libpqxx/

sudo apt install libpq-dev

cd ~/dev/

git clone -b master --recursive https://github.com/jtv/libpqxx.git

cd libpqxx/

./configure --disable-shared --with-postgres-include

make

sudo make install

pg_config --libdir

```
sudo make uninstall
```

```
 /usr/bin/mkdir -p '/usr/local/lib/pkgconfig'
 /usr/bin/install -c -m 644 libpqxx.pc '/usr/local/lib/pkgconfig'

less less /usr/local/lib/pkgconfig/libpqxx.pc

prefix=/usr/local
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: libpqxx
Description: C++ client API for the PostgreSQL database management system.
Version: 7.9.0
Libs: -L${libdir} -lpqxx
Cflags: -I${includedir}


~/dev/libpqxx$ ls -alh /usr/local/lib/
合計 21M
drwxr-xr-x  3 root root 4.0K  3月  6 07:25 .
drwxr-xr-x 11 root root 4.0K  3月  6 07:25 ..
-rw-r--r--  1 root root  21M  3月  6 07:25 libpqxx.a
-rwxr-xr-x  1 root root  884  3月  6 07:25 libpqxx.la
drwxr-xr-x  2 root root 4.0K  3月  6 07:25 pkgconfig
```

sudo -u postgres psql

## mongo-cxx-driver

@see https://www.mongodb.com/docs/languages/cpp/drivers/current/installation/linux/#std-label-cpp-installation-linux

r3.10.1 現時点（2024-03-16）での最新バージョン

```
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.10.1/mongo-cxx-driver-r3.10.1.tar.gz

tar -xzf mongo-cxx-driver-r3.10.1.tar.gz
cd mongo-cxx-driver-r3.10.1/build
```

```
cmake ..                                \
    -DCMAKE_BUILD_TYPE=Release          \
    -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF
```

```
cmake --build .
sudo cmake --build . --target install
```

最終的に次のような出力になった。

```
-- Installing: /usr/local/lib/libmongocxx.so
-- Installing: /usr/local/lib/pkgconfig/libmongocxx.pc  // これを見れば詳細が分かるかも。
-- Installing: /usr/local/share/mongo-cxx-driver/LICENSE
-- Installing: /usr/local/share/mongo-cxx-driver/README.md
-- Installing: /usr/local/share/mongo-cxx-driver/THIRD-PARTY-NOTICES
-- Installing: /usr/local/share/mongo-cxx-driver/uninstall.sh
```


## OSS ORM

この課題が終わったら、次を見てみる（やはり既にあるよね：）。

https://codesynthesis.com/products/odb/
