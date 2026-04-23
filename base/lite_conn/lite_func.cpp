

#include "lite_func.h"

#include "write_log.h"
#include "id_mgr.h"

#ifdef DEBUG
#include <iostream>
#endif

/*----------------------------------------------------------*/

namespace ldb
    {

        int get_db_error(sqlite3* handle)
        {
            const int code = sqlite3_extended_errcode(handle);
            int res = DATABASE_ERROR;
            std::wstring log;
            switch(code)
            {
            case SQLITE_BUSY:
                log = L"busy";
                break;

            case SQLITE_MISUSE:
                log = L"未定義の方法";
                break;

            case SQLITE_ERROR:
                log = L"error";;
                break;

            case SQLITE_CONSTRAINT:
                log = L"制約違反";
                res = CONSTRAINT_ERROR;
                break;

            case SQLITE_CONSTRAINT_UNIQUE:
                log = L"ユニーク制約違反";
                res = CONSTRAINT_UNIQUE;
                break;

            default:
                log = L"エラー";
                break;
            }
            write("lite_user.txt", log);
            return res;
        }

        bool conn(const std::wstring& file, sqlite3** handle)
        {
            if(!empty_id())
                return true;
            auto name = file.c_str();
            int res = sqlite3_open16(name, handle);

            if(!res)
                return true;

            get_db_error(*handle);
            return false;
        }

#ifdef DEBUG
        int prepare(const std::wstring& stmt, sqlite3* handle, sqlite3_stmt** h_stmt, const bool print_stmt)
            {
                if(print_stmt)
                {
                    setlocale(LC_CTYPE, "Japanese");
                    std::wcout << stmt << std::endl;
                }
#else
        bool prepare(const std::wstring& stmt, sqlite3* handle, sqlite3_stmt** h_stmt, int &error)
            {
#endif
                const void **ptr = nullptr;
                const int res = sqlite3_prepare16_v2(handle, stmt.c_str(), -1, h_stmt, ptr);

                if(*h_stmt)
                    if(!res)
                        return PREPARE_SUCCESS;
                //error = get_db_error(handle);
                //sqlite3_finalize(*h_stmt);
                return PREPARE_ERROR;
            }

        int bind(sqlite3_stmt** h_stmt, const BINDER &binder)
            {
                if(!binder.is_empty())
                {
                    int i = 1, nums[3] = {0};
                    int res;

                    for(const auto &it : binder.category)
                    {
                        switch(it)
                        {
                        case null_bind:
                                res = sqlite3_bind_null(*h_stmt, i);
                                break;
                        case int_bind:
                                res = sqlite3_bind_int(*h_stmt, i, binder.nums[nums[it]]);
                                break;
                        case str_bind:
                                res = sqlite3_bind_text16(*h_stmt, i, binder.texts[nums[it]].c_str(), -1, SQLITE_STATIC);
                                break;
                        default:
                            return BIND_ERROR;
                        }
                        if(res)
                            return BIND_ERROR;

                        ++i;
                        ++nums[it];
                    }

                }
                return BIND_SUCCESS;
            }

        int exec(sqlite3_stmt** h_stmt)
            {
                switch(sqlite3_step(*h_stmt))
                {
                case SQLITE_ROW:
                    return NEXT_ROW;

                case SQLITE_DONE:
                    return STMT_SUCCESS;

                default:
                    return STMT_ERROR;
                }

            }

        int fetch(sqlite3 *handle, sqlite3_stmt *stmt, int &error)
            {
                switch(sqlite3_step(stmt))
                {
                case SQLITE_ROW:
                    return NEXT_ROW;

                case SQLITE_DONE:
                    return END_ROW;

                default:
                    error = get_db_error(handle);
                    return FETCH_ERROR;
                }
            }

#ifdef DEBUG
        bool exec(const std::wstring& stmt, sqlite3* handle, const bool print_stmt)
            {
                if(print_stmt)
                {
                    setlocale(LC_CTYPE, "Japanese");
                    std::wcout << stmt << std::endl;
                }
#else
    bool exec(const std::wstring& stmt, sqlite3* handle)
            {
#endif
                const void **ptr = nullptr;
                sqlite3_stmt *h_stmt;
                int res = sqlite3_prepare16_v2(handle, stmt.c_str(), -1, &h_stmt, ptr);
                bool result = false;
                if(h_stmt)
                    if(!res)
                    {
                        res = sqlite3_step(h_stmt);
                        if(res == SQLITE_DONE)
                            result = true;
                    }

                sqlite3_finalize(h_stmt);
                return result;
            }

#ifdef DEBUG
        int count_records(const std::wstring &stmt, sqlite3* handle, const BINDER &binder, const bool print_stmt)
        #else
        int count_records(const std::wstring &stmt, sqlite3* handle, const BINDER &binder, const bool print_stmt)
        #endif
            {
                int back = 0;
                int rows = 0;
                if (stmt.back() == ';')
                    back = 1;
                sqlite3_stmt* h_stmt;
                std::wstring count_stmt = L"SELECT COUNT() FROM (" + stmt.substr(0, stmt.size() - back) + L");";
#ifdef DEBUG
                if (prepare(count_stmt, handle, &h_stmt, print_stmt) == PREPARE_SUCCESS)
#else
                if (prepare(count_stmt, handle, &h_stmt) == PREPARE_SUCCESS)
#endif
                {
                    if (bind(&h_stmt, binder) == BIND_SUCCESS)
                    {
                        switch(ldb::exec(&h_stmt))
                        {
                        case NEXT_ROW:
                            rows = sqlite3_column_int(h_stmt, 0);
                        default:
                            break;
                        }

                    }
                    sqlite3_finalize(h_stmt);
                }
                return rows;
            }

    }