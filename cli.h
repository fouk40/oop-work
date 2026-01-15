#pragma once
#include "Phonebook.h"
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace cli {
    void displayMenu();

    void printContact(const Contact& contact);
    void printAllContacts(const Phonebook& phonebook);

    std::vector<Contact> searchContacts(const Phonebook& phonebook);

    std::string getNameInput(const std::string& prompt, bool isMandatory);
    std::string getAddressInput(const std::string& prompt);
    Date getBirthDateInput(const std::string& prompt);
    std::string getEmailInput(const std::string& forename, const Phonebook& phonebook,
                              int ignoreId = 0);
    std::string getPhoneTypeInput();
    std::string getPhoneNumberInput(const Phonebook& phonebook, const Contact* currentContact, int editIdx = -1);

    void addContact(Phonebook& phonebook);
    void editContact(Phonebook& phonebook);
    void deleteContact(Phonebook& phonebook);
    void sortContacts(Phonebook& phonebook);

    template<typename T> T getInput(const std::string& prompt) {
        T value;
        while (true) {
            std::cout << prompt;
            std::cin >> value;

            if (std::cin.fail()) {
                std::cout << "Invalid input." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (std::cin.peek() != '\n') {
                std::cout << "Entered more than one value." << std::endl;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}
