//
// Created by TatsuyaSano on 2025/11/23.
//

#ifndef DATABASE_LITE_FUNC_H
#define DATABASE_LITE_FUNC_H

#include "constants.h"
#include "sqlite3.h"
#include  <string>


namespace ldb
{
    int get_db_error(sqlite3* handle);
    bool conn(const std::wstring& file, sqlite3** handle);
    int bind_all(sqlite3_stmt** h_stmt, const BINDER &binder, int columns[]);
#ifdef DEBUG
    int prepare(const std::wstring& stmt, sqlite3* handle, sqlite3_stmt** h_stmt, bool print_stmt);
    int count_records(const std::wstring &stmt, sqlite3* handle, const BINDER &binder, bool print_stmt);
    bool exec(const std::wstring& stmt, sqlite3* handle, bool print_stmt);
#else
    int prepare(const std::wstring& stmt, sqlite3* handle, sqlite3_stmt** h_stmt);
    int count_records(const std::wstring &stmt, sqlite3* handle, const BINDER &binder);
    bool exec(const std::wstring& stmt, sqlite3* handle);
#endif

    int bind(sqlite3_stmt** h_stmt, const BINDER &binder);
    int exec(sqlite3_stmt** h_stmt);

    int fetch(sqlite3 *handle, sqlite3_stmt *stmt, int &error);
}
#endif //DATABASE_LITE_FUNC_H