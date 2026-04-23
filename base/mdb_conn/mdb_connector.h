

#ifndef DATABASE_MDB_CONNECTOR_H
#define DATABASE_MDB_CONNECTOR_H

#include "db_connector.h"


#ifdef BUILD_DLL
#ifdef MDB_CONNECTOR_BODY
    #define MDB_CONNECTOR_EXPORT extern "C" __declspec(dllexport)
#else
    #define MDB_CONNECTOR_EXPORT extern "C" __declspec(dllimport)
#endif


class MDB_CONNECTOR_EXPORT mdbConnector : public dbConnector
{

#else
class mdbConnector : public dbConnector
{
#endif

public:
#ifdef DEBUG
    explicit mdbConnector(bool print = true);
#else
    mdbConnector();
#endif

    /*connect---------------------------------------------------------*/
    bool login(
            const std::string &url,
            const std::string &user,
            const std::string &pass,
            const std::string &database,
            const std::string &socket,
            int port,
            unsigned long flags
            );
    void logout() override;

    bool connect(int id) override;
    bool dis_connect(int id) override;

    /*transaction--------------------------------------------------------------------------*/

    bool auto_commit(bool auto_mode) override;
    bool begin(TXN_TYPE type) override;
    bool commit() override;
    void rollback() override;

    /*bind----------------------------------------------------------------------------------------------*/
    void bind(const std::wstring &param) override;

    /*execute-------------------------------------------------------------------------*/
    bool prepare(const std::wstring& stmt) override;
    int execute(const std::wstring& stmt) override;
    int execute() override;
    /*----------------------------------------------------------------------------------------------*/

    int fetch() override;
    [[nodiscard]] int count_rows() const override;


#ifdef BUILD_DLL
    int get_str(int col, wchar_t data[], size_t arr_size, int count) override;
#else
    bool get_name(int col, std::wstring &name) override;
    bool get_str(int col, std::wstring& str) override;

    /*
    std::wstring get_name(int col) override;
    std::wstring get_str(int col) override;
    */

#endif

    bool get_int(int col, int &value) override;
    void reuse_prepared() override;
    void close_prepared() override;
    void close_selected() override;
    void reuse_stmt() override;
    void close_stmt() override;

private:

};

#endif //DATABASE_MDB_CONNECTOR_H