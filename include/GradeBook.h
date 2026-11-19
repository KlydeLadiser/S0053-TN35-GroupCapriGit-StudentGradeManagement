#ifndef GRADE_BOOK_H
#define GRADE_BOOK_H

#include "Student.h"

#include <vector>

void updateStudentRecord(std::vector<Student>& students);
void deleteStudentRecord(std::vector<Student>& students);
void showClassSummary(const std::vector<Student>& students);
void showRanking(const std::vector<Student>& students);

#endif
