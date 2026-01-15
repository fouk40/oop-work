#include "cli.h"
#include "validation.h"
#include <iomanip>
#include <map>

namespace cli {
    void displayMenu() {
        std::cout << "\n--- Phonebook ---" << std::endl;
        std::cout << "1. Show all contacts" << std::endl;
        std::cout << "2. Find contacts" << std::endl;
        std::cout << "3. Add contact" << std::endl;
        std::cout << "4. Edit contact" << std::endl;
        std::cout << "5. Delete contact" << std::endl;
        std::cout << "6. Sort contacts" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "-----------------------------" << std::endl;
    }

    void printContact(const Contact& contact) {
        std::cout << "ID: " << contact.getId() << std::endl;
        std::cout << "Surname: " << contact.getSurname() << std::endl;
        std::cout << "Forename: " << contact.getForename() << std::endl;

        const std::string patronymic = contact.getPatronymic();
        std::cout << "Patronymic: " << (patronymic.empty() ? "(not specified)" : patronymic) << std::endl;

        const std::string address = contact.getAddress();
        std::cout << "Address: " << (address.empty() ? "(not specified)" : address) << std::endl;

        std::cout << "Birth date: ";
        const Date birthDate = contact.getBirthDate();
        if (birthDate.day == 0) {
            std::cout << "(not specified)" << std::endl;
        } else {
            std::cout << std::setfill('0') << std::setw(2) << birthDate.day << "."
                      << std::setfill('0') << std::setw(2) << birthDate.month << "."
                      << birthDate.year << std::endl;
        }

        std::cout << "Email: " << contact.getEmail() << std::endl;

        std::cout << "Phone numbers:" << std::endl;
        for (const PhoneNumber& phone : contact.getPhoneNumbers()) {
            std::cout << "  " << phone.type << ": " << phone.number << std::endl;
        }
        std::cout << "--------------------" << std::endl;
    }

    void printAllContacts(const Phonebook& phonebook) {
        const std::vector<Contact>& contacts = phonebook.getAllContacts();
        if (contacts.empty()) {
            std::cout << "\nPhonebook is empty." << std::endl;
            return;
        }
        std::cout << "\n--- All contacts ---" << std::endl;
        for (const Contact& contact : contacts) {
            printContact(contact);
        }
    }

    std::vector<Contact> searchContacts(const Phonebook& phonebook) {
        if (phonebook.getAllContacts().empty()) {
            std::cout << "\nPhonebook is empty." << std::endl;
            return {};
        }

        std::map<SearchField, std::string> criteria;
        std::string query;

        std::cout << "\n--- Advanced search ---" << std::endl;
        std::cout << "Leave a field empty to not use it for searching." << std::endl;

        std::cout << "ID: ";
        std::getline(std::cin, query);
        if (!query.empty()) {
            criteria.insert({SearchField::ID, query});
        }

        std::cout << "Surname: ";
        std::getline(std::cin, query);
        if (!query.empty()) {
            criteria.insert({SearchField::SURNAME, query});
        }

        std::cout << "Forename: ";
        std::getline(std::cin, query);
        if (!query.empty()) {
            criteria.insert({SearchField::FORENAME, query});
        }

        std::cout << "Patronymic: ";
        std::getline(std::cin, query);
        if (!query.empty()) {
            criteria.insert({SearchField::PATRONYMIC, query});
        }

        std::cout << "Address: ";
        std::getline(std::cin, query);
        if (!query.empty()) {
            criteria.insert({SearchField::ADDRESS, query});
        }

        std::cout << "Day of birth: ";
        std::getline(std::cin, query);
        if (!query.empty()) {
            criteria.insert({SearchField::BIRTH_DAY, query});
        }

        std::cout << "Month of birth: ";
        std::getline(std::cin, query);
        if (!query.empty()) {
            criteria.insert({SearchField::BIRTH_MONTH, query});
        }

        std::cout << "Year of birth: ";
        std::getline(std::cin, query);
        if (!query.empty()) {
            criteria.insert({SearchField::BIRTH_YEAR, query});
        }

        std::cout << "Email: ";
        std::getline(std::cin, query);
        if (!query.empty()) {
            criteria.insert({SearchField::EMAIL, query});
        }

        std::cout << "Phone number: ";
        std::getline(std::cin, query);
        if (!query.empty()) {
            criteria.insert({SearchField::PHONE, query});
        }

        std::vector<Contact> foundContacts = phonebook.searchContacts(criteria);

        if (foundContacts.empty()) {
            std::cout << "\nNothing found for the specified criteria." << std::endl;
        } else {
            std::cout << "\n--- Search results (" << foundContacts.size() << ") ---" << std::endl;
            for (size_t i = 0; i < foundContacts.size(); ++i) {
                std::cout << ">> Index number: " << i + 1 << " <<" << std::endl;
                printContact(foundContacts[i]);
            }
        }
        return foundContacts;
    }

    std::string getNameInput(const std::string& prompt, const bool isMandatory) {
        std::string input;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, input);
            std::string name = validation::trim(input);

            if (!isMandatory && name.empty()) {
                return "";
            }

            if (validation::isValidName(name)) {
                return name;
            }
            std::cout << "Invalid name." << std::endl;
        }
    }

    std::string getAddressInput(const std::string& prompt) {
        std::string input;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, input);
            std::string address = validation::trim(input);

            if (validation::isValidAddress(address)) {
                return address;
            }
            std::cout << "Invalid address." << std::endl;
        }
    }

    Date getBirthDateInput(const std::string& prompt) {
        while (true) {
            const int day = getInput<int>(prompt);

            if (day == 0) {
                return Date();
            }

            const int month = getInput<int>("Enter month of birth: ");
            const int year = getInput<int>("Enter year of birth: ");

            if (validation::isValidDate(day, month, year)) {
                return Date(day, month, year);
            }
            std::cout << "Invalid date." << std::endl;
        }
    }

    std::string getEmailInput(const std::string& forename, const Phonebook& phonebook,
                              const int ignoreId)
    {
        std::string input;
        while (true) {
            std::cout << "Enter email (must contain the forename '" << forename << "'): ";
            std::getline(std::cin, input);
            std::string email = validation::normalizeEmail(input);

            if (!validation::isValidEmail(email)) {
                std::cout << "Invalid email." << std::endl;
                continue;
            }

            if (!validation::isForenameInEmail(email, forename)) {
                std::cout << "Email must contain the forename '" << forename << "'." << std::endl;
                continue;
            }

            if (phonebook.isEmailUnique(email, ignoreId)) {
                return email;
            }
            std::cout << "This email is already in use." << std::endl;
        }
    }

    std::string getPhoneTypeInput() {
        std::string input;
        while (true) {
            std::cout << "Enter phone type: ";
            std::getline(std::cin, input);
            std::string type = validation::trim(input);
            if (validation::isValidPhoneType(type)) {
                return type;
            }
            std::cout << "Invalid phone type." << std::endl;
        }
    }

    std::string getPhoneNumberInput(const Phonebook& phonebook, const Contact* currentContact, const int editIdx) {
        std::string input;
        const int contactId = currentContact ? currentContact->getId() : 0;

        while (true) {
            std::cout << "Enter phone number: ";
            std::getline(std::cin, input);
            std::string number = validation::trim(input);

            if (!validation::isValidPhoneNumber(number)) {
                std::cout << "Invalid phone number." << std::endl;
                continue;
            }

            if (!phonebook.isPhoneNumberUnique(number, contactId)) {
                std::cout << "This phone number is already in use by other contact." << std::endl;
                continue;
            }

            if (currentContact) {
                bool localDuplicate = false;
                std::string normalizedNumber = validation::normalizePhoneNumber(number);

                const auto& numbers = currentContact->getPhoneNumbers();
                for (size_t i = 0; i < numbers.size(); ++i) {
                    if (editIdx != -1 && static_cast<int>(i) == editIdx) {
                        continue;
                    }

                    if (numbers[i].number == normalizedNumber) {
                        localDuplicate = true;
                        break;
                    }
                }

                if (localDuplicate) {
                    std::cout << "Duplicate phone number inside this contact." << std::endl;
                    continue;
                }
            }

            return number;
        }
    }

    void addContact(Phonebook& phonebook) {
        Contact newContact;

        std::cout << "\n--- Adding contact ---" << std::endl;

        newContact.setSurname(getNameInput("Enter surname: ", true));
        newContact.setForename(getNameInput("Enter forename: ", true));
        newContact.setPatronymic(getNameInput("Enter patronymic (or press Enter to skip): ", false));
        newContact.setAddress(getAddressInput("Enter address (or press Enter to skip): "));
        newContact.setBirthDate(getBirthDateInput("Enter day of birth (or 0 to skip): "));
        newContact.setEmail(getEmailInput(newContact.getForename(), phonebook));

        std::string type = getPhoneTypeInput();
        std::string number = getPhoneNumberInput(phonebook, &newContact);
        newContact.addPhoneNumber(type, number);
        std::cout << "First phone number added." << std::endl;

        while (true) {
            const char choice = getInput<char>("\nDo you want to add another phone number? (Y/n): ");
            if (choice == 'N' || choice == 'n') {
                break;
            }
            if (choice == 'Y' || choice == 'y') {
                type = getPhoneTypeInput();
                number = getPhoneNumberInput(phonebook, &newContact);
                newContact.addPhoneNumber(type, number);
                std::cout << "Another phone number added." << std::endl;
            }
        }

        phonebook.addContact(newContact);
        std::cout << "\nContact added." << std::endl;
    }

    void editContact(Phonebook& phonebook) {
        if (phonebook.getAllContacts().empty()) {
            std::cout << "\nPhonebook is empty." << std::endl;
            return;
        }

        std::cout << "\n--- Editing contact ---" << std::endl;
        std::cout << "First, find the contact you want to edit." << std::endl;

        const std::vector<Contact> foundContacts = searchContacts(phonebook);

        if (foundContacts.empty()) {
            return;
        }

        while (true) {
            const auto idx = getInput<size_t>("\nEnter the index number of "
                                              "the contact to edit (0 to cancel): ");

            if (idx == 0) {
                std::cout << "Editing canceled." << std::endl;
                return;
            }

            if (idx > 0 && idx <= foundContacts.size()) {
                const int idToEdit = foundContacts[idx - 1].getId();
                Contact* contactToEdit = phonebook.findContact(idToEdit);

                while (true) {
                    std::cout << "\n--- Editing contact ---" << std::endl;
                    printContact(*contactToEdit);
                    std::cout << " 1. Edit surname" << std::endl;
                    std::cout << " 2. Edit forename" << std::endl;
                    std::cout << " 3. Edit patronymic" << std::endl;
                    std::cout << " 4. Edit address" << std::endl;
                    std::cout << " 5. Edit birth date" << std::endl;
                    std::cout << " 6. Edit email" << std::endl;
                    std::cout << " 7. Add phone number" << std::endl;
                    std::cout << " 8. Edit phone number" << std::endl;
                    std::cout << " 9. Delete phone number" << std::endl;
                    std::cout << " 0. Finish Editing" << std::endl;
                    std::cout << "---------------------------------" << std::endl;

                    const char choice = getInput<char>("Your choice: ");

                    switch (choice) {
                        case '1': {
                            contactToEdit->setSurname(getNameInput("Enter surname: ", true));
                            std::cout << "Surname updated." << std::endl;
                            break;
                        }
                        case '2': {
                            std::string name = getNameInput("Enter forename: ", true);
                            if (!validation::isForenameInEmail(contactToEdit->getEmail(), name)) {
                                std::cout << "Current email does not match the forename '" << name << "'." << std::endl;
                                std::cout << "You must update the email now." << std::endl;

                                std::string email = getEmailInput(name, phonebook, contactToEdit->getId());
                                contactToEdit->setEmail(email);
                                std::cout << "Email updated." << std::endl;
                            }
                            contactToEdit->setForename(name);
                            std::cout << "Forename updated." << std::endl;
                            break;
                        }
                        case '3': {
                            contactToEdit->setPatronymic(getNameInput("Enter patronymic: ", false));
                            std::cout << "Patronymic updated." << std::endl;
                            break;
                        }
                        case '4': {
                            contactToEdit->setAddress(getAddressInput("Enter address (or press Enter "
                                                                      "to clear): "));
                            std::cout << "Address updated." << std::endl;
                            break;
                        }
                        case '5': {
                            contactToEdit->setBirthDate(getBirthDateInput("Enter birth day (or 0 "
                                                                     "to clear): "));
                            std::cout << "Birth date updated." << std::endl;
                            break;
                        }
                        case '6': {
                            contactToEdit->setEmail(
                                getEmailInput(contactToEdit->getForename(), phonebook, contactToEdit->getId()));
                            std::cout << "Email updated." << std::endl;
                            break;
                        }
                        case '7': {
                            std::string type = getPhoneTypeInput();
                            std::string number = getPhoneNumberInput(phonebook, contactToEdit);
                            contactToEdit->addPhoneNumber(type, number);
                            std::cout << "Phone number added." << std::endl;
                            break;
                        }
                        case '8': {
                            const std::vector<PhoneNumber>& numbers = contactToEdit->getPhoneNumbers();

                            for (size_t i = 0; i < numbers.size(); ++i) {
                                std::cout << i + 1 << ". " << numbers[i].type << ": " << numbers[i].number << std::endl;
                            }

                            const auto pIdx = getInput<size_t>("Select a phone number "
                                                               "to edit (0 to cancel): ");
                            if (pIdx == 0) {
                                std::cout << "Editing cancelled.";
                                break;
                            }

                            if (pIdx > 0 && pIdx <= numbers.size()) {
                                std::string type = getPhoneTypeInput();
                                std::string number = getPhoneNumberInput(
                                    phonebook, contactToEdit, static_cast<int>(pIdx - 1));
                                contactToEdit->editPhoneNumber(pIdx - 1, type, number);
                                std::cout << "Phone number updated." << std::endl;
                                break;
                            }
                            std::cout << "Invalid input." << std::endl;
                            break;
                        }
                        case '9': {
                            const std::vector<PhoneNumber>& numbers = contactToEdit->getPhoneNumbers();

                            if (numbers.size() == 1) {
                                std::cout << "Cannot delete the only phone number." << std::endl;
                                break;
                            }

                            for (size_t i = 0; i < numbers.size(); ++i) {
                                std::cout << i + 1 << ". " << numbers[i].type << ": " << numbers[i].number << std::endl;
                            }

                            const auto pIdx = getInput<size_t>("Select a number "
                                                               "to delete (0 to cancel): ");
                            if (pIdx == 0) {
                                std::cout << "Deletion cancelled." << std::endl;
                                break;
                            }

                            if (pIdx > 0 && pIdx <= numbers.size()) {
                                contactToEdit->deletePhoneNumber(pIdx - 1);
                                std::cout << "Phone number deleted." << std::endl;
                                break;
                            }
                            std::cout << "Invalid input." << std::endl;
                            break;
                        }
                        case '0': {
                            std::cout << "Editing finished." << std::endl;
                            return;
                        }
                        default: {
                            std::cout << "Invalid input." << std::endl;
                            break;
                        }
                    }
                }
            }
            std::cout << "Invalid index number." << std::endl;
        }
    }

    void deleteContact(Phonebook& phonebook) {
        if (phonebook.getAllContacts().empty()) {
            std::cout << "\nPhonebook is empty." << std::endl;
            return;
        }

        std::cout << "\n--- Deleting contact ---" << std::endl;
        std::cout << "First, find the contact you want to delete." << std::endl;

        const std::vector<Contact> foundContacts = searchContacts(phonebook);

        if (foundContacts.empty()) {
            return;
        }

        while (true) {
            const auto choice = getInput<size_t>("\nEnter the index number of "
                                                 "the contact to delete (0 to cancel): ");

            if (choice == 0) {
                std::cout << "Deletion canceled." << std::endl;
                return;
            }

            if (choice > 0 && choice <= foundContacts.size()) {
                const int idToDelete = foundContacts[choice - 1].getId();
                phonebook.deleteContact(idToDelete);
                std::cout << "Contact with ID " << idToDelete << " deleted." << std::endl;
                return;
            }
            std::cout << "Invalid index number." << std::endl;
        }
    }

    void sortContacts(Phonebook& phonebook) {
        std::vector<SortCriterion> criteria;

        while (true) {
            std::cout << "\n--- Sort builder ---" << std::endl;
            if (!criteria.empty()) {
                std::cout << "Current sort levels:" << std::endl;
                for (size_t i = 0; i < criteria.size(); ++i) {
                    std::string fieldName;
                    switch (criteria[i].field) {
                        case SortField::ID:
                            fieldName = "ID";
                            break;
                        case SortField::SURNAME:
                            fieldName = "Surname";
                            break;
                        case SortField::FORENAME:
                            fieldName = "Forename";
                            break;
                        case SortField::PATRONYMIC:
                            fieldName = "Patronymic";
                            break;
                        case SortField::ADDRESS:
                            fieldName = "Address";
                            break;
                        case SortField::BIRTH_DATE:
                            fieldName = "Birth date";
                            break;
                        case SortField::EMAIL:
                            fieldName = "Email";
                            break;
                    }

                    std::string directionName = criteria[i].direction == SortDirection::ASCENDING
                                                    ? "Ascending"
                                                    : "Descending";

                    std::cout << "  " << (i + 1) << ". " << fieldName << " (" << directionName << ")" << std::endl;
                }
                std::cout << "---------------------------------" << std::endl;
            }

            std::cout << "Select a field for the next sort level:" << std::endl;
            std::cout << "1. ID" << std::endl;
            std::cout << "2. Surname" << std::endl;
            std::cout << "3. Forename" << std::endl;
            std::cout << "4. Patronymic" << std::endl;
            std::cout << "5. Address" << std::endl;
            std::cout << "6. Birth date" << std::endl;
            std::cout << "7. Email" << std::endl;
            std::cout << "---------------------------------" << std::endl;
            std::cout << "0. Execute sort" << std::endl;

            char choice = getInput<char>("Your choice: ");

            if (choice == '0') {
                break;
            }

            SortField field;
            switch (choice) {
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
                    field = SortField::BIRTH_DATE;
                    break;
                case '7':
                    field = SortField::EMAIL;
                    break;
                default:
                    std::cout << "Invalid input." << std::endl;
                    continue;
            }

            std::cout << "Select direction:" << std::endl;
            std::cout << "1. Ascending" << std::endl;
            std::cout << "2. Descending" << std::endl;

            choice = getInput<char>("Your choice: ");

            SortDirection direction;
            if (choice == '1') {
                direction = SortDirection::ASCENDING;
            } else if (choice == '2') {
                direction = SortDirection::DESCENDING;
            } else {
                std::cout << "Invalid input." << std::endl;
                continue;
            }

            criteria.push_back({field, direction});
            std::cout << "Criterion added." << std::endl;
        }

        if (criteria.empty()) {
            std::cout << "No criteria selected. Sort canceled." << std::endl;
            return;
        }

        phonebook.sortContacts(criteria);
        std::cout << "\nContacts sorted. Here is the new order:" << std::endl;
        printAllContacts(phonebook);
    }
}
