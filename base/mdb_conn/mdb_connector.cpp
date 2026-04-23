
#define MDB_CONNECTOR_BODY
#include "mdb_connector.h"
#include "mdb_func.h"
#include <iostream>
#include "id_mgr.h"

MYSQL *sql_;
MYSQL_STMT *sql_stmt_;
MDB_VAL mdb_val_;
ConnectProperties props_;

#ifdef DEBUG
mdbConnector::mdbConnector(const bool print) : dbConnector(print)
#else
mdbConnector::mdbConnector()
#endif
{
    sql_ = mysql_init(nullptr);
    if(!sql_)
        std::cout << "miss" << std::endl;

    sql_stmt_ = nullptr;
}

bool mdbConnector::login(
        const std::string &url,
        const std::string &user,
        const std::string &pass,
        const std::string &database,
        const std::string &socket,
        const int port,
        const unsigned long flags
        )
{
    props_.host = url;
    props_.user = user;
    props_.pass = pass;
    props_.db = database;
    props_.socket = socket;
    props_.port = port;
    props_.flags = flags;
    bool res = mdb::conn(sql_, props_);

    mysql_close(sql_);
    sql_ = mysql_init(nullptr);
    return res;
}

void mdbConnector::logout()
{
    sql_stmt_ = nullptr;
    sql_ = nullptr;
    mysql_library_end();
}

bool mdbConnector::connect(const int id)
{
    if(empty_id())
        if(!mdb::conn(sql_, props_))
        {
            return false;
        }
    sql_stmt_ = mysql_stmt_init(sql_);
    if(!sql_stmt_)
    {
        mysql_close(sql_);
        sql_ = mysql_init(nullptr);
        return false;
    }
    set_id(id);
    return true;
}

bool mdbConnector::dis_connect(const int id)
{
    if(!release_id(id))
        return false;

    mysql_stmt_close(sql_stmt_);

    if(empty_id())
    {
        mysql_close(sql_);
        sql_ = mysql_init(nullptr);
    }

    return true;
}

void mdbConnector::close_selected()
{
    mdb_val_.clear();
    mysql_stmt_free_result(sql_stmt_);
    sql_stmt_ = mysql_stmt_init(sql_);
}

void mdbConnector::close_prepared()
{
    mdb_val_.clear();
    mysql_stmt_free_result(sql_stmt_);
    sql_stmt_ = mysql_stmt_init(sql_);
}

void mdbConnector::reuse_prepared()
{
    mdb_val_.clear();
}

void mdbConnector::reuse_stmt()
{
    mdb_val_.clear();
}

void mdbConnector::close_stmt()
{
    mdb_val_.clear();
    mysql_stmt_free_result(sql_stmt_);
    sql_stmt_ = mysql_stmt_init(sql_);
}

bool mdbConnector::auto_commit(const bool auto_mode)
{
    my_bool mode;
    if(auto_mode)
        mode = 1;
    else
        mode = 0;
    return mysql_autocommit(sql_, mode) == 0;
}

bool mdbConnector::begin(TXN_TYPE type)
{
    std::string str;
    switch(type)
    {
        case READ_UNCOMMITTED:
            str = "SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED";
            break;
        case READ_COMMITTED:
            str = "SET TRANSACTION ISOLATION LEVEL READ COMMITTED";
            break;
        case REPEATABLE_READ:
            str = "SET TRANSACTION ISOLATION LEVEL REPEATABLE READ";
            break;
        case SERIALIZABLE:
            str = "SET TRANSACTION ISOLATION LEVEL SERIALIZABLE;";
            break;
    }
    return mysql_query(sql_, str.c_str()) == 0;
}

bool mdbConnector::commit()
{
    return !mysql_commit(sql_);
}

void mdbConnector::rollback()
{
    mysql_rollback(sql_);
}

void mdbConnector::bind(const std::wstring &param)
{
    bind_str(param, binder);
}


bool mdbConnector::prepare(const std::wstring &stmt)
{
#ifdef DEBUG
    return mdb::prepare(sql_stmt_, stmt, print_stmt_);
#else
    return prepare_stmt(sql_stmt_, stmt);
#endif
}

int mdbConnector::execute()
{
    int res = BIND_ERROR;
    if(mdb::bind(sql_stmt_, binder) == BIND_SUCCESS)
#ifdef DEBUG
        res = mdb::execute(sql_stmt_, mdb_val_, cols_, print_stmt_);
#else
        res = exec(sql_stmt_, mdb_val_, columns_);
#endif
    if (res & DATABASE_ERROR)
        get_db_stmt_error(sql_stmt_);
    mdb::reset_parameters();
    clear_binder();
    return res;
}


int mdbConnector::execute(const std::wstring &stmt)
{
    int res = BIND_ERROR;
    if(binder.is_empty())
#ifdef DEBUG
        res = mdb::execute(sql_stmt_, mdb_val_, cols_, print_stmt_, stmt);
#else
        res = mdb::execute(sql_stmt_, mdb_val_, columns_, stmt);
#endif
    else
    {
#ifdef DEBUG
        res = mdb::prepare(sql_stmt_, stmt, print_stmt_);
#else
        res = prepare_stmt(sql_stmt_, stmt);
#endif
        if(res & DATABASE_SUCCESS)
        {
            res = mdb::bind(sql_stmt_, binder);
            if(res & BIND_SUCCESS)
#ifdef DEBUG
                res = mdb::execute(sql_stmt_, mdb_val_, cols_, print_stmt_);
#else
                res = mdb::execute(sql_stmt_, mdb_val_, columns_);
#endif
        }
        mdb::reset_parameters();
        clear_binder();
    }
    if (res != END_ROW)
        close_stmt();
    else if(res & DATABASE_ERROR)
        error_ = get_db_stmt_error(sql_stmt_);
    return res;
}


int mdbConnector::fetch()
{
    int res;
    switch(mysql_stmt_fetch(sql_stmt_))
    {
        case MYSQL_NO_DATA:
            return END_ROW;
        case MYSQL_DATA_TRUNCATED:
        default:
            res = NEXT_ROW;
            cols_[0] = 0;
            break;
    }
    return res;
}

int mdbConnector::count_rows() const
{
    return (int) mysql_stmt_num_rows(sql_stmt_);
}

bool mdbConnector::get_int(const int col, int &value)
{
    if (col >= cols_[1])
        return false;
    value = get_int_data(mdb_val_[col]);
    return true;
}

#ifdef BUILD_DLL

#else

bool mdbConnector::get_str(const int col, std::wstring& str)
{
    if (col >= cols_[1])
        return false;
    str = get_str_data(mdb_val_[col]);
    return true;
}

bool mdbConnector::get_name(const int col, std::wstring& name)
{
    if (col >= cols_[1])
        return false;
    name = mdb_val_[col].name;
    return true;
}

/*
std::wstring mdbConnector::get_name(const int col)
{
    return mdb_val_[col].name;
}

std::wstring mdbConnector::get_str(const int col)
{
    return get_str_data(mdb_val_[col]);
}
*/

#endif