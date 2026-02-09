#include "Contact.h"
#include <iostream>
#include <utility>

#include "validation.h"

Contact::Contact(std::string forename, std::string surname, std::string  patronymic,
    std::string  address, const Date& birthDate, std::string  email,
    const std::vector<PhoneNumber>& phoneNumbers)
    : forename(std::move(forename)),
      surname(std::move(surname)),
      patronymic(std::move(patronymic)),
      address(std::move(address)),
      birthDate(birthDate),
      email(std::move(email)),
      phoneNumbers(phoneNumbers) {}


int Contact::getId() const
{
    return id;
}

std::string Contact::getForename() const
{
    return forename;
}

std::string Contact::getSurname() const
{
    return surname;
}

std::string Contact::getPatronymic() const
{
    return patronymic;
}

std::string Contact::getAddress() const
{
    return address;
}

Date Contact::getBirthDate() const
{
    return birthDate;
}

std::string Contact::getEmail() const
{
    return email;
}

std::vector<PhoneNumber> Contact::getPhoneNumbers() const
{
    return phoneNumbers;
}


void Contact::setId(const int _id)
{
    id = _id;
}

void Contact::setForename(const std::string& _forename)
{
    forename = _forename;
}

void Contact::setSurname(const std::string& _surname)
{
    surname = _surname;
}

void Contact::setPatronymic(const std::string& _patronymic)
{
    patronymic = _patronymic;
}

void Contact::setAddress(const std::string& _address)
{
    address = _address;
}

void Contact::setBirthDate(const Date& _birthDate)
{
    birthDate = _birthDate;
}

void Contact::setEmail(const std::string& _email)
{
    email = validation::normalizeEmail(_email);
}


void Contact::addPhoneNumber(const std::string& type, const std::string& number)
{
    phoneNumbers.emplace_back(type, validation::normalizePhoneNumber(number));
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
        phoneNumbers[idx].type = newType;
        phoneNumbers[idx].number = validation::normalizePhoneNumber(newNumber);
        return true;
    }
    return false;
}


void Contact::print() const {
    std::cout << "ID: " << id << std::endl;
    std::cout << "Forename: " << forename << std::endl;
    std::cout << "Surname: " << surname << std::endl;
    std::cout << "Patronymic: " << (patronymic.empty() ? "(not specified)" : patronymic) << std::endl;
    std::cout << "Address: " << (address.empty() ? "(not specified)" : address) << std::endl;
    std::cout << "Birth date: ";
    Date bd = getBirthDate();
    if (bd.year == 0) {
        std::cout << "(not specified)" << std::endl;
    } else {
        std::cout << bd.day << "." << bd.month << "." << bd.year << std::endl;
    }
    std::cout << "Email: " << email << std::endl;
    std::cout << "Phone numbers:" << std::endl;
    for (const PhoneNumber& phone : phoneNumbers) {
        std::cout << "  " << phone.type << ": " << phone.number << std::endl;
    }
}