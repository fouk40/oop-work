#pragma once
#include "ContactStorage.h"
#include "iostream"
#include <string>
#include <utility>

class FileStorage : public ContactStorage {
    std::string filename;
public:
    explicit FileStorage(std::string filename) : filename(std::move(filename)) {}

    std::vector<Contact> load() override;
    bool save(const std::vector<Contact>& contacts) override;
};
