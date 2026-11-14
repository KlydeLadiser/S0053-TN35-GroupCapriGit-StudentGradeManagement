#include "Student.h"

#include <numeric>

double calculateAverage(const Student& student) {
    if (student.grades.empty()) {
        return 0.0;
    }

    const double total = std::accumulate(student.grades.begin(), student.grades.end(), 0.0);
    return total / static_cast<double>(student.grades.size());
}

std::string getStatus(const Student& student) {
    if (student.grades.empty()) {
        return "No grades";
    }
    return calculateAverage(student) >= 60.0 ? "Pass" : "Fail";
}
