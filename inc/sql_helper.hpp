#ifndef _SQL_HELPER_H_
#define _SQL_HELPER_H_

#include <string>

namespace tmp::postgres::helper {
template <class... Args>
std::string insert_sql(const std::string& table, Args&&... fields)
{
    // R"(INSERT INTO contractor (id, company_id, email, password, name, roles) VALUES ($1, $2, $3, $4, $5, $6))"
    size_t size = sizeof...(fields);
    printf("size: %zu\n", size);
    std::string sql = "INSERT INTO " + table + '\n';
    const std::string dollar = "$";
    std::string flds;
    std::string values;
    size_t i = 0;

    auto print_fields_values = [&](const auto& element) {
        i++;
        if(i < size) {
            flds.append(element).append(", ");
            values.append(dollar + std::to_string(i)).append(", ");
        } else {
            flds.append(element);
            values.append(dollar + std::to_string(i));
        }
    };
    (print_fields_values(std::forward<Args>(fields)), ...);

    sql.append("( ");
    sql.append(flds);
    sql.append(" )");
    sql.append("\nVALUES\n");
    sql.append("( ");
    sql.append(values);
    sql.append(" )\n");
    return sql;
}

template <class... Args>
std::string update_by_pkey_sql(const std::string& table, Args&&... fields)
{
    // UPDATE contractor
    // SET id=$1, company_id=$2, email=$3, password=$4, name=$5, roles=$6 
    // WHERE id=$1

    size_t size = sizeof...(fields);
    std::string sql = "UPDATE " + table + '\n';
    const std::string dollar = "$";
    std::string fvals;
    std::string pkey;
    size_t i = 0;

    auto print_fields_values = [&](const auto& element) {
        i++;
        if(i < size) {
            if(i == 1) pkey.append(element).append("=").append(dollar+std::to_string(1));
            fvals.append(element).append("=").append(dollar+std::to_string(i)).append(", ");
        } else {
            fvals.append(element).append("=").append(dollar+std::to_string(i));
        }
    };
    (print_fields_values(std::forward<Args>(fields)), ...);
    sql.append("SET ").append(fvals);
    sql.append("\nWHERE ").append(pkey).append("\n");
    return sql;
}

template <class... Args>
std::string select_by_pkey_sql(const std::string& table, const std::string& pkey, Args&&... fields)
{
    size_t size = sizeof...(fields);
    std::string sql = "SELECT ";
    std::string flds;
    size_t i = 0;

    auto print_field = [&](const auto& element) {
        i++;
        if(i < size) {
            flds.append(element).append(", ");
        } else {
            flds.append(element);
        }
    };
    (print_field(std::forward<Args>(fields)), ...);

    sql.append(flds);
    sql.append(" FROM ").append(table);
    sql.append("\nWHERE ").append(pkey).append("=$1");
    return sql;
}

std::string select_by_pkey_sql(const std::string& table, const std::string& pkey)
{
    std::string sql = "SELECT * FROM " + table + '\n';
    sql.append("WHERE ").append(pkey).append("=$1");
    return sql;
}

std::string delete_by_pkey_sql(const std::string& table, const std::string& pkey)
{
//     DELETE FROM contractor
//     WHERE id = $1
    std::string sql = "DELETE FROM " + table + '\n';
    sql.append("WHERE ").append(pkey).append("=$1");
    return sql;
}

}   // namespace tmp::postgres::helper

namespace tmp::mysql::helper {
template <class... Args>
std::string insert_sql_auto_id(const std::string& table, Args&&... fields)
{
    // R"(INSERT INTO contractor (company_id, email, password, name, roles) VALUES (?, ?, ?, ?, ?))"
    size_t size = sizeof...(fields);
    printf("size: %zu\n", size);
    std::string sql = "INSERT INTO " + table + '\n';
    std::string flds;
    std::string values;
    size_t i = 0;

    auto print_fields_values = [&](const auto& element) {
        i++;
        if(i < size) {
            flds.append(element).append(", ");
            values.append("?").append(", ");
        } else {
            flds.append(element);
            values.append("?");
        }
    };
    (print_fields_values(std::forward<Args>(fields)), ...);

    sql.append("( ");
    sql.append(flds);
    sql.append(" )");
    sql.append("\nVALUES\n");
    sql.append("( ");
    sql.append(values);
    sql.append(" )\n");
    return sql;
}

template <class... Args>
std::string update_by_pkey_sql(const std::string& table, const std::string& pkey, Args&&... fields)
{
    // UPDATE contractor
    // SET company_id=?, email=?, password=?, name=?, roles=?
    // WHERE id=?

    size_t size = sizeof...(fields);
    std::string sql = "UPDATE " + table + '\n';
    std::string fvals;
    size_t i = 0;

    auto print_fields_values = [&](const auto& element) {
        i++;
        if(i < size) {
            fvals.append(element).append("=?").append(", ");
        } else {
            fvals.append(element).append("=?");
        }
    };
    (print_fields_values(std::forward<Args>(fields)), ...);
    sql.append("SET ").append(fvals);
    sql.append("\nWHERE ").append(pkey).append("=?").append("\n");
    return sql;
}

std::string select_by_pkey_sql(const std::string& table, const std::string& pkey)
{
    std::string sql = "SELECT * FROM " + table + '\n';
    sql.append("WHERE ").append(pkey).append("=?\n");
    return sql;
}

std::string delete_by_pkey_sql(const std::string& table, const std::string& pkey)
{
//     DELETE FROM contractor
//     WHERE id = ?
    std::string sql = "DELETE FROM " + table + '\n';
    sql.append("WHERE ").append(pkey).append("=?\n");
    return sql;
}

}   // namespace tmp::mysql::helper

#endif
