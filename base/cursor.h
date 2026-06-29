//
// Created by gijutsu12 on 24/05/15.
//

#ifndef DATABASE_CURSOR_H
#define DATABASE_CURSOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>

/*DATABASE STATUS*/
#define DATABASE_SUCCESS    001
#define DATABASE_ERROR      002

#define CONSTRAINT_ERROR    014
#define CONSTRAINT_UNIQUE   18

#define STMT_SUCCESS        101
#define STMT_ERROR          102
#define BIND_SUCCESS        105
#define BIND_ERROR          106
#define NO_BIND             107
#define PREPARE_SUCCESS     109
#define PREPARE_ERROR       110

/*FETCH RESULT*/
#define NEXT_ROW            209
#define END_ROW             213
#define NEXT_COL            217
#define END_COL             221
#define FETCH_ERROR         210

/*DATA RESULT*/
#define NOT_FOUND           205
#define FOUND_DATA          201
#define NO_AUTHORITY        202

#define NEXT_STR            309
#define END_STR             313
#define STR_ERROR           310

struct DateBlock
{
    int year=NULL, month=NULL, day=NULL;
};

std::string FromWstringToStr(const std::wstring &w_str, size_t &size);
std::wstring FromStrToWstring(const char *origin, size_t size);
void FromStrToWstring(std::wstring &w_str, const char *origin, size_t size);
bool FromWstringToDate(const std::wstring &w_str, DateBlock &date);

enum binder_type
{
    null_bind,
    int_bind,
    str_bind,
    float_bind,
    date_bind
};

struct BINDER {

    std::vector<binder_type> category;
    std::vector<std::wstring> texts;
    std::vector<int> nums;
    std::vector<float> f_num;
    std::vector<DateBlock> date;
    [[nodiscard]] bool is_empty() const {return category.empty();}
    int params = 0;
};

class DataCtrl
{
public:
#ifdef DEBUG
    explicit DataCtrl(const bool print) : open_(false), print_stmt_(print) {}
#else
    DataCtrl() : open_(false){}
#endif

    virtual ~DataCtrl() = default;

    virtual bool bind(int num, const std::wstring& param);
    virtual bool bind(int num, int param);

    /*bind NULL*/
    virtual bool bind(int num);

    virtual int execute(const std::wstring& stmt) = 0;

    virtual int fetch() = 0;

    [[nodiscard]] virtual int count_records() const = 0;
    [[nodiscard]] int count_columns() const{return columns_[1];}


    virtual int get_int(int col) = 0;
#ifdef BUILD_DLL
    virtual int get_str(int col, wchar_t data[], size_t arr_size, int count) = 0;
    virtual int get_name(int col, wchar_t data[], size_t arr_size, int count) = 0;

#else

    virtual std::wstring get_name(int col) = 0;

    virtual std::wstring get_str(int col) = 0;
    void get_data(const int col, std::pair<std::wstring, std::wstring>& value)
    {value = {get_name(col), get_str(col)};}

    void get_data(const int col, std::pair<std::wstring, int>& value)
    {value = {get_name(col), get_int(col)};}

    int get_data(std::wstring& value);
    int get_data(std::pair<std::wstring, std::wstring>& value);
    int get_data(std::pair<std::wstring, int>& value);
#endif

    [[nodiscard]] int get_error() const {return error_;}

protected:
    //void clear_binder() {binder = BINDER();}
    void clear_binder() {binder.category.clear();binder.texts.clear();binder.nums.clear();binder.date.clear();binder.f_num.clear();binder.params=0;}

    void reset_cols() {columns_[0] = columns_[1] = 0;}
    int columns_[2] = {0};
    int error_ = STMT_SUCCESS;
    bool            open_;
    BINDER binder;

#ifdef DEBUG
public:
    void EnablePrintStmt(const bool enable) {print_stmt_ = enable;}
protected:
    bool print_stmt_;
#endif
};

/*-----------------------------------------------------------------------------------*/

class Cursor : public DataCtrl
{
public:
    using DataCtrl::DataCtrl;
    virtual bool open(int& id) = 0;
    virtual bool close(int id) = 0;
    virtual void reset(bool binding) = 0;

    /*---------------------------------------------------------------------------*/

    virtual bool prepare(const std::wstring& stmt) = 0;
    void set_prepare_stmt(const std::unordered_map<int, std::wstring> &stmt_set) {stmt_set_ = stmt_set;}

    bool select_stmt(int key);
    using DataCtrl::execute;
    virtual int execute() = 0;

    /*----------------------------------------------------------------------------------------------*/

private:
    std::unordered_map<int, std::wstring> stmt_set_;
};

typedef std::shared_ptr<Cursor> CURSOR;

#endif //DATABASE_CURSOR_H
