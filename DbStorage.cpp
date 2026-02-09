#include "DbStorage.h"
#include "validation.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <set>
#include <utility>

DbStorage::DbStorage(std::string host, const int port, std::string databaseName, std::string user,
                     std::string password) : host(std::move(host)), port(port), databaseName(std::move(databaseName)),
                                             user(std::move(user)), password(std::move(password)) {}

DbStorage::~DbStorage() {
    if (database.isOpen()) {
        database.close();
    }
}

bool DbStorage::init() {
    database = QSqlDatabase::addDatabase("QPSQL");

    database.setHostName(QString::fromStdString(host));
    database.setPort(port);
    database.setDatabaseName(QString::fromStdString(databaseName));
    database.setUserName(QString::fromStdString(user));
    database.setPassword(QString::fromStdString(password));

    if (!database.open()) {
        lastError = database.lastError().text().toStdString();
        return false;
    }

    return createTables();
}

std::vector<Contact> DbStorage::load() {
    std::vector<Contact> contacts;
    if (!database.isOpen()) {
        return contacts;
    }

    std::set<int> loadedIds;
    std::set<std::string> loadedEmails;
    std::set<std::string> loadedPhoneNumbers;

    QSqlQuery query("SELECT * FROM contacts ORDER BY id");
    QSqlQuery phoneQuery;

    while (query.next()) {
        Contact contact;

        bool ok;
        int id = query.value("id").toInt(&ok);
        if (!ok || id <= 0) {
            lastError = "Invalid ID found (must be positive integer).";
            return {};
        }
        if (loadedIds.count(id)) {
            lastError = "Duplicate ID found: " + std::to_string(id);
            return {};
        }
        loadedIds.insert(id);
        contact.setId(id);

        std::string surname = query.value("surname").toString().toStdString();
        surname = validation::trim(surname);
        if (!validation::isValidName(surname)) {
            lastError = "Invalid surname for ID " + std::to_string(id);
            return {};
        }
        contact.setSurname(surname);

        std::string forename = query.value("forename").toString().toStdString();
        forename = validation::trim(forename);
        if (!validation::isValidName(forename)) {
            lastError = "Invalid forename for ID " + std::to_string(id);
            return {};
        }
        contact.setForename(forename);

        std::string patronymic = query.value("patronymic").toString().toStdString();
        patronymic = validation::trim(patronymic);
        if (!patronymic.empty() && !validation::isValidName(patronymic)) {
            lastError = "Invalid patronymic for ID " + std::to_string(id);
            return {};
        }
        contact.setPatronymic(patronymic);

        std::string address = query.value("address").toString().toStdString();
        address = validation::trim(address);
        if (!validation::isValidAddress(address)) {
            lastError = "Invalid address for ID " + std::to_string(id);
            return {};
        }
        contact.setAddress(address);

        int day = query.value("birth_day").toInt();
        int month = query.value("birth_month").toInt();
        int year = query.value("birth_year").toInt();

        if (!(day == 0 && month == 0 && year == 0)) {
            if (!validation::isValidDate(day, month, year)) {
                lastError = "Invalid birth date for ID " + std::to_string(id);
                return {};
            }
        }
        contact.setBirthDate(Date(day, month, year));

        std::string email = query.value("email").toString().toStdString();
        email = validation::trim(email);
        if (!validation::isValidEmail(email)) {
            lastError = "Invalid email for ID " + std::to_string(id);
            return {};
        }
        if (!validation::isForenameInEmail(email, forename)) {
            lastError = "Email does not contain forename for ID " + std::to_string(id);
            return {};
        }

        std::string normalizedEmail = validation::normalizeEmail(email);
        if (loadedEmails.count(normalizedEmail)) {
            lastError = "Duplicate email found for ID " + std::to_string(id);
            return {};
        }
        loadedEmails.insert(normalizedEmail);
        contact.setEmail(normalizedEmail);

        phoneQuery.exec("SELECT type, number FROM phones WHERE contact_id = " + QString::number(id));

        while (phoneQuery.next()) {
            std::string type = phoneQuery.value("type").toString().toStdString();
            std::string number = phoneQuery.value("number").toString().toStdString();

            type = validation::trim(type);
            number = validation::trim(number);

            if (!validation::isValidPhoneType(type)) {
                lastError = "Invalid phone type for ID " + std::to_string(id);
                return {};
            }
            if (!validation::isValidPhoneNumber(number)) {
                lastError = "Invalid phone number for ID " + std::to_string(id);
                return {};
            }

            std::string normalizedNumber = validation::normalizePhoneNumber(number);
            if (loadedPhoneNumbers.count(normalizedNumber)) {
                lastError = "Duplicate phone number found for ID " + std::to_string(id);
                return {};
            }
            loadedPhoneNumbers.insert(normalizedNumber);

            contact.addPhoneNumber(type, normalizedNumber);
        }

        if (contact.getPhoneNumbers().empty()) {
            lastError = "Contact must have at least one phone number. ID: " + std::to_string(id);
            return {};
        }

        contacts.push_back(contact);
    }

    return contacts;
}

bool DbStorage::save(const std::vector<Contact>& contacts) {
    if (!database.isOpen()) {
        return false;
    }

    database.transaction();

    QSqlQuery query;

    if (!query.exec("TRUNCATE TABLE contacts RESTART IDENTITY CASCADE")) {
        lastError = query.lastError().text().toStdString();
        database.rollback();
        return false;
    }

    query.prepare("INSERT INTO contacts (id, surname, forename, patronymic, address, birth_day, birth_month, birth_year, email) "
                  "VALUES (:id, :surname, :forename, :patronymic, :address, :birth_day, :birth_month, :birth_year, :email)");

    QSqlQuery phoneQuery;
    phoneQuery.prepare("INSERT INTO phones (contact_id, type, number) VALUES (:contact_id, :type, :number)");

    for (const auto& contact : contacts) {
        query.bindValue(":id", contact.getId());
        query.bindValue(":surname", QString::fromStdString(contact.getSurname()));
        query.bindValue(":forename", QString::fromStdString(contact.getForename()));
        query.bindValue(":patronymic", QString::fromStdString(contact.getPatronymic()));
        query.bindValue(":address", QString::fromStdString(contact.getAddress()));

        const Date birthDate = contact.getBirthDate();
        query.bindValue(":birth_day", birthDate.day);
        query.bindValue(":birth_month", birthDate.month);
        query.bindValue(":birth_year", birthDate.year);

        query.bindValue(":email", QString::fromStdString(contact.getEmail()));

        if (!query.exec()) {
            lastError = query.lastError().text().toStdString();
            database.rollback();
            return false;
        }

        for (const auto& phone : contact.getPhoneNumbers()) {
            phoneQuery.bindValue(":contact_id", contact.getId());
            phoneQuery.bindValue(":type", QString::fromStdString(phone.type));
            phoneQuery.bindValue(":number", QString::fromStdString(phone.number));

            if (!phoneQuery.exec()) {
                lastError = phoneQuery.lastError().text().toStdString();
                database.rollback();
                return false;
            }
        }
    }

    return database.commit();
}

bool DbStorage::createTables() {
    QSqlQuery query;

    bool istableCreated = query.exec(
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id SERIAL PRIMARY KEY, "
        "surname TEXT NOT NULL, "
        "forename TEXT NOT NULL, "
        "patronymic TEXT, "
        "address TEXT, "
        "birth_day INT, "
        "birth_month INT, "
        "birth_year INT, "
        "email TEXT NOT NULL)"
    );

    if (!istableCreated) {
        lastError = query.lastError().text().toStdString();
        return false;
    }

    istableCreated = query.exec(
        "CREATE TABLE IF NOT EXISTS phones ("
        "id SERIAL PRIMARY KEY, "
        "contact_id INTEGER REFERENCES contacts(id) ON DELETE CASCADE, "
        "type TEXT NOT NULL, "
        "number TEXT NOT NULL)"
    );
    
    if (!istableCreated) {
        lastError = query.lastError().text().toStdString();
        return false;
    }

    return true;
}
