

#ifndef DATABASE_LITE_CONNECTOR_H
#define DATABASE_LITE_CONNECTOR_H

#include "db_connector.h"

#ifdef BUILD_DLL
    #ifdef LITE_CONNECTOR_BODY
        #define LITE_CONNECTOR_EXPORT extern "C" __declspec(dllexport)
    #else
        #define LITE_CONNECTOR_EXPORT extern "C" __declspec(dllimport)
    #endif

#else
#define LITE_CONNECTOR_EXPORT
#endif


class LITE_CONNECTOR_EXPORT liteConnector final : public dbConnector
{
public:
#ifdef DEBUG
    explicit liteConnector(bool print = true);
#else
    liteConnector();
#endif
    bool login(const std::wstring& file);
    void logout() override;

    bool connect(int id) override;
    bool dis_connect(int id) override;

    /*transaction--------------------------------------------------------------------------*/

    bool auto_commit(bool auto_mode) override;
    bool begin(TXN_TYPE type) override;
    bool commit() override;
    void rollback() override;

    /*----------------------------------------------------------------------------------------------*/
    bool prepare(const std::wstring& stmt) override;
    int execute() override;
    int execute(const std::wstring& stmt) override;

    int fetch() override;
    [[nodiscard]] int count_rows() const override{return rows_;};

    /*execute----------------------------------------------------------------------------------------------*/

#ifdef BUILD_DLL
    int get_name(int col, wchar_t data[], size_t arr_size, int count) override;
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
    std::wstring file_;
    int rows_ = 0;
    std::wstring count_stmt_;
};


#endif //DATABASE_LITE_CONNECTOR_H