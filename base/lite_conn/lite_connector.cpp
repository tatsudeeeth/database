
#define LITE_CONNECTOR_BODY
#include "lite_connector.h"
#include "lite_func.h"
#include "id_mgr.h"

sqlite3* handle_;
sqlite3_stmt* h_stmt_;

/*---------------------------------------------------------------------------------------------*/

#ifdef DEBUG
liteConnector::liteConnector(const bool print)
: dbConnector(print)
#else
liteConnector::liteConnector()
#endif
{
    h_stmt_ = nullptr;
    handle_ = nullptr;
}

bool liteConnector::login(const std::wstring& file)
{
    if(!ldb::conn(file, &handle_))
        return false;

    sqlite3_close(handle_);
    file_ = file;
    return true;
}

void liteConnector::logout()
{
    handle_ = nullptr;
    h_stmt_ = nullptr;
}

bool liteConnector::connect(const int id)
{
    if(!empty_id())
        return true;

    if(!ldb::conn(file_, &handle_))
        return false;

    set_id(id);
    return true;
}

bool liteConnector::dis_connect(const int id)
{
    if(!release_id(id))
        return false;

    if(empty_id())
        sqlite3_close(handle_);
    return true;
}

void liteConnector::close_selected()
{
    sqlite3_finalize(h_stmt_);
    clear_binder();
    reset_cols();
}

void liteConnector::close_prepared()
{
    sqlite3_finalize(h_stmt_);
    reset_cols();
    clear_binder();
}

void liteConnector::reuse_prepared()
{
    sqlite3_reset(h_stmt_);
    clear_binder();
}

void liteConnector::close_stmt()
{
    sqlite3_finalize(h_stmt_);
    reset_cols();
    clear_binder();
}

void liteConnector::reuse_stmt()
{
    sqlite3_reset(h_stmt_);
    clear_binder();
}


/*transaction--------------------------------------------------------------------------*/

bool liteConnector::auto_commit(const bool auto_mode)
{
    std::wstring stmt;
    if(auto_mode)
        stmt = L"END";
    else
        stmt = L"BEGIN";
#ifdef DEBUG
    const bool res = ldb::exec(stmt, handle_, print_stmt_);
#else
    const bool res = ldb::exec(stmt, handle_);
#endif
    if (!res)
        error_ = ldb::get_db_error(handle_);;
    return res;
}

bool liteConnector::begin(TXN_TYPE)
{
#ifdef DEBUG
    const bool res = ldb::exec(L"SAVEPOINT tran", handle_, print_stmt_);
#else
    const bool res = ldb::exec(L"SAVEPOINT tran", handle_);

#endif
    if (!res)
        error_ = ldb::get_db_error(handle_);;
    return res;
}

bool liteConnector::commit()
{
#ifdef DEBUG
    if(ldb::exec(L"RELEASE tran;", handle_, print_stmt_))
    {
        if(ldb::exec(L"SAVEPOINT tran;", handle_, print_stmt_))
            return true;
    }
#else
    if(ldb::exec(L"RELEASE tran;", handle_))
    {
        if(ldb::exec(L"SAVEPOINT tran;", handle_))
            return true;
    }
#endif
    error_ = ldb::get_db_error(handle_);
    return false;
}

void liteConnector::rollback()
{
#ifdef DEBUG
    if (!ldb::exec(L"ROLLBACK TO tran;", handle_, print_stmt_))
#else
    if (!ldb::exec(L"ROLLBACK TO tran;", handle_))
#endif
        error_ = ldb::get_db_error(handle_);
}

/*-----------------------------------------------------------------------------------------*/

bool liteConnector::prepare(const std::wstring& stmt)
{
    count_stmt_ = stmt;

#ifdef DEBUG
    const int res = ldb::prepare(stmt, handle_, &h_stmt_, print_stmt_);
#else
    const int res = ldb::prepare(stmt, handle_, &h_stmt_);
#endif
    if (res & DATABASE_ERROR)
    {
        error_ = ldb::get_db_error(handle_);
        return false;
    }
    return true;
}

int liteConnector::execute()
{
    if (!count_stmt_.empty())
    {

        count_stmt_.clear();
    }
    int res = ldb::bind(&h_stmt_, binder);
    if (res == BIND_SUCCESS)
    {
        res = ldb::exec(&h_stmt_);;
        if (res == NEXT_ROW)
        {
            cols_[1] = sqlite3_column_count(h_stmt_);;
            cols_[0]= 0;
#ifdef DEBUG
            rows_ = ldb::count_records(count_stmt_, handle_, binder, print_stmt_);
#else
            rows_ = ldb::count_records(count_stmt_, handle_, binder);
#endif
        }
    }
    if (res & DATABASE_ERROR)
        error_ = ldb::get_db_error(handle_);
    return res;
}

int liteConnector::execute(const std::wstring& stmt)
{

#ifdef DEBUG
    int res = ldb::prepare(stmt, handle_, &h_stmt_, print_stmt_);
#else
    int res = ldb::prepare(stmt, handle_, &h_stmt_);
#endif

    if (res == PREPARE_SUCCESS)
    {
        res = ldb::bind(&h_stmt_, binder);
        if (res == BIND_SUCCESS)
        {
            cols_[1] = sqlite3_column_count(h_stmt_);
            res = ldb::exec(&h_stmt_);
            switch (res)
            {
                case NEXT_ROW:
#ifdef DEBUG
                    rows_ = ldb::count_records(stmt, handle_, binder, print_stmt_);
#else
                    rows_ = ldb::count_records(stmt, handle_, binder);
#endif
                    cols_[0] = 0;
                    return res;
                case STMT_SUCCESS:
                    if (cols_[1] > 0)
                        res = END_ROW;
                default:
                    break;
            }
        }
    }
    if (res & DATABASE_ERROR)
        error_ = ldb::get_db_error(handle_);
    sqlite3_finalize(h_stmt_);
    clear_binder();
    return res;
}

int liteConnector::fetch()
{
    cols_[0] = 0;
    return ldb::fetch(handle_, h_stmt_, error_);
}

/*---------------------------------------------------------------------------------------------*/
/*

int liteConnector::execute()
{
    int res = bind_all(&stmt_, binder, columns_);
    clear_binder();
    switch(res)
    {
        case NEXT_ROW:
            return res;

        case BIND_ERROR:
        case STMT_ERROR:
            error_ = get_db_error(handle_);
            break;
        default:
            break;
    }
    return res;
}


bool liteConnector::prepare(const std::wstring& stmt)
{
    return prepare_stmt(stmt, handle_, &stmt_, error_);
}
*/

/*---------------------------------------------------------------------------------------------*/

bool liteConnector::get_int(const int col, int &value)
{
    if (col >= cols_[1])
        return false;
    value = sqlite3_column_int(h_stmt_, col);
    return true;
}

#ifdef BUILD_DLL

int liteConnector::get_name(const int col, wchar_t data[], size_t arr_size, int count)
{
    auto ptr = static_cast<const wchar_t*>(sqlite3_column_name16(h_stmt_, col));

    const size_t bytes16 = sizeof(ptr);
    const auto size = bytes16 / sizeof(wchar_t);

    if (size < arr_size)
    {
        std::wmemcpy(data, ptr, size);
        data[size] = L'\0';
        return END_STR;
    }

    auto total = size / (size - size % arr_size) + 1;
    if (count < total - 1)
    {
        std::wmemcpy(data, ptr + count * arr_size, arr_size);
        return NEXT_STR;
    }

    std::wmemcpy(data, ptr + count * arr_size, size % arr_size);
    data[size % arr_size] = L'\0';
    return END_STR;
}

int liteConnector::get_str(const int col, wchar_t data[], const size_t arr_size, const int count)
{
    if(const auto ptr = static_cast<const wchar_t*>(sqlite3_column_text16(h_stmt_, col)))
    {
        const size_t bytes16 = sqlite3_column_bytes16(h_stmt_, col);
        const auto size = bytes16 / sizeof(wchar_t);

        if (size < arr_size)
        {
            std::wmemcpy(data, ptr, size);
            data[size] = L'\0';
            return END_STR;
        }

        auto total = size / (size - size % arr_size) + 1;
        if (count < total - 1)
        {
            std::wmemcpy(data, ptr + count * arr_size, arr_size);
            return NEXT_STR;
        }

        std::wmemcpy(data, ptr + count * arr_size, size % arr_size);
        data[size % arr_size] = L'\0';
        return END_STR;
    }

    data[0] = L'\0';
    return END_STR;

}
#else

bool liteConnector::get_name(const int col, std::wstring& name)
{
    if (col >= cols_[1])
        return false;
    name = (wchar_t*) sqlite3_column_name16(h_stmt_, col);
    return true;
}

bool liteConnector::get_str(const int col, std::wstring& str)
{
    if (col >= cols_[1])
        return false;

    if(const auto ptr = sqlite3_column_text16(h_stmt_, col))
        str = (wchar_t*) ptr;
    else
        str = L"";
    return true;
}

/*
std::wstring liteConnector::get_name(const int col)
{
    return (wchar_t*) sqlite3_column_name16(h_stmt_, col);
}

std::wstring liteConnector::get_str(const int col)
{
    auto ptr = sqlite3_column_text16(h_stmt_, col);
    if(ptr)
        return (wchar_t*) ptr;
    else
        return L"";
}
*/

#endif