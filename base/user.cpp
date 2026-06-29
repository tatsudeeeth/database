#include "user.h"

#include "id_mgr.h"

bool User::set_lock(const int id)
{
    if(locker_ == -1)
    {
        if(has_id(id))
        {
            locker_ = id;
            return true;
        }
    }
    return false;
}

/*

void User::release_lock()
{
    release_id(locker_);
    locker_ = -1;
}
*/

bool User::release_lock(const int id)
{
    if(id != locker_)
        return false;

    locker_ = -1;
    return true;
}
