//
// Created by TatsuyaSano on 2025/11/22.
//

#ifndef DATABASE_DB_CONNECTOR_H
#define DATABASE_DB_CONNECTOR_H

#include "constants.h"

class dbConnector
{
public:
#ifdef DEBUG
    explicit dbConnector(const bool print) : open_(false), print_stmt_(print) {}
#else
    DataCtrl() : open_(false){}
#endif

    virtual ~dbConnector() = default;

    /*connect---------------------------------------------------------*/

    virtual void logout() = 0;
    virtual bool connect(int id) = 0;
    virtual bool dis_connect(int id) = 0;

/*bind----------------------------------------------------------*/
    virtual void bind(const std::wstring& param);
    virtual void bind(int param);

    /*bind NULL*/
    virtual void bind();

/*transaction--------------------------------------------------------------------------*/

    virtual bool auto_commit(bool auto_mode) = 0;
    virtual bool begin(TXN_TYPE type) = 0;
    virtual bool commit() = 0;
    virtual void rollback() = 0;

/*execute---------------------------------------------------------------*/
    virtual bool prepare(const std::wstring& stmt) = 0;

    void set_stmt(const std::unordered_map<int, std::wstring> &stmt_set) {stmt_set_ = stmt_set;}
    bool call_stmt(int key);

    virtual int execute() = 0;
    virtual int execute(const std::wstring& stmt) = 0;
    virtual int fetch() = 0;

    [[nodiscard]] virtual int count_rows() const = 0;
    [[nodiscard]] int count_cols() const{return cols_[1];}


/*get records---------------------------------------------------------*/


#ifdef BUILD_DLL
    virtual int get_str(int col, wchar_t data[], size_t arr_size, int count) = 0;
    virtual int get_name(int col, wchar_t data[], size_t arr_size, int count) = 0;

#else

    //virtual std::wstring get_name(int col) = 0;

    //virtual std::wstring get_str(int col) = 0;
    virtual bool get_str(int col, std::wstring &str) = 0;
    virtual bool get_name(int col, std::wstring &name) = 0;
    bool get_pair(int col, std::pair<std::wstring, std::wstring>& value);
    //{value = {get_name(col), get_str(col)};}

    bool get_pair(int col, std::pair<std::wstring, int>& value);
    //{value = {get_name(col), get_int(col)};}

    int  get_pair(std::pair<std::wstring, std::wstring>& value);
    //int get_pair(std::pair<std::wstring, int>& value);

#endif
    virtual bool get_int(int col, int &value) = 0;
    virtual void close_selected() = 0;
    virtual void reuse_prepared() = 0;
    virtual void close_prepared() = 0;
    virtual void close_stmt() = 0;
    virtual void reuse_stmt() = 0;

    [[nodiscard]] int get_error() const {return error_;}

protected:
    void clear_binder();

    void reset_cols() {cols_[0] = cols_[1] = 0;}
    int cols_[2] = {0};
    int error_ = STMT_SUCCESS;
    bool open_;
    BINDER binder;

private:
    std::unordered_map<int, std::wstring> stmt_set_;

#ifdef DEBUG
public:
    void EnablePrintStmt(const bool enable) {print_stmt_ = enable;}
protected:
    bool print_stmt_;
#endif

};

#endif //DATABASE_DB_CONNECTOR_H