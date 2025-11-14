#include <iostream>
#include <string>

#include "cli.h"
#include "FileManager.h"
#include "Phonebook.h"


int main()
{
    const std::string filename = "contacts.txt";
    Phonebook phonebook;


    std::cout << "Loading contacts from file " << filename << "..." << std::endl;
    const std::vector<Contact> loadedContacts = FileManager::loadFromFile(filename);
    for (const auto& contact : loadedContacts)
    {
        phonebook.addContactFromFile(contact);
    }
    phonebook.initializeNextId();
    std::cout << "Loaded " << loadedContacts.size() << " contact(s)." << std::endl;


    bool running = true;
    while (running)
    {
        cli::displayMenu();
        char choice = cli::getInput<char>("Your choice: ");

        switch (choice)
        {
        case '1':
            phonebook.printAllContacts();
            break;
        case '2':
            cli::addContact(phonebook);
            break;
        case '3':
            cli::searchContacts(phonebook);
            break;
        case '4':
            cli::deleteContact(phonebook);
            break;
        case '5':
            cli::editContact(phonebook);
            break;
        case '6':
            cli::sortContacts(phonebook);
            break;
        case '0':
            running = false;
            break;
        default:
            std::cout << "Invalid input. Please select a menu option." << std::endl;
            break;
        }
    }

    std::cout << "\nSaving contacts to file..." << std::endl;
    if (FileManager::saveToFile(filename, phonebook.getAllContacts()))
    {
        std::cout << "Data saved successfully. Goodbye!" << std::endl;
    }
    else
    {
        std::cout << "Error: Failed to save data to file." << std::endl;
    }

    return 0;
}
