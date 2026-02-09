#include "validation.h"
#include <algorithm>
#include <cctype>
#include <ctime>
#include <regex>

namespace validation {

    std::string trim(const std::string& str) {
        const std::string whitespace = " \t\n\r\f\v";
        const size_t first = str.find_first_not_of(whitespace);
        if (first == std::string::npos) {
            return "";
        }
        const size_t last = str.find_last_not_of(whitespace);
        return str.substr(first, last - first + 1);
    }

    bool isValidName(const std::string& name) {
        const std::regex nameRegex("^[a-zA-Zа-яА-ЯёЁ]([a-zA-Zа-яА-ЯёЁ0-9- ]*[a-zA-Zа-яА-ЯёЁ0-9])?$");
        return std::regex_match(name, nameRegex);
    }

    bool isValidPhoneType(const std::string& type) {
        if (type.empty()) {
            return false;
        }

        const std::regex typeRegex("^[a-zA-Zа-яА-Я0-9]([a-zA-Zа-яА-Я0-9 ]*[a-zA-Zа-яА-Я0-9])?$");
        return std::regex_match(type, typeRegex);
    }

    bool isValidPhoneNumber(const std::string& phone) {
        const std::regex phoneRegex(R"(^(\+7|8) ?\(?\d{3}\)? ?\d{3}(-?\d{2}){2}$)");
        return std::regex_match(phone, phoneRegex);
    }

    bool isValidEmail(const std::string& email) {
        const std::regex emailRegex("^[a-zA-Z0-9_.-]+@[a-zA-Z0-9-]+\\.[a-zA-Z]{2,}$");
        return std::regex_match(email, emailRegex);
    }

    bool isValidDate(const int day, const int month, const int year) {
        if (year < 1900 || month < 1 || month > 12 || day < 1 || day > 31) {
            return false;
        }

        if (month == 4 || month == 6 || month == 9 || month == 11) {
            if (day > 30) return false;
        } else if (month == 2) {
            bool isLeap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
            if (isLeap) {
                if (day > 29) return false;
            } else {
                if (day > 28) return false;
            }
        }

        const time_t t = time(nullptr);
        const tm* now = localtime(&t);
        const int currentYear = now->tm_year + 1900;
        const int currentMonth = now->tm_mon + 1;
        const int currentDay = now->tm_mday;

        const long long inputDateAsNumber = year * 10000 + month * 100 + day;

        const long long currentDateAsNumber = currentYear * 10000 + currentMonth * 100 + currentDay;

        if (inputDateAsNumber > currentDateAsNumber) {
            return false;
        }

        return true;
    }

    bool isValidAddress(const std::string& address) {
        if (address.empty()) {
            return true;
        }

        const std::regex addressRegex(R"(^[a-zA-Zа-яА-ЯёЁ0-9\s\-,.#№()/\\_]+$)");

        return std::regex_match(address, addressRegex);
    }

    std::string normalizePhoneNumber(const std::string& phone) {
        std::string digits;
        std::copy_if(phone.begin(), phone.end(), std::back_inserter(digits),
                     [](char c){ return std::isdigit(c); });

        if (digits.length() == 11 && digits[0] == '8') {
            digits[0] = '7';
        }

        if (digits.length() == 11 && digits[0] == '7') {
            return "+7 (" + digits.substr(1, 3) + ") " + digits.substr(4, 3) +
                   "-" + digits.substr(7, 2) + "-" + digits.substr(9, 2);
        }

        return digits;
    }

    std::string normalizeEmail(const std::string& email) {
        std::string normalized = email;

        normalized.erase( std::remove_if(normalized.begin(), normalized.end(),
                       [](unsigned char c){ return std::isspace(c); }), normalized.end());

        std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return normalized;
    }
}
