//
// Created by TatsuyaSano on 2025/11/23.
//

#ifndef DATABASE_STR_FUNCTION_H
#define DATABASE_STR_FUNCTION_H

#include "constants.h"

void FromWstringToStr(std::string &s_str, const std::wstring &w_str, size_t &size);
void FromStrToWstring(std::wstring &w_str, const char *origin, size_t size);

bool FromWstringToDate(const std::wstring &w_str, DateBinder &date);

#endif //DATABASE_STR_FUNCTION_H