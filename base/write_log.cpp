#include "write_log.h"
#include <fstream>

void write(const std::string &name, const std::string &text)
{
    std::ofstream file(name, std::ios::app);
    if(!file.is_open())
        return;

    auto loc = std::locale("Japanese");
    file.imbue(loc);

    std::time_t dt;
    file << text << std::endl;

    file.close();
}

void write(const std::string &name, const std::wstring &text)
{
    std::wofstream file(name, std::ios::app);
    if(!file.is_open())
        return;

    auto loc = std::locale("Japanese");
    file.imbue(loc);

    file << text << std::endl;
    file.close();
}

void write(const std::string &name, unsigned char *ch, const int num)
{
    std::ofstream file(name, std::ios::app);
    if(!file.is_open())
        return;

    auto loc = std::locale("Japanese");
    file.imbue(loc);

    for(int i = 0 ; i < num ; ++ i)
    {
        if(!ch[i])
            break;
        file << ch[i] << std::endl;
    }

    file << std::endl;

    file.close();
}

void write(const std::string &name, const wchar_t *ch, const int num)
{
    std::wofstream file(name, std::ios::app);
    if(!file.is_open())
        return;

    auto loc = std::locale("Japanese");
    file.imbue(loc);

    std::time_t dt;

    for(int i = 0 ; i < num ; ++ i)
    {
        if(!ch[i])
            break;

        file << ch[i];
    }
    file << std::endl;

    file.close();
}
