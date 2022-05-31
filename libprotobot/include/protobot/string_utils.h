#ifndef PROTOBOT_STRING_UTILS_H
#define PROTOBOT_STRING_UTILS_H

#include <string>
#include <iomanip>
#include <locale>
#include <algorithm>

/**
 * Convert a string to lowercase using tolower()
 */
template <typename T> std::basic_string<T> toLower(const std::basic_string<T>& s)
{
    std::basic_string<T> s2 = s;
    std::transform(s2.begin(), s2.end(), s2.begin(), tolower);
    return std::move(s2);
}

/**
 * Convert a string to uppercase using toupper()
 */
template <typename T> std::basic_string<T> toUpper(const std::basic_string<T>& s)
{
    std::basic_string<T> s2 = s;
    std::transform(s2.begin(), s2.end(), s2.begin(), toupper);
    return std::move(s2);
}

/* Simple search and replace, case sensitive */
std::string replaceString(std::string inputString, const std::string& searchString, const std::string& replaceString) {
    size_t pos = 0;

    std::string lowerInputString = toLower(inputString);
    std::string lowerSearchString = toLower(searchString);
    std::string lowerReplaceString = toLower(replaceString);

    while((pos = lowerInputString.find(lowerSearchString, pos)) != std::string::npos) {

        inputString.replace(pos, searchString.length(), replaceString);
        lowerInputString.replace(pos, lowerSearchString.length(), lowerReplaceString);

        pos += replaceString.length();
    }
    return inputString;
}


/**
 *  trim from end of string (right)
 */
inline std::string rtrim(std::string s)
{
    s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
    return s;
}

/**
 * trim from beginning of string (left)
 */
inline std::string ltrim(std::string s)
{
    s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
    return s;
}

/**
 * trim from both ends of string (right then left)
 */
inline std::string trim(std::string s)
{
    return ltrim(rtrim(s));
}

/**
 * Convert any value from a string to another type using stringstream.
 * The optional second parameter indicates the format of the input string,
 * e.g. std::dec for decimal, std::hex for hex, std::oct for octal.
 */
template <typename T> T fromString(const std::string &s, std::ios_base & (*f)(std::ios_base&))
{
    T t;
    std::istringstream iss(s);
    iss >> f, iss >> t;
    return t;
}

#endif //PROTOBOT_STRING_UTILS_H
