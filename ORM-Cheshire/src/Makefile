# ベタ書きではじめる。

# コンパイラ
CC  = g++

# コンパイルオプション
CFLAGS_D  = -O3 -DDEBUG  -std=c++20 -pedantic-errors -Wall -Werror
CFLAGS_N  = -O3 -DNDEBUG -std=c++20 -pedantic-errors -Wall -Werror

# インクルードファイルのあるディレクトリパス
INCDIR  = -I../inc/ -I/usr/include/mysql-cppconn-8/

# ライブラリファイルのあるディレクトリパス
LIBDIR  = -L/usr/lib/x86_64-linux-gnu/
 
# 追加するライブラリファイル
LIBS    = -lmysqlcppconn -lmysqlcppconn8 -lpqxx -lpq

# 実行ファイル名
TARGET  = ../bin/main

objects:
	$(CC) $(CFLAGS_N) $(INCDIR) -c ./data/PersonData.cpp -o ../bin/PersonData.o
	$(CC) $(CFLAGS_N) $(INCDIR) -c ./model/PersonStrategy.cpp -o ../bin/PersonStrategy.o
	$(CC) $(CFLAGS_N) $(INCDIR) -c ./driver/MySQLDriver.cpp -o ../bin/MySQLDriver.o
	$(CC) $(CFLAGS_N) $(INCDIR) -c ./connection/MySQLConnection.cpp -o ../bin/MySQLConnection.o
	$(CC) $(CFLAGS_N) $(INCDIR) -c ./sql_generator.cpp -o ../bin/sql_generator.o
	$(CC) $(CFLAGS_N) $(INCDIR) -c ./repository/PersonRepository.cpp -o ../bin/PersonRepository.o

target:
	$(CC) $(CFLAGS_N) $(INCDIR) $(LIBDIR) ./test/test_1.cpp main.cpp $(LIBS) ../bin/PersonRepository.o ../bin/sql_generator.o ../bin/PersonStrategy.o ../bin/PersonData.o ../bin/MySQLDriver.o ../bin/MySQLConnection.o -o $(TARGET)

clean:
	-rm -f ../bin/*.o $(TARGET)

