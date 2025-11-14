#include "Phonebook.h"
#include "validation.h"
#include <algorithm>
#include <iostream>


Phonebook::Phonebook() : nextId(1) {}

void Phonebook::initializeNextId()
{
    if (contacts.empty())
    {
        nextId = 1;
    }
    else
    {
        const auto maxIt = std::max_element(contacts.begin(), contacts.end(),
        [](const Contact& a, const Contact& b){ return a.getId() < b.getId(); });
        nextId = maxIt->getId() + 1;
    }
}

void Phonebook::addContact(Contact& contact)
{
    contact.setId(nextId++);
    contacts.push_back(contact);
}

void Phonebook::addContactFromFile(const Contact& contact)
{
    contacts.push_back(contact);
}

bool Phonebook::deleteContact(int id)
{
    const auto it = std::find_if(contacts.begin(), contacts.end(),
    [id](const Contact& c) { return c.getId() == id; });
    if (it != contacts.end())
    {
        contacts.erase(it);
        return true;
    }
    return false;
}

Contact* Phonebook::findContactById(int id)
{
    const auto it = std::find_if(contacts.begin(), contacts.end(),
    [id](const Contact& c) { return c.getId() == id; });
    if (it != contacts.end())
    {
        return &(*it);
    }
    return nullptr;
}

std::vector<Contact> Phonebook::searchContacts(const std::map<SearchField, std::string>& criteria) const
{
    if (criteria.empty())
    {
        return contacts;
    }

    std::vector<Contact> foundContacts;

    for (const Contact& contact : contacts)
    {
        bool matchesAllCriteria = true;

        for (const auto& criterion : criteria)
        {
            const SearchField field = criterion.first;
            std::string query = criterion.second;
            bool currentCriterionMatch = false;

            std::string normalizedQuery = query;
            if (field != SearchField::PHONE)
            {
                std::transform(normalizedQuery.begin(), normalizedQuery.end(), normalizedQuery.begin(),
                               [](unsigned char c) { return std::tolower(c); });
            }
            else
            {
                normalizedQuery = validation::normalizePhoneNumber(query);
            }

            switch (field)
            {
            case SearchField::ID:
                {
                    try
                    {
                        if (contact.getId() == std::stoi(query))
                        {
                            currentCriterionMatch = true;
                        }
                    }
                    catch (const std::exception&) {}
                    break;
                }
            case SearchField::FORENAME:
                {
                    std::string value = contact.getForename();
                    std::transform(value.begin(), value.end(), value.begin(),
                        [](unsigned char c) { return std::tolower(c); });
                    if (value.find(normalizedQuery) != std::string::npos)
                    {
                        currentCriterionMatch = true;
                    }
                    break;
                }
            case SearchField::SURNAME:
                {
                    std::string value = contact.getSurname();
                    std::transform(value.begin(), value.end(), value.begin(),
                        [](unsigned char c) { return std::tolower(c); });
                    if (value.find(normalizedQuery) != std::string::npos)
                    {
                        currentCriterionMatch = true;
                    }
                    break;
                }
            case SearchField::PATRONYMIC:
                {
                    std::string value = contact.getPatronymic();
                    std::transform(value.begin(), value.end(), value.begin(),
                        [](unsigned char c) { return std::tolower(c); });
                    if (value.find(normalizedQuery) != std::string::npos)
                    {
                        currentCriterionMatch = true;
                    }
                    break;
                }
            case SearchField::ADDRESS:
                {
                    std::string value = contact.getAddress();
                    std::transform(value.begin(), value.end(), value.begin(),
                        [](unsigned char c) { return std::tolower(c); });
                    if (value.find(normalizedQuery) != std::string::npos)
                    {
                        currentCriterionMatch = true;
                    }
                    break;
                }
            case SearchField::EMAIL:
                {
                    std::string value = contact.getEmail();
                    if (value.find(normalizedQuery) != std::string::npos)
                    {
                        currentCriterionMatch = true;
                    }
                    break;
                }
            case SearchField::PHONE:
                {
                    for (const auto& phone : contact.getPhoneNumbers())
                    {
                        if (phone.number.find(normalizedQuery) != std::string::npos)
                        {
                            currentCriterionMatch = true;
                            break;
                        }
                    }
                    break;
                }
            case SearchField::BIRTH_DAY:
                {
                    if (contact.getBirthDate().day != 0)
                    {
                        try
                        {
                            if (contact.getBirthDate().day == std::stoi(query))
                            {
                                currentCriterionMatch = true;
                            }
                        }
                        catch (const std::exception&) {}
                    }
                    break;
                }
            case SearchField::BIRTH_MONTH:
                {
                    if (contact.getBirthDate().month != 0)
                    {
                        try
                        {
                            if (contact.getBirthDate().month == std::stoi(query))
                            {
                                currentCriterionMatch = true;
                            }
                        }
                        catch (const std::exception&) {}
                    }
                    break;
                }
            case SearchField::BIRTH_YEAR:
                {
                    if (contact.getBirthDate().year != 0)
                    {
                        try
                        {
                            if (contact.getBirthDate().year == std::stoi(query))
                            {
                                currentCriterionMatch = true;
                            }
                        }
                        catch (const std::exception&) {}
                    }
                    break;
                }
            }
            if (!currentCriterionMatch)
            {
                matchesAllCriteria = false;
                break;
            }
        }
        if (matchesAllCriteria)
        {
            foundContacts.push_back(contact);
        }
    }
    return foundContacts;
}

void Phonebook::sortContacts(const std::vector<SortCriterion>& criteria)
{
    if (criteria.empty())
    {
        return;
    }

    std::sort(contacts.begin(), contacts.end(),
              [&criteria](const Contact& a, const Contact& b)
              {
                  for (const auto& criterion : criteria)
                  {
                      switch (criterion.field)
                      {
                      case SortField::ID:
                          if (a.getId() != b.getId())
                          {
                              return criterion.direction == SortDirection::ASCENDING
                                         ? a.getId() < b.getId()
                                         : a.getId() > b.getId();
                          }
                          break;
                      case SortField::SURNAME:
                          if (a.getSurname() != b.getSurname())
                          {
                              return criterion.direction == SortDirection::ASCENDING
                                         ? a.getSurname() < b.getSurname()
                                         : a.getSurname() > b.getSurname();
                          }
                          break;
                      case SortField::FORENAME:
                          if (a.getForename() != b.getForename())
                          {
                              return criterion.direction == SortDirection::ASCENDING
                                         ? a.getForename() < b.getForename()
                                         : a.getForename() > b.getForename();
                          }
                          break;
                      case SortField::PATRONYMIC:
                          if (a.getPatronymic() != b.getPatronymic())
                          {
                              return criterion.direction == SortDirection::ASCENDING
                                         ? a.getPatronymic() < b.getPatronymic()
                                         : a.getPatronymic() > b.getPatronymic();
                          }
                          break;
                      case SortField::ADDRESS:
                          if (a.getAddress() != b.getAddress())
                          {
                              return criterion.direction == SortDirection::ASCENDING
                                         ? a.getAddress() < b.getAddress()
                                         : a.getAddress() > b.getAddress();
                          }
                          break;
                      case SortField::EMAIL:
                          if (a.getEmail() != b.getEmail())
                          {
                              return criterion.direction == SortDirection::ASCENDING
                                         ? a.getEmail() < b.getEmail()
                                         : a.getEmail() > b.getEmail();
                          }
                          break;
                      case SortField::BIRTH_DATE:
                          if (a.getBirthDate() < b.getBirthDate() || b.getBirthDate() < a.getBirthDate())
                          {
                              return criterion.direction == SortDirection::ASCENDING
                                         ? a.getBirthDate() < b.getBirthDate()
                                         : b.getBirthDate() < a.getBirthDate();
                          }
                          break;
                      }
                  }
                  return false;
              });
}

const std::vector<Contact>& Phonebook::getAllContacts() const
{
    return contacts;
}

void Phonebook::printAllContacts() const
{
    if (contacts.empty())
    {
        std::cout << "Phonebook is empty." << std::endl;
        return;
    }

    std::cout << "--- ALL CONTACTS ---" << std::endl;
    for (const Contact& contact : contacts)
    {
        contact.print();
        std::cout << "--------------------" << std::endl;
    }
}

bool Phonebook::isEmailUnique(const std::string& email, const int ignoreId) const {
    const std::string normalizedEmail = validation::normalizeEmail(email);

    const auto it = std::find_if(contacts.begin(), contacts.end(),
        [&](const Contact& contact) {
            return contact.getId() != ignoreId && contact.getEmail() == normalizedEmail;
        });

    return it == contacts.end();
}

bool Phonebook::isPhoneNumberUnique(const std::string& number, const int ignoreId) const {
    const std::string normalizedNumber = validation::normalizePhoneNumber(number);

    for (const Contact& contact : contacts) {
        if (contact.getId() == ignoreId) {
            continue;
        }
        for (const PhoneNumber& phone : contact.getPhoneNumbers()) {
            if (phone.number == normalizedNumber) {
                return false;
            }
        }
    }
    return true;
}
