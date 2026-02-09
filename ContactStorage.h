#pragma once
#include "Contact.h"
#include <string>
#include <vector>

class ContactStorage {
public:
    virtual ~ContactStorage() = default;

    virtual std::vector<Contact> load() = 0;
    virtual bool save(const std::vector<Contact>& contacts) = 0;

    virtual std::string getLastError() const {
        return lastError;
    }

protected:
    std::string lastError;
};