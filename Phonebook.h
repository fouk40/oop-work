#pragma once
#include "Contact.h"
#include <map>
#include <string>
#include <vector>

enum class SearchField {
    ID,
    SURNAME,
    FORENAME,
    PATRONYMIC,
    ADDRESS,
    BIRTH_DAY,
    BIRTH_MONTH,
    BIRTH_YEAR,
    EMAIL,
    PHONE
};

enum class SortField {
    ID,
    SURNAME,
    FORENAME,
    PATRONYMIC,
    ADDRESS,
    BIRTH_DATE,
    EMAIL
};

enum class SortDirection {
    ASCENDING,
    DESCENDING
};

struct SortCriterion {
    SortField field;
    SortDirection direction;
};

class Phonebook {
    std::vector<Contact> contacts;
    int nextId;

public:
    Phonebook();

    void initializeNextId();

    void addContact(Contact& contact);
    void addContactFromStorage(const Contact& contact);

    bool deleteContact(int id);

    Contact* findContact(int id);
    std::vector<Contact> searchContacts(const std::map<SearchField, std::string>& criteria) const;
    void sortContacts(const std::vector<SortCriterion>& criteria);
    const std::vector<Contact>& getAllContacts() const;

    std::vector<Contact> searchAllFields(const std::string& query) const;

    void reorderContacts(const std::vector<int>& orderedIds);

    bool isEmailUnique(const std::string& email, int ignoreId) const;
    bool isPhoneNumberUnique(const std::string& number, int ignoreId) const;
};
