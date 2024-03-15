#include "../../inc/PersonRepository.hpp"

PersonRepository::PersonRepository(const MySQLConnection* _con) : con(_con)
{}
std::optional<PersonData> PersonRepository::insert(const PersonData& data) const
{
    puts("------ PersonRepository::insert");
    const std::string sql = makeInsertSql(data.getTableName(), data.getColumns());
    ptr_lambda_debug<const char*,const decltype(sql)&>("sql: ", sql);
    std::unique_ptr<sql::PreparedStatement> prep_stmt(con->prepareStatement(sql));
    auto[id_nam, id_val] = data.getId().bind();
    auto[name_nam, name_val] = data.getName().bind();
    prep_stmt->setString(1, name_val);
    auto[email_nam, email_val] = data.getEmail().bind();
    prep_stmt->setString(2, email_val);
    if(data.getAge().has_value()) {
        auto[age_nam, age_val] = data.getAge().value().bind();
        prep_stmt->setInt(3, age_val);
    }
    int ret = prep_stmt->executeUpdate();                       // INSERT 実行
    ptr_lambda_debug<const char*, const int&>("ret is ", ret);

    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    std::string sql_last_insert_id = "SELECT LAST_INSERT_ID()";
    ptr_lambda_debug<const char*,const decltype(sql_last_insert_id)&>("sql_last_insert_id: ", sql_last_insert_id);
    std::unique_ptr<sql::ResultSet> res( stmt->executeQuery(sql_last_insert_id) );  // SELECT ... Auto Increment されたライマリキを取得する
    while(res->next()) {
        puts("------ A");
        auto id = res->getInt64(1);
        DataField<std::size_t> d_id("id", id);
        DataField<std::string> d_name("name", data.getName().getValue());
        DataField<std::string> d_email("email", data.getEmail().getValue());
        std::optional<DataField<int>> d_age = std::nullopt;
        if(data.getAge().has_value()) {
            d_age = DataField<int>("age", data.getAge().value().getValue());
        }
        return PersonData(data.getDataStrategy(), d_id, d_name, d_email, d_age);
        // ptr_lambda_debug<const char*, const decltype(id)&>("id is ", id);
        // ptr_lambda_debug<const char*, const std::string&>("id type is ", typeid(id).name());
        // auto sql_2 = makeFindOneSql(data.getTableName(), id_nam, data.getColumns());
        // ptr_lambda_debug<const char*,const decltype(sql_2)&>("sql_2: ", sql_2);
        // std::unique_ptr<sql::PreparedStatement> prep_stmt_2(con->prepareStatement(sql_2));
        // prep_stmt_2->setBigInt(1, std::to_string(id));
        // std::unique_ptr<sql::ResultSet> res_2( prep_stmt_2->executeQuery() );       // SELECT ... 登録されたデータを取得する
        // while(res_2->next()) {
        //     puts("------ B");
        //     // デバッグ
        //     auto res_id    = res_2->getUInt64(1);
        //     auto res_name  = res_2->getString(2);
        //     auto res_email = res_2->getString(3);
        //     ptr_lambda_debug<const char*,const decltype(res_id)&>("res_id: ", res_id);
        //     ptr_lambda_debug<const char*,const decltype(res_name)&>("res_name: ", res_name);
        //     ptr_lambda_debug<const char*,const decltype(res_email)&>("res_email: ", res_email);
        //     if(data.getAge().has_value()){              // std::optional と RDB 上の Null を許可したカラムについてはもっとテストと考察が必要、現状では煩雑過ぎる。
        //         auto res_age   = res_2->getInt(4);
        //         ptr_lambda_debug<const char*,const decltype(res_age)&>("res_age: ", res_age);
        //         return PersonData::factory(res_2.get(), data.getDataStrategy());
        //     } else {
        //         return PersonData::factoryNoAge(res_2.get(), data.getDataStrategy());
        //     }
        //     /**
        //      * std::optional が複数あるデータの場合は、上記のような factory を用いずに、データから ResultSet の有無を推論する仕組み
        //      * の方がより自然で汎用性が高いと思う。
        //     */
        // }
    }
    return std::nullopt;
}

std::optional<PersonData> PersonRepository::update(const PersonData& data) const
{
    puts("------ PersonRepository::update");
    ptr_lambda_debug<const char*,const RdbDataStrategy<PersonData>*>("stragety addr is ", data.getDataStrategy());
    auto[debug_id_nam, debug_id_val] = data.getId().bind();
    ptr_lambda_debug<const char*,const decltype(debug_id_nam)&>("debug_id_nam is ", debug_id_nam);
    ptr_lambda_debug<const char*,const decltype(debug_id_val)&>("debug_id_val is ", debug_id_val);

    const std::string sql = makeUpdateSql(data.getTableName(), data.getId().getName(), data.getColumns());
    ptr_lambda_debug<const char*,const std::string&>("sql: ", sql);
    std::unique_ptr<sql::PreparedStatement> prep_stmt(con->prepareStatement(sql));
    auto[name_nam, name_val] = data.getName().bind();
    prep_stmt->setString(1, name_val);
    auto[email_nam, email_val] = data.getEmail().bind();
    prep_stmt->setString(2, email_val);
    if(data.getAge().has_value()) {
        auto[age_nam, age_val] = data.getAge().value().bind();
        prep_stmt->setInt(3, age_val);
    }
    auto[id_nam, id_val] = data.getId().bind();
    prep_stmt->setBigInt(4, std::to_string(id_val));
    int ret = prep_stmt->executeUpdate();                       // Update 実行
    ptr_lambda_debug<const char*, const int&>("ret is ", ret);
    // return data;        // findOne したものを返却すべきなのか、悩ましい。
    return findOne(data.getId().getValue());
}

void PersonRepository::remove(const std::size_t& pkey) const
{   
    puts("------ PersonRepository::remove");
    PersonData data = PersonData::dummy();
    std::string sql = makeDeleteSql(data.getTableName(), data.getId().getName());
    ptr_lambda_debug<const char*, const std::string&>("sql: ", sql);
    std::unique_ptr<sql::PreparedStatement> prep_stmt(con->prepareStatement(sql));
    prep_stmt->setBigInt(1, std::to_string(pkey));
    int ret = prep_stmt->executeUpdate();                       // Delete 実行
    ptr_lambda_debug<const char*, const int&>("ret is ", ret);

}

std::optional<PersonData> PersonRepository::findOne(const std::size_t& pkey) const
{
    /**
     * 前言撤回だな、結論から言えば、利用する側からしたら、今のインタフェースの方が使い勝手がいい。
     * よって、内部の実装で PersonData を利用すればいいし、何らかの手段で動的に SQL が構築できれば
     * 問題ないと考える。
    */
    puts("------ PersonRepository::findOne");
    DataField<std::size_t> id("id", pkey);
    DataField<std::string> name("name", "");
    DataField<std::string> email("email", "");
    DataField<int>         age("age", 0);
    std::unique_ptr<RdbDataStrategy<PersonData>> dataStratedy = std::make_unique<PersonStrategy>(PersonStrategy());
    // 上記一連を作る factory が欲しくなる、どこに作るべきかな、最終的に PersonData を返却してくれたらいいので、PersonData の static メンバ関数ではどうだろうか。
    PersonData data(dataStratedy.get(), id, name, email, age);
    std::string sql = makeFindOneSql(data.getTableName(), id.getName(), data.getColumns());     // makeFindOneSql に namespace は必要かな？
    ptr_lambda_debug<const char*, const std::string&>("sql: ", sql);
    std::unique_ptr<sql::PreparedStatement> prep_stmt(con->prepareStatement(sql));
    prep_stmt->setBigInt(1, std::to_string(pkey));
    std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());
    while(res->next()) {
        puts("------ A");
        return PersonData::factory(res.get(), nullptr);
    }
    return std::nullopt;
}

/**
 * 以下
 * namespace ormx
*/

ormx::PersonRepository::PersonRepository(mysqlx::Session* _session): session(_session)
{}
std::optional<ormx::PersonData> ormx::PersonRepository::insert(const ormx::PersonData& data) const
{
    puts("------ ormx::PersonRepository::insert()");
    // 実装
    mysqlx::Schema cheshire = session->getSchema("cheshire");
    mysqlx::Table person = cheshire.getTable("person");
        
    mysqlx::Result res;
    if( data.getAge().has_value() ) {
        res = person.insert("name", "email", "age")
                    .values(data.getName(), data.getEmail(), data.getAge().value())
                    .execute();
        std::cout <<  res.getAutoIncrementValue() << std::endl;     // 最初に Insert したレコードの pkey (AUTO INCREMENT) の値
        return ormx::PersonData(res.getAutoIncrementValue(), data.getName(), data.getEmail(), data.getAge().value());
    } else {
        res = person.insert("name", "email")
                    .values(data.getName(), data.getEmail())
                    .execute();
        std::cout <<  res.getAutoIncrementValue() << std::endl;     // 最初に Insert したレコードの pkey (AUTO INCREMENT) の値
        return ormx::PersonData(res.getAutoIncrementValue(), data.getName(), data.getEmail());
    }
}
std::optional<ormx::PersonData> ormx::PersonRepository::update(const ormx::PersonData& data) const {
    // TODO 実装
    return std::nullopt;
}
void ormx::PersonRepository::remove(const std::size_t& pkey) const {
    // TODO 実装
}
std::optional<ormx::PersonData> ormx::PersonRepository::findOne(const std::size_t& pkey) const {
    // TODO 実装
    return std::nullopt;        
}

