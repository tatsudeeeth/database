//
// Created by gijutsu12 on 24/05/15.
//

#include "cursor.h"

#include <iostream>

std::string FromWstringToStr(const std::wstring &w_str, size_t &size)
{
    size_t converted;

    size = (wcslen(w_str.c_str()) + 1) * 2;
    char *ch = new char[size];

    wcstombs_s(&converted, ch, size, w_str.c_str(), _TRUNCATE);
    size = converted;

    std::string s_str(ch);
    delete []ch;
    return s_str;
}

std::wstring FromStrToWstring(const char *origin, size_t size)
{
    size_t converted;
    auto *ch = new wchar_t[size];
    mbstowcs_s(&converted, ch, size, origin, _TRUNCATE);

    std::wstring w_str(ch);
    delete []ch;
    return w_str;
}

void FromStrToWstring(std::wstring &w_str, const char *origin, size_t size)
{
    size_t converted;
    auto *ch = new wchar_t[size];
    mbstowcs_s(&converted, ch, size, origin, _TRUNCATE);

    w_str = ch;
    delete []ch;
}

bool FromWstringToDate(const std::wstring &w_str, DateBlock &date)
{
    int i = 0;
    size_t pos = NULL;
    std::wstring value[3], str= w_str;

    for( ; i < 2 ; ++i)
    {
        pos = str.find('-');
        if(pos == std::wstring::npos)
            return false;
        value[i] = str.substr(0, pos);
        str = str.substr(pos+1);
    }
    value[2] = str.substr(0, pos);


    for(i = 0 ; i < 3 ; ++i)
    {
        try
        {
            const int v = std::stoi(value[i]);
            switch(i)
            {
                case 0:
                    date.year = v;
                    break;
                case 1:
                    date.month = v;
                    break;
                default:
                    date.day = v;
                    break;
            }
        }
        catch(std::invalid_argument &e)
        {
            return false;
        }
    }
    return true;
}

/*-------------------------------------------------------*/

int DataCtrl::get_data(std::wstring& value)
{
    if(columns_[0] == columns_[1])
        return END_COL;

    value = get_str(columns_[0]);

    ++columns_[0];
    return NEXT_COL;
}

int DataCtrl::get_data(std::pair<std::wstring, std::wstring>& value)
{
    if(columns_[0] == columns_[1])
        return END_COL;

    value = {get_name(columns_[0]), get_str(columns_[0])};
    ++columns_[0];
    return NEXT_COL;
}

int DataCtrl::get_data(std::pair<std::wstring, int>& value)
{
    if(columns_[0] == columns_[1])
        return END_COL;

    value = {get_name(columns_[0]), get_int(columns_[0])};
    ++columns_[0];
    return NEXT_COL;
}

bool DataCtrl::bind(int, const std::wstring &param)
{
    binder.category.emplace_back(str_bind);
    binder.texts.emplace_back(param);
    ++binder.params;
    return true;
}

bool DataCtrl::bind(int, const int param)
{
    binder.category.emplace_back(int_bind);
    binder.nums.emplace_back(param);
    ++binder.params;
    return true;
}

bool DataCtrl::bind(int)
{
    binder.category.emplace_back(null_bind);
    ++binder.params;
    return true;
}

/*-------------------------------------------------------*/

bool Cursor::select_stmt(const int key)
{
    if(!stmt_set_.count(key))
        return false;
    return prepare(stmt_set_.at(key));
}
