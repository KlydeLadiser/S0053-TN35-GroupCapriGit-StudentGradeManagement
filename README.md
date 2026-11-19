# Student Grade Management System

This folder contains a simple C++ program for managing student grades.

## What the program can do

- Add student records
- View all students
- Search for a student by ID
- Update student information and grades
- Delete student records
- Show the class summary
- Show the student ranking
- Save records to `students.csv`

## How to run

Open a terminal in the project folder:

```powershell
cd "Student_Grade\Student_Grade_CS0053\Student_Grade_CS0053"
```

If the program has already been built, run:

```powershell
.\build\student_grade_manager.exe
```

To build the program again:

```powershell
cmake --build build
.\build\student_grade_manager.exe
```

## Important file

The program uses `students.csv` to load and save student records. Keep this file in the same folder where the program is run.

## Project folders

- `src` - program source files
- `include` - header files
- `tests` - test files
- `build` - compiled program files
