#ifndef USER_H
#define USER_H

#include "cursor.h"

//transaction type
enum TXN_TYPE
{
    READ_UNCOMMITTED,
    READ_COMMITTED,
    REPEATABLE_READ,
    SERIALIZABLE
};

class User : public DataCtrl
{
public:
    using DataCtrl::DataCtrl;
    virtual bool login(const std::wstring& stmt) = 0;
    virtual void logout() = 0;
    //virtual bool connect() = 0;
    virtual bool connect(int id) = 0;
    //virtual bool dis_connect() = 0;
    virtual bool dis_connect(int id) = 0;

    virtual CURSOR create_cursor() = 0;
    virtual void reset_cursor() = 0;

    /*----------------------------------------------------------------------------------*/

    bool set_lock(int id);
    bool release_lock(int id);//{locker_ = -1;}

    [[nodiscard]] bool has_lock() const{return locker_ != -1;}
    [[nodiscard]] bool is_locker(const int id) const{return id == locker_;}

    /*transaction management--------------------------------------------------------------------------*/

    virtual bool auto_commit(bool auto_mode) {return true;}
    virtual bool begin(TXN_TYPE type) = 0;
    virtual bool commit() = 0;
    virtual void rollback() = 0;

protected:
    [[nodiscard]] int get_locker() const{return locker_;}

private:
    int locker_ = -1;



};

#endif //USER_H
