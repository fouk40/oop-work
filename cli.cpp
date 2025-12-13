#include "cli.h"
#include "validation.h"
#include <iostream>
#include <map>
#include <vector>


namespace cli
{
    void displayMenu()
    {
        std::cout << "\n--- Phone Book ---" << std::endl;
        std::cout << "1. Show All Contacts" << std::endl;
        std::cout << "2. Add New Contact" << std::endl;
        std::cout << "3. Find Contacts" << std::endl;
        std::cout << "4. Delete Contact" << std::endl;
        std::cout << "5. Edit Contact" << std::endl;
        std::cout << "6. Sort Contacts" << std::endl;
        std::cout << "0. Save and Exit" << std::endl;
        std::cout << "-----------------------------" << std::endl;
    }

    void addContact(Phonebook& phonebook)
    {
        std::string input;
        Contact newContact;

        std::cout << "\n--- Adding a New Contact ---" << std::endl;

        while (true)
        {
            std::cout << "Enter forename: ";
            std::getline(std::cin, input);
            std::string trimmed = validation::trim(input);
            if (validation::isValidName(trimmed))
            {
                newContact.setForename(trimmed);
                break;
            }
            std::cout << "Error: Empty or invalid forename. Please try again." << std::endl;
        }

        while (true)
        {
            std::cout << "Enter surname: ";
            std::getline(std::cin, input);
            std::string trimmed = validation::trim(input);
            if (validation::isValidName(trimmed))
            {
                newContact.setSurname(trimmed);
                break;
            }
            std::cout << "Error: Invalid or empty surname. Please try again." << std::endl;
        }

        while (true)
        {
            std::cout << "Enter patronymic (or press Enter to skip): ";
            std::getline(std::cin, input);
            std::string trimmed = validation::trim(input);
            if (trimmed.empty() || validation::isValidName(trimmed))
            {
                newContact.setPatronymic(trimmed);
                break;
            }
            std::cout << "Error: Invalid patronymic. Please try again." << std::endl;
        }

        while (true)
        {
            std::cout << "Enter address (or press Enter to skip): ";
            std::getline(std::cin, input);
            std::string trimmed = validation::trim(input);
            if (validation::isValidAddress(trimmed))
            {
                newContact.setAddress(trimmed);
                break;
            }
            std::cout << "Error: Invalid address. Please try again." << std::endl;
        }

        while (true)
        {
            int day = getInput<int>("Enter day of birth (or 0 to skip): ");

            if (day == 0)
            {
                break;
            }

            int month = getInput<int>("Enter month of birth: ");
            int year = getInput<int>("Enter year of birth: ");

            if (validation::isValidDate(day, month, year))
            {
                newContact.setBirthDate(Date(day, month, year));
                break;
            }
            std::cout << "Error: Invalid date. Please try again." << std::endl;
        }

        while (true)
        {
            std::cout << "Enter email: ";
            std::getline(std::cin, input);
            std::string trimmed = validation::trim(input);
            if (validation::isValidEmail(trimmed))
            {
                if (phonebook.isEmailUnique(trimmed))
                {
                    newContact.setEmail(trimmed);
                    break;
                }
                std::cout << "Error: This email is already in use. Please enter a different one." << std::endl;
            }
            else
            {
                std::cout << "Error: Invalid email format. Please try again." << std::endl;
            }
        }

        std::string type, number;
        while (true)
        {
            std::cout << "Enter phone type (e.g. Mobile): ";
            std::getline(std::cin, type);
            type = validation::trim(type);
            if (validation::isValidPhoneType(type))
            {
                break;
            }
            std::cout << "Error: Invalid type format. Please try again." << std::endl;
        }

        while (true)
        {
            std::cout << "Enter phone number: ";
            std::getline(std::cin, number);

            std::string trimmedNumber = validation::trim(number);
            if (validation::isValidPhoneNumber(trimmedNumber))
            {
                if (phonebook.isPhoneNumberUnique(trimmedNumber))
                {
                    newContact.addPhoneNumber(type, trimmedNumber);
                    std::cout << "The first number has been added successfully." << std::endl;
                    break;
                }
                std::cout << "Error: This phone number is already in use. Please enter a different one." << std::endl;
            }
            else
            {
                std::cout << "Error: Invalid number format. Please try again." << std::endl;
            }
        }

        while (true)
        {
            std::cout << "\nDo you want to add another phone number?";
            std::getline(std::cin, input);
            if (input != "y" && input != "Y")
            {
                break;
            }

            while (true)
            {
                std::cout << "Enter phone type (e.g. Mobile): ";
                std::getline(std::cin, type);
                type = validation::trim(type);
                if (validation::isValidPhoneType(type))
                {
                    break;
                }
                std::cout << "Error: Invalid type format. Please try again." << std::endl;
            }
            while (true)
            {
                std::cout << "Enter phone number: ";
                std::getline(std::cin, number);
                std::string trimmedNumber = validation::trim(number);
                if (validation::isValidPhoneNumber(trimmedNumber))
                {
                    if (phonebook.isPhoneNumberUnique(trimmedNumber))
                    {
                        newContact.addPhoneNumber(type, trimmedNumber);
                        break;
                    }
                    std::cout << "Error: This phone number is already in use. "
                                 "Please enter a different one." << std::endl;
                }
                else
                {
                    std::cout << "Error: Incorrect number format. Please try again." << std::endl;
                }
            }
        }

        phonebook.addContact(newContact);
        std::cout << "\nContact saved successfully." << std::endl;
    }

    std::vector<Contact> searchContacts(const Phonebook& phonebook)
    {
        std::map<SearchField, std::string> criteria;
        std::string query;

        std::cout << "\n--- Advanced Search ---" << std::endl;
        std::cout << "Leave a field empty (press Enter) to not use it for searching" << std::endl;

        std::cout << "ID: ";
        std::getline(std::cin, query);
        if (!query.empty())
        {
            criteria.insert({SearchField::ID, query});
        }

        std::cout << "Forename: ";
        std::getline(std::cin, query);
        if (!query.empty())
        {
            criteria.insert({SearchField::FORENAME, query});
        }

        std::cout << "Surname: ";
        std::getline(std::cin, query);
        if (!query.empty())
        {
            criteria.insert({SearchField::SURNAME, query});
        }

        std::cout << "Patronymic: ";
        std::getline(std::cin, query);
        if (!query.empty())
        {
            criteria.insert({SearchField::PATRONYMIC, query});
        }

        std::cout << "Address: ";
        std::getline(std::cin, query);
        if (!query.empty())
        {
            criteria.insert({SearchField::ADDRESS, query});
        }

        std::cout << "Email: ";
        std::getline(std::cin, query);
        if (!query.empty())
        {
            criteria.insert({SearchField::EMAIL, query});
        }

        std::cout << "Day of birth: ";
        std::getline(std::cin, query);
        if (!query.empty())
        {
            criteria.insert({SearchField::BIRTH_DAY, query});
        }

        std::cout << "Month of birth: ";
        std::getline(std::cin, query);
        if (!query.empty())
        {
            criteria.insert({SearchField::BIRTH_MONTH, query});
        }

        std::cout << "Year of birth: ";
        std::getline(std::cin, query);
        if (!query.empty())
        {
            criteria.insert({SearchField::BIRTH_YEAR, query});
        }

        std::cout << "Phone number: ";
        std::getline(std::cin, query);
        if (!query.empty())
        {
            criteria.insert({SearchField::PHONE, query});
        }


        std::vector<Contact> foundContacts = phonebook.searchContacts(criteria);

        if (foundContacts.empty())
        {
            std::cout << "\nNothing found for the specified criteria." << std::endl;
        }
        else
        {
            std::cout << "\n--- Search Results (" << foundContacts.size() << ") ---" << std::endl;
            for (size_t i = 0; i < foundContacts.size(); ++i)
            {
                std::cout << ">> Index number: " << i + 1 << " <<\n";
                foundContacts[i].print();
                std::cout << "--------------------" << std::endl;
            }
        }
        return foundContacts;
    }

    void deleteContact(Phonebook& phonebook)
    {
        std::cout << "\n--- Deleting a contact ---" << std::endl;
        std::cout << "First, find the contact you want to delete" << std::endl;

        const std::vector<Contact> foundContacts = searchContacts(phonebook);

        if (foundContacts.empty())
        {
            return;
        }

        while (true)
        {
            const auto choice = getInput<size_t>("\nEnter the index number of the contact to delete (0 to cancel): ");

            if (choice == 0)
            {
                std::cout << "Deletion canceled." << std::endl;
                return;
            }

            if (choice > 0 && choice <= foundContacts.size())
            {
                int idToDelete = foundContacts[choice - 1].getId();

                if (phonebook.deleteContact(idToDelete))
                {
                    std::cout << "Contact with ID " << idToDelete << " successfully deleted." << std::endl;
                }
                else
                {
                    std::cout << "Internal error: Cannot delete contact with ID " << idToDelete << "." << std::endl;
                }
                break;
            }
            std::cout << "Error: Failed to delete contact. The index number is out of range. "
                         "Please try again." << std::endl;
        }
    }

    void editContact(Phonebook& phonebook)
    {
        std::cout << "\n--- Editing a Contact ---" << std::endl;
        std::cout << "First, find the contact you want to edit." << std::endl;

        std::vector<Contact> foundContacts = searchContacts(phonebook);

        if (foundContacts.empty())
        {
            return;
        }

        while (true)
        {
            auto idx = getInput<size_t>("\nEnter the index number of the contact to edit (0 to cancel): ");

            if (idx == 0)
            {
                std::cout << "Editing canceled." << std::endl;
                return;
            }

            if (idx > 0 && idx <= foundContacts.size())
            {
                int idToEdit = foundContacts[idx - 1].getId();
                Contact* contactToEdit = phonebook.findContactById(idToEdit);

                if (contactToEdit == nullptr)
                {
                    std::cout << "Internal error: failed to find contact for editing." << std::endl;
                    break;
                }

                bool editing = true;
                while (editing)
                {
                    std::cout << "\n--- Editing contact ---" << std::endl;
                    contactToEdit->print();
                    std::cout << "---------------------------------" << std::endl;
                    std::cout << " 1. Edit Forename" << std::endl;
                    std::cout << " 2. Edit Surname" << std::endl;
                    std::cout << " 3. Edit Patronymic" << std::endl;
                    std::cout << " 4. Edit Address" << std::endl;
                    std::cout << " 5. Edit Email" << std::endl;
                    std::cout << " 6. Edit Birth Date" << std::endl;
                    std::cout << " 7. Add Phone Number" << std::endl;
                    std::cout << " 8. Edit Phone Number" << std::endl;
                    std::cout << " 9. Delete Phone Number" << std::endl;
                    std::cout << " 0. Finish Editing" << std::endl;
                    std::cout << "---------------------------------" << std::endl;

                    char choice = getInput<char>("Your choice: ");
                    std::string input;

                    switch (choice)
                    {
                    case '1':
                        {
                            while (true)
                            {
                                std::cout << "Enter new forename: ";
                                std::getline(std::cin, input);
                                std::string trimmed = validation::trim(input);
                                if (validation::isValidName(trimmed))
                                {
                                    contactToEdit->setForename(trimmed);
                                    std::cout << "Forename changed." << std::endl;
                                    break;
                                }
                                std::cout << "Error: invalid forename. Please try again." << std::endl;
                            }
                            break;
                        }
                    case '2':
                        {
                            while (true)
                            {
                                std::cout << "Enter new surname: ";
                                std::getline(std::cin, input);
                                std::string trimmed = validation::trim(input);
                                if (validation::isValidName(trimmed))
                                {
                                    contactToEdit->setSurname(trimmed);
                                    std::cout << "Surname changed." << std::endl;
                                    break;
                                }
                                std::cout << "Error: invalid surname. Please try again." << std::endl;
                            }
                            break;
                        }
                    case '3':
                        {
                            while (true)
                            {
                                std::cout << "Enter patronymic (or press Enter to clear): ";
                                std::getline(std::cin, input);
                                std::string trimmed = validation::trim(input);
                                if (trimmed.empty() || validation::isValidName(trimmed))
                                {
                                    contactToEdit->setPatronymic(trimmed);
                                    std::cout << "Patronymic changed." << std::endl;
                                    break;
                                }
                                std::cout << "Error: Invalid patronymic. Please try again." << std::endl;
                            }
                            break;
                        }
                    case '4':
                        {
                            while (true)
                            {
                                std::cout << "Enter new address (Enter to clear):";
                                std::getline(std::cin, input);
                                std::string trimmed = validation::trim(input);
                                if (validation::isValidAddress(trimmed))
                                {
                                    contactToEdit->setAddress(trimmed);
                                    std::cout << "Address changed." << std::endl;
                                    break;
                                }
                                std::cout << "Error: Invalid address. Please try again." << std::endl;
                            }
                            break;
                        }
                    case '5':
                        {
                            while (true)
                            {
                                std::cout << "Enter new email: ";
                                std::getline(std::cin, input);
                                std::string trimmed = validation::trim(input);
                                if (validation::isValidEmail(trimmed))
                                {
                                    if (phonebook.isEmailUnique(trimmed, contactToEdit->getId()))
                                    {
                                        contactToEdit->setEmail(trimmed);
                                        std::cout << "Email changed." << std::endl;
                                        break;
                                    }
                                    std::cout << "Error: this email is already in use. "
                                                 "Please enter a different one." << std::endl;
                                }
                                else
                                {
                                    std::cout << "Error: invalid email format." << std::endl;
                                }
                            }
                            break;
                        }
                    case '6':
                        {
                            while (true)
                            {
                                int day = getInput<int>("Enter day of birth (or 0 to clear): ");
                                if (day == 0)
                                {
                                    contactToEdit->setBirthDate(Date(0, 0, 0));
                                    std::cout << "Birth date cleared." << std::endl;
                                    break;
                                }
                                int month = getInput<int>("Enter month of birth: ");
                                int year = getInput<int>("Enter year of birth: ");
                                if (validation::isValidDate(day, month, year))
                                {
                                    contactToEdit->setBirthDate(Date(day, month, year));
                                    std::cout << "Birth date changed." << std::endl;
                                    break;
                                }
                                std::cout << "Error: Invalid date. Please try again." << std::endl;
                            }
                            break;
                        }
                    case '7':
                        {
                            std::string type, number;
                            while (true)
                            {
                                std::cout << "Enter phone type (e.g. Mobile): ";
                                std::getline(std::cin, type);
                                type = validation::trim(type);
                                if (validation::isValidPhoneType(type))
                                {
                                    break;
                                }
                                std::cout << "Error: Invalid type format. Please try again." << std::endl;
                            }

                            while (true)
                            {
                                std::cout << "Enter phone number: ";
                                std::getline(std::cin, number);
                                std::string trimmedNumber = validation::trim(number);
                                if (validation::isValidPhoneNumber(trimmedNumber))
                                {
                                    if (phonebook.isPhoneNumberUnique(trimmedNumber))
                                    {
                                        contactToEdit->addPhoneNumber(validation::trim(type), trimmedNumber);
                                        std::cout << "Number successfully added." << std::endl;
                                        break;
                                    }
                                    std::cout << "Error: this phone number is already in use." << std::endl;
                                }
                                else
                                {
                                    std::cout << "Error: invalid number format." << std::endl;
                                }
                            }
                            break;
                        }
                    case '8':
                        {
                            const auto& numbers = contactToEdit->getPhoneNumbers();
                            std::cout << "Select a phone number to edit:" << std::endl;
                            for (size_t i = 0; i < numbers.size(); ++i)
                            {
                                std::cout << i + 1 << ". " << numbers[i].type << ": " << numbers[i].number << std::endl;
                            }
                            idx = getInput<size_t>("Your choice (0 to cancel): ");
                            if (idx > 0 && idx <= numbers.size())
                            {
                                std::string newType, newNumber;
                                while (true)
                                {
                                    std::cout << "Enter phone type (e.g. Mobile): ";
                                    std::getline(std::cin, newType);
                                    newType = validation::trim(newType);
                                    if (validation::isValidPhoneType(newType))
                                    {
                                        break;
                                    }
                                    std::cout << "Error: Invalid type format. Please try again." << std::endl;
                                }

                                while (true)
                                {
                                    std::cout << "Enter new number: ";
                                    std::getline(std::cin, newNumber);
                                    std::string trimmedNumber = validation::trim(newNumber);
                                    if (validation::isValidPhoneNumber(trimmedNumber))
                                    {
                                        std::string normalizedNumber = validation::normalizePhoneNumber(trimmedNumber);
                                        if (normalizedNumber == numbers[idx - 1].number ||
                                            phonebook.isPhoneNumberUnique(normalizedNumber, contactToEdit->getId()))
                                        {
                                            contactToEdit->editPhoneNumber(idx - 1, newType, normalizedNumber);
                                            std::cout << "Number successfully changed." << std::endl;
                                            break;
                                        }
                                        std::cout << "Error: this number is already in use." << std::endl;
                                    }
                                    else
                                    {
                                        std::cout << "Error: invalid number format." << std::endl;
                                    }
                                }
                            }
                            else
                            {
                                std::cout << "Edit cancelled or invalid input." << std::endl;
                            }
                            break;
                        }
                    case '9':
                        {
                            const auto& numbers = contactToEdit->getPhoneNumbers();
                            if (numbers.size() <= 1)
                            {
                                std::cout << "Cannot delete the only one phone number." << std::endl;
                                break;
                            }
                            std::cout << "Select a number to delete:" << std::endl;
                            for (size_t i = 0; i < numbers.size(); ++i)
                            {
                                std::cout << i + 1 << ". " << numbers[i].type << ": " << numbers[i].number << std::endl;
                            }
                            idx = getInput<size_t>("Your choice (0 to cancel): ");
                            if (idx > 0 && idx <= numbers.size())
                            {
                                if (contactToEdit->deletePhoneNumber(idx - 1))
                                {
                                    std::cout << "Number successfully deleted." << std::endl;
                                }
                            }
                            else
                            {
                                std::cout << "Deletion cancelled or invalid input." << std::endl;
                            }
                            break;
                        }
                    case '0':
                        editing = false;
                        std::cout << "Editing finished." << std::endl;
                        break;
                    default:
                        std::cout << "Invalid input." << std::endl;
                        break;
                    }
                }
            }
            std::cout << "Error: Invalid index number. Please try again." << std::endl;
        }
    }

    void sortContacts(Phonebook& phonebook)
    {
        std::vector<SortCriterion> criteria;

        while (true)
        {
            std::cout << "\n--- Sort Builder ---" << std::endl;
            if (!criteria.empty())
            {
                std::cout << "Levels already added: " << criteria.size() << std::endl;
            }
            std::cout << "Select a field for the next sort level:" << std::endl;
            std::cout << "1. ID" << std::endl;
            std::cout << "2. Surname" << std::endl;
            std::cout << "3. Forename" << std::endl;
            std::cout << "4. Patronymic" << std::endl;
            std::cout << "5. Address" << std::endl;
            std::cout << "6. Email" << std::endl;
            std::cout << "7. Birth Date" << std::endl;
            std::cout << "---------------------------------" << std::endl;
            std::cout << "0. Execute Sort" << std::endl;

            char choice = getInput<char>("Your choice: ");

            if (choice == '0')
            {
                break;
            }

            SortField field;
            switch (choice)
            {
            case '1':
                field = SortField::ID;
                break;
            case '2':
                field = SortField::SURNAME;
                break;
            case '3':
                field = SortField::FORENAME;
                break;
            case '4':
                field = SortField::PATRONYMIC;
                break;
            case '5':
                field = SortField::ADDRESS;
                break;
            case '6':
                field = SortField::EMAIL;
                break;
            case '7':
                field = SortField::BIRTH_DATE;
                break;
            default:
                std::cout << "Invalid choice." << std::endl;
                continue;
            }

            std::cout << "Select direction:" << std::endl;
            std::cout << "1. Ascending" << std::endl;
            std::cout << "2. Descending" << std::endl;

            choice = getInput<char>("Your choice: ");

            SortDirection direction;
            if (choice == '1')
            {
                direction = SortDirection::ASCENDING;
            }
            else if (choice == '2')
            {
                direction = SortDirection::DESCENDING;
            }
            else
            {
                std::cout << "Invalid choice." << std::endl;
                continue;
            }

            criteria.push_back({field, direction});
            std::cout << "Criterion added." << std::endl;
        }

        if (criteria.empty())
        {
            std::cout << "No criteria selected. Sort canceled." << std::endl;
            return;
        }

        phonebook.sortContacts(criteria);
        std::cout << "\nContacts sorted. Here is the new order:" << std::endl;
        phonebook.printAllContacts();
    }
}
