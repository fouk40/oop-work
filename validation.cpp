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
        const std::regex nameRegex("^[a-zA-Z]([a-zA-Z0-9- ]*[a-zA-Z0-9])?$");
        return std::regex_match(name, nameRegex);
    }

    bool isValidAddress(const std::string& address) {
        return true;
    }

    bool isValidDate(const int day, const int month, const int year) {
        if (year < 1900 || month < 1 || month > 12 || day < 1 || day > 31) {
            return false;
        }

        if (month == 4 || month == 6 || month == 9 || month == 11) {
            if (day > 30) {
                return false;
            }
        } else if (month == 2) {
            const bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
            if (isLeapYear) {
                if (day > 29) {
                    return false;
                }
            } else {
                if (day > 28) {
                    return false;
                }
            }
        }

        const time_t t = time(nullptr);
        const tm* now = localtime(&t);
        const int currentYear = now->tm_year + 1900;
        const int currentMonth = now->tm_mon + 1;
        const int currentDay = now->tm_mday;

        const int inputDateAsNumber = year * 10000 + month * 100 + day;

        const int currentDateAsNumber = currentYear * 10000 + currentMonth * 100 + currentDay;

        if (inputDateAsNumber > currentDateAsNumber) {
            return false;
        }

        return true;
    }

    bool isValidEmail(const std::string& email) {
        const std::regex emailRegex("^[a-zA-Z0-9_.-]+@[a-zA-Z0-9-]+\\.[a-zA-Z]{2,}$");
        return std::regex_match(email, emailRegex);
    }

    bool isValidPhoneType(const std::string& type) {
        return !type.empty();
    }

    bool isValidPhoneNumber(const std::string& phone) {
        const std::regex phoneRegex(R"(^(\+7|8) ?\(?\d{3}\)? ?\d{3}(-?\d{2}){2}$)");
        return std::regex_match(phone, phoneRegex);
    }

    std::string normalizeEmail(const std::string& email) {
        std::string normalizedEmail = email;

        normalizedEmail.erase( std::remove_if(normalizedEmail.begin(), normalizedEmail.end(),
                                         [](unsigned char c){ return std::isspace(c); }), normalizedEmail.end());

        std::transform(normalizedEmail.begin(), normalizedEmail.end(), normalizedEmail.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return normalizedEmail;
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

    bool isForenameInEmail(const std::string& email, const std::string& forename) {
        std::string normalizedEmail = normalizeEmail(email);
        std::string lowerName = forename;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                       [](unsigned char c){ return std::tolower(c); });

        const size_t atPos = normalizedEmail.find('@');
        if (atPos == std::string::npos) {
            return false;
        }
        const std::string localPart = normalizedEmail.substr(0, atPos);

        return localPart.find(lowerName) != std::string::npos;
    }
}
