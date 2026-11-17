// Member 2 tests. Part 1 checks the calculation logic on its own; part 2 checks
// it together with Member 3's file storage (edit -> save -> load -> compare).
// Every check prints its test ID, the expected value and the actual value.
//
// The source file is included directly (instead of through a header) so the test
// can use its functions and types. Do not also compile src/GradeBook.cpp.
#include <cstdio>
#include <limits>

#include "../src/GradeBook.cpp"
#include "Storage.h"

// Tiny test helpers. Every check prints the test ID, the description, the
// expected value and the actual value, so the console output can be pasted
// straight into the group report's test table.
namespace testing {

inline int g_failureCount = 0;
inline int g_checkCount = 0;

template <typename Value>
void expectEqual(const std::string& id, const std::string& description, const Value& expected,
                 const Value& actual) {
    ++g_checkCount;
    const bool passed = (expected == actual);
    std::cout << (passed ? "[PASS] " : "[FAIL] ") << id << " - " << description << '\n'
              << "         expected: " << std::boolalpha << expected << '\n'
              << "         actual:   " << actual << '\n';
    if (!passed) {
        ++g_failureCount;
    }
}

inline void expectNear(const std::string& id, const std::string& description, double expected,
                       double actual, double tolerance = 0.0001) {
    ++g_checkCount;
    const bool passed = std::fabs(expected - actual) <= tolerance;
    std::cout << (passed ? "[PASS] " : "[FAIL] ") << id << " - " << description << '\n'
              << std::fixed << std::setprecision(4) << "         expected: " << expected << '\n'
              << "         actual:   " << actual << '\n';
    if (!passed) {
        ++g_failureCount;
    }
}

inline void expectContains(const std::string& id, const std::string& description,
                           const std::string& text, const std::string& expectedPart) {
    ++g_checkCount;
    const bool passed = text.find(expectedPart) != std::string::npos;
    std::cout << (passed ? "[PASS] " : "[FAIL] ") << id << " - " << description << '\n'
              << "         expected text to contain: " << expectedPart << '\n'
              << "         actual text: " << text << '\n';
    if (!passed) {
        ++g_failureCount;
    }
}

// Prints the totals and returns the process exit code (0 = all passed).
inline int finish(const std::string& suiteName) {
    std::cout << "\n"
              << suiteName << ": " << (g_checkCount - g_failureCount) << " of " << g_checkCount
              << " checks passed.\n";
    return g_failureCount == 0 ? 0 : 1;
}

}  // namespace testing

using testing::expectContains;
using testing::expectEqual;
using testing::expectNear;

// ===== Part 1: calculation tests =====
namespace {

std::vector<Student> sampleClass() {
    return {
        {1, "Ana", {90.0, 80.0}},  // average 85.00 -> B
        {2, "Ben", {100.0}},       // average 100.00 -> A
        {3, "Cy", {50.0, 59.0}},   // average 54.50 -> F
        {4, "Di", {}},             // no grades
        {5, "Eli", {60.0}},        // average 60.00 -> D (exactly on the pass mark)
    };
}

void testLetterGrades() {
    expectEqual<std::string>("LG-01", "normal: 95 is an A", "A", getLetterGrade(95.0));
    expectEqual<std::string>("LG-02", "boundary: exactly 90 is an A", "A", getLetterGrade(90.0));
    expectEqual<std::string>("LG-03", "boundary: 89.99 is a B", "B", getLetterGrade(89.99));
    expectEqual<std::string>("LG-04", "boundary: exactly 80 is a B", "B", getLetterGrade(80.0));
    expectEqual<std::string>("LG-05", "boundary: 79.99 is a C", "C", getLetterGrade(79.99));
    expectEqual<std::string>("LG-06", "boundary: exactly 70 is a C", "C", getLetterGrade(70.0));
    expectEqual<std::string>("LG-07", "boundary: 69.99 is a D", "D", getLetterGrade(69.99));
    expectEqual<std::string>("LG-08", "boundary: exactly 60 is a D (passing)", "D",
                             getLetterGrade(60.0));
    expectEqual<std::string>("LG-09", "boundary: 59.99 is an F", "F", getLetterGrade(59.99));
    expectEqual<std::string>("LG-10", "boundary: 0 is an F", "F", getLetterGrade(0.0));
    expectEqual<std::string>("LG-11", "boundary: 100 is an A", "A", getLetterGrade(100.0));
}

void testClassSummary() {
    const ClassSummary summary = summarizeClass(sampleClass());
    expectEqual<std::size_t>("SM-01", "normal: total students", 5, summary.studentCount);
    expectEqual<std::size_t>("SM-02", "normal: students with grades", 4, summary.gradedCount);
    expectEqual<std::size_t>("SM-03", "normal: students without grades", 1, summary.noGradeCount);
    expectNear("SM-04", "normal: class average = (85+100+54.5+60)/4", 74.875, summary.classAverage);
    expectNear("SM-05", "normal: median of 54.5, 60, 85, 100 = (60+85)/2", 72.5,
               summary.medianAverage);
    expectEqual<std::string>("SM-06", "normal: highest student", "Ben", summary.highest.name);
    expectEqual<std::string>("SM-07", "normal: lowest student", "Cy", summary.lowest.name);
    expectEqual<std::size_t>("SM-08", "normal: students who passed", 3, summary.passCount);
    expectEqual<std::size_t>("SM-09", "normal: students who failed", 1, summary.failCount);
    expectNear("SM-10", "normal: pass rate (3 of 4 graded)", 75.0, summary.passRate);
    expectEqual<std::size_t>("SM-11", "normal: number of A grades", 1, summary.letterCounts[0]);
    expectEqual<std::size_t>("SM-12", "normal: number of D grades", 1, summary.letterCounts[3]);

    const ClassSummary empty = summarizeClass({});
    expectEqual<std::size_t>("SM-13", "boundary: empty class has 0 students", 0,
                             empty.studentCount);
    expectNear("SM-14", "boundary: empty class average is 0 (no divide by zero)", 0.0,
               empty.classAverage);

    const ClassSummary noGrades = summarizeClass({{9, "Only Name", {}}});
    expectEqual<std::size_t>("SM-15", "boundary: nobody graded, graded count", 0,
                             noGrades.gradedCount);
    expectNear("SM-16", "boundary: nobody graded, pass rate is 0 (no divide by zero)", 0.0,
               noGrades.passRate);

    const ClassSummary tie = summarizeClass({{7, "Zed", {90.0}}, {3, "Amy", {90.0}}});
    expectEqual<int>("SM-17", "boundary: tie for highest goes to the smaller ID", 3,
                     tie.highest.id);
    expectEqual<int>("SM-18", "boundary: tie for lowest goes to the smaller ID", 3, tie.lowest.id);

    const ClassSummary single = summarizeClass({{1, "Solo", {72.5}}});
    expectNear("SM-19", "boundary: one student, median equals the average", 72.5,
               single.medianAverage);

    expectContains("SM-20", "report text shows the class average", formatClassSummary(summary),
                   "Class average  : 74.88");
    expectContains("SM-21", "report text for an empty class explains why",
                   formatClassSummary(noGrades), "No grades have been recorded yet");
}

void testRanking() {
    const std::vector<RankedStudent> ranking = rankStudents(sampleClass());
    expectEqual<std::size_t>("RK-01", "normal: students without grades are not ranked", 4,
                             ranking.size());
    expectEqual<std::string>("RK-02", "normal: rank 1 is the highest average", "Ben",
                             ranking[0].score.name);
    expectEqual<std::string>("RK-03", "normal: rank 2", "Ana", ranking[1].score.name);
    expectEqual<std::string>("RK-04", "normal: rank 3", "Eli", ranking[2].score.name);
    expectEqual<int>("RK-05", "normal: last rank number", 4, ranking[3].rank);

    const std::vector<RankedStudent> tied =
        rankStudents({{2, "Bo", {90.0}}, {1, "Al", {90.0}}, {3, "Cat", {80.0}}});
    expectEqual<int>("RK-06", "boundary: first of two tied students has rank 1", 1, tied[0].rank);
    expectEqual<int>("RK-07", "boundary: second tied student shares rank 1", 1, tied[1].rank);
    expectEqual<int>("RK-08", "boundary: next rank after a tie skips to 3", 3, tied[2].rank);
    expectEqual<int>("RK-09", "boundary: tied students are ordered by ID", 1, tied[0].score.id);

    expectEqual<std::size_t>("RK-10", "boundary: empty class gives an empty ranking", 0,
                             rankStudents({}).size());
    expectContains("RK-11", "empty ranking prints a helpful message", formatRanking({}),
                   "No graded students to rank.");
    expectContains("RK-12", "ranking table shows the top average with its letter grade",
                   formatRanking(ranking), "100.00    A");
}

void testValidationRules() {
    const double notANumber = std::numeric_limits<double>::quiet_NaN();
    const double infinity = std::numeric_limits<double>::infinity();
    expectEqual<bool>("VR-01", "normal: 85.5 is a valid grade", true, isValidGrade(85.5));
    expectEqual<bool>("VR-02", "boundary: 0 is a valid grade", true, isValidGrade(0.0));
    expectEqual<bool>("VR-03", "boundary: 100 is a valid grade", true, isValidGrade(100.0));
    expectEqual<bool>("VR-04", "invalid: -0.01 is rejected", false, isValidGrade(-0.01));
    expectEqual<bool>("VR-05", "invalid: 100.01 is rejected", false, isValidGrade(100.01));
    expectEqual<bool>("VR-06", "invalid: NaN is rejected", false, isValidGrade(notANumber));
    expectEqual<bool>("VR-07", "invalid: infinity is rejected", false, isValidGrade(infinity));
    expectEqual<bool>("VR-08", "normal: 'Ana' is a valid name", true, isValidName("Ana"));
    expectEqual<bool>("VR-09", "invalid: empty name is rejected", false, isValidName(""));
    expectEqual<bool>("VR-10", "invalid: blank name is rejected", false, isValidName("   "));
    expectEqual<bool>("VR-11", "invalid: name with a line break is rejected", false,
                      isValidName("A\nB"));
}

void testEditing() {
    std::string message;

    Student student{10, "Ana", {80.0, 90.0}};
    expectEqual<bool>("ED-01", "normal: add a grade of 70", true, addGrade(student, 70.0, message));
    expectEqual<std::size_t>("ED-02", "normal: student now has 3 grades", 3, student.grades.size());
    expectEqual<bool>("ED-03", "invalid: add a grade of 101", false,
                      addGrade(student, 101.0, message));
    expectEqual<std::size_t>("ED-04", "invalid: a rejected grade is not stored", 3,
                             student.grades.size());

    Student nearlyFull{11, "Full", std::vector<double>(19, 75.0)};
    expectEqual<bool>("ED-05", "boundary: the 20th grade is accepted", true,
                      addGrade(nearlyFull, 75.0, message));
    expectEqual<bool>("ED-06", "boundary: the 21st grade is rejected", false,
                      addGrade(nearlyFull, 75.0, message));
    expectContains("ED-07", "boundary: rejection explains the limit", message, "at most 20 grades");

    expectEqual<bool>("ED-08", "normal: change grade 1 to 95", true,
                      changeGrade(student, 1, 95.0, message));
    expectNear("ED-09", "normal: grade 1 now holds 95", 95.0, student.grades[0]);
    expectEqual<bool>("ED-10", "boundary: grade number 0 is rejected", false,
                      changeGrade(student, 0, 50.0, message));
    expectEqual<bool>("ED-11", "boundary: grade number 4 of 3 is rejected", false,
                      changeGrade(student, 4, 50.0, message));
    expectEqual<bool>("ED-12", "invalid: replacement grade -5 is rejected", false,
                      changeGrade(student, 2, -5.0, message));
    expectNear("ED-13", "invalid: a rejected replacement leaves the old grade", 90.0,
               student.grades[1]);

    expectEqual<bool>("ED-14", "normal: rename to 'Ana Reyes'", true,
                      renameStudent(student, "Ana Reyes", message));
    expectEqual<std::string>("ED-15", "normal: name changed", "Ana Reyes", student.name);
    expectEqual<bool>("ED-16", "invalid: rename to blank text is rejected", false,
                      renameStudent(student, "  ", message));
    expectEqual<std::string>("ED-17", "invalid: a rejected rename keeps the old name", "Ana Reyes",
                             student.name);

    std::vector<Student> students = sampleClass();
    expectEqual<bool>("ED-18", "normal: delete ID 3", true, removeStudent(students, 3, message));
    expectEqual<std::size_t>("ED-19", "normal: one fewer student remains", 4, students.size());
    expectEqual<bool>("ED-20", "invalid: delete ID 3 again fails", false,
                      removeStudent(students, 3, message));
    expectContains("ED-21", "invalid: message names the missing ID", message,
                   "Student ID 3 was not found.");
    std::vector<Student> nobody;
    expectEqual<bool>("ED-22", "boundary: delete from an empty list fails", false,
                      removeStudent(nobody, 1, message));

    expectEqual<bool>("ED-23", "normal: findStudent locates ID 5", true,
                      findStudent(students, 5) != nullptr);
    expectEqual<bool>("ED-24", "invalid: findStudent returns nothing for ID 999", true,
                      findStudent(students, 999) == nullptr);
}

}  // namespace

// ===== Part 2: integration tests with Member 3's storage =====
namespace {

const std::string TEST_FILE = "integration_test.csv";

// Saves, then loads into a fresh vector. Returns false if either step fails.
bool roundTrip(const std::vector<Student>& original, std::vector<Student>& reloaded) {
    std::string message;
    if (!saveStudents(original, TEST_FILE, message)) {
        std::cout << "         save failed: " << message << '\n';
        return false;
    }
    reloaded.clear();
    if (!loadStudents(reloaded, TEST_FILE, message)) {
        std::cout << "         load failed: " << message << '\n';
        return false;
    }
    return true;
}

std::vector<Student> sampleClassForStorage() {
    return {
        {1, "Ana", {90.0, 80.0}}, {2, "Ben", {100.0}}, {3, "Cy", {50.0, 59.0}}, {4, "Di", {}},
        {5, "Eli", {60.0}},
    };
}

void testSummaryAfterReload() {
    const std::vector<Student> original = sampleClassForStorage();
    std::vector<Student> reloaded;
    expectEqual<bool>("IN-01", "normal: sample class saves and reloads", true,
                      roundTrip(original, reloaded));

    const ClassSummary before = summarizeClass(original);
    const ClassSummary after = summarizeClass(reloaded);
    expectEqual<std::size_t>("IN-02", "class summary: student count survives a reload",
                             before.studentCount, after.studentCount);
    expectNear("IN-03", "class summary: class average survives a reload", before.classAverage,
               after.classAverage);
    expectEqual<std::size_t>("IN-04", "class summary: pass count survives a reload",
                             before.passCount, after.passCount);
    expectEqual<std::size_t>("IN-05", "boundary: a student with no grades survives a reload", 1,
                             after.noGradeCount);

    const std::vector<RankedStudent> rankBefore = rankStudents(original);
    const std::vector<RankedStudent> rankAfter = rankStudents(reloaded);
    expectEqual<std::size_t>("IN-06", "ranking: same number of ranked students after reload",
                             rankBefore.size(), rankAfter.size());
    expectEqual<std::string>("IN-07", "ranking: same winner after reload", rankBefore[0].score.name,
                             rankAfter[0].score.name);
}

void testEditsPersist() {
    std::vector<Student> students = sampleClassForStorage();
    std::string message;
    std::vector<Student> reloaded;

    changeGrade(*findStudent(students, 3), 2, 95.0, message);
    addGrade(*findStudent(students, 4), 88.5, message);
    expectEqual<bool>("IN-08", "normal: edited records save and reload", true,
                      roundTrip(students, reloaded));
    expectNear("IN-09", "an updated grade is still there after reload", 95.0,
               findStudent(reloaded, 3)->grades[1]);
    expectEqual<std::size_t>("IN-10",
                             "a grade added to a student with none is still there after reload", 1,
                             findStudent(reloaded, 4)->grades.size());

    expectEqual<bool>("IN-11", "normal: delete ID 2", true, removeStudent(students, 2, message));
    expectEqual<bool>("IN-12", "normal: records save after a delete", true,
                      roundTrip(students, reloaded));
    expectEqual<std::size_t>("IN-13", "a deleted student is gone after reload", 4, reloaded.size());
    expectEqual<bool>("IN-14", "a deleted student cannot be found after reload", true,
                      findStudent(reloaded, 2) == nullptr);
}

void testSpecialNamesPersist() {
    std::vector<Student> students = sampleClassForStorage();
    std::string message;
    std::vector<Student> reloaded;

    const std::string trickyName = "O'Brien, \"Jr.\"";
    expectEqual<bool>("IN-15", "boundary: rename to a name with a comma and quotes", true,
                      renameStudent(*findStudent(students, 1), trickyName, message));
    expectEqual<bool>("IN-16", "a record with a comma/quote name saves and reloads", true,
                      roundTrip(students, reloaded));
    expectEqual<std::string>("IN-17", "the comma/quote name is identical after reload", trickyName,
                             findStudent(reloaded, 1)->name);
}

void testEmptyAndInvalid() {
    std::vector<Student> reloaded;
    reloaded.push_back({99, "Leftover", {50.0}});
    expectEqual<bool>("IN-18", "boundary: an empty class saves and reloads", true,
                      roundTrip({}, reloaded));
    expectEqual<std::size_t>("IN-19", "boundary: reloading an empty file leaves no records", 0,
                             reloaded.size());

    // Edits that Member 2's code rejects must never leave a record that Member 3's save would
    // refuse.
    std::vector<Student> students = sampleClassForStorage();
    std::string message;
    changeGrade(*findStudent(students, 1), 1, 150.0, message);  // rejected
    addGrade(*findStudent(students, 1), -3.0, message);         // rejected
    renameStudent(*findStudent(students, 1), "   ", message);   // rejected
    expectEqual<bool>("IN-20", "invalid: after rejected edits the data still saves", true,
                      roundTrip(students, reloaded));
    expectEqual<std::string>("IN-21", "invalid: a rejected rename kept the original name", "Ana",
                             findStudent(reloaded, 1)->name);
}

}  // namespace

int main() {
    testLetterGrades();
    testClassSummary();
    testRanking();
    testValidationRules();
    testEditing();

    testSummaryAfterReload();
    testEditsPersist();
    testSpecialNamesPersist();
    testEmptyAndInvalid();
    std::remove(TEST_FILE.c_str());
    return testing::finish("member2_tests");
}
