#pragma once
#include "Date.h"
#include <string>
#include <vector>
#include <utility>

struct PhoneNumber {
    std::string type;
    std::string number;

    PhoneNumber(std::string type, std::string number) : type(std::move(type)), number(std::move(number)) {}
};

class Contact {
    int id = 0;
    std::string surname;
    std::string forename;
    std::string patronymic;
    std::string address;
    Date birthDate;
    std::string email;
    std::vector<PhoneNumber> phoneNumbers;

public:
    Contact() = default;
    ~Contact() = default;

    int getId() const;
    std::string getSurname() const;
    std::string getForename() const;
    std::string getPatronymic() const;
    std::string getAddress() const;
    Date getBirthDate() const;
    std::string getEmail() const;
    std::vector<PhoneNumber> getPhoneNumbers() const;

    void setId(int _id);
    void setSurname(const std::string& _surname);
    void setForename(const std::string& _forename);
    void setPatronymic(const std::string& _patronymic);
    void setAddress(const std::string& _address);
    void setBirthDate(const Date& _birthDate);
    void setEmail(const std::string& _email);

    void addPhoneNumber(const std::string& type, const std::string& number);
    bool deletePhoneNumber(size_t idx);
    bool editPhoneNumber(size_t idx, const std::string& newType, const std::string& newNumber);

    void clearPhoneNumbers();
};
