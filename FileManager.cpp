#include "FileManager.h"
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

std::vector<Contact> FileManager::loadFromFile(const std::string& filename)
{
    std::vector<Contact> contacts;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        return contacts;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        std::stringstream ss(line);
        std::string token;

        Contact contact;
        Date birthDate;

        std::getline(ss, token, ';');
        contact.setId(std::stoi(token));
        std::getline(ss, token, ';');
        contact.setSurname(token);
        std::getline(ss, token, ';');
        contact.setForename(token);
        std::getline(ss, token, ';');
        contact.setPatronymic(token);
        std::getline(ss, token, ';');
        contact.setAddress(token);

        std::getline(ss, token, ';');
        birthDate.day = token.empty() ? 0 : std::stoi(token);
        std::getline(ss, token, ';');
        birthDate.month = token.empty() ? 0 : std::stoi(token);
        std::getline(ss, token, ';');
        birthDate.year = token.empty() ? 0 : std::stoi(token);
        contact.setBirthDate(birthDate);

        std::getline(ss, token, ';');
        contact.setEmail(token);

        std::string phonesStr;
        std::getline(ss, phonesStr);
        if (!phonesStr.empty())
        {
            std::stringstream phones_ss(phonesStr);
            std::string phone_pair_str;
            while (std::getline(phones_ss, phone_pair_str, '|'))
            {
                size_t colon_pos = phone_pair_str.find(':');
                if (colon_pos != std::string::npos)
                {
                    std::string type = phone_pair_str.substr(0, colon_pos);
                    std::string number = phone_pair_str.substr(colon_pos + 1);
                    contact.addPhoneNumber(type, number);
                }
            }
        }

        contacts.push_back(contact);
    }

    file.close();
    return contacts;
}

bool FileManager::saveToFile(const std::string& filename, const std::vector<Contact>& contacts)
{
    std::ofstream file(filename);

    if (!file.is_open())
    {
        return false;
    }

    for (const Contact& contact : contacts)
    {
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
        const std::vector<PhoneNumber>& phoneNumbers = contact.getPhoneNumbers();
        for (size_t i = 0; i < phoneNumbers.size(); ++i)
        {
            phonesStream << phoneNumbers[i].type << ":" << phoneNumbers[i].number;
            if (i < phoneNumbers.size() - 1)
            {
                phonesStream << "|";
            }
        }

        file << phonesStream.str() << std::endl;
    }

    file.close();
    return true;
}