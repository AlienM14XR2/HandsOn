#ifndef _RDBDATA_H_
#define _RDBDATA_H_

/**
 * テーブルデータに関する共通処理を宣言している、インタフェース。
 * 現状はタグに等しい存在であり、具体的な処理は Strategy パターン
 * を用いた個別のクラスオブジェクトに処理を委譲すること。
 * 
 * e.g. PersonData.
*/

class RdbData {
public:
    virtual ~RdbData() = default;
    /**
     * かラム名を返す。
    */
    virtual std::vector<std::string> getColumns() const = 0;

    /**
     * テーブル情報を返す。
     * タプルの組み合わせは、カラム名とその RDBMS 側でのデータ型、Key 制約。
    */
    virtual std::vector<std::tuple<std::string, std::string, std::string>> getTableInfo() const = 0;
};


#endif
