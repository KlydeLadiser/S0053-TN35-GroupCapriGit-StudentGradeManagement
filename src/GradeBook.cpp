

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "InputValidation.h"
#include "Student.h"

// Grading rules

namespace GradeRules {

constexpr double MIN_GRADE = 0.0;
constexpr double MAX_GRADE = 100.0;
constexpr std::size_t MAX_GRADES_PER_STUDENT = 20;

// Average needed to pass. Letter-grade cut-offs are inclusive lower bounds.
constexpr double PASSING_AVERAGE = 60.0;
constexpr double A_CUTOFF = 90.0;
constexpr double B_CUTOFF = 80.0;
constexpr double C_CUTOFF = 70.0;
constexpr double D_CUTOFF = PASSING_AVERAGE;

}  // namespace GradeRules

// Result types
struct StudentScore {
    int id{};
    std::string name;
    double average{};
};

struct ClassSummary {
    std::size_t studentCount{};  // every record, including students without grades
    std::size_t gradedCount{};   // students with at least one grade
    std::size_t noGradeCount{};  // students with no grades yet
    std::size_t passCount{};
    std::size_t failCount{};
    double passRate{};                          // percent of graded students who pass
    double classAverage{};                      // mean of the graded students' averages
    double medianAverage{};                     // median of the graded students' averages
    StudentScore highest;                       // ties: the smaller student ID wins
    StudentScore lowest;                        // ties: the smaller student ID wins
    std::array<std::size_t, 5> letterCounts{};  // A, B, C, D, F
};

struct RankedStudent {
    int rank{};  // students with equal averages share a rank (1, 2, 2, 4)
    StudentScore score;
};

// ---------- Logic ----------
namespace {

constexpr double TIE_TOLERANCE = 1e-9;
constexpr std::size_t LETTER_COUNT = 5;
constexpr std::array<const char*, LETTER_COUNT> LETTERS{"A", "B", "C", "D", "F"};

std::size_t letterIndex(double average) {
    if (average >= GradeRules::A_CUTOFF) {
        return 0;
    }
    if (average >= GradeRules::B_CUTOFF) {
        return 1;
    }
    if (average >= GradeRules::C_CUTOFF) {
        return 2;
    }
    if (average >= GradeRules::D_CUTOFF) {
        return 3;
    }
    return 4;
}

StudentScore makeScore(const Student& student) {
    return StudentScore{student.id, student.name, calculateAverage(student)};
}

// Students who have at least one grade, as scores.
std::vector<StudentScore> collectGradedScores(const std::vector<Student>& students) {
    std::vector<StudentScore> scores;
    for (const Student& student : students) {
        if (!student.grades.empty()) {
            scores.push_back(makeScore(student));
        }
    }
    return scores;
}

double medianOf(std::vector<double> values) {
    if (values.empty()) {
        return 0.0;
    }
    std::sort(values.begin(), values.end());
    const std::size_t middle = values.size() / 2;
    if (values.size() % 2 == 1) {
        return values[middle];
    }
    return (values[middle - 1] + values[middle]) / 2.0;
}

bool hasOnlyWhitespace(const std::string& text) {
    return std::all_of(text.begin(), text.end(),
                       [](unsigned char character) { return std::isspace(character) != 0; });
}

}  // namespace

// ---------- Letter grades ----------

std::string getLetterGrade(double average) { return LETTERS[letterIndex(average)]; }

// ---------- Class statistics ----------

ClassSummary summarizeClass(const std::vector<Student>& students) {
    ClassSummary summary;
    summary.studentCount = students.size();

    const std::vector<StudentScore> scores = collectGradedScores(students);
    summary.gradedCount = scores.size();
    summary.noGradeCount = summary.studentCount - summary.gradedCount;
    if (scores.empty()) {
        return summary;
    }

    std::vector<double> averages;
    summary.highest = scores.front();
    summary.lowest = scores.front();
    for (const StudentScore& score : scores) {
        averages.push_back(score.average);
        ++summary.letterCounts[letterIndex(score.average)];

        if (score.average >= GradeRules::PASSING_AVERAGE) {
            ++summary.passCount;
        } else {
            ++summary.failCount;
        }

        if (score.average > summary.highest.average ||
            (score.average == summary.highest.average && score.id < summary.highest.id)) {
            summary.highest = score;
        }
        if (score.average < summary.lowest.average ||
            (score.average == summary.lowest.average && score.id < summary.lowest.id)) {
            summary.lowest = score;
        }
    }

    summary.classAverage = std::accumulate(averages.begin(), averages.end(), 0.0) /
                           static_cast<double>(averages.size());
    summary.medianAverage = medianOf(averages);
    summary.passRate =
        100.0 * static_cast<double>(summary.passCount) / static_cast<double>(summary.gradedCount);
    return summary;
}

std::string formatClassSummary(const ClassSummary& summary) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2);
    out << "\n=== Class Summary ===\n";
    out << "Total students : " << summary.studentCount << '\n';
    out << "With grades    : " << summary.gradedCount << '\n';
    out << "Without grades : " << summary.noGradeCount << '\n';

    if (summary.gradedCount == 0) {
        out << "No grades have been recorded yet, so no statistics are available.\n";
        return out.str();
    }

    out << "Class average  : " << summary.classAverage << '\n';
    out << "Median average : " << summary.medianAverage << '\n';
    out << "Highest        : " << summary.highest.name << " (ID " << summary.highest.id << ", "
        << summary.highest.average << ")\n";
    out << "Lowest         : " << summary.lowest.name << " (ID " << summary.lowest.id << ", "
        << summary.lowest.average << ")\n";
    out << "Passed / Failed: " << summary.passCount << " / " << summary.failCount << "  ("
        << summary.passRate << "% pass rate)\n";
    out << "Letter grades  :";
    for (std::size_t index = 0; index < LETTER_COUNT; ++index) {
        out << ' ' << LETTERS[index] << '=' << summary.letterCounts[index];
    }
    out << '\n';
    return out.str();
}

// ---------- Ranking ----------

std::vector<RankedStudent> rankStudents(const std::vector<Student>& students) {
    std::vector<StudentScore> scores = collectGradedScores(students);
    std::sort(scores.begin(), scores.end(),
              [](const StudentScore& left, const StudentScore& right) {
                  if (left.average != right.average) {
                      return left.average > right.average;
                  }
                  return left.id < right.id;
              });

    std::vector<RankedStudent> ranking;
    for (std::size_t index = 0; index < scores.size(); ++index) {
        int rank = static_cast<int>(index) + 1;
        if (index > 0 &&
            std::fabs(scores[index].average - scores[index - 1].average) < TIE_TOLERANCE) {
            rank = ranking.back().rank;
        }
        ranking.push_back(RankedStudent{rank, scores[index]});
    }
    return ranking;
}

std::string formatRanking(const std::vector<RankedStudent>& ranking) {
    std::ostringstream out;
    if (ranking.empty()) {
        out << "No graded students to rank.\n";
        return out.str();
    }

    out << "\n=== Student Ranking ===\n";
    out << std::left << std::setw(6) << "Rank" << std::setw(8) << "ID" << std::setw(24) << "Name"
        << std::setw(10) << "Average" << "Letter\n";
    out << std::string(54, '-') << '\n';
    out << std::fixed << std::setprecision(2);
    for (const RankedStudent& entry : ranking) {
        out << std::left << std::setw(6) << entry.rank << std::setw(8) << entry.score.id
            << std::setw(24) << entry.score.name << std::setw(10) << entry.score.average
            << getLetterGrade(entry.score.average) << '\n';
    }
    return out.str();
}

// ---------- Record editing ----------

Student* findStudent(std::vector<Student>& students, int id) {
    const auto found = std::find_if(students.begin(), students.end(),
                                    [id](const Student& student) { return student.id == id; });
    return found == students.end() ? nullptr : &*found;
}

const Student* findStudent(const std::vector<Student>& students, int id) {
    const auto found = std::find_if(students.begin(), students.end(),
                                    [id](const Student& student) { return student.id == id; });
    return found == students.end() ? nullptr : &*found;
}

bool isValidGrade(double grade) {
    return std::isfinite(grade) && grade >= GradeRules::MIN_GRADE && grade <= GradeRules::MAX_GRADE;
}

bool isValidName(const std::string& name) {
    return !hasOnlyWhitespace(name) && name.find_first_of("\r\n") == std::string::npos;
}

bool renameStudent(Student& student, const std::string& newName, std::string& errorMessage) {
    if (!isValidName(newName)) {
        errorMessage = "A name cannot be empty, blank, or contain a line break.";
        return false;
    }
    student.name = newName;
    errorMessage.clear();
    return true;
}

bool addGrade(Student& student, double grade, std::string& errorMessage) {
    if (!isValidGrade(grade)) {
        errorMessage = "A grade must be a number from 0 to 100.";
        return false;
    }
    if (student.grades.size() >= GradeRules::MAX_GRADES_PER_STUDENT) {
        errorMessage = "A student can have at most " +
                       std::to_string(GradeRules::MAX_GRADES_PER_STUDENT) + " grades.";
        return false;
    }
    student.grades.push_back(grade);
    errorMessage.clear();
    return true;
}

bool changeGrade(Student& student, std::size_t position, double grade, std::string& errorMessage) {
    if (position < 1 || position > student.grades.size()) {
        errorMessage =
            "Grade number " + std::to_string(position) + " does not exist for this student.";
        return false;
    }
    if (!isValidGrade(grade)) {
        errorMessage = "A grade must be a number from 0 to 100.";
        return false;
    }
    student.grades[position - 1] = grade;
    errorMessage.clear();
    return true;
}

bool removeStudent(std::vector<Student>& students, int id, std::string& errorMessage) {
    const auto found = std::find_if(students.begin(), students.end(),
                                    [id](const Student& student) { return student.id == id; });
    if (found == students.end()) {
        errorMessage = "Student ID " + std::to_string(id) + " was not found.";
        return false;
    }
    students.erase(found);
    errorMessage.clear();
    return true;
}

// ---------- Menu features ----------
namespace {

constexpr int MAX_STUDENT_ID = 999999999;

void printGrades(const Student& student) {
    std::cout << "Current record: ID " << student.id << ", " << student.name << ", average "
              << std::fixed << std::setprecision(2) << calculateAverage(student) << " ("
              << getLetterGrade(calculateAverage(student)) << ")\n";
    if (student.grades.empty()) {
        std::cout << "  No grades recorded.\n";
        return;
    }
    for (std::size_t index = 0; index < student.grades.size(); ++index) {
        std::cout << "  Grade " << (index + 1) << ": " << student.grades[index] << '\n';
    }
}

void reportResult(bool succeeded, const std::string& successMessage,
                  const std::string& errorMessage) {
    if (succeeded) {
        std::cout << successMessage << " Choose \"Save records\" to keep this change.\n";
    } else {
        std::cout << "Not changed: " << errorMessage << '\n';
    }
}

}  // namespace

void updateStudentRecord(std::vector<Student>& students) {
    if (students.empty()) {
        std::cout << "No student records are available to update.\n";
        return;
    }

    const int id = readInt("Enter the ID of the student to update: ", 1, MAX_STUDENT_ID);
    Student* student = findStudent(students, id);
    if (student == nullptr) {
        std::cout << "Student not found.\n";
        return;
    }

    printGrades(*student);
    std::cout << "1. Change name\n"
              << "2. Add a grade\n"
              << "3. Change an existing grade\n"
              << "4. Cancel\n";
    const int choice = readInt("Choose an update: ", 1, 4);

    std::string errorMessage;
    switch (choice) {
        case 1: {
            const std::string newName = readRequiredText("New name: ");
            reportResult(renameStudent(*student, newName, errorMessage), "Name updated.",
                         errorMessage);
            break;
        }
        case 2: {
            const double grade =
                readDouble("New grade (0-100): ", GradeRules::MIN_GRADE, GradeRules::MAX_GRADE);
            reportResult(addGrade(*student, grade, errorMessage), "Grade added.", errorMessage);
            break;
        }
        case 3: {
            if (student->grades.empty()) {
                std::cout << "This student has no grades to change.\n";
                break;
            }
            const int position = readInt("Which grade number to change? ", 1,
                                         static_cast<int>(student->grades.size()));
            const double grade = readDouble("Replacement grade (0-100): ", GradeRules::MIN_GRADE,
                                            GradeRules::MAX_GRADE);
            reportResult(
                changeGrade(*student, static_cast<std::size_t>(position), grade, errorMessage),
                "Grade changed.", errorMessage);
            break;
        }
        default:
            std::cout << "Update cancelled.\n";
            break;
    }
}

void deleteStudentRecord(std::vector<Student>& students) {
    if (students.empty()) {
        std::cout << "No student records are available to delete.\n";
        return;
    }

    const int id = readInt("Enter the ID of the student to delete: ", 1, MAX_STUDENT_ID);
    const Student* student = findStudent(students, id);
    if (student == nullptr) {
        std::cout << "Student not found.\n";
        return;
    }

    printGrades(*student);
    const int confirm = readInt("Type 1 to delete this student, or 0 to cancel: ", 0, 1);
    if (confirm == 0) {
        std::cout << "Delete cancelled.\n";
        return;
    }

    std::string errorMessage;
    reportResult(removeStudent(students, id, errorMessage), "Student deleted.", errorMessage);
}

void showClassSummary(const std::vector<Student>& students) {
    if (students.empty()) {
        std::cout << "No student records are available.\n";
        return;
    }
    std::cout << formatClassSummary(summarizeClass(students));
}

void showRanking(const std::vector<Student>& students) {
    if (students.empty()) {
        std::cout << "No student records are available.\n";
        return;
    }
    std::cout << formatRanking(rankStudents(students));
}
