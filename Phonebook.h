#pragma once
#include "Contact.h"
#include <map>
#include <string>
#include <vector>


enum class SearchField
{
    ID,
    FORENAME,
    SURNAME,
    PATRONYMIC,
    ADDRESS,
    EMAIL,
    BIRTH_DAY,
    BIRTH_MONTH,
    BIRTH_YEAR,
    PHONE
};

enum class SortField
{
    ID,
    SURNAME,
    FORENAME,
    PATRONYMIC,
    ADDRESS,
    EMAIL,
    BIRTH_DATE
};

enum class SortDirection
{
    ASCENDING,
    DESCENDING
};

struct SortCriterion
{
    SortField field;
    SortDirection direction;
};


class Phonebook
{
    std::vector<Contact> contacts;
    int nextId;

public:
    Phonebook();

    void initializeNextId();

    void addContact(Contact& contact);
    void addContactFromFile(const Contact& contact);

    bool deleteContact(int id);

    Contact* findContactById(int id);
    std::vector<Contact> searchContacts(const std::map<SearchField, std::string>& criteria) const;
    void sortContacts(const std::vector<SortCriterion>& criteria);
    const std::vector<Contact>& getAllContacts() const;

    void printAllContacts() const;

    bool isEmailUnique(const std::string& email, int ignoreId = 0) const;
    bool isPhoneNumberUnique(const std::string& number, int ignoreId = 0) const;
};
