#pragma once
#include "Phonebook.h"
#include <iostream>
#include <limits>
#include <vector>

namespace cli
{
    void displayMenu();

    std::vector<Contact> searchContacts(const Phonebook& phonebook);

    void addContact(Phonebook& phonebook);
    void deleteContact(Phonebook& phonebook);
    void editContact(Phonebook& phonebook);
    void sortContacts(Phonebook& phonebook);

    template<typename T>
    T getInput(const std::string& prompt) {
        T value;
        while (true) {
            std::cout << prompt;
            std::cin >> value;

            if (std::cin.fail()) {
                std::cout << "Error: invalid input format. Please try again." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (std::cin.peek() != '\n') {
                std::cout << "Error: Entered more than one value. Please try again." << std::endl;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}
