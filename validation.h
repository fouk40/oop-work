#pragma once
#include <string>


namespace validation
{
    std::string trim(const std::string& str);

    bool isValidName(const std::string& name);

    bool isValidPhoneType(const std::string& type);

    bool isValidPhoneNumber(const std::string& phone);

    bool isValidEmail(const std::string& email);

    bool isValidDate(int day, int month, int year);

    bool isValidAddress(const std::string& address);

    std::string normalizePhoneNumber(const std::string& phone);

    std::string normalizeEmail(const std::string& email);
}
