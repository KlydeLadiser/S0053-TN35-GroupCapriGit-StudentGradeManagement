#include "Storage.h"
#include "Student.h"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool nearlyEqual(double left, double right) {
    return std::abs(left - right) < 0.001;
}

}  // namespace

int main() {
    const std::string testFile = "storage_test.csv";
    std::string errorMessage;

    std::vector<Student> original{
        {101, "Ana Cruz", {90.0, 84.5, 95.0}},
        {102, "Lee, \"Marcus\"", {60.0, 70.0}}
    };

    assert(saveStudents(original, testFile, errorMessage));

    std::vector<Student> loaded;
    assert(loadStudents(loaded, testFile, errorMessage));
    assert(loaded.size() == 2);
    assert(loaded[0].id == 101);
    assert(loaded[1].name == "Lee, \"Marcus\"");
    assert(loaded[0].grades.size() == 3);
    assert(nearlyEqual(loaded[0].grades[1], 84.5));
    assert(nearlyEqual(calculateAverage(loaded[0]), 89.833333));

    const std::vector<Student> duplicateIds{
        {201, "First Student", {75.0}},
        {201, "Second Student", {80.0}}
    };
    assert(!saveStudents(duplicateIds, testFile, errorMessage));
    assert(errorMessage.find("duplicate student ID") != std::string::npos);

    const std::vector<Student> invalidGrades{
        {202, "Invalid Grade", {-1.0}}
    };
    assert(!saveStudents(invalidGrades, testFile, errorMessage));
    assert(errorMessage.find("invalid grade") != std::string::npos);

    const std::vector<Student> multilineName{
        {203, "Broken\nName", {80.0}}
    };
    assert(!saveStudents(multilineName, testFile, errorMessage));
    assert(errorMessage.find("line break") != std::string::npos);

    loaded.clear();
    assert(loadStudents(loaded, testFile, errorMessage));
    assert(loaded.size() == 2);
    assert(loaded[0].id == 101);

    {
        std::ofstream invalidFile(testFile);
        invalidFile << "id,name,grades\n103,Test Student,120\n";
    }
    assert(!loadStudents(loaded, testFile, errorMessage));
    assert(errorMessage.find("Invalid grade") != std::string::npos);

    {
        std::ofstream duplicateFile(testFile);
        duplicateFile << "id,name,grades\n"
                      << "104,First Student,80\n"
                      << "104,Duplicate Student,90\n";
    }
    assert(!loadStudents(loaded, testFile, errorMessage));
    assert(errorMessage.find("Duplicate student ID") != std::string::npos);

    std::filesystem::remove(testFile);
    std::cout << "All storage tests passed.\n";
    return 0;
}
