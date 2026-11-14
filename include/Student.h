#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

struct Student {
    int id{};
    std::string name;
    std::vector<double> grades;
};

double calculateAverage(const Student& student);
std::string getStatus(const Student& student);

#endif
