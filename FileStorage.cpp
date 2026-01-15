#include "FileStorage.h"
#include "validation.h"
#include <fstream>
#include <set>
#include <sstream>

bool stringToInt(const std::string& str, int& outValue) {
    try {
        size_t pos;
        outValue = std::stoi(str, &pos);
        return pos == str.length();
    } catch (...) {
        return false;
    }
}

std::vector<Contact> FileStorage::load() {
    lastError.clear();
    std::vector<Contact> contacts;
    std::ifstream file(filename);

    if (!file.is_open()) {
        lastError = "Cannot open file '" + filename + "' for reading.";
        return contacts;
    }

    std::string line;
    int lineNum = 0;

    std::set<int> loadedIds;
    std::set<std::string> loadedEmails;
    std::set<std::string> loadedPhoneNumbers;

    while (std::getline(file, line)) {
        lineNum++;

        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> parts;

        while (std::getline(ss, token, ';')) {
            parts.push_back(token);
        }

        if (parts.size() < 10) {
            lastError = "Line " + std::to_string(lineNum) + ": not enough columns.";
            return {};
        }

        Contact contact;

        int id;
        if (!stringToInt(parts[0], id)) {
            lastError = "Line " + std::to_string(lineNum) + ": ID is not a number.";
            return {};
        }
        if (id <= 0) {
            lastError = "Line " + std::to_string(lineNum) + ": ID must be positive.";
            return {};
        }
        if (loadedIds.count(id)) {
            lastError = "Line " + std::to_string(lineNum) + ": duplicate ID found: " + std::to_string(id);
            return {};
        }
        loadedIds.insert(id);
        contact.setId(id);

        std::string surname = validation::trim(parts[1]);
        if (!validation::isValidName(surname)) {
            lastError = "Line " + std::to_string(lineNum) + ": invalid surname.";
            return {};
        }
        contact.setSurname(surname);

        std::string forename = validation::trim(parts[2]);
        if (!validation::isValidName(forename)) {
            lastError = "Line " + std::to_string(lineNum) + ": invalid forename.";
            return {};
        }
        contact.setForename(forename);

        std::string patronymic = validation::trim(parts[3]);
        if (!patronymic.empty() && !validation::isValidName(patronymic)) {
            lastError = "Line " + std::to_string(lineNum) + ": invalid patronymic.";
            return {};
        }
        contact.setPatronymic(patronymic);

        std::string address = validation::trim(parts[4]);
        if (!validation::isValidAddress(address)) {
            lastError = "Line " + std::to_string(lineNum) + ": invalid address.";
            return {};
        }
        contact.setAddress(address);

        int day, month, year;
        bool dateOk = true;
        dateOk &= stringToInt(parts[5], day);
        dateOk &= stringToInt(parts[6], month);
        dateOk &= stringToInt(parts[7], year);

        if (!dateOk) {
            lastError = "Line " + std::to_string(lineNum) + ": birth date must be valid numbers.";
            return {};
        }

        if (!(day == 0 && month == 0 && year == 0)) {
            if (!validation::isValidDate(day, month, year)) {
                lastError = "Line " + std::to_string(lineNum) + ": invalid birth date.";
                return {};
            }
        }
        contact.setBirthDate(Date(day, month, year));

        std::string email = validation::trim(parts[8]);
        if (!validation::isValidEmail(email)) {
            lastError = "Line " + std::to_string(lineNum) + ": invalid email.";
            return {};
        }
        if (!validation::isForenameInEmail(email, forename)) {
            lastError = "Line " + std::to_string(lineNum) + ": email does not contain forename.";
            return {};
        }

        std::string normalizedEmail = validation::normalizeEmail(email);
        if (loadedEmails.count(normalizedEmail)) {
            lastError = "Line " + std::to_string(lineNum) + ": duplicate email found.";
            return {};
        }
        loadedEmails.insert(normalizedEmail);
        contact.setEmail(normalizedEmail);

        std::stringstream phonesStream(parts[9]);
        std::string phonePairStr;
        while (std::getline(phonesStream, phonePairStr, '|')) {
            size_t colonPos = phonePairStr.find(':');
            if (colonPos != std::string::npos) {
                std::string type = validation::trim(phonePairStr.substr(0, colonPos));
                std::string number = validation::trim(phonePairStr.substr(colonPos + 1));

                if (!validation::isValidPhoneType(type)) {
                    lastError = "Line " + std::to_string(lineNum) + ": invalid phone type.";
                    return {};
                }
                if (!validation::isValidPhoneNumber(number)) {
                    lastError = "Line " + std::to_string(lineNum) + ": invalid phone number.";
                    return {};
                }

                std::string normalizedNumber = validation::normalizePhoneNumber(number);
                if (loadedPhoneNumbers.count(normalizedNumber)) {
                    lastError = "Line " + std::to_string(lineNum) + ": duplicate phone number found.";
                    return {};
                }
                loadedPhoneNumbers.insert(normalizedNumber);

                contact.addPhoneNumber(type, normalizedNumber);
            } else {
                 lastError = "Line " + std::to_string(lineNum) + ": malformed phone entry.";
                 return {};
            }
        }

        if (contact.getPhoneNumbers().empty()) {
            lastError = "Line " + std::to_string(lineNum) + ": contact must have at least one phone number.";
            return {};
        }

        contacts.push_back(contact);
    }

    file.close();
    return contacts;
}

bool FileStorage::save(const std::vector<Contact>& contacts) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    for (const Contact& contact : contacts) {
        file << contact.getId() << ";"
            << contact.getSurname() << ";"
            << contact.getForename() << ";"
            << contact.getPatronymic() << ";"
            << contact.getAddress() << ";"
            << contact.getBirthDate().day << ";"
            << contact.getBirthDate().month << ";"
            << contact.getBirthDate().year << ";"
            << contact.getEmail() << ";";

        std::stringstream phonesStream;
        const std::vector<PhoneNumber>& phones = contact.getPhoneNumbers();
        for (size_t i = 0; i < phones.size(); ++i) {
            phonesStream << phones[i].type << ":" << phones[i].number;
            if (i < phones.size() - 1) {
                phonesStream << "|";
            }
        }

        file << phonesStream.str() << std::endl;
    }

    file.close();
    return true;
}
