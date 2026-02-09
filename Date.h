#pragma once

struct Date {
    int day;
    int month;
    int year;

    explicit Date(const int _day = 0, const int _month = 0, const int _year = 0)
        : day(_day), month(_month), year(_year) {}
};

inline bool operator<(const Date& left, const Date& right) {
    if (left.year != right.year) {
        return left.year < right.year;
    }
    if (left.month != right.month) {
        return left.month < right.month;
    }
    return left.day < right.day;
}
