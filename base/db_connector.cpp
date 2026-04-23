
#include "db_connector.h"


/*-------------------------------------------------------*/

bool dbConnector::get_pair(const int col, std::pair<std::wstring, std::wstring>& value)
{
    if (!get_name(col, value.first))
        return false;
    get_str(col, value.second);
    return true;
}

bool dbConnector::get_pair(const int col, std::pair<std::wstring, int>& value)
{
    if (!get_name(col, value.first))
        return false;
    get_int(col, value.second);
    return true;
}

int dbConnector::get_pair(std::pair<std::wstring, std::wstring>& value)
{
    if(cols_[0] == cols_[1])
        return END_COL;

    get_name(cols_[0], value.first);
    get_str(cols_[0], value.second);
    //value.second = get_int(cols_[1]);

    //value = {get_name(cols_[0]), get_str(cols_[0])};
    ++cols_[0];
    return NEXT_COL;
}

/*
int dbConnector::get_pair(std::pair<std::wstring, int>& value)
{
    if(cols_[0] == cols_[1])
        return END_COL;
    get_name(cols_[0], value.first);
    value.second = get_int(cols_[1]);
    ++cols_[0];
    return NEXT_COL;
}
*/

void dbConnector::bind(const std::wstring& param)
{
    binder.category.emplace_back(str_bind);
    binder.texts.emplace_back(param);
    ++binder.params;
}

void dbConnector::bind(const int param)
{
    binder.category.emplace_back(int_bind);
    binder.nums.emplace_back(param);
    ++binder.params;
}

void dbConnector::bind()
{
    binder.category.emplace_back(null_bind);
    ++binder.params;
}

void dbConnector::clear_binder()
{
    binder.category.clear();
    binder.texts.clear();
    binder.nums.clear();
    binder.date.clear();
    binder.f_num.clear();
    binder.params = 0;
}
/*-------------------------------------------------------*/

bool dbConnector::call_stmt(const int key)
{
    if(!stmt_set_.count(key))
        return false;
    return prepare(stmt_set_.at(key));
}