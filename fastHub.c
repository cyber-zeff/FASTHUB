#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#define MAX_STRING 100
#define MIN_COURSES 4
#define MAX_COURSES 8
#define MAX_STUDENTS 1000
#define MAX_COURSES_TOTAL 300
#define MAX_SEMESTERS 8

// Admin credentials
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "admin123"	

typedef struct {
    char code[MAX_STRING];
    float marks;
    int semester;
} StudentCourse;

typedef struct {
    char name[MAX_STRING];
    char email[MAX_STRING];
    char rollNumber[MAX_STRING];
    char program[MAX_STRING];
    int semester;
    char password[MAX_STRING];
    StudentCourse courses[MAX_COURSES * MAX_SEMESTERS];
    int numCourses;
    int numSelectedCourses;
    float semesterGPA[MAX_SEMESTERS];
    float cgpa;
} Student;

typedef struct {
    char name[MAX_STRING];
    char code[MAX_STRING];
    int semester;
    char program[MAX_STRING];
    int creditHours;
} Course;

// function declarations
void displayMenu();
bool adminLogin();
void adminMenu();
void studentMenu(Student* student);
void blockStudent(const char* rollNumber);
void generateTranscript(Student* student);
void blockStudent(const char* rollNumber);
void unblockStudent(const char* rollNumber);
bool isStudentBlocked(const char* rollNumber);
void changeSemester();
void selectCourses(Student* student);
void viewGPA(Student* student);
void generateTranscript(Student* student);
void registerStudent();
void addCourse();
void viewAllCourses();
void viewGPAHistory(const char* rollNumber);
void addMarks(Student* student);
void dropCourse(Student* student);
void saveStudentData(Student* student);
void loadStudentData(Student* student);
float calculateGPA(float* marks, int numCourses);
void viewAllStudents();
void viewBlockedStudents();
bool studentLogin(Student* student);

// functions actual code
bool isValidEmail(const char* email) {
    const char* at = strchr(email, '@');
    const char* dot = strrchr(email, '.');

    if (at == NULL || dot == NULL || at > dot) {
        return false;
    }

    if (at == email) {
        return false;
    }

    if (dot - at < 2) {
        return false;
    }

    if (*(dot + 1) == '\0') {
        return false;
    }

    return true;
}

void registerStudent() {
    Student newStudent;
    char tempProgram[MAX_STRING];
    
    printf("\033[1;34m");
    printf("\n=== Student Registration ===\n");
    printf("\033[0m");
    
    printf("Name: ");
    scanf(" %[^\n]s", newStudent.name);
    
    while (true) {
        printf("Email: ");
        scanf("%s", newStudent.email);
        if (isValidEmail(newStudent.email)) {
            break;
        } else {
        	printf("\033[1;31m");
            printf("Invalid email format. Please enter a valid email.\n");
            printf("\033[0m");
        }
    }
    
    printf("Roll Number: ");
    scanf("%s", newStudent.rollNumber);
    
    do {
        printf("Program (CS/SE/AI/CY/DS/CE): ");
        scanf("%s", tempProgram);
        if (strcmp(tempProgram, "CS") == 0 || strcmp(tempProgram, "SE") == 0 || 
            strcmp(tempProgram, "AI") == 0 || strcmp(tempProgram, "CY") == 0 || 
            strcmp(tempProgram, "DS") == 0 || strcmp(tempProgram, "CE") == 0) {
            strcpy(newStudent.program, tempProgram);
            break;
        }
        printf("\033[1;31m");
        printf("Invalid program! Please choose from CS, SE, AI, CY, DS, or CE.\n");
        printf("\033[0m");
    } while (1);
    
    newStudent.semester = 1;
    
    printf("Password: ");
    scanf("%s", newStudent.password);
    
    newStudent.numCourses = 0;
    newStudent.numSelectedCourses = 0;
    newStudent.cgpa = 0.0;
    for(int i = 0; i < MAX_SEMESTERS; i++) {
        newStudent.semesterGPA[i] = 0.0;
    }
    
    // Save to file
    FILE* file = fopen("students.txt", "a");
    if(file == NULL) {
    	printf("\033[1;31m");
        printf("Error: Unable to register student!\n");
        printf("\033[0m");
        return;
    }
    
    fprintf(file, "name:%s,email:%s,roll:%s,program:%s,semester:%d,password:%s\n",
            newStudent.name, newStudent.email, newStudent.rollNumber,
            newStudent.program, newStudent.semester, newStudent.password);
    fclose(file);
    
    printf("\033[1;32m");
    printf("\nStudent registered successfully!\n");
    printf("\033[0m");
}

void addCourse() {
    Course newCourse;
    
    printf("\033[1;34m");
    printf("\n=== Add New Course ===\n");
    printf("\033[0m");
    printf("Course Name: ");
    scanf(" %[^\n]s", newCourse.name);
    printf("Course Code: ");
    scanf("%s", newCourse.code);
    printf("Semester (1-8): ");
    scanf("%d", &newCourse.semester);
    printf("Program (CS/SE/CY/AI/DS/CE): ");
    scanf("%s", newCourse.program);
    printf("Credit Hours: ");
    scanf("%d", &newCourse.creditHours);
    
    FILE* file = fopen("courses.txt", "a");
    if(file == NULL) {
    	printf("\033[1;31m");
        printf("Error: Unable to add course!\n");
        printf("\033[0m");
        return;
    }
    
    fprintf(file, "\nname: %s,code: %s,semester: %d,program: %s,credit_hours: %d\n",
            newCourse.name, newCourse.code, newCourse.semester,
            newCourse.program, newCourse.creditHours);
    fclose(file);
    
    printf("\033[1;32m");
    printf("\nCourse added successfully!\n");
    printf("\033[0m");
}

void viewAllCourses() {
    FILE* file = fopen("courses.txt", "r");
    if(file == NULL) {
    	printf("\033[1;35m");
        printf("No courses found!\n");
        printf("\033[0m");
        return;
    }
    
    printf("\033[1;34m");
    printf("\n=== All Courses ===\n");
    printf("\033[0m");
    printf("\033[1;36m");
    printf("Code  |   Name   |    Program    |    Semester   |   Credit Hours\n");
    printf("-------------------------------------------------------------------\n");
    printf("\033[0m");
    
    char line[MAX_STRING * 5];
    char name[MAX_STRING], code[MAX_STRING], program[MAX_STRING];
    int semester, creditHours;
    
    while(fgets(line, sizeof(line), file)) {
        sscanf(line, "name: %[^,], code: %[^,], semester: %d, program: %[^,], credit_hours: %d",
               name, code, &semester, program, &creditHours);
        printf("%s  | %s  | %s  | %d  | %d\n", 
               code, name, program, semester, creditHours);
    }
    fclose(file);
}

void viewGPAHistory(const char* rollNumber) {
    char filename[MAX_STRING];
    sprintf(filename, "%s_data.txt", rollNumber);
    
    FILE* file = fopen(filename, "r");
    if(file == NULL) {
    	printf("\033[1;35m");
        printf("No GPA history found for this student!\n");
        printf("\033[0m");
        return;
    }
    
    printf("\033[1;34m");
    printf("\n=== GPA History for %s ===\n", rollNumber);
    printf("\033[0m");
    
    char line[MAX_STRING];
    while(fgets(line, sizeof(line), file)) {
        int sem;
        float gpa;
        if(sscanf(line, "semester_%d_gpa:%f", &sem, &gpa) == 2) {
            printf("Semester %d: %.2f\n", sem, gpa);
        }
        else if(sscanf(line, "cgpa:%f", &gpa) == 1) {
            printf("CGPA: %.2f\n", gpa);
        }
    }
    fclose(file);
}

void displayMenu() {
    system("cls || clear");
    printf("\033[1;36m");
    printf("\n=================================================\n");
    printf("           	WELCOME TO FASTHUB\n");
    printf("=================================================\n\n");
    printf("\033[0m");
    printf("1. Admin Login\n");
    printf("2. Student Login\n");
    printf("3. Register New Student\n");
    printf("\033[1;31m");
    printf("4. Exit\n");
    printf("\033[0m");
    printf("\033[1;36m");
    printf("\n=================================================\n");
    printf("\033[0m");
    printf("Enter your choice: ");
}

bool adminLogin() {
    char username[MAX_STRING];
    char password[MAX_STRING];
    
    printf("\033[1;34m");
    printf("\n=== Admin Login ===\n");
    printf("\033[0m");
    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);
    
    return (strcmp(username, ADMIN_USERNAME) == 0 && 
            strcmp(password, ADMIN_PASSWORD) == 0);
}

void adminMenu() {
    while(1) {
        system("cls || clear");
        printf("\033[1;36m");
        printf("\n=================================================\n");
        printf("              Admin Dashboard\n");
        printf("=================================================\n\n");
        printf("\033[0m");
        printf("1. Add New Course\n");
        printf("2. View All Courses\n");
        printf("3. View All Students\n");
        printf("4. View Blocked Students\n");
        printf("5. Block/Unblock Student\n");
        printf("6. Change Student Semester\n");
        printf("7. View Student GPA History\n");
        printf("\033[1;31m");
        printf("8. Logout\n");
        printf("\033[0m");
        printf("\033[1;36m");
        printf("\n=================================================\n");
        printf("\033[0m");
        printf("Enter your choice: ");
        
        int choice;
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                addCourse();
                break;
            case 2:
                viewAllCourses();
                break;
            case 3:
                viewAllStudents();
                break;
            case 4:
                viewBlockedStudents();
                break;
            case 5: {
                char rollNumber[MAX_STRING];
                int action;
                printf("Enter student roll number: ");
                scanf("%s", rollNumber);
                printf("1. Block\n2. Unblock\nChoice: ");
                scanf("%d", &action);
                if(action == 1) blockStudent(rollNumber);
                else unblockStudent(rollNumber);
                break;
            }
            case 6:
                changeSemester();
                break;
            case 7: {
                char rollNumber[MAX_STRING];
                printf("Enter student roll number: ");
                scanf("%s", rollNumber);
                viewGPAHistory(rollNumber);
                break;
            }
            case 8:
                return;
            default:
            	printf("\033[1;31m");
                printf("Invalid choice!\n");
                printf("\033[0m");
        }
        printf("\033[1;32m");
        printf("\nPress Enter to continue...");
        printf("\033[0m");
        getchar();
        getchar();
    }
}

void studentMenu(Student* student) {
    while(1) {
        system("cls || clear");
        printf("\033[1;36m");
        printf("\n=================================================\n");
        printf("               	  FASTHUB\n");
        printf("=================================================\n");
        printf("Welcome, %s!\n", student->name);
        printf("Program: %s | Semester: %d | CGPA: %.2f\n", 
               student->program, student->semester, student->cgpa);
		printf("=================================================\n\n");
		printf("\033[0m");
        printf("1. Select Courses\n");
        printf("2. View Selected Courses\n");
        printf("3. Add Marks\n");
        printf("4. Drop Course\n");
        printf("5. View GPA\n");
        printf("6. Generate Transcript\n");
        printf("\033[1;31m");
        printf("7. Logout\n");
        printf("\033[0m");
        printf("\033[1;36m");
        printf("\n=================================================\n\n");
        printf("\033[0m");
        printf("Enter your choice: ");
        
        int choice;
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                selectCourses(student);
                break;
            case 2:
            	printf("\033[1;34m");
                printf("\n=== Selected Courses ===\n");
                printf("\033[0m");
                for(int i = 0; i < student->numCourses; i++) {
                    if(student->courses[i].semester == student->semester) {
                        printf("%s (Marks: %.2f)\n", 
                               student->courses[i].code,
                               student->courses[i].marks);
                    }
                }
                break;
            case 3:
                addMarks(student);
                break;
            case 4:
                dropCourse(student);
                break;
            case 5:
                viewGPA(student);
                break;
            case 6:
                generateTranscript(student);
                break;
            case 7:
                return;
            default:
            	printf("\033[1;31m");
                printf("Invalid choice!\n");
                printf("\033[0m");
        }
        printf("\033[1;32m");
        printf("\nPress Enter to continue...");
        printf("\033[0m");
        getchar();
        getchar();
    }
}

bool studentLogin(Student* student) {
    char rollNumber[MAX_STRING];
    char password[MAX_STRING];
    
    printf("\033[1;34m");
    printf("\n=== Student Login ===\n");
    printf("\033[0m");
    printf("Roll Number: ");
    if(scanf("%s", rollNumber) != 1) {
    	printf("\033[1;31m");
        printf("Invalid input\n");
        printf("\033[0m");
        return false;
    }
    printf("Password: ");
    if(scanf("%s", password) != 1) {
    	printf("\033[1;31m");
        printf("Invalid input\n");
        printf("\033[0m");
        return false;
    }
    
    if(isStudentBlocked(rollNumber)) {
    	printf("\033[1;31m");
        printf("Your account is blocked. Please contact admin.\n");
        printf("\033[0m");
        return false;
    }
    
    FILE* file = fopen("students.txt", "r");
    if(file == NULL) {
    	printf("\033[1;31m");
        printf("Error accessing student records\n");
        printf("\033[0m");
        return false;
    }
    
    char line[MAX_STRING * 6];
    bool found = false;
    
    while(fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        
        memset(student, 0, sizeof(Student));
        
        if(sscanf(line, "name:%[^,],email:%[^,],roll:%[^,],program:%[^,],semester:%d,password:%s",
                  student->name, student->email, student->rollNumber,
                  student->program, &student->semester, student->password) == 6) {
            
            if(strcmp(rollNumber, student->rollNumber) == 0 && 
               strcmp(password, student->password) == 0) {
                found = true;
                break;
            }
        }
    }
    fclose(file);
    
    if(found) {
        char dataFilename[MAX_STRING];
        sprintf(dataFilename, "%s_data.txt", student->rollNumber);
        FILE* dataFile = fopen(dataFilename, "r");
        
        if(dataFile != NULL) {
            char line[MAX_STRING];
            while(fgets(line, sizeof(line), dataFile)) {
                line[strcspn(line, "\n")] = 0;
                
                if(strncmp(line, "courses:", 8) == 0) {
                    sscanf(line, "courses:%d", &student->numCourses);
                    for(int i = 0; i < student->numCourses; i++) {
                        fgets(line, sizeof(line), dataFile);
                        line[strcspn(line, "\n")] = 0;
                        sscanf(line, "%[^,],%f,%d", 
                               student->courses[i].code,
                               &student->courses[i].marks,
                               &student->courses[i].semester);
                    }
                }
                else if(strncmp(line, "semester_", 9) == 0) {
                    int sem;
                    float gpa;
                    if(sscanf(line, "semester_%d_gpa:%f", &sem, &gpa) == 2) {
                        if(sem > 0 && sem <= MAX_SEMESTERS) {
                            student->semesterGPA[sem-1] = gpa;
                        }
                    }
                }
                else if(strncmp(line, "cgpa:", 5) == 0) {
                    sscanf(line, "cgpa:%f", &student->cgpa);
                }
            }
            fclose(dataFile);
        }
        return true;
    } else {
    	printf("\033[1;31m");
        printf("Invalid roll number or password\n");
        printf("\033[0m");
        return false;
    }
}

void viewAllStudents() {
    FILE* file = fopen("students.txt", "r");
    if(file == NULL) {
    	printf("\033[1;35m");
        printf("No students found!\n");
        printf("\033[0m");
        return;
    }
    
    printf("\033[1;34m");
    printf("\n=== All Students ===\n");
    printf("\033[0m");
    printf("\033[1;36m");
    printf("Name | Roll Number | Program | Semester\n");
    printf("----------------------------------------\n");
    printf("\033[0m");
    
    char line[MAX_STRING * 6];
    char name[MAX_STRING], roll[MAX_STRING], program[MAX_STRING];
    int semester;
    
    while(fgets(line, sizeof(line), file)) {
        sscanf(line, "name:%[^,],email:%*[^,],roll:%[^,],program:%[^,],semester:%d",
               name, roll, program, &semester);
        printf("%s | %s | %s | %d\n", name, roll, program, semester);
    }
    fclose(file);
}

void viewBlockedStudents() {
    FILE* file = fopen("blocked_students.txt", "r");
    if(file == NULL) {
    	printf("\033[1;35m");
        printf("No blocked students!\n");
        printf("\033[0m");
        return;
    }
    
    printf("\033[1;34m");
    printf("\n=== Blocked Students ===\n");
    printf("\033[0m");
    printf("\033[1;36m");
    printf("Name | Roll Number | Email | Program\n");
    printf("----------------------------------------\n");
    printf("\033[0m");
    
    char line[MAX_STRING * 4];
    char name[MAX_STRING], email[MAX_STRING], roll[MAX_STRING], program[MAX_STRING];
    
    while(fgets(line, sizeof(line), file)) {
        sscanf(line, "name:%[^,],email:%[^,],roll:%[^,],program:%[^\n]",
               name, email, roll, program);
        printf("%s | %s | %s | %s\n", name, roll, email, program);
    }
    fclose(file);
}

bool isStudentBlocked(const char* rollNumber) {
    FILE* file = fopen("blocked_students.txt", "r");
    if(file == NULL) return false;
    
    char line[MAX_STRING * 4];
    char name[MAX_STRING];
    char email[MAX_STRING];
    char roll[MAX_STRING];
    
    while(fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        
        if(sscanf(line, "name:%[^,], email:%[^,], roll:%[^,]", 
                  name, email, roll) == 3) {
            char* p = roll;
            while(*p == ' ') p++;
            
            if(strcmp(rollNumber, p) == 0) {
                fclose(file);
                return true;
            }
        }
    }
    fclose(file);
    return false;
}

void blockStudent(const char* rollNumber) {
    if(isStudentBlocked(rollNumber)) {
    	printf("\033[1;31m");
        printf("Student is already blocked!\n");
        printf("\033[0m");
        return;
    }
    
    FILE* studentFile = fopen("students.txt", "r");
    if(studentFile == NULL) {
    	printf("\033[1;31m");
        printf("Error: Unable to access student records!\n");
        printf("\033[0m");
        return;
    }
    
    char line[MAX_STRING * 6];
    char name[MAX_STRING], email[MAX_STRING], program[MAX_STRING];
    bool found = false;
    
    while(fgets(line, sizeof(line), studentFile)) {
        char tempRoll[MAX_STRING];
        sscanf(line, "name:%[^,],email:%[^,],roll:%[^,],program:%[^,]",
               name, email, tempRoll, program);
        
        if(strcmp(rollNumber, tempRoll) == 0) {
            found = true;
            break;
        }
    }
    fclose(studentFile);
    
    if(!found) {
    	printf("\033[1;31m");
        printf("Student not found!\n");
        printf("\033[0m");
        return;
    }
    
    FILE* file = fopen("blocked_students.txt", "a");
    if(file == NULL) {
    	printf("\033[1;31m");
        printf("Error: Unable to block student!\n");
        printf("\033[0m");
        return;
    }
    
    fprintf(file, "name:%s,email:%s,roll:%s,program:%s\n", 
            name, email, rollNumber, program);
    fclose(file);
    
    printf("\033[1;32m");
    printf("Student blocked successfully!\n");
    printf("\033[0m");
}

void unblockStudent(const char* rollNumber) {
    if(!isStudentBlocked(rollNumber)) {
    	printf("\033[1;35m");
        printf("Student is not blocked!\n");
        printf("\033[0m");
        return;
    }
    
    FILE* file = fopen("blocked_students.txt", "r");
    FILE* temp = fopen("temp_blocked.txt", "w");
    
    if(file == NULL || temp == NULL) {
        if(file) fclose(file);
        if(temp) fclose(temp);
        printf("\033[1;31m");
        printf("Error: Unable to access blocked students file!\n");
        printf("\033[0m");
        return;
    }
    
    char line[MAX_STRING * 4];
    char name[MAX_STRING];
    char email[MAX_STRING];
    char roll[MAX_STRING];
    bool found = false;
    
    while(fgets(line, sizeof(line), file)) {
        char lineCopy[MAX_STRING * 4];
        strcpy(lineCopy, line);
        
        lineCopy[strcspn(lineCopy, "\n")] = 0;
        
        if(sscanf(line, "name:%[^,], email:%[^,], roll:%[^,]", 
                  name, email, roll) == 3) {
            char* p = roll;
            while(*p == ' ') p++;
            
            if(strcmp(rollNumber, p) != 0) {
                fprintf(temp, "%s", lineCopy);
                if(lineCopy[strlen(lineCopy)-1] != '\n') {
                    fprintf(temp, "\n");
                }
            } else {
                found = true;
            }
        } else {
            fprintf(temp, "%s", lineCopy);
            if(lineCopy[strlen(lineCopy)-1] != '\n') {
                fprintf(temp, "\n");
            }
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if(!found) {
        remove("temp_blocked.txt");
        printf("\033[1;31m");
        printf("Error: Student record not found in blocked list!\n");
        printf("\033[0m");
        return;
    }
    
    if(remove("blocked_students.txt") != 0) {
    	printf("\033[1;31m");
        printf("Error: Unable to update blocked students list!\n");
        printf("\033[0m");
        remove("temp_blocked.txt");
        return;
    }
    
    if(rename("temp_blocked.txt", "blocked_students.txt") != 0) {
    	printf("\033[1;31m");
        printf("Error: Unable to finalize unblocking operation!\n");
        printf("\033[0m");
        return;
    }
    
    printf("\033[1;32m");
    printf("Student unblocked successfully!\n");
    printf("\033[0m");
}

void changeSemester() {
    char rollNumber[MAX_STRING];
    int newSemester;
    
    printf("Enter student roll number: ");
    scanf("%s", rollNumber);
    printf("Enter new semester (1-8): ");
    scanf("%d", &newSemester);
    
    if(newSemester < 1 || newSemester > 8) {
    	printf("\033[1;31m");
        printf("Invalid semester number! Must be between 1 and 8.\n");
        printf("\033[0m");
        return;
    }
    
    FILE* file = fopen("students.txt", "r");
    FILE* temp = fopen("temp.txt", "w");
    if(file == NULL || temp == NULL) {
    	printf("\033[1;31m");
        printf("Error accessing files!\n");
        printf("\033[0m");
        return;
    }
    
    char line[MAX_STRING * 6];
    bool found = false;
    
    while(fgets(line, sizeof(line), file)) {
        char name[MAX_STRING], email[MAX_STRING], roll[MAX_STRING], 
             program[MAX_STRING], password[MAX_STRING];
        int semester;
        
        sscanf(line, "name:%[^,],email:%[^,],roll:%[^,],program:%[^,],semester:%d,password:%[^\n]",
               name, email, roll, program, &semester, password);
        
        if(strcmp(roll, rollNumber) == 0) {
            fprintf(temp, "name:%s,email:%s,roll:%s,program:%s,semester:%d,password:%s\n",
                    name, email, roll, program, newSemester, password);
            found = true;
        } else {
            fprintf(temp, "%s", line);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if(!found) {
    	printf("\033[1;31m");
        printf("Student not found!\n");
        printf("\033[0m");
        remove("temp.txt");
        return;
    }
    
    remove("students.txt");
    rename("temp.txt", "students.txt");
    
    printf("\033[1;32m");
    printf("Semester updated successfully!\n");
    printf("\033[0m");
}

void selectCourses(Student* student) {
    FILE* file = fopen("courses.txt", "r");
    if(file == NULL) {
    	printf("\033[1;35m");
        printf("No courses available!\n");
        printf("\033[0m");
        return;
    }
    
    printf("\033[1;36m");
    printf("\n=== Available Courses for %s Program, Semester %d ===\n",
           student->program, student->semester);
    printf("Code | Name | Credit Hours\n");
    printf("-------------------------\n");
    printf("\033[0m");
    
    Course courses[MAX_COURSES_TOTAL];
    int numCourses = 0;
    char line[MAX_STRING * 5];
    
    while(fgets(line, sizeof(line), file) && numCourses < MAX_COURSES_TOTAL) {
        Course course;
        sscanf(line, "name: %[^,], code: %[^,], semester: %d, program: %[^,], credit_hours: %d",
               course.name, course.code, &course.semester,
               course.program, &course.creditHours);
               
        if(course.semester == student->semester && 
           strcmp(course.program, student->program) == 0) {
            courses[numCourses++] = course;
            printf("%d. %s | %s | %d\n", 
                   numCourses, course.code, course.name, course.creditHours);
        }
    }
    fclose(file);
    
    if(numCourses == 0) {
    	printf("\033[1;35m");
        printf("No courses available for your semester and program!\n");
        printf("\033[0m");
        return;
    }
    
    printf("\nSelect courses (enter course numbers, 0 to finish (e.g: 1 2 3 0)):\n");
    int selectedCount = 0;
    
    while(selectedCount < MAX_COURSES) {
        int choice;
        scanf("%d", &choice);
        
        if(choice == 0) break;
        if(choice < 1 || choice > numCourses) {
        	printf("\033[1;31m");
            printf("Invalid course number!\n");
            printf("\033[0m");
            continue;
        }
        
        strcpy(student->courses[student->numCourses].code, courses[choice-1].code);
        student->courses[student->numCourses].marks = 0;
        student->courses[student->numCourses].semester = student->semester;
        student->numCourses++;
        selectedCount++;
        
        printf("\033[1;32m");
        printf("Course %s added successfully!\n", courses[choice-1].code);
        printf("\033[0m");
    }
    
    student->numSelectedCourses = selectedCount;
    
    saveStudentData(student);
    printf("\033[1;32m");
    printf("\nCourses saved successfully!\n");
    printf("\033[0m");
}

void viewGPA(Student* student) {
	printf("\033[1;35m");
    printf("\n=== GPA Information ===\n");
    printf("\033[0m");
    printf("Current Semester: %d\n", student->semester);
    printf("Current Semester GPA: %.2f\n", 
           student->semesterGPA[student->semester - 1]);
    printf("CGPA: %.2f\n", student->cgpa);
    
    printf("\nGPA History:\n");
    for(int i = 0; i < student->semester; i++) {
        printf("Semester %d: %.2f\n", i + 1, student->semesterGPA[i]);
    }
}

void generateTranscript(Student* student) {
    char filename[MAX_STRING];
    sprintf(filename, "student_%s_transcript.txt", student->rollNumber);
    
    FILE* file = fopen(filename, "w");
    if(file == NULL) {
    	printf("\033[1;31m");
        printf("Error: Unable to generate transcript!\n");
        printf("\033[0m");
        return;
    }
    
	fprintf(file, "==========================================\n");
    fprintf(file, "           OFFICIAL TRANSCRIPT\n");
    fprintf(file, "==========================================\n\n");
    
    fprintf(file, "Student Name: %s\n", student->name);
    fprintf(file, "Roll Number: %s\n", student->rollNumber);
    fprintf(file, "Program: %s\n", student->program);
    fprintf(file, "Current Semester: %d\n", student->semester);
    fprintf(file, "CGPA: %.2f\n\n", student->cgpa);
    
    for(int i = 0; i < student->semester; i++) {
        fprintf(file, "----------------------------------------\n");
        fprintf(file, "Semester %d\n", i + 1);
        fprintf(file, "GPA: %.2f\n", student->semesterGPA[i]);
        
        char coursesFile[MAX_STRING];
        sprintf(coursesFile, "student_%s_courses.txt", student->rollNumber);
        FILE* courses = fopen(coursesFile, "r");
        
        if(courses != NULL) {
            char course[MAX_STRING];
            fprintf(file, "\nCourses:\n");
            while(fgets(course, sizeof(course), courses)) {
                course[strcspn(course, "\n")] = 0;
                fprintf(file, "- %s\n", course);
            }
            fclose(courses);
        }
        fprintf(file, "\n");
    }
    
    fprintf(file, "==========================================\n");
    fprintf(file, "End of Transcript\n");
    fprintf(file, "Generated on: ");
    
    time_t t = time(NULL);
    struct tm* current_time = localtime(&t);
    char date[MAX_STRING];
    strftime(date, sizeof(date), "%Y-%m-%d", current_time);
    fprintf(file, "%s\n", date);
    
    fclose(file);
    printf("\033[1;32m");
    printf("Transcript generated successfully!\n");
    printf("\033[0m");
    printf("Saved as: %s\n", filename);
}

void saveStudentData(Student* student) {
    char filename[MAX_STRING];
    sprintf(filename, "%s_data.txt", student->rollNumber);
    
    FILE* file = fopen(filename, "w");
    if(file == NULL) {
    	printf("\033[1;31m");
        printf("Error saving student data!\n");
        printf("\033[0m");
        return;
    }
    
    fprintf(file, "name:%s\n", student->name);
    fprintf(file, "email:%s\n", student->email);
    fprintf(file, "program:%s\n", student->program);
    fprintf(file, "semester:%d\n", student->semester);
    
    fprintf(file, "courses:%d\n", student->numCourses);
    for(int i = 0; i < student->numCourses; i++) {
        fprintf(file, "%s,%f,%d\n", 
                student->courses[i].code, 
                student->courses[i].marks,
                student->courses[i].semester);
    }
    
    for(int i = 0; i < MAX_SEMESTERS; i++) {
        fprintf(file, "semester_%d_gpa:%f\n", i+1, student->semesterGPA[i]);
    }
    fprintf(file, "cgpa:%f\n", student->cgpa);
    
    fclose(file);
}

void loadStudentData(Student* student) {
    char filename[MAX_STRING];
    sprintf(filename, "%s_data.txt", student->rollNumber);
    
    FILE* file = fopen(filename, "r");
    if(file == NULL) return;
    
    char line[MAX_STRING];
    char key[MAX_STRING];
    char value[MAX_STRING];
    
    while(fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        
        if(sscanf(line, "courses:%d", &student->numCourses) == 1) {
            for(int i = 0; i < student->numCourses; i++) {
                fgets(line, sizeof(line), file);
                sscanf(line, "%[^,],%f,%d", 
                       student->courses[i].code,
                       &student->courses[i].marks,
                       &student->courses[i].semester);
            }
        }
        else if(sscanf(line, "semester_%*d_gpa:%f", &student->semesterGPA[0]) == 1) {
            int sem;
            float gpa;
            sscanf(line, "semester_%d_gpa:%f", &sem, &gpa);
            student->semesterGPA[sem-1] = gpa;
        }
        else if(sscanf(line, "cgpa:%f", &student->cgpa) == 1) {
            continue;
        }
        else if(sscanf(line, "%[^:]:%[^\n]", key, value) == 2) {
            if(strcmp(key, "name") == 0) strcpy(student->name, value);
            else if(strcmp(key, "email") == 0) strcpy(student->email, value);
            else if(strcmp(key, "program") == 0) strcpy(student->program, value);
            else if(strcmp(key, "semester") == 0) student->semester = atoi(value);
        }
    }
    fclose(file);
}

float calculateGPA(float* marks, int numCourses) {
    if(numCourses == 0) return 0.0;
    
    float totalGradePoints = 0.0;
    int totalCourses = 0;
    
    for(int i = 0; i < numCourses; i++) {
        float gradePoint;
        if(marks[i] >= 85) gradePoint = 4.0;
        else if(marks[i] >= 80) gradePoint = 3.7;
        else if(marks[i] >= 75) gradePoint = 3.3;
        else if(marks[i] >= 70) gradePoint = 3.0;
        else if(marks[i] >= 65) gradePoint = 2.7;
        else if(marks[i] >= 60) gradePoint = 2.3;
        else if(marks[i] >= 55) gradePoint = 2.0;
        else if(marks[i] >= 50) gradePoint = 1.7;
        else gradePoint = 0.0;
        
        totalGradePoints += gradePoint;
        totalCourses++;
    }
    
    return totalGradePoints / totalCourses;
}

void addMarks(Student* student) {
	printf("\033[1;34m");
    printf("\n=== Add Marks for Selected Courses ===\n");
    printf("\033[0m");
    
    float semesterMarks[MAX_COURSES];
    int numSemesterCourses = 0;
    
    for(int i = 0; i < student->numCourses; i++) {
        if(student->courses[i].semester == student->semester) {
            printf("Enter marks for %s (0-100): ", student->courses[i].code);
            float marks;
            scanf("%f", &marks);
            
            if(marks < 0 || marks > 100) {
            	printf("\033[1;31m");
                printf("Invalid marks! Must be between 0 and 100.\n");
                printf("\033[0m");
                i--;
                continue;
            }
            
            student->courses[i].marks = marks;
            semesterMarks[numSemesterCourses++] = marks;
        }
    }
    
    student->semesterGPA[student->semester - 1] = calculateGPA(semesterMarks, numSemesterCourses);
    
    float totalGPA = 0;
    int completedSemesters = 0;
    for(int i = 0; i < student->semester; i++) {
        if(student->semesterGPA[i] > 0) {
            totalGPA += student->semesterGPA[i];
            completedSemesters++;
        }
    }
    student->cgpa = completedSemesters > 0 ? totalGPA / completedSemesters : 0;
    
    saveStudentData(student);
    
    printf("\033[1;32m");
    printf("\nMarks added successfully!\n");
    printf("\033[0m");
}

void dropCourse(Student* student) {
	printf("\033[1;34m");
    printf("\n=== Drop Course ===\n");
    printf("\033[0m");
    printf("Currently selected courses for semester %d:\n", student->semester);
    
    int currentSemesterCourses[MAX_COURSES];
    int numCurrentCourses = 0;
    
    for(int i = 0; i < student->numCourses; i++) {
        if(student->courses[i].semester == student->semester) {
            printf("%d. %s\n", numCurrentCourses + 1, student->courses[i].code);
            currentSemesterCourses[numCurrentCourses++] = i;
        }
    }
    
    if(numCurrentCourses == 0) {
    	printf("\033[1;35m");
        printf("No courses selected for current semester!\n");
        printf("\033[0m");
        return;
    }
    
    printf("\nEnter course number to drop (0 to cancel): ");
    int choice;
    scanf("%d", &choice);
    
    if(choice == 0) return;
    if(choice < 1 || choice > numCurrentCourses) {
    	printf("\033[1;31m");
        printf("Invalid course number!\n");
        printf("\033[0m");
        return;
    }
    
    int courseIndex = currentSemesterCourses[choice - 1];
    
    for(int i = courseIndex; i < student->numCourses - 1; i++) {
        student->courses[i] = student->courses[i + 1];
    }
    student->numCourses--;
    
    saveStudentData(student);
    
    printf("\033[1;32m");
    printf("Course dropped successfully!\n");
    printf("\033[0m");
}

int main() {
    int choice;
    Student currentStudent;
    
    while(1) {
        displayMenu();
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                if(adminLogin()) {
                	printf("\033[1;32m");
                    printf("\nAdmin login successful!\n");
                    printf("Press Enter to continue...");
                    printf("\033[0m");
                    getchar();
                    getchar();
                    adminMenu();
                } else {
                	printf("\033[1;31m");
                    printf("\nInvalid credentials!\n");
                    printf("\033[0m");
                }
                break;
                
            case 2:
                if(studentLogin(&currentStudent)) {
                	printf("\033[1;32m");
                    printf("\nLogin successful!\n");
                    printf("Press Enter to continue...");
                    printf("\033[0m");
                    getchar();
                    getchar();
                    studentMenu(&currentStudent);
                } else {
                	printf("\033[1;31m");
                    printf("\nInvalid credentials or account blocked!\n");
                    printf("\033[0m");
                }
                break;
                
            case 3:
                registerStudent();
                break;
                
            case 4:
            	printf("\033[1;32m");
                printf("\nThank you for using FastHub!\n");
                printf("\033[0m");
                return 0;
                
            default:
            	printf("\033[1;31m");
                printf("\nInvalid choice!\n");
                printf("\033[0m");
        }
        
        printf("\033[1;32m");
        printf("\nPress Enter to continue...");
        printf("\033[0m");
        getchar();
        getchar();
    }
    
    return 0;
}