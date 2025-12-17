cmake_minimum_required(VERSION 3.15)
project(GenericDatabaseTests CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- 依存ライブラリの検索 ---
find_package(GTest REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)
pkg_check_modules(PQ REQUIRED libpq)

# MySQL C++ コネクタは、find_package で見つからないため手動パス指定とする


# --- ターゲット（実行ファイル）の定義 ---

# 1. PostgreSQL テストランナー
add_executable(pqxx_template_test_runner src/pqxx_template_test.cpp)

target_include_directories(pqxx_template_test_runner PRIVATE 
    inc  # プロジェクト内のインクルードディレクトリ (相対パス)
    ${PQXX_INCLUDE_DIRS}
    ${PQ_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(pqxx_template_test_runner PRIVATE 
    ${PQXX_LIBRARIES} 
    ${PQ_LIBRARIES} 
    GTest::gtest_main 
)

target_compile_options(pqxx_template_test_runner PRIVATE 
    -Wall 
    -Wextra
    -Werror
    -Wno-unused-parameter
    $<$<CONFIG:Release>:-O3 -pedantic-errors> # リリースビルド時のみ最適化と pedantic-errors を追加
)

target_compile_definitions(pqxx_template_test_runner PRIVATE 
    $<$<CONFIG:Debug>:DEBUG> # デバッグビルド時のみ -DDEBUG マクロを定義
)


# 2. MySQL テストランナー
add_executable(mysql_template_test_runner src/mysql_template_test.cpp)

target_include_directories(mysql_template_test_runner PRIVATE 
    inc/
    /usr/include/mysql-cppconn/ # 手動指定パス1
    /usr/local/include          # 手動指定パス2
    ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(mysql_template_test_runner PRIVATE 
    mysqlcppconn                # 手動指定ライブラリ名
    mysqlcppconnx               # 手動指定ライブラリ名
    GTest::gtest_main
)

target_compile_options(mysql_template_test_runner PRIVATE 
    -Wall 
    -Wextra
    -Werror
    -Wno-unused-parameter
    $<$<CONFIG:Release>:-O3 -pedantic-errors> # リリースビルド時のみ最適化と pedantic-errors を追加
)

target_compile_definitions(mysql_template_test_runner PRIVATE 
    $<$<CONFIG:Debug>:DEBUG> # デバッグビルド時のみ -DDEBUG マクロを定義
)
