
#include "str_function.h"


void FromWstringToStr(std::string &s_str, const std::wstring &w_str, size_t &size)
{
    size_t converted;

    size = (wcslen(w_str.c_str()) + 1) * 2;
    auto *ch = new char[size];

    wcstombs_s(&converted, ch, size, w_str.c_str(), _TRUNCATE);
    size = converted;
    s_str = std::string(ch);
    delete []ch;
}

void FromStrToWstring(std::wstring &w_str, const char *origin, const size_t size)
{
    size_t converted;
    auto *ch = new wchar_t[size];
    mbstowcs_s(&converted, ch, size, origin, _TRUNCATE);

    w_str = ch;
    delete []ch;
}

bool FromWstringToDate(const std::wstring &w_str, DateBinder &date)
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