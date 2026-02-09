#pragma once
#include "Contact.h"
#include <string>
#include <vector>

class FileManager
{
public:
    static std::vector<Contact> loadFromFile(const std::string& filename);
    static bool saveToFile(const std::string& filename, const std::vector<Contact>& contacts);
};
