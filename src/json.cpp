/**
 * C++ JSON ライブラリ動作確認
 * 
 * nlohmann-json
 * ```
 * sudo apt install nlohmann-json3-dev
 * ```
 * /usr/include/nlohmann
 * 上記ディレクトリ以下に必要なヘッダファイルがある。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror json.cpp -o ../bin/main
*/
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(void) {
    puts("=== START C++ JSON ライブラリ動作確認");
    puts("C++ JSON ライブラリ動作確認   END ===");
}
