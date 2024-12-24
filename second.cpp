#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
using namespace std;

class Course {
public:
    string courseName;
    string courseNumber;
    int creditHours;
    char grade;

    // Constructor
    Course(string cName = "", string cNumber = "", int credits = 0, char g = 'F')
        : courseName(cName), courseNumber(cNumber), creditHours(credits), grade(g) {
    }

    // Method to display course details
    void displayCourse() const {
        cout << setw(10) << courseNumber << " "
            << setw(15) << courseName << " "
            << setw(8) << creditHours << " "
            << setw(5) << grade << endl;
    }
};

class Person {
protected:
    string firstName;
    string lastName;

public:
    // Constructor
    Person(string fName = "", string lName = "") : firstName(fName), lastName(lName) {}

    // Method to get full name of the person
    string getFullName() const {
        return firstName + " " + lastName;
    }
};

class Student : public Person {
private:
    string studentID;
    bool isTuitionPaid;
    Course* courses;
    int numCourses;
    int tuitionRate;

    // Method to calculate GPA
    double calculateGPA() const {
        double totalPoints = 0;
        int totalCredits = 0;
        for (int i = 0; i < numCourses; ++i) {
            int points = 0;
            switch (courses[i].grade) {
            case 'A': points = 4; break;
            case 'B': points = 3; break;
            case 'C': points = 2; break;
            case 'D': points = 1; break;
            case 'F': points = 0; break;
            }
            totalPoints += points * courses[i].creditHours;
            totalCredits += courses[i].creditHours;
        }
        return totalCredits == 0 ? 0 : totalPoints / totalCredits;
    }

    // Method to calculate total credits
    int calculateTotalCredits() const {
        int totalCredits = 0;
        for (int i = 0; i < numCourses; ++i) {
            totalCredits += courses[i].creditHours;
        }
        return totalCredits;
    }

public:
    // Constructor
    Student(string fName = "", string lName = "", string id = "", bool tuitionPaid = false, int rate = 0, int nCourses = 0)
        : Person(fName, lName), studentID(id), isTuitionPaid(tuitionPaid), tuitionRate(rate), numCourses(nCourses) {
        if (numCourses > 0) {
            courses = new(nothrow) Course[numCourses];
            if (!courses) {
                cerr << "Memory allocation failed for courses." << endl;
                numCourses = 0;
            }
        }
        else {
            courses = nullptr;
        }
    }

    // Destructor
    ~Student() {
        delete[] courses;
    }

    // Method to add a course
    void addCourse(int index, const Course& course) {
        if (courses && index >= 0 && index < numCourses) {
            courses[index] = course;
        }
    }

    // Method to display student report
    void displayStudentReport(ostream& output) const {
        output << "Student Name: " << getFullName() << endl;
        output << "Student ID: " << studentID << endl;
        output << "Number of courses enrolled: " << numCourses << endl;

        if (isTuitionPaid) {
            output << left << setw(10) << "Course No" << setw(15) << "Course Name"
                << setw(8) << "Credits" << "Grade" << endl;
            for (int i = 0; i < numCourses; ++i) {
                output << setw(10) << courses[i].courseNumber << " "
                    << setw(15) << courses[i].courseName << " "
                    << setw(8) << courses[i].creditHours << " "
                    << setw(5) << courses[i].grade << endl;
            }
            output << "Total number of credits: " << calculateTotalCredits() << endl;
            output << "Mid-Semester GPA: " << fixed << setprecision(2) << calculateGPA() << endl;
        }
        else {
            output << "Tuition not paid. Grades are withheld." << endl;
            output << "Tuition due: $" << calculateTotalCredits() * tuitionRate << endl;
        }
    }

    // Method to sort courses based on course number
    void sortCourses() {
        if (courses) {
            sort(courses, courses + numCourses, [](const Course& a, const Course& b) {
                return a.courseNumber < b.courseNumber;
                });
        }
    }
};

// Main function
int main() {
    ifstream inputFile("input.txt");
    ofstream outputFile("output.txt");

    if (!inputFile) {
        cerr << "Error opening input file." << endl;
        return 1;
    }

    int numStudents, tuitionRate;
    inputFile >> numStudents >> tuitionRate;

    if (numStudents <= 0) {
        cerr << "Invalid number of students." << endl;
        return 1;
    }

    Student* students = new(nothrow) Student[numStudents];
    if (!students) {
        cerr << "Memory allocation failed for students." << endl;
        return 1;
    }

    // Read student data from input file
    for (int i = 0; i < numStudents; ++i) {
        string fName, lName, studentID;
        char tuitionPaid;
        int numCourses;

        inputFile >> fName >> lName >> studentID >> tuitionPaid >> numCourses;

        if (numCourses < 0) {
            cerr << "Invalid number of courses for student " << i + 1 << "." << endl;
            delete[] students;
            return 1;
        }

        students[i] = Student(fName, lName, studentID, tuitionPaid == 'Y', tuitionRate, numCourses);

        for (int j = 0; j < numCourses; ++j) {
            string courseName, courseNumber;
            int creditHours;
            char grade;

            inputFile >> courseName >> courseNumber >> creditHours >> grade;
            students[i].addCourse(j, Course(courseName, courseNumber, creditHours, grade));
        }

        students[i].sortCourses();
    }

    // Display student reports and write to output file
    for (int i = 0; i < numStudents; ++i) {
        students[i].displayStudentReport(cout);
        outputFile << endl;
        students[i].displayStudentReport(outputFile);
    }

    delete[] students;
    inputFile.close();
    outputFile.close();

    return 0;
}
