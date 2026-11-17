# Member 2 - Business Logic and Feature Development

This feature branch adds the grade calculations and four new menu features to the
Student Grade Management project. It builds on Member 3's storage and validation code
without changing it.

## Contribution

Member 2's implementation is in `src/GradeBook.cpp`. The public menu functions are declared in
`include/GradeBook.h` so `main.cpp` can use them without duplicating declarations:

- Grading rules - one place for the constants (0-100 range, 20 grades per student, 60 pass mark,
  letter-grade cut-offs)
- Pure business logic with no input/output, so it can be unit tested:
  - letter grades (A-F)
  - class summary: class average, median, highest, lowest, pass/fail counts, pass rate, letter counts
  - ranking with shared ranks for ties (1, 2, 2, 4)
  - record editing: rename, add a grade, change a grade, delete a student
- Menu screens for the features above, using Member 3's `readInt`, `readDouble` and
  `readRequiredText`
- New menu options 4-7: Update student, Delete student, Class summary, Student ranking
- Tests in `tests/member2_tests.cpp` (100 checks): calculation tests and integration tests with
  Member 3's storage

`src/main.cpp` declares the four menu functions at the top and calls them from the new menu options.

## How this connects to Member 3

| Member 2 (logic)                        | Member 3 (storage / validation)                                  |
| --------------------------------------- | ---------------------------------------------------------------- |
| Editing functions enforce the same rules as `saveStudents()` | A record Member 2 accepts can always be saved           |
| Update / delete change records in memory | "Save records" (and Exit) write them to `students.csv`          |
| Summary and ranking are calculated from the loaded records | Loaded records are validated by `loadStudents()`        |
| Integration tests edit -> save -> load -> compare | `saveStudents()` / `loadStudents()` round trip            |

## Rules and decisions

- Letter grades: A >= 90, B >= 80, C >= 70, D >= 60, F below 60 (edit the `GradeRules` constants at the top of `GradeBook.cpp` to change).
- Letter grade, Pass/Fail and ranking all use the unrounded average, so they always agree.
- Class average is the mean of the students' averages. Students with no grades are counted in the
  total but left out of averages, pass rate and ranking.
- Tied highest/lowest students: the smaller student ID is reported.

## Build the application

```powershell
New-Item -ItemType Directory -Force build | Out-Null
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/Student.cpp src/Storage.cpp src/InputValidation.cpp src/GradeBook.cpp src/main.cpp -o build/student_grade_manager.exe
.\build\student_grade_manager.exe
```

## Run the tests

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/Student.cpp src/Storage.cpp src/InputValidation.cpp tests/member2_tests.cpp -o build/member2_tests.exe
.\build\member2_tests.exe
```

The test file includes `src/GradeBook.cpp` directly so it can reach the functions, so do **not** also
list `src/GradeBook.cpp` in the test command. Each check prints its test ID, the expected value and the
actual value. The program exits with code 0 only when every check passes.

## Sample session

```text
=== Class Summary ===
Total students : 3
With grades    : 3
Without grades : 0
Class average  : 79.83
Median average : 85.00
Highest        : Ben Lim (ID 102, 100.00)
Lowest         : Cy Tan (ID 103, 54.50)
Passed / Failed: 2 / 1  (66.67% pass rate)
Letter grades  : A=1 B=1 C=0 D=0 F=1

=== Student Ranking ===
Rank  ID      Name                    Average   Letter
------------------------------------------------------
1     102     Ben Lim                 100.00    A
2     101     Ana Cruz                85.00     B
3     103     Cy Tan                  54.50     F
```

This branch should be reviewed by another member before it is merged into `main`.
