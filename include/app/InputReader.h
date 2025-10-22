#pragma once
#include <regex>
#include "company.h"
#include "license.h"
#include "email.h"

static void reader(std::string& out, std::string promt);
static void trim(std::string &str);

namespace InputReaderCompany {

	void readCompanyName(Company &c);
	void readContactName(Company &c);
	void readPhone(Company &c);
	void readExtraPhone(Company &c);
	void readEmail(Company &c);
	void readNotification(Company &c);
	void readPosition(Company &c);

};

namespace InputReaderLicense {

	void readExpirationDate(License &l);
	void readSupportEndDate(License &l);
	void readMachineId(License &l);
	void readNotification(License &l);
	void readName(License &l);
	void readType(License &l);
	void readNum(License &l);
	void readEmail(std::string& email);
	void readID(std::string& id);
};

namespace InputReaderEmail {
	void readEmail(std::string& email);
	void readID(std::string& id);
	void readNotification(Email &e);
};

namespace ConsoleCommands {

	void clearConsole();
	std::string readFind(std::string promt);
	std::string readDelete(std::string promt);
	int readConsoleCommand(std::string promt);
	std::string readFindMachineId(std::string promt);

};