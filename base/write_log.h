//
// Created by gijutsu12 on 24/05/08.
//

#ifndef DATABASE_WRITE_LOG_H
#define DATABASE_WRITE_LOG_H

#include <string>

void write(const std::string &name, const std::string &text);
void write(const std::string &name, const std::wstring &text);
void write(const std::string &name, unsigned char *ch, int num);
void write(const std::string &name, const wchar_t *ch, int num);

#endif //DATABASE_WRITE_LOG_H

