#include "InputValidation.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {

std::string trim(const std::string& value) {
    const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char character) {
        return std::isspace(character);
    });
    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char character) {
        return std::isspace(character);
    }).base();

    if (first >= last) {
        return "";
    }
    return std::string(first, last);
}

template <typename Number>
Number readNumber(const std::string& prompt, Number minimum, Number maximum) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            throw std::runtime_error("Input stream was closed.");
        }

        std::istringstream input(line);
        Number value{};
        char extraCharacter{};
        if ((input >> value) && !(input >> extraCharacter) && value >= minimum && value <= maximum) {
            return value;
        }

        std::cout << "Invalid input. Enter a value from " << minimum << " to " << maximum << ".\n";
    }
}

}  // namespace

int readInt(const std::string& prompt, int minimum, int maximum) {
    return readNumber<int>(prompt, minimum, maximum);
}

double readDouble(const std::string& prompt, double minimum, double maximum) {
    return readNumber<double>(prompt, minimum, maximum);
}

std::string readRequiredText(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string value;
        if (!std::getline(std::cin, value)) {
            throw std::runtime_error("Input stream was closed.");
        }

        value = trim(value);
        if (!value.empty()) {
            return value;
        }
        std::cout << "This value cannot be empty.\n";
    }
}

bool studentIdExists(const std::vector<Student>& students, int id) {
    return std::any_of(students.begin(), students.end(), [id](const Student& student) {
        return student.id == id;
    });
}
