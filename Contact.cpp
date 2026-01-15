#include "Contact.h"
#include "validation.h"

int Contact::getId() const {
    return id;
}

std::string Contact::getSurname() const {
    return surname;
}

std::string Contact::getForename() const {
    return forename;
}

std::string Contact::getPatronymic() const {
    return patronymic;
}

std::string Contact::getAddress() const {
    return address;
}

Date Contact::getBirthDate() const {
    return birthDate;
}

std::string Contact::getEmail() const {
    return email;
}

std::vector<PhoneNumber> Contact::getPhoneNumbers() const {
    return phoneNumbers;
}

void Contact::setId(const int _id) {
    id = _id;
}

void Contact::setSurname(const std::string& _surname) {
    surname = validation::trim(_surname);
}

void Contact::setForename(const std::string& _forename) {
    forename = validation::trim(_forename);
}

void Contact::setPatronymic(const std::string& _patronymic) {
    patronymic = validation::trim(_patronymic);
}

void Contact::setAddress(const std::string& _address) {
    address = validation::trim(_address);
}

void Contact::setBirthDate(const Date& _birthDate) {
    birthDate = _birthDate;
}

void Contact::setEmail(const std::string& _email) {
    email = validation::normalizeEmail(_email);
}

void Contact::addPhoneNumber(const std::string& type, const std::string& number) {
    phoneNumbers.emplace_back(validation::trim(type), validation::normalizePhoneNumber(number));
}

bool Contact::deletePhoneNumber(const size_t idx) {
    if (phoneNumbers.size() <= 1) {
        return false;
    }
    if (idx < phoneNumbers.size()) {
        phoneNumbers.erase(phoneNumbers.begin() + idx);
        return true;
    }
    return false;
}

bool Contact::editPhoneNumber(const size_t idx, const std::string& newType, const std::string& newNumber) {
    if (idx < phoneNumbers.size()) {
        phoneNumbers[idx].type = validation::trim(newType);
        phoneNumbers[idx].number = validation::normalizePhoneNumber(newNumber);
        return true;
    }
    return false;
}

void Contact::clearPhoneNumbers() {
    phoneNumbers.clear();
}
