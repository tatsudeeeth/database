//
// Created by TatsuyaSano on 2025/11/23.
//

#ifndef DATABASE_SSR_CONNECTOR_H
#define DATABASE_SSR_CONNECTOR_H

#include "db_connector.h"

#ifdef BUILD_DLL
#ifdef SSR_CONNECTOR_BODY
    #define SSR_CONNECTOR_EXPORT extern "C" __declspec(dllexport)
#else
    #define SSR_CONNECTOR_EXPORT extern "C" __declspec(dllimport)
#endif


class SSR_USER_EXPORT ssrConnector : public Connector
{

#else
class ssrConnector final : public dbConnector
{
#endif
public:
#ifdef DEBUG
    explicit ssrConnector(bool print = true);
#else
    ssrConnector();
#endif

    /*connect---------------------------------------------------------*/
    bool login(const std::wstring& stmt);
    void logout() override;

    bool connect(int id) override;
    bool dis_connect(int id) override;

    /*transaction--------------------------------------------------------------------------*/

    bool auto_commit(bool auto_mode) override;
    bool begin(TXN_TYPE type) override;
    bool commit() override;
    void rollback() override;

    /*execute----------------------------------------------------------------------------------------------*/

    bool prepare(const std::wstring& stmt) override;

    int execute() override;
    int execute(const std::wstring& stmt) override;
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
    void close_stmt() override;
    void reuse_stmt() override;

private:
    std::wstring    conn_;

};
#endif //DATABASE_SSR_CONNECTOR_H