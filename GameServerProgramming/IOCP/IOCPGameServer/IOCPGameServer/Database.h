#pragma once
#include <string_view>
#include <tuple>
#include <windows.h>
#include <sqlext.h>

class Database {
public:
    Database() = default;
    ~Database();
    void initalize(const std::wstring_view& dbName);
    std::tuple<std::wstring_view, int, int> getUserInfo(const std::wstring_view& userId);
    bool setUserInfo(const std::tuple<std::wstring_view, int, int>& info);
private:
    void operator=(Database rhs) = delete;

    SQLHENV henv{};
    SQLHDBC hdbc{};
    SQLHSTMT hstmt{};
    SQLRETURN retcode{};
};