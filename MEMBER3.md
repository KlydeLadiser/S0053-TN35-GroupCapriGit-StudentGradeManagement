# Member 3 - Storage and Input Validation

This feature branch implements the Member 3 responsibilities for the Student
Grade Management project.

## Contribution

- Student records containing an ID, name, and multiple grades
- CSV save and load operations
- Rejection of invalid IDs, duplicate IDs, empty names, and grades outside 0-100
- Safe file replacement with a temporary backup
- Reusable validation for numeric input, menu choices, and required text
- Automated storage tests covering normal, boundary, and invalid data

## Build the application

```powershell
New-Item -ItemType Directory -Force build | Out-Null
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/Student.cpp src/Storage.cpp src/InputValidation.cpp src/main.cpp -o build/student_grade_manager.exe
.\build\student_grade_manager.exe
```

## Run the tests

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/Student.cpp src/Storage.cpp src/InputValidation.cpp tests/storage_tests.cpp -o build/storage_tests.exe
.\build\storage_tests.exe
```

This branch should be reviewed before it is merged into `main`.
