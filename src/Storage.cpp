#include "Storage.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_set>

namespace {

std::string escapeCsv(const std::string& value) {
    if (value.find_first_of(",\"") == std::string::npos) {
        return value;
    }

    std::string escaped = "\"";
    for (const char character : value) {
        if (character == '"') {
            escaped += "\"\"";
        } else {
            escaped += character;
        }
    }
    escaped += '"';
    return escaped;
}

bool parseCsvRow(const std::string& line, std::vector<std::string>& fields) {
    fields.clear();
    std::string field;
    bool insideQuotes = false;

    for (std::size_t index = 0; index < line.size(); ++index) {
        const char character = line[index];
        if (character == '"') {
            if (insideQuotes && index + 1 < line.size() && line[index + 1] == '"') {
                field += '"';
                ++index;
            } else {
                insideQuotes = !insideQuotes;
            }
        } else if (character == ',' && !insideQuotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += character;
        }
    }

    if (insideQuotes) {
        return false;
    }

    fields.push_back(field);
    return true;
}

bool parseIntStrictly(const std::string& text, int& value) {
    try {
        std::size_t usedCharacters = 0;
        value = std::stoi(text, &usedCharacters);
        return usedCharacters == text.size();
    } catch (...) {
        return false;
    }
}

bool parseGradeStrictly(const std::string& text, double& value) {
    try {
        std::size_t usedCharacters = 0;
        value = std::stod(text, &usedCharacters);
        return usedCharacters == text.size() && value >= 0.0 && value <= 100.0;
    } catch (...) {
        return false;
    }
}

}  // namespace

bool saveStudents(const std::vector<Student>& students,
                  const std::string& filename,
                  std::string& errorMessage) {
    const std::filesystem::path destination(filename);
    const std::filesystem::path temporaryFile = destination.string() + ".tmp";

    std::ofstream output(temporaryFile);
    if (!output) {
        errorMessage = "Could not open the temporary data file for writing.";
        return false;
    }

    output << "id,name,grades\n";
    output << std::fixed << std::setprecision(2);
    for (const Student& student : students) {
        output << student.id << ',' << escapeCsv(student.name);
        for (const double grade : student.grades) {
            output << ',' << grade;
        }
        output << '\n';
    }

    output.close();
    if (!output) {
        errorMessage = "An error occurred while writing the student records.";
        std::error_code ignored;
        std::filesystem::remove(temporaryFile, ignored);
        return false;
    }

    std::error_code fileError;
    std::filesystem::remove(destination, fileError);
    fileError.clear();
    std::filesystem::rename(temporaryFile, destination, fileError);
    if (fileError) {
        errorMessage = "Could not replace the data file: " + fileError.message();
        std::error_code ignored;
        std::filesystem::remove(temporaryFile, ignored);
        return false;
    }

    errorMessage.clear();
    return true;
}

bool loadStudents(std::vector<Student>& students,
                  const std::string& filename,
                  std::string& errorMessage) {
    std::ifstream input(filename);
    if (!input) {
        if (!std::filesystem::exists(filename)) {
            students.clear();
            errorMessage.clear();
            return true;
        }
        errorMessage = "The student data file exists but could not be opened.";
        return false;
    }

    std::vector<Student> loadedStudents;
    std::unordered_set<int> loadedIds;
    std::string line;
    std::size_t lineNumber = 0;

    while (std::getline(input, line)) {
        ++lineNumber;
        if (lineNumber == 1 && line == "id,name,grades") {
            continue;
        }
        if (line.empty()) {
            continue;
        }

        std::vector<std::string> fields;
        if (!parseCsvRow(line, fields) || fields.size() < 2) {
            errorMessage = "Invalid CSV format on line " + std::to_string(lineNumber) + ".";
            return false;
        }

        Student student;
        if (!parseIntStrictly(fields[0], student.id) || student.id <= 0) {
            errorMessage = "Invalid student ID on line " + std::to_string(lineNumber) + ".";
            return false;
        }
        if (fields[1].empty()) {
            errorMessage = "Missing student name on line " + std::to_string(lineNumber) + ".";
            return false;
        }
        if (!loadedIds.insert(student.id).second) {
            errorMessage = "Duplicate student ID on line " + std::to_string(lineNumber) + ".";
            return false;
        }

        student.name = fields[1];
        for (std::size_t index = 2; index < fields.size(); ++index) {
            double grade = 0.0;
            if (!parseGradeStrictly(fields[index], grade)) {
                errorMessage = "Invalid grade on line " + std::to_string(lineNumber) + ".";
                return false;
            }
            student.grades.push_back(grade);
        }

        loadedStudents.push_back(student);
    }

    if (!input.eof()) {
        errorMessage = "An error occurred while reading the student data file.";
        return false;
    }

    students = std::move(loadedStudents);
    errorMessage.clear();
    return true;
}
