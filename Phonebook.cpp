#include "Phonebook.h"
#include "validation.h"
#include <algorithm>
#include <cctype>
#include <iostream>

Phonebook::Phonebook() : nextId(1) {}

void Phonebook::initializeNextId() {
    if (contacts.empty()) {
        nextId = 1;
    } else {
        const auto maxIt = std::max_element(contacts.begin(), contacts.end(),
        [](const Contact& a, const Contact& b){ return a.getId() < b.getId(); });
        nextId = maxIt->getId() + 1;
    }
}

void Phonebook::addContact(Contact& contact) {
    contact.setId(nextId++);
    contacts.push_back(contact);
}

void Phonebook::addContactFromStorage(const Contact& contact) {
    contacts.push_back(contact);
}

bool Phonebook::deleteContact(int id) {
    const auto it = std::find_if(contacts.begin(), contacts.end(),
    [id](const Contact& c) { return c.getId() == id; });
    if (it != contacts.end()) {
        contacts.erase(it);
        return true;
    }
    return false;
}

Contact* Phonebook::findContact(int id) {
    const auto it = std::find_if(contacts.begin(), contacts.end(),
    [id](const Contact& c) { return c.getId() == id; });
    if (it != contacts.end()) {
        return &(*it);
    }
    return nullptr;
}

std::vector<Contact> Phonebook::searchContacts(const std::map<SearchField, std::string>& criteria) const {
    if (criteria.empty()) {
        return contacts;
    }

    std::vector<Contact> foundContacts;

    for (const Contact& contact : contacts) {
        bool matchesAllCriteria = true;

        for (const auto& criterion : criteria) {
            const SearchField field = criterion.first;
            std::string query = validation::trim(criterion.second);
            if (query.empty()) {
                continue;
            }
            bool currentCriterionMatch = false;

            if (field != SearchField::PHONE) {
                std::transform(query.begin(), query.end(), query.begin(),
                               [](unsigned char c) { return std::tolower(c); });
            }

            switch (field) {
                case SearchField::ID: {
                    try {
                         if (contact.getId() == std::stoi(query)) {
                             currentCriterionMatch = true;
                         }
                    } catch (const std::exception&) {}
                    break;
                }
                case SearchField::SURNAME: {
                    std::string surname = contact.getSurname();
                    std::transform(surname.begin(), surname.end(), surname.begin(),
                    [](unsigned char c) { return std::tolower(c); });
                    if (surname.find(query) != std::string::npos) {
                        currentCriterionMatch = true;
                    }
                    break;
                }
                case SearchField::FORENAME: {
                    std::string forename = contact.getForename();
                    std::transform(forename.begin(), forename.end(), forename.begin(),
                    [](unsigned char c) { return std::tolower(c); });
                    if (forename.find(query) != std::string::npos) {
                        currentCriterionMatch = true;
                    }
                    break;
                }
                case SearchField::PATRONYMIC: {
                    std::string patronymic = contact.getPatronymic();
                    std::transform(patronymic.begin(), patronymic.end(), patronymic.begin(),
                    [](unsigned char c) { return std::tolower(c); });
                    if (patronymic.find(query) != std::string::npos) {
                        currentCriterionMatch = true;
                    }
                    break;
                }
                case SearchField::ADDRESS: {
                    std::string address = contact.getAddress();
                    std::transform(address.begin(), address.end(), address.begin(),
                    [](unsigned char c) { return std::tolower(c); });
                    if (address.find(query) != std::string::npos) {
                        currentCriterionMatch = true;
                    }
                    break;
                }
                case SearchField::BIRTH_DAY: {
                    if (contact.getBirthDate().day != 0) {
                        try {
                            if (contact.getBirthDate().day == std::stoi(query)) {
                                currentCriterionMatch = true;
                            }
                        } catch (const std::exception&) {}
                    }
                    break;
                }
                case SearchField::BIRTH_MONTH: {
                    if (contact.getBirthDate().month != 0) {
                        try {
                            if (contact.getBirthDate().month == std::stoi(query)) {
                                currentCriterionMatch = true;
                            }
                        } catch (const std::exception&) {}
                    }
                    break;
                }
                case SearchField::BIRTH_YEAR: {
                    if (contact.getBirthDate().year != 0) {
                        try {
                            if (contact.getBirthDate().year == std::stoi(query)) {
                                currentCriterionMatch = true;
                            }
                        } catch (const std::exception&) {}
                    }
                    break;
                }
                case SearchField::EMAIL: {
                    std::string email = contact.getEmail();
                    if (email.find(query) != std::string::npos) {
                        currentCriterionMatch = true;
                    }
                    break;
                }
                case SearchField::PHONE: {
                    std::string queryDigits;
                    std::copy_if(query.begin(), query.end(), std::back_inserter(queryDigits),
                    [](char c){ return std::isdigit(c); });

                    if (!queryDigits.empty() && queryDigits.front() == '8') {
                        queryDigits[0] = '7';
                    }

                    if (queryDigits.empty()) {
                        currentCriterionMatch = false;
                        break;
                    }

                    for (const auto& phone : contact.getPhoneNumbers()) {
                        std::string storedDigits;
                        std::copy_if(phone.number.begin(), phone.number.end(), std::back_inserter(storedDigits),
                        [](char c){ return std::isdigit(c); });

                        if (storedDigits.find(queryDigits) != std::string::npos) {
                            currentCriterionMatch = true;
                            break;
                        }
                    }
                    break;
                }
            }
            if (!currentCriterionMatch) {
                matchesAllCriteria = false;
                break;
            }
        }
        if (matchesAllCriteria) {
            foundContacts.push_back(contact);
        }
    }
    return foundContacts;
}

void Phonebook::sortContacts(const std::vector<SortCriterion>& criteria) {
    if (criteria.empty()) {
        return;
    }

    std::stable_sort(contacts.begin(), contacts.end(),
              [&criteria](const Contact& a, const Contact& b) {
                  for (const auto& criterion : criteria) {
                      switch (criterion.field) {
                          case SortField::ID:
                              if (a.getId() != b.getId()) {
                                  return criterion.direction == SortDirection::ASCENDING
                                             ? a.getId() < b.getId()
                                             : a.getId() > b.getId();
                              }
                              break;
                          case SortField::SURNAME:
                              if (a.getSurname() != b.getSurname()) {
                                  return criterion.direction == SortDirection::ASCENDING
                                             ? a.getSurname() < b.getSurname()
                                             : a.getSurname() > b.getSurname();
                              }
                              break;
                          case SortField::FORENAME:
                              if (a.getForename() != b.getForename()) {
                                  return criterion.direction == SortDirection::ASCENDING
                                             ? a.getForename() < b.getForename()
                                             : a.getForename() > b.getForename();
                              }
                              break;
                          case SortField::PATRONYMIC:
                              if (a.getPatronymic() != b.getPatronymic()) {
                                  return criterion.direction == SortDirection::ASCENDING
                                             ? a.getPatronymic() < b.getPatronymic()
                                             : a.getPatronymic() > b.getPatronymic();
                              }
                              break;
                          case SortField::ADDRESS:
                              if (a.getAddress() != b.getAddress()) {
                                  return criterion.direction == SortDirection::ASCENDING
                                             ? a.getAddress() < b.getAddress()
                                             : a.getAddress() > b.getAddress();
                              }
                              break;
                          case SortField::BIRTH_DATE:
                              if (a.getBirthDate() < b.getBirthDate() || b.getBirthDate() < a.getBirthDate()) {
                                  return criterion.direction == SortDirection::ASCENDING
                                             ? a.getBirthDate() < b.getBirthDate()
                                             : b.getBirthDate() < a.getBirthDate();
                              }
                              break;
                          case SortField::EMAIL:
                              if (a.getEmail() != b.getEmail()) {
                                  return criterion.direction == SortDirection::ASCENDING
                                             ? a.getEmail() < b.getEmail()
                                             : a.getEmail() > b.getEmail();
                              }
                              break;
                      }
                  }
                  return false;
              });
}

const std::vector<Contact>& Phonebook::getAllContacts() const {
    return contacts;
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

std::vector<Contact> Phonebook::searchAllFields(const std::string& query) const {
    std::string trimmedQuery = validation::trim(query);
    if (trimmedQuery.empty()) {
        return contacts;
    }

    std::vector<Contact> result;

    std::string lowerQuery = trimmedQuery;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    std::string queryDigits;
    std::copy_if(trimmedQuery.begin(), trimmedQuery.end(), std::back_inserter(queryDigits),
                 [](char c){ return std::isdigit(c); });

    if (!queryDigits.empty() && queryDigits.front() == '8') {
        queryDigits[0] = '7';
    }

    for (const auto& contact : contacts) {
        bool match = false;

        if (std::to_string(contact.getId()).find(trimmedQuery) != std::string::npos) {
            match = true;
        }

        auto checkTextValue = [&](const std::string& value) {
            if (value.empty()) {
                return false;
            }

            std::string lowerValue = value;
            std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            return lowerValue.find(lowerQuery) != std::string::npos;
        };

        if (checkTextValue(contact.getSurname()) ||
            checkTextValue(contact.getForename()) ||
            checkTextValue(contact.getPatronymic()) ||
            checkTextValue(contact.getAddress()) ||
            checkTextValue(contact.getEmail())) {
            match = true;
        }

        if (!match && !queryDigits.empty()) {
            for (const auto& phone : contact.getPhoneNumbers()) {
                std::string storedDigits;
                std::copy_if(phone.number.begin(), phone.number.end(), std::back_inserter(storedDigits),
                             [](char c){ return std::isdigit(c); });

                if (storedDigits.find(queryDigits) != std::string::npos) {
                    match = true;
                    break;
                }
            }
        }

        const Date birthDate = contact.getBirthDate();
        std::string dateStr = std::to_string(birthDate.day) + "." + std::to_string(birthDate.month) +
            "." + std::to_string(birthDate.year);
        if (dateStr.find(trimmedQuery) != std::string::npos) {
            match = true;
        }

        if (match) {
            result.push_back(contact);
        }
    }
    return result;
}

void Phonebook::reorderContacts(const std::vector<int>& orderedIds) {
    std::vector<Contact> newOrder;

    for (const int id : orderedIds) {
        const Contact* ptr = findContact(id);
        if (ptr != nullptr) {
            newOrder.push_back(*ptr);
        }
    }

    contacts = newOrder;
}
