

#ifndef DATABASE_MDB_FUNC_H
#define DATABASE_MDB_FUNC_H

#include "constants.h"
#include <mysql.h>

struct ConnectProperties
{
    std::string host, user, pass, db, socket;
    int port;
    unsigned long flags;
};

struct struct_mdb
{
    enum_field_types buffer_type = MYSQL_TYPE_DECIMAL;
    std::wstring name;
    my_bool is_null = NULL, error = NULL;
    void *ptr = nullptr;
    char *str = nullptr;

    MYSQL_TIME tm;
    unsigned long length = NULL;
    ~struct_mdb(){delete []str;}
};


typedef std::vector<struct_mdb> MDB_VAL;


int get_db_stmt_error(MYSQL_STMT *sql_stmt);



bool bind_str(const std::wstring &param, BINDER &binder);
std::wstring get_str_data(const struct_mdb& mdb_val);
//int get_str_data(const struct_mdb& mdb_val, wchar_t data[], size_t arr_size, int count);
int get_int_data(const struct_mdb& mdb_val);

namespace mdb
{
    void reset_parameters();
    int get_db_error(MYSQL *sql);
    bool conn(MYSQL *sql, const ConnectProperties &props);

#ifdef DEBUG
    //bool exec(MYSQL *sql, MYSQL_RES *sql_res, int columns[], const std::wstring &stmt, bool print_stmt);
    int execute(MYSQL_STMT *sql_stmt, const std::wstring &stmt, bool print_stmt);
    int execute(MYSQL_STMT *sql_stmt, MDB_VAL &mdb_val, int columns[], bool print_stmt, const std::wstring &stmt=L"");
    bool prepare(MYSQL_STMT *sql_stmt, const std::wstring &stmt, bool print_stmt);
#else
    int execute(MYSQL_STMT *sql_stmt, const std::wstring &stmt);
    int execute(MYSQL_STMT *sql_stmt, MDB_VAL &mdb_val, int columns[], const std::wstring &stmt = L"");
    int prepare(MYSQL_STMT *sql_stmt, const std::wstring &stmt);
#endif

    int bind(MYSQL_STMT *stmt, BINDER &binder);
    int reverse_bind(MDB_VAL &mdb_val, MYSQL_STMT *sql_stmt);
    int execute(MYSQL_STMT *sql_stmt);

    int fetch(MYSQL_RES *res);
}


#endif //DATABASE_MDB_FUNC_H