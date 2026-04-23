
#ifndef DATABASE_SSR_FUNC_H
#define DATABASE_SSR_FUNC_H

#include "constants.h"
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sqlucode.h>

int get_db_error(SQLSMALLINT type, SQLHDBC hdbc, SQLHSTMT hstmt);
bool conn(const std::wstring& conn, SQLHDBC hdbc);

bool open_cursor(SQLHDBC& hdbc, SQLHSTMT& hstmt);
void close_cursor(SQLHSTMT& hstmt);

bool bind_all(SQLHSTMT hstmt, BINDER &binder);

#ifdef DEBUG
bool prepare_stmt(SQLHSTMT hstmt, const std::wstring& stmt, int &error, int &total_cols, bool print_stmt);
int exec(SQLHSTMT hstmt, int columns[], bool print_stmt, const std::wstring& stmt = L"");
#else
bool prepare_stmt(SQLHSTMT hstmt, const std::wstring& stmt, int &error, int &total_cols);
int exec(SQLHSTMT hstmt, int columns[], const std::wstring& stmt = L"");
#endif

int fetch0(SQLHSTMT hstmt, int &error);


#endif //DATABASE_SSR_FUNC_H