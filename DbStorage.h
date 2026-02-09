#pragma once
#include "ContactStorage.h"
#include <QSqlDatabase>
#include <string>
#include <vector>

class DbStorage : public ContactStorage {
public:
    DbStorage(std::string host, int port, std::string databaseName, std::string user, std::string password);
    ~DbStorage() override;

    bool init();

    std::vector<Contact> load() override;
    bool save(const std::vector<Contact>& contacts) override;

private:
    QSqlDatabase database;
    std::string host;
    int port;
    std::string databaseName;
    std::string user;
    std::string password;

    bool createTables();
};
