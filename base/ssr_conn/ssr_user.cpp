#define SSR_USER_BODY
#include "ssr_user.h"
#include "ssr_cursor.h"

SQLHANDLE	    henv_;//void 環境ハンドル
SQLHDBC	        hdbc_;//void　接続ハンドル
SQLHSTMT        hstmt_;//void

/*----------------------------------------------------------------------------------------------*/

#ifdef DEBUG
ssrUser::ssrUser(bool print)
: User(print)
#else
ssrUser::ssrUser()
#endif
{
    henv_ = SQL_NULL_HANDLE;
    hdbc_ = SQL_NULL_HANDLE;
    hstmt_ = SQL_NULL_HANDLE;
}

bool ssrUser::login(const std::wstring& stmt)
{
    // 環境ハンドルの作成
    SQLRETURN res = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv_);
    if (SQL_SUCCEEDED(res))
    {
        SQLSetEnvAttr(henv_, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0);

        // 接続ハンドルの作成
        res = SQLAllocHandle(SQL_HANDLE_DBC, henv_, &hdbc_);

        if (SQL_SUCCEEDED(res))
        {
            if (conn(stmt, hdbc_))
            {
                conn_ = stmt;
                SQLDisconnect(hdbc_);
                return true;
            }
            else
                error_ = get_db_error(SQL_HANDLE_DBC, hdbc_, nullptr);
            SQLFreeHandle(SQL_HANDLE_DBC, hdbc_);
        }
    }
    SQLFreeHandle(SQL_HANDLE_ENV, henv_);

    return false;
}

void ssrUser::logout()
{
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc_);
    SQLFreeHandle(SQL_HANDLE_ENV, henv_);
    hdbc_ = SQL_NULL_HANDLE;
    henv_ = SQL_NULL_HANDLE;
}

bool ssrUser::connect(int id)
{
    if(empty_id())
        if(!conn(conn_, hdbc_))
        {
            error_ = get_db_error(SQL_HANDLE_DBC, hdbc_, nullptr);
            return false;
        }

    if(!open_cursor(hdbc_, hstmt_))
    {
        error_ = get_db_error(SQL_HANDLE_STMT, nullptr, hdbc_);
        return false;
    }

    set_id(id);
    return true;
}

bool ssrUser::dis_connect(int id)
{
    if(!release_id(id))
        return false;

    close_cursor(hstmt_);

    if(empty_id())
        SQLDisconnect(hdbc_);
    return true;
}


CURSOR ssrUser::create_cursor()
{
#ifdef DEBUG
    std::shared_ptr<ssrCursor> cursor(new ssrCursor(hdbc_, &conn_, print_stmt_));
#else
    std::shared_ptr<ssrCursor> cursor(new ssrCursor(hdbc_, &conn_));
#endif
    return cursor;
}

void ssrUser::reset_cursor()
{
    SQLCloseCursor(hstmt_);
}

/*トランザクション関連--------------------------------------------------------------------------*/

bool ssrUser::auto_commit(bool auto_mode)
{
    SQLPOINTER v_ptr;

    if(auto_mode)
        v_ptr = (SQLPOINTER*)SQL_AUTOCOMMIT_ON;
    else
        v_ptr = (SQLPOINTER*)SQL_AUTOCOMMIT_OFF;

    SQLRETURN res = SQLSetConnectAttr(hdbc_, SQL_ATTR_AUTOCOMMIT, v_ptr, SQL_IS_POINTER);

    switch(res)
    {
        case SQL_SUCCESS:
            return true;

        case SQL_SUCCESS_WITH_INFO:
        case SQL_ERROR:
        {
            error_ = get_db_error(SQL_HANDLE_DBC, hdbc_, nullptr);
            return false;
        }

        default:
            return false;
    }
}

bool ssrUser::begin(TXN_TYPE type)
{
    SQLPOINTER v_ptr;

    switch (type)
    {
        case READ_UNCOMMITTED:
            v_ptr = (SQLPOINTER*)SQL_TXN_READ_UNCOMMITTED;
            break;

        case READ_COMMITTED:
            v_ptr = (SQLPOINTER*)SQL_TXN_READ_COMMITTED;
            break;

        case REPEATABLE_READ:
            v_ptr = (SQLPOINTER*)SQL_TXN_REPEATABLE_READ;
            break;

        case SERIALIZABLE:
            v_ptr = (SQLPOINTER*)SQL_TXN_SERIALIZABLE;
            break;

        default:
            return false;
    }

    SQLRETURN res = SQLSetConnectAttr(hdbc_, SQL_ATTR_TXN_ISOLATION, v_ptr, SQL_IS_POINTER);

    switch (res)
    {
        case SQL_SUCCESS:
            break;

        case SQL_SUCCESS_WITH_INFO:
        case SQL_ERROR:
            get_db_error(SQL_HANDLE_DBC, hdbc_, nullptr);
            return false;

        default:
            return false;
    }

    return true;
}

bool ssrUser::commit()
{
    SQLRETURN res = SQLEndTran(SQL_HANDLE_DBC, hdbc_, SQL_COMMIT);

    if (res != SQL_SUCCESS)
    {
        get_db_error(SQL_HANDLE_DBC, hdbc_, nullptr);//txn_info();
        return false;
    }

    return true;
}

void ssrUser::rollback()
{
    SQLRETURN res = SQLEndTran(SQL_HANDLE_DBC, hdbc_, SQL_ROLLBACK);

    if (res != SQL_SUCCESS)
        get_db_error(SQL_HANDLE_DBC, hdbc_, nullptr);//txn_info();
}

/*-----------------------------------------------------------------------------------------*/
/*

int ssrUser::execute()
{
    int res = NULL;
    if(bind_all(hstmt_, binder, res))
        res = exec(hstmt_, columns_);

    if(res & DATABASE_ERROR)
        error_ = get_db_error(SQL_HANDLE_STMT, nullptr, hstmt_);
    clear_binder();
    return res;
}
*/

int ssrUser::execute(const std::wstring& stmt)
{
    int res = BIND_ERROR;
    if(bind_all(hstmt_, binder))
#ifdef DEBUG
        res = exec(hstmt_, columns_, print_stmt_, stmt);
#else
        res = exec(hstmt_, columns_, stmt);
#endif

    if(res & DATABASE_ERROR)
        error_ = get_db_error(SQL_HANDLE_STMT, nullptr, hstmt_);
    clear_binder();
    return res;
}

/*------------------------------------------------------------------------*/
/*

bool ssrUser::prepare(const std::wstring& stmt)
{
    return prepare_stmt(hstmt_, stmt, error_, columns_[1]);
}
*/

int ssrUser::fetch()
{
    columns_[0] = 0;
    return fetch0(hstmt_, error_);
}

int ssrUser::count_records() const
{
    SQLLEN count;
    SQLRowCount(hstmt_, &count);
    return (int) count;
}



int ssrUser::get_int(const int col)
{
    INT64 len;
    int num = NULL;
    SQLGetData(hstmt_, col + 1, SQL_C_SLONG, &num, 0, &len);
    return num;
}

#ifdef BUILD_DLL

int ssrUser::get_str(int col, wchar_t data[], size_t arr_size, int count)
{
    SQLLEN len;
    SQLGetData(hstmt_, col + 1, SQL_C_WCHAR, data, static_cast<SQLLEN>(arr_size), &len);

    return END_STR;
}

#else
std::wstring ssrUser::get_str(const int col)
{
    SQLLEN len;
    wchar_t value[200];
    SQLGetData(hstmt_, col + 1, SQL_C_WCHAR, value, 200, &len);

    if(len > 0)
        return value;

    return L"";
}

std::wstring ssrUser::get_name(const int col)
{
    SQLWCHAR name[200] = {};
    short name_length, data_type, d_digit, null_able;
    UINT64 word_length;
    SQLDescribeColW(hstmt_, col + 1, name, 200, &name_length, &data_type, &word_length, &d_digit, &null_able);

    return name;
}

#endif