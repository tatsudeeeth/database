//
// Created by gijutsu12 on 24/05/16.
//

#ifndef DATABASE_SSR_CURSOR_H
#define DATABASE_SSR_CURSOR_H

#include "cursor.h"
#include "id_mgr.h"
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



class ssrCursor final: public Cursor
{
public:
#ifdef DEBUG
    ssrCursor(SQLHDBC hdbc, std::wstring* conn, bool print);
#else
    explicit ssrCursor(SQLHDBC hdbc, std::wstring* conn);
#endif
    bool open(int& id) override;
    bool close(int id) override;
    void reset(bool) override;

    /*----------------------------------------------------------------------------------------------*/

    [[nodiscard]] int count_records() const override;
    int execute(const std::wstring& stmt) override;
    int execute() override;
    bool prepare(const std::wstring& stmt) override;

    /*----------------------------------------------------------------------------------------------*/

    int fetch() override;
    int get_int(int col) override;

#ifdef BUILD_DLL
    int get_str(int col, wchar_t data[], size_t arr_size, int count) override;

#else
    std::wstring get_name(int col) override;
    std::wstring get_str(int col) override;
#endif

private:
    std::wstring*   conn_;
    SQLHDBC	        hdbc_;//voidÅ@ê⁄ë±ÉnÉìÉhÉã
    SQLHSTMT        hstmt_;//void

};

#endif //DATABASE_SSR_CURSOR_H
