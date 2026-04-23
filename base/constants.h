//
// Created by TatsuyaSano on 2025/11/23.
//

#ifndef DATABASE_CONSTANTS_H
#define DATABASE_CONSTANTS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>

/*DATABASE STATUS*/
#define DATABASE_SUCCESS    001
#define DATABASE_ERROR      002

#define CONSTRAINT_ERROR    014
#define CONSTRAINT_UNIQUE   18

#define STMT_SUCCESS        101
#define STMT_ERROR          102
#define BIND_SUCCESS        105
#define BIND_ERROR          106
#define NO_BIND             107
#define PREPARE_SUCCESS     109
#define PREPARE_ERROR       110

/*FETCH RESULT*/
#define NEXT_ROW            209
#define END_ROW             213
#define NEXT_COL            217
#define END_COL             221
#define FETCH_ERROR         210

/*DATA RESULT*/
#define NOT_FOUND           205
#define FOUND_DATA          201
#define NO_AUTHORITY        202

#define NEXT_STR            309
#define END_STR             313
#define STR_ERROR           310

enum TXN_TYPE
{
    READ_UNCOMMITTED,
    READ_COMMITTED,
    REPEATABLE_READ,
    SERIALIZABLE
};

enum bind_type
{
    null_bind,
    int_bind,
    str_bind,
    float_bind,
    date_bind
};

struct DateBinder
{
    int year=NULL, month=NULL, day=NULL;
};

struct BINDER {

    std::vector<bind_type> category;
    std::vector<std::wstring> texts;
    std::vector<int> nums;
    std::vector<float> f_num;
    std::vector<DateBinder> date;
    [[nodiscard]] bool is_empty() const {return category.empty();}
    int params = 0;
};

#endif //DATABASE_CONSTANTS_H