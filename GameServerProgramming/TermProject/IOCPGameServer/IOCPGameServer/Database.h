#pragma once

#include <windows.h>
#include <sqlext.h>

struct UserInfo {
    std::wstring_view name{};
    int x{}, y{};
    int level{};
    int exp{};
    int hp{};
};

class Database {
public:
    Database() = default;
    ~Database();
    void initalize(const std::wstring_view& dbName);
    UserInfo getUserInfo(const std::wstring_view& userId);
    bool addUserInfo(const std::wstring_view& userId);
    bool setUserInfo(const UserInfo& info);
private:
    void operator=(Database rhs) = delete;

    SQLHENV henv{};
    SQLHDBC hdbc{};
    SQLHSTMT hstmt{};
    SQLRETURN retcode{};
};