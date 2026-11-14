#ifndef INPUT_VALIDATION_H
#define INPUT_VALIDATION_H

#include "Student.h"

#include <string>
#include <vector>

int readInt(const std::string& prompt, int minimum, int maximum);
double readDouble(const std::string& prompt, double minimum, double maximum);
std::string readRequiredText(const std::string& prompt);
bool studentIdExists(const std::vector<Student>& students, int id);

#endif
