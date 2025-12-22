/**
 * mysql_template.cpp
 * 
 * 概要としては builder_template_m.cpp からの引き継ぎ。
 * リポジトリとエンティティに関する考察とサンプリングに始まり、現在はいかに複製コードを
 * 減らせるかについて考えている。コンパイル時間が長くなったのでサンプリング用のソース
 * ファイルを分割した。
 * 
 * 具体的な問題点は以下。
 * - テーブル単位でリポジトリのサブクラスが作られるのを防ぎたい。
 * 
 * 具体的な改善案は以下。
 * - エンティティ（テーブルクラス or 構造体）毎に必要なメタ情報を取得可能なメンバ関数を
 *   設ける。この仕組みが巨大化、複雑化したら問題だと思う。
 * - リポジトリのサブクラスをひとつとする（汎用クラスで共通の仕組みを持つこと）。
 * 
 * 懸念点は ...
 * その実現性に関して現時点で、あまりに不透明であること。
 * 私の知識・技術不足と汎用クラスを用いた処理が複雑化しないかということ。
 * 似通ったリポジトリは減らせるが、エンティティは無くせない。
 * 
 * e.g. 環境構築メモ
 * MySQL 処理再び、今回はDocker コンテナ上のMySQL を利用する。
 * Docker Engine のインストール。
 * ```
 * sudo apt-get update
 * sudo apt-get install docker-ce docker-ce-cli containerd.io docker-compose-plugin
 * ```
 * 
 * Docker にMySQL をインストールする。
 * ```
 * docker pull mysql:8.0.44-debian
 * docker run --name mysql -e MYSQL_ROOT_PASSWORD=root1234 \
 * -p 3306:3306 -p 33060:33060 \
 * -v $(pwd)/mysql-data:/var/lib/mysql \
 * -d mysql:8.0.44-debian
 * 
 * docker exec -it mysql bash
 * mysql -u root -p
 * ```
 * 
 * データベースとテーブルの事前準備
 * ```
 * CREATE DATABASE IF NOT EXISTS test;
 * 
 * DROP TABLE IF EXISTS contractor;
 * CREATE TABLE contractor (
 *  id              SERIAL          PRIMARY KEY,
 *  company_id      VARCHAR(100)    NOT NULL,
 *  email           VARCHAR(100)    NOT NULL,
 *  password        VARCHAR(255)    NOT NULL,
 *  name            VARCHAR(100)    NOT NULL,
 *  roles           VARCHAR(100));
 * ALTER TABLE contractor ADD CONSTRAINT email_uk unique (email);
 * 
 * INSERT INTO contractor(company_id, email, password, name, roles)
 * VALUES
 * ('A1_1000', 'admin@example.com', '$2a$10$Ff.Tr2q.fBciJKiA1b4wAOwNpjr9RypX3DHeQwLQQg0GMxGrl4FcO', 'admin', 'ROLE_ADMIN'),
 * ('A1_1000', 'student@example.com', '$2a$10$fzJDR7BSMQnRg9Odg/JDzemQdLc6g2a7lR/ccHoMQEoxpxhs6dT0m', 'student', 'ROLE_USER'); 
 * 
 * // MySQL のTx が有効であることを確認する
 * use information_schema
 * select table_schema, table_name, engine from tables where table_name = 'contractor';
 * +--------------+------------+--------+
 * | TABLE_SCHEMA | TABLE_NAME | ENGINE |
 * +--------------+------------+--------+
 * | test         | contractor | InnoDB |
 * +--------------+------------+--------+
 * ```
 * 
 * MySQL C++ Connector について
 * ```
 * sudo apt update
 * # MySQL Connector/C++ の開発パッケージをインストールします
 * sudo apt install libmysqlcppconn-dev
 * # インストールライブラリの所在確認
 * dpkg -L libmysqlcppconn10
 * dpkg -L libmysqlcppconn-dev
 * ```
 * 
 * e.g. compile
 * g++ -O3 -std=c++20 -DDEBUG -pedantic-errors -Wall -Wextra -Werror -I../inc/ -I/usr/include/mysql-cppconn/ -L/usr/lib/x86_64-linux-gnu/ mysql_template.cpp -lmysqlcppconn -lmysqlcppconnx -o ../bin/mysql_template
 */
#ifndef MYSQL_TEMPLATE_H_
#define MYSQL_TEMPLATE_H_

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <cxxabi.h>
#include <cassert>
#include <any>
#include <functional>
#include <optional>
#include <sstream>
#include <string_view> // C++17以降 @see void safe_print(const char* p)
#include <format> // C++20

#include <Repository.hpp>
#include <mysql/jdbc.h>
#include <mysqlx/xdevapi.h>



namespace tmp::mysql::r2
{

// フィールド情報を保持する構造体
struct FieldMeta {
    std::string db_column_name;
    // 型安全なアクセサを関数オブジェクトとして保持
    // std::function<std::any()> getter;
    // std::function<void(std::any)> setter;
    std::function<std::any(const std::any&)> getter;
    std::function<void(std::any&, const std::any&)> setter;
};

// Data型 (Entity) からメタデータを取得するためのコンセプト
template <typename T>
concept EntityWithMeta = requires {
    { T::get_metadata() } -> std::same_as<const std::map<std::string, FieldMeta>&>;
};

// Contractor Entity
struct Contractor final {
    uint64_t id;
    std::string companyId;
    std::string email;
    std::string password;
    std::string name;
    std::optional<std::string> roles;

    explicit Contractor():id{0}
    {}
    explicit Contractor(const uint64_t& _id, const std::string& _companyId, const std::string& _email, const std::string& _password, const std::string& _name, const std::optional<std::string>& _roles)
    : id{_id}, companyId{_companyId}, email{_email}, password{_password}, name{_name}, roles{_roles}
    {}

    // uint64_t getId()           noexcept { return id; }
    // std::string getCompanyId() noexcept { return companyId; }
    // std::string getEmail()     noexcept { return email; }
    // std::string getPassword()  noexcept { return password; }
    // std::string getName()      noexcept { return name; }
    // std::optional<std::string> getRoles() noexcept {return roles;}

    // EntityWithMeta コンセプトを満たす静的メソッド
    static const std::map<std::string, FieldMeta>& get_metadata() {
        static const std::map<std::string, FieldMeta> metadata = {
            {"primaryKey", {"id", // DBカラム名
                [](const std::any& e) -> std::any { return std::any_cast<const Contractor&>(e).id; }, // Getter
                [](std::any& e, const std::any& v) { std::any_cast<Contractor&>(e).id = std::any_cast<uint64_t>(v); }  // Setter
            }},
            {"companyId", {"company_id",
                [](const std::any& e) -> std::any { return std::any_cast<const Contractor&>(e).companyId; },
                [](std::any& e, const std::any& v) { std::any_cast<Contractor&>(e).companyId = std::any_cast<std::string>(v); }
            }},
            {"email", {"email",
                [](const std::any& e) -> std::any { return std::any_cast<const Contractor&>(e).email; },
                [](std::any& e, const std::any& v) { std::any_cast<Contractor&>(e).email = std::any_cast<std::string>(v); }
            }},
            {"password", {"password",
                [](const std::any& e) -> std::any { return std::any_cast<const Contractor&>(e).password; },
                [](std::any& e, const std::any& v) { std::any_cast<Contractor&>(e).password = std::any_cast<std::string>(v); }
            }},
            {"name", {"name",
                [](const std::any& e) -> std::any { return std::any_cast<const Contractor&>(e).name; },
                [](std::any& e, const std::any& v) { std::any_cast<Contractor&>(e).name = std::any_cast<std::string>(v); }
            }},
            {"roles", {"roles",
                [](const std::any& e) -> std::any { return std::any_cast<const Contractor&>(e).roles; },
                [](std::any& e, const std::any& v) { std::any_cast<Contractor&>(e).roles = std::any_cast<std::optional<std::string>>(v); }
            }}
        };
        return metadata;
    }
    void print()
    {
        puts("Contractor::print");
        std::cout << "id: " << id << '\t';
        std::cout << "companyId: " << companyId << '\t';
        std::cout << "email: " << email << '\t';
        std::cout << "password: " << password << '\t';
        std::cout << "name: " << name << '\t';
        if(roles) {
            std::cout << "roles: " << roles.value();
        }
        std::cout << std::endl;
    }
};


// 一旦この静的な型安全性の高いパターンは保留する。
template <class ID, class Data>
requires EntityWithMeta<Data>
class GenericRepository : public tmp::Repository<ID, Data> {
private:
    mysqlx::Session* const session;
    std::string db;
    std::string table;

public:
    GenericRepository(mysqlx::Session* const _session, const std::string& _db, const std::string& _table)
    : session{_session}, db{_db}, table{_table}
    {}

    ID insert(Data&& data) override {
        const auto& meta = Data::get_metadata();
        std::stringstream sql_cols;
        std::stringstream sql_vals;

        for (auto const& [field_name, field_meta] : meta) {
            // Getterを使って動的に値を取得
            std::any value = field_meta.getter(std::any(data));
            print_debug("field_name: ", field_name);
            // valueを使ってSQLパラメータをバインドする処理が必要 (ここがDB依存)
            // std::cout << "Inserting " << field_meta.db_column_name << " with value: " << /* cast value */ << std::endl;
            sql_cols << field_meta.db_column_name << ",";
            sql_vals << "?,"; // プリペアドステートメントのプレースホルダ

            // 実際にはここでDBバインディングAPIを呼び出す
        }
        
        // 構築したSQL (INSERT INTO table (cols) VALUES (vals)) を実行
        // DBから返されたIDを返す
        std::cout << "DEBUG SQL: INSERT INTO ... (" << sql_cols.str() << ") VALUES (" << sql_vals.str() << ")" << std::endl;
        return ID{1}; // 仮の戻り値
    }

    // findByIdの実装
    std::optional<Data> findById(const ID& id) const override {
        // SELECT * FROM table WHERE id = ? を実行し、結果セットを取得
        // ResultSet res = db_connection->execute("SELECT ...")

        Data result_entity;
        // const auto& meta = Data::get_metadata();
        // 結果セットの各カラム名に対応する Setter を呼び出し、Dataオブジェクトを動的に構築
        // field_meta.setter(std::any(&result_entity), retrieved_db_value);

        return result_entity;
    }

    void update(const ID&, Data&&) override { /* ... */ }
    void remove(const ID&) override { /* ... */ }
};


}   // namespace tmp::mysql::r2


namespace tmp::mysql::r3
{

std::vector<mysqlx::Value> convertVarNodeToSqlValues(const tmp::VarNode& data_tree)
{
    std::vector<mysqlx::Value> values_to_bind;
    for (const auto& child : data_tree.children) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                // mysqlx::Value() は内部的に NULL を保持するオブジェクトを生成する
                values_to_bind.push_back(mysqlx::Value()); 
            } else {
                // その他の型は mysqlx::Value のコンストラクタが自動的に処理する
                values_to_bind.push_back(mysqlx::Value(arg));
            }
        }, child->data);
    }
    return values_to_bind;
}

// `convertVarNodeToSqlValues` の逆を行うヘルパー関数
tmp::ValueType convertSqlValueToVarNode_Corrected(const mysqlx::Value& sql_val) {
    using Type = mysqlx::abi2::r0::Value::Type;
    switch (sql_val.getType()) {
        case Type::VNULL:
            return std::monostate{};
        case Type::UINT64:
            return sql_val.get<uint64_t>();
        case Type::INT64:
            return sql_val.get<int64_t>();
        case Type::FLOAT:
            return sql_val.get<float>();
        case Type::DOUBLE:
            return sql_val.get<double>();
        case Type::BOOL:
            return sql_val.get<bool>();
        case Type::STRING:
            return std::string{sql_val.get<mysqlx::string>()};
        default:
            throw std::runtime_error("Unsupported mysqlx::Value type encountered.");
    }
}

template <class ID>
class VarNodeRepository : public tmp::Repository<ID, tmp::VarNode>
{
private:
    mysqlx::Session* const session;
    std::string dbName;
    std::string tableName;
    const std::string primaryKeyName;

public:
    VarNodeRepository(
        mysqlx::Session* const _session
        , const std::string&   _db
        , const std::string&   _table
        , const std::string&   _primaryKeyName = "id"
    )
    : session{_session}, dbName{_db}, tableName{_table}, primaryKeyName{_primaryKeyName}
    {}

    ID insert(tmp::VarNode&& data) override
    {
        int status;
        print_debug("insert ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        std::vector<std::string> cols;
        for(size_t i=0; i < data.children.size(); i++) {
            cols.push_back(data.children[i]->key);
        }
        std::vector<mysqlx::Value> vals = convertVarNodeToSqlValues(data);
        mysqlx::Schema db{session->getSchema(dbName)};
        mysqlx::Table table{db.getTable(tableName)};
        mysqlx::Result res = table.insert(cols)
                .values(vals)
                .execute();
        // 上記で Insert されたレコード件数
        print_debug("affected items count: ", res.getAffectedItemsCount());
        return res.getAutoIncrementValue();
    }
    
    // findByIdの実装
    std::optional<tmp::VarNode> findById(const ID& id) const override
    {
        int status;
        print_debug("findById ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        // ここからは元のロジックを維持
        mysqlx::Schema db{session->getSchema(dbName)};
        mysqlx::Table table{db.getTable(tableName)};
        std::string condition = primaryKeyName + " = :id";
        mysqlx::RowResult rows = table.select()
                                      .where(condition)
                                      .bind("id", id)
                                      .execute();
        
        if (rows.count() == 0) {
            return std::nullopt;
        }
        mysqlx::Row row = rows.fetchOne();
        // Columns オブジェクトを参照で受け取る (size() 等は使えない)
        const auto& meta = rows.getColumns();
        
        // VarNode は tmp::VarNode を想定
        tmp::VarNode result_entity("result_row", std::monostate{});
        // インデックスではなく、範囲ベース for ループとカウンタを併用する
        size_t i = 0;
        for (const auto& col_meta : meta) {
            const std::string& col_name = col_meta.getColumnName();
            // 行オブジェクトからはインデックスで値を取得する
            mysqlx::Value db_value = row.get(i);
            // 正しい変換関数を使用
            // ValueType は tmp::VarNode::ValueType を想定
            ValueType variant_value = convertSqlValueToVarNode_Corrected(db_value);
            result_entity.addChild(col_name, variant_value);
            i++;
        }
        return result_entity;
    }

    void update(const ID& id, tmp::VarNode&& data) override
    {
        int status;
        print_debug("update ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        
        // 借りたセッションを使用してデータベース操作を実行
        mysqlx::Schema db{session->getSchema(dbName)};
        mysqlx::Table table{db.getTable(tableName)};
        // 1. まず update ステートメントを開始する
        auto tableUpdate = table.update();

        // 2. VarNode の子ノード（更新対象カラム）をループ処理し、動的に set() を呼び出す
        for (const auto& child : data.children) {
            // ... (visit のロジックは変更なし) ...
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!std::is_same_v<T, std::monostate>) {
                    mysqlx::Value sql_val(arg);
                    tableUpdate.set(child->key, sql_val);
                }
            }, child->data);
        }
        // 3. WHERE句を設定して実行する
        std::string condition = primaryKeyName + " = :id";
        mysqlx::Result res = tableUpdate.where(condition)
                                        .bind("id", id)
                                        .execute();
        print_debug("affected items count: ", res.getAffectedItemsCount());
        
        // ここで session_guard のデストラクタが呼ばれ、コネクションがプールに戻る
    }
    void remove(const ID& id) override
    {
        int status;
        print_debug("remove ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        
        // 借りたセッションを使用してデータベース操作を実行
        mysqlx::Schema db{session->getSchema(dbName)};
        mysqlx::Table table{db.getTable(tableName)};
        std::string condition = primaryKeyName + " = :id";
        mysqlx::Result res = table.remove()
                            .where(condition)
                            .bind("id", id)
                            .execute();
        print_debug("affected items count: ", res.getAffectedItemsCount());        
        // ここで session_guard のデストラクタが呼ばれ、コネクションがプールに戻る
	}
};  // VarNodeRepository

class MySqlTransaction final : public tmp::Transaction {
private:
    mysqlx::Session* const tx;
public:
    MySqlTransaction(mysqlx::Session* const _tx): tx{_tx}
    {}
    void begin() override
    {
        int status;
        tmp::print_debug("tx begin ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        tx->startTransaction();
    }
    void commit() override
    {
        int status;
        tmp::print_debug("tx commit ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        tx->commit();
    }
    void rollback() override
    {
        int status;
        tmp::print_debug("tx rollback ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        tx->rollback();
    }
    // コピー禁止
    MySqlTransaction(const MySqlTransaction&) = delete;
    MySqlTransaction& operator=(const MySqlTransaction&) = delete;
};  // MySqlTransaction

/**
 * ServiceExecutor を受け取り、トランザクション境界を提供するラップ関数
 * 
 */
void execute_service_with_tx(tmp::Transaction& tx, tmp::ServiceExecutor& service) {
    try {
        tx.begin();
        service.execute();
        tx.commit();
    } catch (const std::exception& e) {
        tx.rollback();
        tmp::ptr_print_error<decltype(e)&>(e);
        throw;
    } catch (...) {
        tx.rollback();
        tmp::print_debug("Transaction failed due to unknown error.");
        throw;
    }
}

class SqlExecutor final {
    using Data = std::map<std::string, std::string>;
private:
    mysqlx::Session* const sess;
    const std::string db;
public:
    SqlExecutor(mysqlx::Session* const _sess, const std::string& _db)
    : sess{_sess}, db{_db}
    {}
    std::vector<Data> findProc_v1(std::vector<std::string>&& sqlSyntax)
    {
    // e.g.
    // mysql> PREPARE stmt FROM 'SELECT id, company_id, email FROM contractor WHERE company_id = ?';
    // mysql> SET @1 = 'A1_1000';
    // mysql> EXECUTE stmt USING @1;
        using Type = mysqlx::Value::Type; // abi2::r0 を排除
        std::vector<Data> result;
        sess->sql("USE "+db).execute();
        size_t size = sqlSyntax.size();
        size_t i = 0;

        for(auto& s: sqlSyntax) {
            i++;
            try {
                mysqlx::SqlResult sqlResult = sess->sql(s).execute();
                // 最後のクエリ（SELECT等）のみ結果セットを処理する
                if(i == size) {
                    // mysqlx::RowResult rowResult = sqlResult; // SqlResultからRowResultへ
                    auto& rowResult = sqlResult;
                    auto cols = rowResult.getColumnCount();
                    tmp::print_debug("row count: ", rowResult.count());
                    tmp::print_debug("column count: ", cols);

                    for(mysqlx::Row r: rowResult) { // abi2::r0 を排除
                        Data data;
                        for(decltype(cols) j=0; j < cols; j++) {
                            auto colName = rowResult.getColumn(j).getColumnName();
                            
                            if(r.get(j).isNull()) {
                                data.insert({colName, "NULL"}); // CUIトレースとしてNULLを明示
                                continue;
                            }

                            // 型に応じた文字列変換
                            switch(r.get(j).getType()) {
                                case Type::VNULL:
                                    data.insert({colName, "NULL"});
                                    break;
                                case Type::UINT64:
                                    data.insert({colName, std::to_string((uint64_t)r.get(j))});
                                    break;
                                case Type::INT64:
                                    data.insert({colName, std::to_string((int64_t)r.get(j))});
                                    break;
                                case Type::FLOAT:
                                    data.insert({colName, std::to_string((float)r.get(j))});
                                    break;
                                case Type::DOUBLE:
                                    data.insert({colName, std::to_string((double)r.get(j))});
                                    break;
                                case Type::BOOL:
                                    data.insert({colName, (bool)r.get(j) ? "true" : "false"});
                                    break;
                                case Type::STRING:
                                    data.insert({colName, (std::string)r.get(j)});
                                    break;
                                default:
                                    break;
                            }
                        }
                        result.push_back(data);
                    }
                }
            } catch (const mysqlx::Error& err) {
                tmp::print_debug("SQL Error at step ", i);
                tmp::print_debug("Query: ", s);
                tmp::print_debug("Message: ", err.what());
                throw;
            }
        }
        return result;
    }
    void cudProc_v1(std::vector<std::string>&& sqlSyntax)
    {
        sess->sql("USE "+db).execute();
        size_t size = sqlSyntax.size();
        size_t i = 0;
        for(auto& s: sqlSyntax) {
            i++;
            try {
                mysqlx::SqlResult result = sess->sql(s).execute();
                
                if(i == size) {
                    tmp::print_debug("affected items: ", result.getAffectedItemsCount());
                    if (result.getWarningsCount() > 0) {
                        for (const auto& warn : result.getWarnings()) {
                            tmp::print_debug("Warning: ", warn.getMessage());
                        }
                    }
                }
            } catch (const mysqlx::Error& err) {
                // エラーが発生した具体的なSQL文を記録して、コンソールのように振る舞う
                tmp::print_debug("SQL Error at step ", i);
                tmp::print_debug("Query: ", s);
                tmp::print_debug("Message: ", err.what());
                throw; // トランザクション・ロールバックのために再スロー
            }
        }
    }
};  // SqlExecutor

class SimplePrepare final {
private:
    std::string name;
    std::string query;
    std::vector<std::string> sets;

    // 1 & 2: ロジックを集約し、エスケープとクォートを管理
    void valueToSets(std::string val, bool is_string)
    {
        if (is_string) {
            // 簡易エスケープ: ' を \' に置換
            size_t pos = 0;
            while ((pos = val.find("'", pos)) != std::string::npos) {
                val.replace(pos, 1, "\\'");
                pos += 2;
            }
            // クォートで囲む
            val = "'" + val + "'";
        }

        const size_t count = sets.size() + 1;
        std::string elm;
        elm.append("SET @").append(std::to_string(count)).append("=").append(val).append(";");
        sets.push_back(elm);
    }

public:
    SimplePrepare(const std::string _name): name{_name}
    {}

    SimplePrepare& setQuery(const std::string _query) noexcept
    {
        query = _query;
        return *this;
    }

    // 数値型は is_string = false で集約
    SimplePrepare& set(const int64_t& val)
    {
        valueToSets(std::to_string(val), false);
        return *this;
    }

    SimplePrepare& set(const int& val)
    {
        valueToSets(std::to_string(val), false);
        return *this;
    }

    SimplePrepare& set(const double& val)
    {
        // std::to_string(val) ではなく、精度を指定して文字列化
        // "{}" はデフォルトで最適な精度を選択し、丸めを最小限に抑えます
        valueToSets(std::format("{}", val), false);
        return *this;
    }

    // 文字列型は is_string = true で集約
    SimplePrepare& set(const std::string& val)
    {
        valueToSets(val, true);
        return *this;
    }

    std::vector<std::string> build() const
    {
        std::vector<std::string> result;
        std::string prepare_syntax{"PREPARE "};
        std::string using_syntax{""};
        std::string execute_syntax{""};
        size_t i = 0;
        // PREPARE からクエリまで
        prepare_syntax.append(name).append(" FROM '\n")
        .append(query).append("';");
        result.push_back(prepare_syntax);
        // SET 句以降
        for(const std::string& s: sets) {
            result.push_back(std::string(s + "\n"));
            if(i == 0) using_syntax.append("@").append(std::to_string(++i));
            else using_syntax.append(", @").append(std::to_string(++i));
        }
        execute_syntax.append("EXECUTE ").append(name).append(" USING ").append(using_syntax).append(";");
        result.push_back(execute_syntax);
        return result;
    }
};  // SimplePrepare

}   // namespace tmp::mysql::r3


// int test_VarNodeRepository_Remove(uint64_t* id)
// {
//     puts("------ test_VarNodeRepository_Remove");
//     using Data = tmp::VarNode;
//     std::clock_t start_1 = clock();
//     mysqlx::Session sess("localhost", 33060, "root", "root1234");
//     try {
//         std::clock_t start_2 = clock();
//         std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
//             = std::make_unique<tmp::mysql::r3::VarNodeRepository<uint64_t>>(&sess, "test", "contractor", "id");

//         sess.startTransaction();
//         irepo->remove(*id);
//         sess.commit();
//         std::clock_t end = clock();
//         std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
//         std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         sess.rollback();
//         tmp::ptr_print_error<decltype(e)&>(e);
//         return EXIT_FAILURE;
//     }
// }

// int test_VarNodeRepository_Update(uint64_t* id)
// {
//     puts("------ test_VarNodeRepository_Update");
//     using Data = tmp::VarNode;
//     std::clock_t start_1 = clock();
//     mysqlx::Session sess("localhost", 33060, "root", "root1234");
//     try {
//         std::clock_t start_2 = clock();
//         std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
//             = std::make_unique<tmp::mysql::r3::VarNodeRepository<uint64_t>>(&sess, "test", "contractor", "id");

//         // e.g. Data sample
//         // Data data{"B3_3333", "foo@loki.org", "foo1111", "Foo", roles};
//         // ポリシの問題だが、更新時は別途、仮引数で値を渡しているため、ここでの設定はいらない（プライマリキの更新は不可が正しい）。
//         // データベース上でNULL を許可しているカラムは設定しなくてよい（contractor table では roles にあたる）。
//         // root ノードのKey とValue は空でなければ何でもよい（現状は）。
//         std::string pkcol{"id"};
//         Data root{"primaryKey", pkcol};
//         std::string companyId{"B3_3333"};
//         root.addChild("company_id", companyId);
//         std::string email{"foo@loki.org"};
//         root.addChild("email", email);
//         std::string password{"foo1111"};
//         root.addChild("password", password);
//         std::string name{"Foo"};
//         root.addChild("name", name);
//         std::string roles{"Admin,User"};
//         root.addChild("roles", roles);
//         sess.startTransaction();
//         irepo->update(*id, std::move(root));
//         sess.commit();
//         std::clock_t end = clock();
//         std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
//         std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         sess.rollback();
//         tmp::ptr_print_error<decltype(e)&>(e);
//         return EXIT_FAILURE;
//     }
// }

// int test_VarNodeRepository_FindById(uint64_t* id)
// {
//     puts("------ test_VarNodeRepository_FindById");
//     using Data = tmp::VarNode;
//     std::clock_t start_1 = clock();
//     mysqlx::Session sess("localhost", 33060, "root", "root1234");
//     try {
//         std::clock_t start_2 = clock();
//         std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
//             = std::make_unique<tmp::mysql::r3::VarNodeRepository<uint64_t>>(&sess, "test", "contractor");

//         sess.startTransaction();
//         std::optional<Data> resultVNode = irepo->findById(*id);
//         sess.commit();
//         std::clock_t end = clock();
//         if(resultVNode) Data::debug(&(resultVNode.value()));
//         else throw std::runtime_error("test_VarNodeRepository_FindById() is failed.");
//         std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
//         std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         sess.rollback();
//         tmp::ptr_print_error<decltype(e)&>(e);
//         return EXIT_FAILURE;
//     }
// }

// int test_VarNodeRepository_Insert(uint64_t* id)
// {
//     puts("------ test_VarNodeRepository_Insert");
//     using Data = tmp::VarNode;
//     std::clock_t start_1 = clock();
//     mysqlx::Session sess("localhost", 33060, "root", "root1234");
//     try {
//         std::clock_t start_2 = clock();
//         std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
//             = std::make_unique<tmp::mysql::r3::VarNodeRepository<uint64_t>>(&sess, "test", "contractor");

//         // e.g. Data sample
//         // Data data{0, "B3_1000", "alice@loki.org", "alice1111", "Alice", std::nullopt};
//         // 
//         // Data の作り方にひと工夫必要なはず。
//         // primaryKey is xxx という情報が最低限必要（VarNoderepository のコンストラクタの仮引数として解決した：）
//         // データベース上でNULL を許可しているカラムは設定しなくてよい（contractor table では roles にあたる）。
//         // root ノードのKey とValue は空でなければ何でもよい（現状は）。
//         std::string pkcol{"id"};
//         Data root{"primaryKey", pkcol};
//         std::string companyId{"B3_1000"};
//         root.addChild("company_id", companyId);
//         std::string email{"alice@loki.org"};
//         root.addChild("email", email);
//         std::string password{"alice1111"};
//         root.addChild("password", password);
//         std::string name{"Alice"};
//         root.addChild("name", name);
//         sess.startTransaction();
//         *id = irepo->insert(std::move(root));
//         sess.commit();
//         std::clock_t end = clock();
//         tmp::print_debug("id: ", *id);
//         std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
//         std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         sess.rollback();
//         tmp::ptr_print_error<decltype(e)&>(e);
//         return EXIT_FAILURE;
//     }
// }

// int test_GenericRepository()
// {
//     puts("------ test_GenericRepository");
//     // using namespace tmp::mysql::r2;
//     // using Repo = tmp::mysql::r2::GenericRepository;
//     using Data = tmp::mysql::r2::Contractor;
//     mysqlx::Session sess("localhost", 33060, "root", "root1234");
//     try {
//         std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
//             = std::make_unique<tmp::mysql::r2::GenericRepository<uint64_t, Data>>(&sess, "test", "contractor");
//         Data data{0, "B3_1000", "alice@loki.org", "alice1111", "Alice", std::nullopt};
//         sess.startTransaction();
//         uint64_t id = irepo->insert(std::move(data));
//         sess.commit();
//         tmp::print_debug("id: ", id);
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         sess.rollback();
//         tmp::ptr_print_error<decltype(e)&>(e);
//         return EXIT_FAILURE;
//     }
// }

#endif
