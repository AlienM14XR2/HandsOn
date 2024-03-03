#include "../inc/sql_generator.hpp"

/**
 * 備忘録、クラスを持たないソースファイルはスネークケースで src 直下に置く。
*/


/**
 * SQL 文の構築に関する考察
 * 
 * INSERT INTO テーブル名 (列1, 列2, ...)
 * VALUES (値1, 値2, ...);
 *
 * INSERT INTO customer (name, route, saved_date, price) 
 * VALUES ('松田', 'ad2', '2023-05-24 19:49:28', 2500);
 *
 * prep_stmt = con->prepareStatement("INSERT INTO test(id, label) VALUES (?, ?)");      // MySQL の Prepared Statements の例
 * 
 * SQL インジェクションの可能性を考慮すれば、Prepared Statements の利用は必須と考える。
 * よって、今回の 動的 SQL 文の最終型は 『"INSERT INTO test (id, label) VALUES (?, ?)"』とする。
 * 
 * カラム名とその数分の ? の出力ということ。
 * 
*/

std::string makeInsertSql(const std::string& tableName, const std::vector<std::string>& colNames) {
    std::string sql("INSERT INTO ");
    sql.append(tableName);
    // カラム
    std::string cols(" (");
    // 値
    std::string vals("VALUES (");
    for(std::size_t i=0 ; i<colNames.size(); i++) {
        cols.append(colNames.at(i));
        vals.append("?");
        if( i < colNames.size()-1 ) {
            cols.append(", ");
            vals.append(", ");
        } 
    }
    cols.append(") ");
    vals.append(")");
    sql.append(std::move(cols));
    sql.append(std::move(vals));
    return sql;
}


/**
 * UPDATE (表名) SET (カラム名1) = (値1) WHERE id = ?
 * 
 * UPDATE table_reference
    SET col_name1 = value1 [, col_name2 = value2, ...]
    [WHERE where_condition]
 * 
 * UPDATE test SET name = ?, label = ? WHERE id = ?
 * 
 * 更新すべき対象は 1行 としたい、Pkey を条件にする。
*/

std::string makeUpdateSql(const std::string& tableName, const std::string& pkName, const std::vector<std::string>& colNames ) {
    std::string sql("UPDATE ");
    sql.append(tableName);
    std::string set(" SET ");
    std::string condition(" WHERE ");
    condition.append(pkName).append(" = ?");
    for(std::size_t i=0; i<colNames.size(); i++) {
        set.append(colNames.at(i)).append(" = ?");
        if( i < colNames.size()-1 ) {
            set.append(", ");
        } 

    }
    sql.append(set).append(condition);
    return sql;    
}


/**
 * DELETE FROM table_name WHERE id = ?
 * 
 * 削除すべき対象は 1行 としたい、Pkey を条件にする。
*/

std::string makeDeleteSql(const std::string& tableName, const std::string& pkName) {
    std::string sql("DELETE FROM ");
    sql.append(tableName).append(" WHERE ").append(pkName).append(" = ?");
    return sql;
}


/**
 * SELECT col1, col2, col3 FROM table WHERE primary-key = ?
 * SELECT primary-key, col1, col2, col3 FROM table WHERE primary-key = ?
 * 
 * 仕様の問題、現状は pkey は RDBMS の Auto Increment を利用することを想定している、これは、次の実装に影響が及ぶ。
 * - PersonStrategy::getColumns メンバ関数に pkey を含めるか否かということ。含めた場合は、別途シーケンステーブルと pkey の取得処理が必要になる。
 * - makeFindOne の SELECT 句 の pkey の取り扱い方法について、上記に依存するため、この関数も本質的には 2 つ必要と思われる。
 * 
 * colNames に pkey は存在しないものとして次の関数は実装する。
 * 
*/

std::string makeFindOneSql(const std::string& tableName, const std::string& pkeyName, const std::vector<std::string>& colNames) {
    std::string sql("SELECT ");
    std::string cols(pkeyName);
    cols.append(", ");
    for(std::size_t i = 0; i < colNames.size(); i++) {
        cols.append(colNames.at(i));
        if( i < colNames.size()-1 ) {
            cols.append(", ");
        } 
    }
    sql.append(cols).append(" FROM ").append(tableName).append(" WHERE ").append(pkeyName).append(" = ?");
    return sql;
}


/**
 * TODO 各 テーブル情報を管理するクラスに CREATE TABLE 文を自動作成する機能がほしい。
 * 
 * CREATE TABLE テーブル名(
 *  列名1 列1の型名 PRIMARY KEY,
 *  列名2 列2の型名 UNIQUE,
 *  :
 *  列名X 列Xの型名
 * )
 * 
 * ※ 問題、Key 制約の管理を RdbData の派生クラスに持たせる必要があることが判明した。
 * このメソッドは必須には該当しないし、ER 図作成アプリから SQL の生成は可能だ。また、
 * many-to-one などのリレーションなどを考え始めるとややこしくなるだろう。その点を割り
 * 切った実装としたい。つまり、あくまでも補助機能だ、であれば、RdbData の純粋仮想関数
 * という位置づけでは問題がある。
 *
 * e.g. CREATE TABLE 文（MySQL） 
CREATE TABLE person (
id    BIGINT AUTO_INCREMENT PRIMARY KEY,
name  VARCHAR(128) NOT NULL,
email VARCHAR(256) NOT NULL UNIQUE,
age   INT
);
 * 
*/

std::string makeCreateTableSql(const std::string& tableName, const std::vector<std::tuple<std::string,std::string,std::string>>& tblInfos) {
    std::string sql("CREATE TABLE ");
    sql.append(tableName).append(" (");
    std::string colsDef("");
    for(std::size_t i = 0; i < tblInfos.size(); i++) {
        auto col = tblInfos.at(i);
        colsDef.append(get<0>(col));colsDef.append(" ");colsDef.append(get<1>(col));colsDef.append(" ");colsDef.append(get<2>(col));
        if( i < tblInfos.size()-1 ) {
            colsDef.append(", ");
        }
    }
    sql.append(colsDef).append(")");
    return sql;
}
