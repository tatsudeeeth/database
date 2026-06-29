#ifndef SSR_USER_H
#define SSR_USER_H

#include "user.h"

#ifdef BUILD_DLL
#ifdef SSR_USER_BODY
    #define SSR_USER_EXPORT extern "C" __declspec(dllexport)
#else
    #define SSR_USER_EXPORT extern "C" __declspec(dllimport)
#endif

class SSR_USER_EXPORT ssrUser : public User
{

#else
class ssrUser final : public User
{
#endif
public:
#ifdef DEBUG
    explicit ssrUser(bool print = true);
#else
    ssrUser();
#endif
    bool login(const std::wstring& stmt) override;
    void logout() override;

    bool connect(int id) override;
    bool dis_connect(int id) override;

    void reset_cursor() override;
    CURSOR create_cursor() override;

    /*transaction--------------------------------------------------------------------------*/

    bool auto_commit(bool auto_mode) override;
    bool begin(TXN_TYPE type) override;
    bool commit() override;
    void rollback() override;

    /*----------------------------------------------------------------------------------------------*/

    int execute(const std::wstring& stmt) override;

    /*----------------------------------------------------------------------------------------------*/

    //int execute() override;
    //bool prepare(const std::wstring& stmt) override;


    /*----------------------------------------------------------------------------------------------*/
    int fetch() override;
    [[nodiscard]] int count_records() const override;
    int get_int(int col) override;

#ifdef BUILD_DLL
    int get_str(int col, wchar_t data[], size_t arr_size, int count) override;
#else
    using User::get_data;
    std::wstring get_name(int col) override;
    std::wstring get_str(int col) override;
#endif

private:
    std::wstring    conn_;

};

#endif //SSR_USER_H

