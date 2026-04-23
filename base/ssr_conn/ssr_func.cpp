//
// Created by TatsuyaSano on 2025/11/23.
//

#include "ssr_func.h"
#include "id_mgr.h"
#include "write_log.h"
#ifdef DEBUG
#include <iostream>
#endif

SQLLEN SSR_NULL = SQL_NULL_DATA;

/*----------------------------------------------------------------------------------------------*/

int get_db_error(const SQLSMALLINT type, SQLHDBC hdbc, SQLHSTMT hstmt)
{
    SQLWCHAR szState[5] = {}, szErrorMsg[1024] = {};
    SQLINTEGER  nErrorCode = 0;
    SQLSMALLINT nSize = 0;
    switch(type)
    {
        case SQL_HANDLE_DBC:
            SQLGetDiagRecW(type, hdbc, 1, szState, &nErrorCode, szErrorMsg, 1024, &nSize);
            break;

        case SQL_HANDLE_STMT:
            SQLGetDiagRecW(type, hstmt, 1, szState, &nErrorCode, szErrorMsg, 1024, &nSize);
            break;

        default:
            break;
    }

    write("ssr_conn.text", szErrorMsg, 1024);

#ifdef DEBUG
    setlocale(LC_CTYPE, "Japanese");
    std::wcout << szErrorMsg << std::endl;
#endif
    return DATABASE_ERROR;
}

bool bind_error(int res, SQLHSTMT hstmt)
{
    switch(res)
    {
        case SQL_SUCCESS:
            return true;

        case SQL_SUCCESS_WITH_INFO:
            get_db_error(SQL_HANDLE_STMT, nullptr, hstmt);
            return true;

        case SQL_ERROR:
        case SQL_INVALID_HANDLE:
        default:
            get_db_error(SQL_HANDLE_STMT, nullptr, hstmt);
            return false;
    }
}

bool conn(const std::wstring& conn, SQLHDBC hdbc)
{
    if(!empty_id())
        return true;

    SQLWCHAR	szConnStrOut[1024] = {};
    SQLSMALLINT cchConnStrOut = 0;

    SQLRETURN res = SQLDriverConnectW(hdbc, nullptr, (SQLWCHAR*)conn.c_str(), SQL_NTS, szConnStrOut, 1024, &cchConnStrOut, SQL_DRIVER_NOPROMPT);

    return SQL_SUCCEEDED(res);
}

bool open_cursor(SQLHDBC& hdbc, SQLHSTMT& hstmt)
{
    SQLRETURN res = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    return SQL_SUCCEEDED(res);
}

void close_cursor(SQLHSTMT& hstmt)
{
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    hstmt = SQL_NULL_HANDLE;
}


bool bind_all(SQLHSTMT hstmt, BINDER &binder)
{
    if(binder.is_empty())
        return true;

    int i = 1, nums[3] {0};
    short res;
    for(auto &it : binder.category)
    {
        switch(it)
        {
            case null_bind:
            {
                res = SQLBindParameter(hstmt, i, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_SMALLINT, 0, 0, nullptr, 0, &SSR_NULL);
                break;
            }
            case int_bind:
            {
                res = SQLBindParameter(hstmt, i, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &binder.nums.at(nums[it]), 0, nullptr);
                break;
            }
            case str_bind:
            {
                res = SQLBindParameter(hstmt, i, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, binder.texts.at(nums[it]).size(), 0, (SQLWCHAR*)binder.texts[nums[it]].c_str(), 0, nullptr);
                break;
            }
            default:
                return false;
        }
#ifdef DEBUG
        switch(res)
        {
            case SQL_SUCCESS:
                break;
            case SQL_SUCCESS_WITH_INFO:
                std::cout << "SQL SUCCESS WITH INFO" << std::endl;
                break;
            case SQL_ERROR:
                std::cout << "SQL ERROR" << std::endl;
                break;
            default:
                std::cout << "SQL INVALID HANDLE" << std::endl;
                break;
        }
#endif
        if(!SQL_SUCCEEDED(res))
            return false;

        ++i;
        ++nums[it];
    }
    return true;
}


#ifdef DEBUG
bool prepare_stmt(SQLHSTMT hstmt, const std::wstring& stmt, int &error, int &total_cols, bool print_stmt)
{
    if(print_stmt && !stmt.empty())
    {
        setlocale(LC_CTYPE, "Japanese");
        std::wcout << stmt << std::endl;
    }

#else
bool prepare_stmt(SQLHSTMT hstmt, const std::wstring& stmt, int &error, int &total_cols)
{
#endif
    auto res = SQLPrepareW(hstmt, (SQLWCHAR*)stmt.c_str(), (SQLINTEGER)stmt.size());
    switch(res)
    {
        case SQL_SUCCESS:
        {
            SQLSMALLINT col;
            SQLNumResultCols(hstmt, &col);
            total_cols = col;
            return true;
        }

        case SQL_SUCCESS_WITH_INFO:
            error = get_db_error(SQL_HANDLE_STMT, nullptr, hstmt);
            return true;

        default:
            error = get_db_error(SQL_HANDLE_STMT, nullptr, hstmt);
            return false;
    }
}


#ifdef DEBUG
int exec(SQLHSTMT hstmt, int columns[], bool print_stmt, const std::wstring& stmt)
{
    if(print_stmt && !stmt.empty())
    {
        setlocale(LC_CTYPE, "Japanese");
        std::wcout << stmt << std::endl;
    }
#else
int exec(SQLHSTMT hstmt, int columns[], const std::wstring& stmt)
{
#endif
    int result = NULL;
    bool res = false;
    if(stmt.empty())
        result = SQLExecute(hstmt);
    else
    {
        result = SQLExecDirectW(hstmt, (SQLWCHAR*)stmt.c_str(), (long)stmt.length());
        if(SQL_SUCCEEDED(result))
        {
            SQLSMALLINT col;
            SQLNumResultCols(hstmt, &col);//??ï
            columns[1] = col;
        }
    }

    switch(result)
    {
#ifdef DEBUG
        case SQL_SUCCESS:
        {
            res = true;
            break;
        }
        case SQL_SUCCESS_WITH_INFO:
        {
            res = true;
            std::cout << "sql success with info" << std::endl;
            break;
        }

        case SQL_NEED_DATA:
        {
            std::cout << "sql need data" << std::endl;
            break;
        }
        case SQL_NO_DATA:
        {
            std::cout << "sql no data" << std::endl;
            break;
        }
        case SQL_INVALID_HANDLE:
        {
            std::cout << "sql invalid handle" << std::endl;
            break;
        }
        case SQL_ERROR:
        {
            std::cout << "sql error" << std::endl;
            break;
        }
#else
        case SQL_SUCCESS:
        case SQL_SUCCESS_WITH_INFO:
        {
            res = true;
            break;
        }
#endif
        default:
            break;
    }
    if(!res)
        return STMT_ERROR;

    if(!columns[1])
        return STMT_SUCCESS;

    switch(SQLFetch(hstmt))
    {
        case SQL_SUCCESS:
            columns[0] = 0;
            return NEXT_ROW;

        case SQL_NO_DATA:
            result = END_ROW;
            break;

        default:
            result = FETCH_ERROR;
            break;
    }
    SQLCloseCursor(hstmt);
    return result;
}

int fetch0(SQLHSTMT hstmt, int &error)
{
    switch(SQLFetch(hstmt))
    {
        case SQL_SUCCESS:
            return NEXT_ROW;

        case SQL_NO_DATA:
            return END_ROW;

        default:
            error = get_db_error(SQL_HANDLE_STMT, nullptr, hstmt);
            return DATABASE_ERROR;
    }
}