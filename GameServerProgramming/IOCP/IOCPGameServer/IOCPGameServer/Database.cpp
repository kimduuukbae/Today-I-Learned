#include "Database.h"
#include <string>
#include <sstream>
using namespace std;

Database::~Database(){
	SQLCancel(hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
}

void Database::initalize(const std::wstring_view& dbName){
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	retcode = SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
	retcode = SQLConnect(hdbc, (SQLWCHAR*)dbName.data(), SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);
}

std::tuple<std::wstring_view, int, int> Database::getUserInfo(const std::wstring_view& userId){
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

	std::wstring strings{ L"EXEC SELECT_USERINFO "s };
	strings.append(userId.data());

	if (auto code{ SQLExecDirect(hstmt, (SQLWCHAR*)strings.data(), SQL_NTS) }; code != SQL_SUCCESS && code != SQL_SUCCESS_WITH_INFO)
		return { L"ERROR", 0, 0 };

	SQLWCHAR id[20]{};
	SQLINTEGER userX{}, userY{};
	SQLLEN cdId{}, cdX{}, cdY{};
	retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, id, 20, &cdId);
	retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &userX, 100, &cdX);
	retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &userY, 100, &cdY);
	
	retcode = SQLFetch(hstmt);

	SQLCancel(hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	if (retcode == SQL_NO_DATA)
		return { L"ERROR", 0, 0 };

	return { id, userX, userY };
}

bool Database::setUserInfo(const std::tuple<std::wstring_view, int, int>& info){
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

	std::wstring strings{ L"EXEC UPDATE_USERINFO "s };

	std::wostringstream ws{};
	ws << get<0>(info) << ", " << get<1>(info) << ", " << get<2>(info);
	strings.append(ws.str());
	
	if (auto code{ SQLExecDirect(hstmt, (SQLWCHAR*)strings.data(), SQL_NTS) }; code != SQL_SUCCESS && code != SQL_SUCCESS_WITH_INFO)
		return false;

	SQLCancel(hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	return !(retcode == SQL_NO_DATA);
}

