cmake_minimum_required(VERSION 3.15)
project(GenericDatabaseTests CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- 依存ライブラリの検索 ---
find_package(GTest REQUIRED)
find_package(PkgConfig REQUIRED)

# PostgreSQL
pkg_check_modules(PQXX REQUIRED libpqxx)
pkg_check_modules(PQ REQUIRED libpq)

# MongoDB (pkg-config --cflags --libs libmongocxx に対応)
pkg_check_modules(MONGOCXX REQUIRED libmongocxx)

# --- 共通のコンパイルオプション定義 ---
set(COMMON_COMPILE_OPTIONS 
    -Wall 
    -Wextra 
    -Werror 
    -Wno-unused-parameter
    $<$<CONFIG:Release>:-O3 -pedantic-errors>
)

set(COMMON_COMPILE_DEFINITIONS
    $<$<CONFIG:Debug>:DEBUG>
)

# --- ターゲット（実行ファイル）の定義 ---

# 1. PostgreSQL テストランナー
add_executable(pqxx_template_test_runner src/pqxx_template_test.cpp)
target_include_directories(pqxx_template_test_runner PRIVATE 
    inc 
    ${PQXX_INCLUDE_DIRS}
    ${PQ_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(pqxx_template_test_runner PRIVATE 
    ${PQXX_LIBRARIES} 
    ${PQ_LIBRARIES} 
    GTest::gtest_main 
)
target_compile_options(pqxx_template_test_runner PRIVATE ${COMMON_COMPILE_OPTIONS})
target_compile_definitions(pqxx_template_test_runner PRIVATE ${COMMON_COMPILE_DEFINITIONS})


# 2. MySQL テストランナー
add_executable(mysql_template_test_runner src/mysql_template_test.cpp)
target_include_directories(mysql_template_test_runner PRIVATE 
    inc
    /usr/include/mysql-cppconn/ 
    /usr/local/include          
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(mysql_template_test_runner PRIVATE 
    mysqlcppconn                
    mysqlcppconnx               
    GTest::gtest_main
)
target_compile_options(mysql_template_test_runner PRIVATE ${COMMON_COMPILE_OPTIONS})
target_compile_definitions(mysql_template_test_runner PRIVATE ${COMMON_COMPILE_DEFINITIONS})


# 3. MongoDB テストランナー (追加分)
add_executable(mongo_template_test_runner src/mongo_template_test.cpp)
target_include_directories(mongo_template_test_runner PRIVATE 
    inc
    /usr/local/include/bson-2.1.2/ 
    ${MONGOCXX_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(mongo_template_test_runner PRIVATE 
    ${MONGOCXX_LIBRARIES}
    bson2                          
    GTest::gtest_main
)
target_compile_options(mongo_template_test_runner PRIVATE ${COMMON_COMPILE_OPTIONS})
target_compile_definitions(mongo_template_test_runner PRIVATE ${COMMON_COMPILE_DEFINITIONS})


# 4. ObjectPool 単体テストランナー (今回追加)
add_executable(object_pool_test_runner src/object_pool_test.cpp)
target_include_directories(object_pool_test_runner PRIVATE 
    inc 
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(object_pool_test_runner PRIVATE 
    GTest::gtest_main 
)
target_compile_options(object_pool_test_runner PRIVATE ${COMMON_COMPILE_OPTIONS})
target_compile_definitions(object_pool_test_runner PRIVATE ${COMMON_COMPILE_DEFINITIONS})
