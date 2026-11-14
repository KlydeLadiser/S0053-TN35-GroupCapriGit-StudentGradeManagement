#ifndef STORAGE_H
#define STORAGE_H

#include "Student.h"

#include <string>
#include <vector>

bool saveStudents(const std::vector<Student>& students,
                  const std::string& filename,
                  std::string& errorMessage);

bool loadStudents(std::vector<Student>& students,
                  const std::string& filename,
                  std::string& errorMessage);

#endif
