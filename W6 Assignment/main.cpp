// ITCS 2530 - Week 06 Programming Assignment
// 3D Print Project Tracker
// Contributor: Farris Qureshi (working on Vaughn Chaudhuri's repo)
//
// NOTE: The original W1 codebase used STL vector and C++20
// features (starts_with), which are not permitted per Ch. 1-8
// requirements. This W6 implementation rebuilds the program
// using only concepts from Chapters 1-8, fixing those issues.

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <windows.h>

using namespace std;

// ── Constants ───────────────────────────────────────────────
const int MAX_PROJECTS = 10;          // max projects in session
const double MAX_FILAMENT_KG = 50.0; // sanity cap for filament
const double MIN_HOURS = 0.1;        // shortest valid print time
const double MAX_HOURS = 500.0;      // longest valid print time
const int MAX_COLORS = 16;            // filament color slots

// ── Enum: print difficulty level ────────────────────────────
enum DifficultyLevel
{
    BEGINNER = 1,
    INTERMEDIATE,
    ADVANCED
};

// ── Struct to hold one project's data ───────────────────────
struct PrintProject
{
    string name;
    double printHours;
    double filamentKg;
    int    numColors;
    DifficultyLevel difficulty;
};

//  FUNCTION PROTOTYPES
void        setColor(int colorCode);
void        displayBanner();
int         displayMenu();
void        collectProjects(PrintProject projects[], int& count);
bool        validateHours(double hours);
bool        validateFilament(double kg);
bool        validateColors(int n);
DifficultyLevel getDifficulty();
void        fillHoursArray(double hoursArr[], int size);
double      calcAverage(const double values[], int size);
double      findHighest(const double values[], int size);
double      findLowest(const double values[], int size);
void        displaySummaryTable(const PrintProject projects[],
                                int count,
                                const double hoursArr[], int arrSize);
void        saveReport(const PrintProject projects[],
                       int count,
                       const double hoursArr[], int arrSize);
void        showAllProjects(const PrintProject projects[], int count);

//  setColor  – change Windows console text color
void setColor(int colorCode)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, colorCode);
}

//  displayBanner  – welcome screen
void displayBanner()
{
    setColor(11); // bright cyan
    cout << "=============================================" << endl;
    cout << "     3D PRINT PROJECT TRACKER  v2.0         " << endl;
    cout << "      ITCS 2530 - Week 06 Assignment        " << endl;
    cout << "=============================================" << endl;
    setColor(15); // white
    cout << "  Track your prints, filament, and time!    " << endl;
    cout << "---------------------------------------------" << endl;
    cout << endl;
}

//  displayMenu  – switch-based menu, returns user choice
int displayMenu()
{
    setColor(14); // yellow
    cout << endl;
    cout << "========== MAIN MENU ==========" << endl;
    setColor(15);
    cout << "  1. Add print projects" << endl;
    cout << "  2. View all projects" << endl;
    cout << "  3. View summary report" << endl;
    cout << "  4. Save report to file" << endl;
    cout << "  5. Exit" << endl;
    setColor(14);
    cout << "================================" << endl;
    setColor(15);
    cout << "Enter choice (1-5): ";

    int choice = 0;
    cin >> choice;

    // validate menu input
    while (cin.fail() || choice < 1 || choice > 5)
    {
        cin.clear();
        cin.ignore(1000, '\n');
        setColor(12); // red
        cout << "  Invalid choice. Enter 1-5: ";
        setColor(15);
        cin >> choice;
    }
    cin.ignore(1000, '\n');
    return choice;
}

//  validateHours / validateFilament / validateColors
bool validateHours(double hours)
{
    return (hours >= MIN_HOURS && hours <= MAX_HOURS);
}

bool validateFilament(double kg)
{
    return (kg > 0.0 && kg <= MAX_FILAMENT_KG);
}

bool validateColors(int n)
{
    return (n >= 1 && n <= MAX_COLORS);
}

//  getDifficulty  – prompts user to pick an enum value
DifficultyLevel getDifficulty()
{
    cout << "  Difficulty  1=Beginner  2=Intermediate  3=Advanced: ";
    int d = 0;
    cin >> d;

    while (cin.fail() || d < 1 || d > 3)
    {
        cin.clear();
        cin.ignore(1000, '\n');
        setColor(12);
        cout << "  Invalid. Enter 1, 2, or 3: ";
        setColor(15);
        cin >> d;
    }
    cin.ignore(1000, '\n');
    return static_cast<DifficultyLevel>(d);
}

//  collectProjects  – fill the projects array (do-while loop)
void collectProjects(PrintProject projects[], int& count)
{
    char addMore = 'y';

    do  // do-while: keep adding until user says no or array full
    {
        if (count >= MAX_PROJECTS)
        {
            setColor(12);
            cout << "  Project limit reached (" << MAX_PROJECTS << ")." << endl;
            setColor(15);
            break;
        }

        PrintProject p;

        setColor(11);
        cout << "\n--- Project #" << (count + 1) << " ---" << endl;
        setColor(15);

        // --- string input ---
        cout << "  Project name: ";
        getline(cin, p.name);
        while (p.name.empty())
        {
            setColor(12);
            cout << "  Name cannot be empty. Try again: ";
            setColor(15);
            getline(cin, p.name);
        }

        // --- double input: print hours ---
        cout << "  Estimated print time (hours): ";
        cin >> p.printHours;
        while (cin.fail() || !validateHours(p.printHours))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            setColor(12);
            cout << "  Invalid. Enter hours between "
                 << MIN_HOURS << " and " << MAX_HOURS << ": ";
            setColor(15);
            cin >> p.printHours;
        }
        cin.ignore(1000, '\n');

        // --- double input: filament ---
        cout << "  Filament used (kg): ";
        cin >> p.filamentKg;
        while (cin.fail() || !validateFilament(p.filamentKg))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            setColor(12);
            cout << "  Invalid. Enter kg between 0.001 and "
                 << MAX_FILAMENT_KG << ": ";
            setColor(15);
            cin >> p.filamentKg;
        }
        cin.ignore(1000, '\n');

        // --- int input: colors ---
        cout << "  Number of filament colors (1-" << MAX_COLORS << "): ";
        cin >> p.numColors;
        while (cin.fail() || !validateColors(p.numColors))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            setColor(12);
            cout << "  Invalid. Enter 1 to " << MAX_COLORS << ": ";
            setColor(15);
            cin >> p.numColors;
        }
        cin.ignore(1000, '\n');

        // --- enum input ---
        p.difficulty = getDifficulty();

        projects[count] = p;
        count++;

        cout << "  Add another project? (y/n): ";
        cin >> addMore;
        cin.ignore(1000, '\n');

    } while ((addMore == 'y' || addMore == 'Y') && count < MAX_PROJECTS);
}

//  fillHoursArray  – accepts array param, fills with prompt
//  (demonstrates array parameter; for loop fills elements)
void fillHoursArray(double hoursArr[], int size)
{
    setColor(11);
    cout << "\n--- Daily Print-Time Log (last " << size << " days) ---" << endl;
    setColor(15);

    // for loop: fixed number of iterations
    for (int i = 0; i < size; i++)
    {
        cout << "  Day " << (i + 1) << " hours printed: ";
        cin >> hoursArr[i];

        // array validation: ensure no negative or absurd entry
        while (cin.fail() || hoursArr[i] < 0.0 || hoursArr[i] > 24.0)
        {
            cin.clear();
            cin.ignore(1000, '\n');
            setColor(12);
            cout << "  Invalid (0-24 hrs). Re-enter day "
                 << (i + 1) << ": ";
            setColor(15);
            cin >> hoursArr[i];
        }
        cin.ignore(1000, '\n');
    }
}

//  calcAverage / findHighest / findLowest  (array params)
double calcAverage(const double values[], int size)
{
    if (size <= 0) return 0.0;
    double total = 0.0;
    for (int i = 0; i < size; i++)
        total += values[i];
    return total / size;
}

double findHighest(const double values[], int size)
{
    double high = values[0];
    for (int i = 1; i < size; i++)
        if (values[i] > high)
            high = values[i];
    return high;
}

double findLowest(const double values[], int size)
{
    double low = values[0];
    for (int i = 1; i < size; i++)
        if (values[i] < low)
            low = values[i];
    return low;
}

//  difficultyLabel  – helper: enum -> string
string difficultyLabel(DifficultyLevel d)
{
    if (d == BEGINNER)      return "Beginner";
    if (d == INTERMEDIATE)  return "Intermediate";
    return "Advanced";
}

//  showAllProjects  – menu option 2
void showAllProjects(const PrintProject projects[], int count)
{
    if (count == 0)
    {
        setColor(12);
        cout << "  No projects entered yet." << endl;
        setColor(15);
        return;
    }

    setColor(11);
    cout << "\n--- All Projects ---" << endl;
    setColor(15);

    // while loop: iterate through projects
    int i = 0;
    while (i < count)
    {
        cout << "  [" << (i + 1) << "] " << projects[i].name
             << "  |  " << projects[i].printHours << " hrs"
             << "  |  " << projects[i].filamentKg << " kg"
             << "  |  " << projects[i].numColors << " color(s)"
             << "  |  " << difficultyLabel(projects[i].difficulty) << endl;
        i++;
    }
}

//  displaySummaryTable  – formatted table to console
void displaySummaryTable(const PrintProject projects[],
                         int count,
                         const double hoursArr[], int arrSize)
{
    if (count == 0)
    {
        setColor(12);
        cout << "  No project data to display." << endl;
        setColor(15);
        return;
    }

    setColor(11);
    cout << "\n========== PROJECT SUMMARY REPORT ==========" << endl;
    setColor(15);

    // table header
    cout << left
         << setw(20) << "Project"
         << setw(12) << "Hrs"
         << setw(12) << "Filament(kg)"
         << setw(8)  << "Colors"
         << setw(14) << "Difficulty"
         << endl;
    cout << string(66, '-') << endl;

    // derived values
    double totalHours    = 0.0;
    double totalFilament = 0.0;

    for (int i = 0; i < count; i++)
    {
        // if/else with compound Boolean: flag long advanced prints
        if (projects[i].difficulty == ADVANCED && projects[i].printHours >= 20.0)
        {
            setColor(12); // red highlight for demanding prints
        }
        else if (projects[i].difficulty == BEGINNER && projects[i].printHours <= 2.0)
        {
            setColor(10); // green for quick beginner prints
        }
        else
        {
            setColor(15);
        }

        cout << left
             << setw(20) << projects[i].name
             << setw(12) << fixed << setprecision(2) << projects[i].printHours
             << setw(12) << fixed << setprecision(3) << projects[i].filamentKg
             << setw(8)  << projects[i].numColors
             << setw(14) << difficultyLabel(projects[i].difficulty)
             << endl;

        totalHours    += projects[i].printHours;
        totalFilament += projects[i].filamentKg;
    }

    setColor(15);
    cout << string(66, '-') << endl;
    cout << left
         << setw(20) << "TOTALS"
         << setw(12) << fixed << setprecision(2) << totalHours
         << setw(12) << fixed << setprecision(3) << totalFilament
         << endl;

    // derived: average hours per project
    double avgHours = totalHours / count;
    setColor(14);
    cout << "\n  Avg print time per project : "
         << fixed << setprecision(2) << avgHours << " hrs" << endl;

    // daily log stats (array)
    if (arrSize > 0)
    {
        cout << "\n--- Daily Print-Time Log Stats ---" << endl;
        cout << "  Days tracked : " << arrSize << endl;
        cout << "  Average      : "
             << fixed << setprecision(2) << calcAverage(hoursArr, arrSize) << " hrs/day" << endl;
        cout << "  Highest day  : "
             << fixed << setprecision(2) << findHighest(hoursArr, arrSize) << " hrs" << endl;
        cout << "  Lowest day   : "
             << fixed << setprecision(2) << findLowest(hoursArr, arrSize) << " hrs" << endl;
    }
    setColor(15);
}

//  saveReport  – writes formatted data to report.txt
void saveReport(const PrintProject projects[],
                int count,
                const double hoursArr[], int arrSize)
{
    ofstream outFile("report.txt");
    if (!outFile)
    {
        setColor(12);
        cout << "  ERROR: Could not open report.txt for writing." << endl;
        setColor(15);
        return;
    }

    outFile << "=============================================" << endl;
    outFile << "     3D PRINT PROJECT TRACKER - REPORT      " << endl;
    outFile << "=============================================" << endl;
    outFile << endl;

    outFile << left
            << setw(20) << "Project"
            << setw(12) << "Hrs"
            << setw(14) << "Filament(kg)"
            << setw(8)  << "Colors"
            << setw(14) << "Difficulty"
            << endl;
    outFile << string(68, '-') << endl;

    double totalHours    = 0.0;
    double totalFilament = 0.0;

    for (int i = 0; i < count; i++)
    {
        outFile << left
                << setw(20) << projects[i].name
                << setw(12) << fixed << setprecision(2) << projects[i].printHours
                << setw(14) << fixed << setprecision(3) << projects[i].filamentKg
                << setw(8)  << projects[i].numColors
                << setw(14) << difficultyLabel(projects[i].difficulty)
                << endl;
        totalHours    += projects[i].printHours;
        totalFilament += projects[i].filamentKg;
    }

    outFile << string(68, '-') << endl;
    outFile << left
            << setw(20) << "TOTALS"
            << setw(12) << fixed << setprecision(2) << totalHours
            << setw(14) << fixed << setprecision(3) << totalFilament
            << endl;

    outFile << endl;
    outFile << "  Avg print time per project : "
            << fixed << setprecision(2) << (totalHours / count) << " hrs" << endl;

    // array contents in file
    if (arrSize > 0)
    {
        outFile << endl;
        outFile << "--- Daily Print-Time Log ---" << endl;
        for (int i = 0; i < arrSize; i++)
        {
            outFile << "  Day " << setw(3) << (i + 1) << ": "
                    << fixed << setprecision(2) << hoursArr[i] << " hrs" << endl;
        }
        outFile << endl;
        outFile << "  Average : "
                << fixed << setprecision(2) << calcAverage(hoursArr, arrSize) << " hrs/day" << endl;
        outFile << "  Highest : "
                << fixed << setprecision(2) << findHighest(hoursArr, arrSize) << " hrs" << endl;
        outFile << "  Lowest  : "
                << fixed << setprecision(2) << findLowest(hoursArr, arrSize) << " hrs" << endl;
    }

    outFile.close();
    setColor(10);
    cout << "  Report saved to report.txt!" << endl;
    setColor(15);
}

//  main
int main()
{
    // project data array
    PrintProject projects[MAX_PROJECTS];
    int projectCount = 0;

    // daily hours array (fixed size: 7 days)
    const int DAYS = 7;
    double dailyHours[DAYS] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    bool hoursLogged = false;

    displayBanner();

    // collect daily hours array first (required array feature)
    cout << "First, let's log your recent daily printing activity." << endl;
    fillHoursArray(dailyHours, DAYS);
    hoursLogged = true;

    // main menu loop (do-while keeps running until exit)
    bool running = true;
    do
    {
        int choice = displayMenu();

        switch (choice)
        {
            case 1: // Add projects
                collectProjects(projects, projectCount);
                break;

            case 2: // View all projects
                showAllProjects(projects, projectCount);
                break;

            case 3: // Summary report to console
                displaySummaryTable(projects, projectCount,
                                    dailyHours, hoursLogged ? DAYS : 0);
                break;

            case 4: // Save report to file
                if (projectCount == 0)
                {
                    setColor(12);
                    cout << "  No projects to save. Add some first." << endl;
                    setColor(15);
                }
                else
                {
                    saveReport(projects, projectCount,
                               dailyHours, hoursLogged ? DAYS : 0);
                }
                break;

            case 5: // Exit
                running = false;
                setColor(11);
                cout << "\n  Thanks for using 3D Print Tracker. Happy printing!" << endl;
                setColor(15);
                break;

            default:
                setColor(12);
                cout << "  Unexpected option." << endl;
                setColor(15);
                break;
        }

    } while (running);

    return 0;
}