#include "InputValidation.h"
#include "Storage.h"
#include "Student.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

const std::string DATA_FILE = "students.csv";

void printStudent(const Student& student) {
    std::cout << std::left << std::setw(8) << student.id
              << std::setw(24) << student.name
              << std::setw(10) << std::fixed << std::setprecision(2) << calculateAverage(student)
              << getStatus(student) << '\n';
}

void addStudent(std::vector<Student>& students) {
    int id = 0;
    while (true) {
        id = readInt("Student ID: ", 1, 999999999);
        if (!studentIdExists(students, id)) {
            break;
        }
        std::cout << "That student ID already exists.\n";
    }

    Student student;
    student.id = id;
    student.name = readRequiredText("Student name: ");
    const int gradeCount = readInt("Number of grades (1-20): ", 1, 20);
    for (int index = 0; index < gradeCount; ++index) {
        student.grades.push_back(readDouble("Grade " + std::to_string(index + 1) + " (0-100): ", 0.0, 100.0));
    }

    students.push_back(student);
    std::cout << "Student added successfully.\n";
}

void viewStudents(const std::vector<Student>& students) {
    if (students.empty()) {
        std::cout << "No student records are available.\n";
        return;
    }

    std::cout << '\n' << std::left << std::setw(8) << "ID"
              << std::setw(24) << "Name"
              << std::setw(10) << "Average" << "Status\n";
    std::cout << std::string(54, '-') << '\n';
    for (const Student& student : students) {
        printStudent(student);
    }
}

void searchStudent(const std::vector<Student>& students) {
    const int id = readInt("Enter the student ID: ", 1, 999999999);
    const auto found = std::find_if(students.begin(), students.end(), [id](const Student& student) {
        return student.id == id;
    });

    if (found == students.end()) {
        std::cout << "Student not found.\n";
        return;
    }

    std::cout << std::left << std::setw(8) << "ID"
              << std::setw(24) << "Name"
              << std::setw(10) << "Average" << "Status\n";
    printStudent(*found);
}

bool saveChanges(const std::vector<Student>& students) {
    std::string errorMessage;
    if (!saveStudents(students, DATA_FILE, errorMessage)) {
        std::cerr << "Save failed: " << errorMessage << '\n';
        return false;
    }
    std::cout << "Records saved to " << DATA_FILE << ".\n";
    return true;
}

void showMenu() {
    std::cout << "\n=== Student Grade Management ===\n"
              << "1. Add student\n"
              << "2. View all students\n"
              << "3. Search by student ID\n"
              << "4. Save records\n"
              << "5. Exit\n";
}

}  // namespace

int main() {
    std::vector<Student> students;
    std::string errorMessage;
    if (!loadStudents(students, DATA_FILE, errorMessage)) {
        std::cerr << "Could not load records: " << errorMessage << '\n';
        std::cerr << "Fix or rename the invalid data file, then restart the program.\n";
        return 1;
    }

    std::cout << "Loaded " << students.size() << " student record(s).\n";

    try {
        while (true) {
            showMenu();
            const int choice = readInt("Choose an option: ", 1, 5);
            switch (choice) {
                case 1:
                    addStudent(students);
                    break;
                case 2:
                    viewStudents(students);
                    break;
                case 3:
                    searchStudent(students);
                    break;
                case 4:
                    saveChanges(students);
                    break;
                case 5:
                    if (saveChanges(students)) {
                        std::cout << "Goodbye.\n";
                        return 0;
                    }
                    std::cout << "Exit cancelled because the latest changes were not saved.\n";
                    break;
                default:
                    break;
            }
        }
    } catch (const std::runtime_error& error) {
        std::cerr << "\nProgram stopped: " << error.what() << '\n';
        return 1;
    }
}
