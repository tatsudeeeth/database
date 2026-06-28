#include "test_common.h"


bool  InsertWithoutPrepare(dbConnector &user, const RECS &recs)
{

    bool res = true;

    const std::wstring stmt = L"INSERT INTO proto (dt, texts, number, m_number, combo, checked) VALUES (?, ?, ?, ?, ?, ?)";
    const std::vector<std::wstring> fields = {L"dt", L"texts", L"number", L"m_number", L"combo", L"checked"};

    for(auto &rec : recs)
    {
        for (auto &it : fields)
        {
            if(const auto& value = rec.at(it) ; value.empty())
                user.bind();
            else
                user.bind(value);
        }

        if(user.execute(stmt) & DATABASE_ERROR)
        {
            std::cout << "error in copy record without prepare" << std::endl;
            res = false;
            break;
        }
    }

    return res;
}

bool InsertWithPrepare(dbConnector &user, const RECS &recs)
{

    bool res = true;

    const std::wstring stmt = L"INSERT INTO proto (dt, texts, number, m_number, combo, checked) VALUES (?, ?, ?, ?, ?, ?)";
    const std::vector<std::wstring> fields = {L"dt", L"texts", L"number", L"m_number", L"combo", L"checked"};

    if (!user.prepare(stmt))
    {
        res = false;
        goto err;
    }

    for(auto &rec : recs)
    {

        for (auto &it : fields)
        {
            if(const auto& value = rec.at(it) ; value.empty())
                user.bind();
            else
                user.bind(value);
        }

        if(user.execute(stmt) & DATABASE_ERROR)
        {
            std::cout << "error in copy record with prepare" << std::endl;
            res = false;
            break;
        }
        user.reuse_stmt();
    }

    err:
    user.close_stmt();
    return res;
}


int copy_record(dbConnector &user)
{
    RECS recs;
    const auto lite = new liteConnector;
    lite->login(L"origin.sqlite3");
    lite->connect(100);
    if (lite->execute(L"SELECT * FROM proto;") != NEXT_ROW)
        return 1;

    create_records(*lite, recs);

    lite->dis_connect(100);
    lite->logout();

    const int id = 10;
    int res = 0;

    if(!user.connect(id))
        return 3;
    if(!user.auto_commit(false))
        return 4;
    if(!user.begin(READ_UNCOMMITTED))
        return 5;

    if (user.execute(L"delete from proto;") & DATABASE_ERROR)
    {
        res = 7;
        goto err;
    }

#ifdef INSERT_WITH_PROCEDURE
    if (!InsertWithPrepare(user, recs))
#else
    if (!InsertWithoutPrepare(user, recs))
#endif
        res = 6;;

    err:
    if(!res)
        user.commit();
    else
        user.rollback();

    user.auto_commit(true);
    user.dis_connect(id);

    return res;
}

int insert_test(dbConnector &user)
{
    int id = 10;
    int res = 0;
    user.connect(id);
    RECS recs;
    {
        REC rec;
#ifdef USE_SQLITE
        if (user.execute(L"insert into proto default values returning No") != NEXT_ROW)
#elif USE_MARIADB
        if (user.execute(L"insert into proto (number) values (default) returning id;") != NEXT_ROW)
#else
        if (user.execute(L"insert into proto output inserted.no default values;") != NEXT_ROW)
#endif
        {
            res = 1;
            goto err;
        }
        create_record(user, rec);
    }

    {
        user.bind(L"10");
        if (user.execute(L"insert into proto (number) values (?)") != STMT_SUCCESS)
        {
            res = 2;
            goto err;
        }

    }
    err:
    user.dis_connect(id);
    return res;
}

int update_test(dbConnector &user)
{
    return 0;
}


int select_test(dbConnector &user)
{
    int res = 0;
    user.auto_commit(false);
    user.begin(READ_UNCOMMITTED);
    RECS recs;
    {

        recs.clear();
        switch(user.execute(L"select * from proto;"))
        {
        case NEXT_ROW:
            create_records(user, recs);
        case END_ROW:
        case STMT_SUCCESS:
            break;

        default:
            res = 1;
            goto err;
        }

    }

    {
        recs.clear();
        int data[] = {1, 36, 37};
        const std::wstring stmt = L"select * from proto where id = ?;";
        for(const auto &it : data)
        {
            user.bind(it);
            switch(user.execute(stmt))
            {
            case NEXT_ROW:
                create_records(user, recs);
                break;
            case END_ROW:
                user.close_stmt();
                break;
            default:
                res = 2;
                goto err;
            }
        }

        user.prepare(stmt);
        for (const auto &it : data)
        {
            user.bind(it);
            switch(user.execute())
            {
            case NEXT_ROW:
                create_records(user, recs);
                break;
            case END_ROW:
                break;
            default:
                res = 3;
                goto err;
            }
            user.reuse_prepared();
        }
        user.close_prepared();
    }
    {
        user.bind(L"%1%");
        if(user.execute(L"select * from proto where id like ?") != NEXT_ROW)
        {
            res = 4;
        }
        create_records(user, recs);
    }

    err:
    user.rollback();
    user.auto_commit(true);
    return res;

}


int read_csv(RECS &recs) {

    //std::ifstream file(L"proto.csv");
    std::vector<std::wstring> header;

    std::ifstream file(L"proto.csv");
    std::string line, str;
    std::wstring value;
    if(file.is_open())
    {
        bool i = false;
        int j;
        size_t pos, count;
        std::wstring w_str;
        while (getline(file, line))
        {
            j = 0;

            //str = FromCharToWstring(line.c_str(), line.size());
            setlocale(LC_CTYPE, "Japanese");

            std::cout << line << std::endl;

            if (!i)
            {
                count = std::count(line.begin(), line.end(), ',');
                for(; j <= count ; ++j)
                {
                    pos = str.find(L',');
                    str = line.substr(0, pos);
                    if(j != 0)
                    {
                        //header.emplace_back(FromStrToWstring(str.c_str(), str.size()));
                        FromStrToWstring(w_str, str.c_str(), str.size());
                        header.emplace_back(w_str);;
                    }

                    line = line.substr(pos + 1, std::wstring::npos);
                }

                //header.emplace_back(line);
                i = true;
            }
            else
            {
                REC rec;

            }
        }
    }

    file.close();
    return 0;
}








int main()
{

    int res = 0;
    dbConnector *user;
    constexpr int id = 10;

#ifdef USE_MARIADB
    auto mdb = new mdbConnector;
    if (!mdb->login("192.168.2.5", "tester", "test_pass", "test", "", 3306, 0))
        return 1;
    user = mdb;

#else
    if(!ssr.login(L"dsn=test;database=test;uid=sa;pwd=ssr_secret1"))
        return 1;
    user = &ssr;

#endif


#ifdef FROM_ORIGIN

    res = copy_record(*user);
    if (res)
    {
        std::cout << "COPY RECORD ERROR :" << res << std::endl;
        return 1;
    }

#endif


#ifdef INSERT_TEST
    res = insert_test(*user);
    if(res)
    {
        std::cout << res << std::endl;
        return 6;
    }
#endif

#ifdef UPDATE_TEST
    res = update_test(*user);
    if(res)
    {
        std::cout << res << std::endl;
        return 7;
    }
#endif

#ifdef SELECT_TEST
    user->connect(id);
    res = select_test(*user);
    if(res)
    {
        std::cout << "select: " << res << std::endl;
        return 8;
    }
    user->dis_connect(id);
#endif

    user->logout();
    delete user;
    return res;
}