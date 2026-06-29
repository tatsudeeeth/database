//
// Created by gijutsu12 on 24/05/29.
//

#include "id_mgr.h"
#include <unordered_map>

int START_ID = 50;

std::unordered_map<int, bool> CONNECTOR;

void set_id(const int id)
{
    CONNECTOR.insert({id, true});
}

int get_id()
{
    CONNECTOR.insert({START_ID, true});
    ++START_ID;
    return START_ID - 1;
}

bool has_id(const int id)
{
    return !CONNECTOR.count(id);
}

bool release_id(const int id)
{
    if(!CONNECTOR.count(id))
        return false;
    CONNECTOR.erase(id);
    return true;
}

bool empty_id()
{
    return CONNECTOR.empty();
}