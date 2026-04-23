

#include "mdb_func.h"
#include "write_log.h"
#include "str_function.h"

#ifdef DEBUG
    #include <iostream>
#endif

//std::vector<unsigned long> LENGTH;
std::vector<MYSQL_BIND> m_bind;
std::vector<MYSQL_TIME> sql_time;
std::vector<std::string> Str;



int get_db_stmt_error(MYSQL_STMT *sql_stmt)
{
#ifdef DEBUG
    std::cout << mysql_stmt_error(sql_stmt) << std::endl;
#endif
    auto err = mysql_stmt_errno(sql_stmt);
    return (int) err;
}

bool bind_str(const std::wstring &param, BINDER &binder)
{
    bool success = false;
    if(param.find('-') != std::wstring::npos)
    {
        DateBinder date;
        if(FromWstringToDate(param, date))
        {
            binder.date.emplace_back(date);
            binder.category.emplace_back(date_bind);
            success = true;
        }
        else
            return false;
    }

    if(!success)
    {
        binder.texts.emplace_back(param);
        binder.category.emplace_back(str_bind);
    }
    ++binder.params;
    return true;
}

std::wstring get_str_data(const struct_mdb &mdb_val)
{
    std::wstring value;
    if(mdb_val.is_null)
        return L"";

    switch (mdb_val.buffer_type)
    {
        case MYSQL_TYPE_TINY:
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_LONG:
            {
                //long val = (long &) mdb_val.ptr;
                if (const auto val = static_cast<long*>(mdb_val.ptr))
                    value = std::to_wstring(*val);
                break;
            }

        case MYSQL_TYPE_BIT:
            {
                //auto val = (bool&)mdb_val.ptr;
                const bool val = static_cast<bool*>(mdb_val.ptr);
                if(val)
                    value = L"1";
                else
                    value = L"0";
                break;
            }

        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_VARCHAR:
            {
                FromStrToWstring(value, mdb_val.str, mdb_val.length);
                //value = FromStrToWstring(mdb_val.str, mdb_val.length);
                break;
            }

        case MYSQL_TYPE_DOUBLE:
            {
                //double val = (double &) mdb_val.ptr;
                if (const auto val = static_cast<double*>(mdb_val.ptr))
                        value = std::to_wstring(*val);
                break;
            }

        case MYSQL_TYPE_FLOAT:
            {
                //float val = (float &) mdb_val.ptr;
                if (const auto val = static_cast<float*>(mdb_val.ptr))
                        value = std::to_wstring(*val);
                break;
            }

        case MYSQL_TYPE_DATE: {
            //MYSQL_TIME tm = (MYSQL_TIME &) mdb_val.tm;
                const auto tm = static_cast<MYSQL_TIME>(mdb_val.tm);
                value = std::to_wstring(tm.year);
                if (tm.month < 10)
                    value += L"-0" + std::to_wstring(tm.month);
                else
                    value += L"-" + std::to_wstring(tm.month);

                if (tm.day < 10)
                    value += L"-0" + std::to_wstring(tm.day);
                else
                    value += L"-" + std::to_wstring(tm.day);
                break;
            }
        default:
            value = L"";
            break;

    }

    return value;
}

/*
int get_str_data(const struct_mdb &mdb_val, wchar_t data[], size_t arr_size, int count)
{
    std::wstring value;
    if(mdb_val.is_null)
    {
        data[0] = L'\0';
        return END_STR;
    }

    switch (mdb_val.buffer_type)
    {
    case MYSQL_TYPE_TINY:
    case MYSQL_TYPE_SHORT:
    case MYSQL_TYPE_LONG:
        {
            long val = *static_cast<long*>(mdb_val.ptr);
            value = std::to_wstring(val);
            break;
        }

    case MYSQL_TYPE_BIT:
        {
            auto val = *static_cast<double*>(mdb_val.ptr);
            if(val)
                data[0] = L'1';
            else
                data[0] = L'0';
            break;
        }

    case MYSQL_TYPE_VAR_STRING:
    case MYSQL_TYPE_STRING:
    case MYSQL_TYPE_VARCHAR:
        {
            FromStrToWstring(value, mdb_val.str, mdb_val.length);
            //value = FromStrToWstring(mdb_val.str, mdb_val.length);
            break;
        }

    case MYSQL_TYPE_DOUBLE:
        {
            const double val = *static_cast<double*>(mdb_val.ptr);
            value = std::to_wstring(val);
            break;
        }

    case MYSQL_TYPE_FLOAT:
        {
            const float val = *static_cast<float*>(mdb_val.ptr);
            value = std::to_wstring(val);
            break;
        }

    case MYSQL_TYPE_DATE: {
            MYSQL_TIME tm = (MYSQL_TIME &) mdb_val.tm;
            value = std::to_wstring(tm.year);
            if (tm.month < 10)
                value += L"-0" + std::to_wstring(tm.month);
            else
                value += L"-" + std::to_wstring(tm.month);

            if (tm.day < 10)
                value += L"-0" + std::to_wstring(tm.day);
            else
                value += L"-" + std::to_wstring(tm.day);
            break;
    }
    default:
        data[0] = L'\0';
        return END_STR;

    }

    return END_STR;
}
*/

int get_int_data(const struct_mdb& mdb_val)
{
    if(mdb_val.is_null)
        return NULL;
    int value = NULL;
    switch(mdb_val.buffer_type)
    {
        case MYSQL_TYPE_TINY:
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_LONG:
        {
            value = (long &) mdb_val.ptr;
            break;
        }
        default:
            break;
    }
    return value;
}

namespace mdb
{
    int get_db_error(MYSQL *sql)
    {
#ifdef DEBUG
        std::cout << mysql_error(sql) << std::endl;
#endif
        auto err = mysql_errno(sql);
        return static_cast<int>(err);
    }


    bool conn(MYSQL *sql, const ConnectProperties &props)
    {

        sql = mysql_real_connect
                (
                        sql,
                        props.host.c_str(),
                        props.user.c_str(),
                        props.pass.c_str(),
                        props.db.c_str(),
                        props.port,
                        props.socket.c_str(),
                        props.flags
                        );

        if(!sql)
            return false;

        //shift-jisÇì¸óÕÇ≈Ç´ÇÈÇÊÇ§Ç…serverê›íËïœçX
        std::string str = {
            "SET "
            "character_set_client=cp932, "
            "character_set_connection=cp932, "
            "character_set_results=cp932;"
            //"character_set_server=utf8mb4, "
            //"character_set_database=utf8mb3;"
    };

        return mysql_query(sql, str.c_str()) == 0;
    }

#ifdef DEBUG
    int execute(MYSQL_STMT *sql_stmt, MDB_VAL &mdb_val, int columns[], const bool print_stmt, const std::wstring &stmt)
    {
        if(print_stmt && !stmt.empty())
        {
            setlocale(LC_CTYPE, "Japanese");
            std::wcout << stmt << std::endl;
        }
#else
    int exec(MYSQL_STMT *sql_stmt, MDB_VAL &mdb_val, int columns[], const std::wstring &stmt)
        {
#endif
            if(stmt.empty())
            {
                if(mysql_stmt_execute(sql_stmt))
                    return PREPARE_ERROR;
            }
            else
            {
                size_t size;
                std::string str;
                //auto str = FromWstringToStr(stmt, size);
                FromWstringToStr(str, stmt, size);
                if(mariadb_stmt_execute_direct(sql_stmt, str.c_str(), size))
                    return STMT_ERROR;
            }

            if(sql_stmt->state == MYSQL_STMT_EXECUTED)
                return STMT_SUCCESS;

            columns[1] = (int)mysql_stmt_field_count(sql_stmt);

            mdb_val.resize(columns[1]);

            auto fields = mariadb_stmt_fetch_fields(sql_stmt);

            std::vector<MYSQL_BIND> sql_bind;
            sql_bind.resize(columns[1]);

            for (int i = 0; i < columns[1]; ++i)
            {
                mdb_val[i].buffer_type = fields[i].type;
                //mdb_val[i].name = FromStrToWstring(fields[i].name, sizeof(fields[i].name));
                FromStrToWstring(mdb_val[i].name, fields[i].name, fields[i].name_length + 1);

                sql_bind[i].buffer_type = fields[i].type;
                sql_bind[i].is_null = &mdb_val[i].is_null;
                sql_bind[i].error = &mdb_val[i].error;

                switch (fields[i].type)
                {
                case MYSQL_TYPE_VAR_STRING:
                case MYSQL_TYPE_STRING:
                case MYSQL_TYPE_VARCHAR:

                    mdb_val[i].str = new char[fields[i].length];
                    sql_bind[i].buffer_length = fields[i].length;
                    sql_bind[i].buffer = mdb_val[i].str;
                    sql_bind[i].length = &mdb_val[i].length;
                    break;

                case MYSQL_TYPE_DATE:
                    sql_bind[i].buffer = &mdb_val[i].tm;
                    break;

                case MYSQL_TYPE_BIT:
                    sql_bind[i].buffer_length = fields[i].length;

                default:
                    sql_bind[i].buffer = &mdb_val[i].ptr;
                    sql_bind[i].length = &mdb_val[i].length;

                    break;
                }
            }

            mysql_stmt_attr_set(sql_stmt, STMT_ATTR_ARRAY_SIZE, &columns[1]);

            auto res = mysql_stmt_bind_result(sql_stmt, &sql_bind[0]);
            if(!res)
            {
                if(!mysql_stmt_store_result(sql_stmt))
                {
                    if(mysql_stmt_fetch(sql_stmt) == MYSQL_NO_DATA)
                        return END_ROW;
                    columns[0] = 0;
                    return NEXT_ROW;
                }
                mysql_stmt_close(sql_stmt);
                return STMT_SUCCESS;
            }
            return FETCH_ERROR;
        }
#ifdef DEBUG
    bool prepare(MYSQL_STMT *sql_stmt, const std::wstring &stmt, const bool print_stmt)
        {
            if(print_stmt)
            {
                setlocale(LC_CTYPE, "Japanese");
                std::wcout << stmt << std::endl;
            }
#else
    int prepare(MYSQL_STMT *sql_stmt, const std::wstring &stmt)
        {
#endif
            size_t size;
            std::string str;
            FromWstringToStr(str, stmt, size);
            if(mysql_stmt_prepare(sql_stmt, str.c_str(), size))
                return false;
            return true;
        }

    int execute(MYSQL_STMT *sql_stmt)
        {
            if(mysql_stmt_execute(sql_stmt))
                return STMT_ERROR;
            if(sql_stmt->state == MYSQL_STMT_EXECUTED)
                return STMT_SUCCESS;
            return NEXT_ROW;
        }

#ifdef DEBUG
    int execute(MYSQL_STMT *sql_stmt, const std::wstring &stmt, const bool print_stmt)
        {
            if(print_stmt && !stmt.empty())
            {
                setlocale(LC_CTYPE, "Japanese");
                std::wcout << stmt << std::endl;
            }
#else
    int exec(MYSQL_STMT *sql_stmt, const std::wstring &stmt)
        {
#endif

            size_t size;
            std::string str;
            FromWstringToStr(str, stmt, size);
            if(mariadb_stmt_execute_direct(sql_stmt, str.c_str(), size))
                return STMT_ERROR;

            if(sql_stmt->state == MYSQL_STMT_EXECUTED)
                return STMT_SUCCESS;
            return NEXT_ROW;
        }


    int bind(MYSQL_STMT* sql_stmt, BINDER &binder)
            {
                if(!binder.is_empty())
                {
                    int i = 1, nums[5] = {0};

                    m_bind.resize(binder.category.size());
                    sql_time.resize(binder.date.size());
                    Str.resize(binder.texts.size());

                    for(const auto &it : binder.category)
                    {
                        const int number = nums[it];
                        switch(it)
                        {
                        case null_bind:
                            m_bind[i - 1].buffer_type = MYSQL_TYPE_NULL;
                            break;
                        case int_bind:
                            m_bind[i - 1].buffer_type = MYSQL_TYPE_LONG;
                            m_bind[i - 1].buffer = &binder.nums[number];
                            break;
                        case float_bind:
                            m_bind[i - 1].buffer_type = MYSQL_TYPE_FLOAT;
                            m_bind[i - 1].buffer = &binder.f_num[number];
                            break;
                        case str_bind:
                            size_t size;
                            FromWstringToStr(Str[number], binder.texts[number], size);
                            m_bind[i - 1].buffer_type = MYSQL_TYPE_STRING;
                            m_bind[i - 1].buffer = (void*)Str[number].c_str();
                            m_bind[i - 1].buffer_length = strlen(Str[number].c_str());
                            break;
                        case date_bind:
                            sql_time[number].time_type=MYSQL_TIMESTAMP_NONE;
                            sql_time[number].year = binder.date[number].year;
                            sql_time[number].month = binder.date[number].month;
                            sql_time[number].day = binder.date[number].day;
                            m_bind[i - 1].buffer_type = MYSQL_TYPE_DATE;
                            m_bind[i - 1].buffer = &sql_time[number];
                            break;

                        }
                        ++i;
                        ++nums[it];
                    }

                    auto res = mysql_stmt_bind_param(sql_stmt, &m_bind[0]);
                    if(res)
                        return BIND_ERROR;
                    return BIND_SUCCESS;
                }
                return NO_BIND;
            }

    int reverse_bind(MDB_VAL &mdb_val, MYSQL_STMT *sql_stmt)
            {
                const int cols = static_cast<int>(mysql_stmt_field_count(sql_stmt));

                mdb_val.resize(cols);

                const auto fields = mariadb_stmt_fetch_fields(sql_stmt);
                std::vector<MYSQL_BIND> sql_bind;
                sql_bind.resize(cols);

                for (int i = 0; i < cols; ++i)
                {
                    mdb_val[i].buffer_type = fields[i].type;
                    FromStrToWstring(mdb_val[i].name, fields[i].name, fields[i].name_length + 1);

                    sql_bind[i].buffer_type = fields[i].type;
                    sql_bind[i].is_null = &mdb_val[i].is_null;
                    sql_bind[i].error = &mdb_val[i].error;

                    switch (fields[i].type)
                    {
                    case MYSQL_TYPE_VAR_STRING:
                    case MYSQL_TYPE_STRING:
                    case MYSQL_TYPE_VARCHAR:

                        mdb_val[i].str = new char[fields[i].length];
                        sql_bind[i].buffer_length = fields[i].length;
                        sql_bind[i].buffer = mdb_val[i].str;
                        sql_bind[i].length = &mdb_val[i].length;
                        break;

                    case MYSQL_TYPE_DATE:
                        sql_bind[i].buffer = &mdb_val[i].tm;
                        break;

                    case MYSQL_TYPE_BIT:
                        sql_bind[i].buffer_length = fields[i].length;

                    default:
                        sql_bind[i].buffer = &mdb_val[i].ptr;
                        sql_bind[i].length = &mdb_val[i].length;

                        break;
                    }
                }

                mysql_stmt_attr_set(sql_stmt, STMT_ATTR_ARRAY_SIZE, &cols);

                auto res = mysql_stmt_bind_result(sql_stmt, &sql_bind[0]);
                if(!res)
                {
                    if(!mysql_stmt_store_result(sql_stmt))
                    {
                        if(mysql_stmt_fetch(sql_stmt) == MYSQL_NO_DATA)
                            return END_ROW;
                        return NEXT_ROW;
                    }
                    mysql_stmt_close(sql_stmt);
                    return STMT_SUCCESS;
                }
                return FETCH_ERROR;
            }

    void reset_parameters()
            {
                Str.clear();
                m_bind.clear();
            }

    int fetch(MYSQL_RES *res)
            {
                MYSQL_ROW row = mysql_fetch_row(res);
                if(row == nullptr)
                    return END_ROW;
                return NEXT_ROW;
            }
}
