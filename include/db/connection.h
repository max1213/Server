#pragma once

#include <sqlite3.h>
#include <string>

class ConnectDataBase {

	
public:
	ConnectDataBase();
	~ConnectDataBase();

	

	ConnectDataBase& operator=(const ConnectDataBase&) = delete;
	ConnectDataBase(const ConnectDataBase&) = delete;

public:
	sqlite3* db = nullptr;
};

