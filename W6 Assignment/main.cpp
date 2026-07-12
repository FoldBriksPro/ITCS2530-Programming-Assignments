// ITCS 2530 - Week 08 Programming Assignment
// 3D Print Project Tracker
// Contributor: Farris Qureshi (working on Vaughn Chaudhuri's repo)
//
// NOTE: The original W1 codebase used STL vector and C++20
// features (starts_with), which are not permitted per Ch. 1-8
// requirements. This implementation only uses concepts from
// Chapters 1-8, fixing those issues.
//
// Week 08 changes:
//   - Added a PrintTracker class to encapsulate the project data
//     (struct array, daily-hours array, counts) and the operations
//     on that data (add, report, save, menu).
//   - The projects[] array of PrintProject structs and the
//     dailyHours[] array moved from local variables in main() into
//     private data members of PrintTracker.
//   - Added a constructor that sets the tracker to a safe starting
//     state (0 projects, hours log cleared).
//   - Added computeAverageDuration() as a new derived-value member
//     function.
//   - Added a second if/else block with a compound Boolean
//     condition inside addSession() to flag notable prints.

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
const int DAYS = 7;                   // days tracked in the daily log

// ── Enum: print difficulty level ────────────────────────────
enum DifficultyLevel
{
    BEGINNER = 1,
    INTERMEDIATE,
    ADVANCED
};

// ── Struct to hold one project's data ───────────────────────
// Stays globally defined per course rules; only instances of it
// (the projects[] array below) are declared locally, as a private
// member of the PrintTracker class, whose own object lives in main().
struct PrintProject
{
    string name;
    double printHours;
    double filamentKg;
    int    numColors;
    DifficultyLevel difficulty;
};

//  FREE FUNCTION PROTOTYPES (helpers used by the class)
void        setColor(int colorCode);
void        displayBanner();
int         displayMenu();
bool        validateHours(double hours);
bool        validateFilament(double kg);
bool        validateColors(int n);
DifficultyLevel getDifficulty();
double      calcAverage(const double values[], int size);
double      findHighest(const double values[], int size);
double      findLowest(const double values[], int size);
string      difficultyLabel(DifficultyLevel d);
int         findMostFilamentUsed(const PrintProject projects[], int count);

// ── PrintTracker class ──────────────────────────────────────
// Organizes the hobby data (struct array + daily log) and the
// operations that work on that data.
class PrintTracker
{
private:
    PrintProject projects[MAX_PROJECTS]; // array of structs - class member data
    int projectCount;
    double dailyHours[DAYS];
    bool hoursLogged;

public:
    PrintTracker(); // constructor - sets a safe starting state

    void   logDailyHours();
    void   addSession();
    void   showAllSessions() const;
    void   showReport() const;
    void   saveReportToFile(const string& filename) const;
    double computeAverageDuration() const;
    void   runMenu();
};

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
    cout << "     3D PRINT PROJECT TRACKER  v3.0         " << endl;
    cout << "      ITCS 2530 - Week 08 Assignment        " << endl;
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

//  findMostFilamentUsed  – processes array of structs,
//  returns index of the project that used the most filament
int findMostFilamentUsed(const PrintProject projects[], int count)
{
    int maxIndex = 0;

    // for loop: fixed number of iterations through the struct array
    for (int i = 1; i < count; i++)
    {
        if (projects[i].filamentKg > projects[maxIndex].filamentKg)
        {
            maxIndex = i;
        }
    }

    return maxIndex;
}

// ── PrintTracker member function definitions ────────────────

//  constructor – safe starting state: no projects, hours log cleared
PrintTracker::PrintTracker()
{
    projectCount = 0;
    hoursLogged = false;

    // for loop: fixed number of iterations to zero out the array
    for (int i = 0; i < DAYS; i++)
    {
        dailyHours[i] = 0.0;
    }
}

//  logDailyHours – fills the private dailyHours[] array
void PrintTracker::logDailyHours()
{
    setColor(11);
    cout << "\n--- Daily Print-Time Log (last " << DAYS << " days) ---" << endl;
    setColor(15);

    // for loop: fixed number of iterations
    for (int i = 0; i < DAYS; i++)
    {
        cout << "  Day " << (i + 1) << " hours printed: ";
        cin >> dailyHours[i];

        // array validation: ensure no negative or absurd entry
        while (cin.fail() || dailyHours[i] < 0.0 || dailyHours[i] > 24.0)
        {
            cin.clear();
            cin.ignore(1000, '\n');
            setColor(12);
            cout << "  Invalid (0-24 hrs). Re-enter day "
                 << (i + 1) << ": ";
            setColor(15);
            cin >> dailyHours[i];
        }
        cin.ignore(1000, '\n');
    }

    hoursLogged = true;
}

//  addSession – prompts the user and fills the next PrintProject
//  (do-while loop: keep adding until user says no or array full)
void PrintTracker::addSession()
{
    char addMore = 'y';

    do
    {
        if (projectCount >= MAX_PROJECTS)
        {
            setColor(12);
            cout << "  Project limit reached (" << MAX_PROJECTS << ")." << endl;
            setColor(15);
            break;
        }

        PrintProject p;

        setColor(11);
        cout << "\n--- Project #" << (projectCount + 1) << " ---" << endl;
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

        // NEW Week 08: second if/else block with a compound Boolean
        // condition (the first lives in showReport()). Gives the
        // user a quick heads-up based on filament amount + difficulty.
        if (p.filamentKg >= 10.0 && p.difficulty == ADVANCED)
        {
            setColor(12);
            cout << "  Heads up: big advanced print - budget extra time and filament." << endl;
        }
        else if (p.filamentKg < 2.0 && p.printHours < 1.0)
        {
            setColor(10);
            cout << "  Quick small print - good one for testing new settings." << endl;
        }
        setColor(15);

        projects[projectCount] = p;
        projectCount++;

        cout << "  Add another project? (y/n): ";
        cin >> addMore;
        cin.ignore(1000, '\n');

    } while ((addMore == 'y' || addMore == 'Y') && projectCount < MAX_PROJECTS);
}

//  showAllSessions – menu option 2 (while loop: iterate through projects)
void PrintTracker::showAllSessions() const
{
    if (projectCount == 0)
    {
        setColor(12);
        cout << "  No projects entered yet." << endl;
        setColor(15);
        return;
    }

    setColor(11);
    cout << "\n--- All Projects ---" << endl;
    setColor(15);

    int i = 0;
    while (i < projectCount)
    {
        cout << "  [" << (i + 1) << "] " << projects[i].name
             << "  |  " << projects[i].printHours << " hrs"
             << "  |  " << projects[i].filamentKg << " kg"
             << "  |  " << projects[i].numColors << " color(s)"
             << "  |  " << difficultyLabel(projects[i].difficulty) << endl;
        i++;
    }
}

//  computeAverageDuration – new Week 08 derived-value member function
double PrintTracker::computeAverageDuration() const
{
    if (projectCount == 0) return 0.0;

    double totalHours = 0.0;
    for (int i = 0; i < projectCount; i++)
    {
        totalHours += projects[i].printHours;
    }
    return totalHours / projectCount;
}

//  showReport – formatted summary table printed to console
void PrintTracker::showReport() const
{
    if (projectCount == 0)
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

    double totalFilament = 0.0;

    for (int i = 0; i < projectCount; i++)
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

        totalFilament += projects[i].filamentKg;
    }

    double avgHours   = computeAverageDuration();
    double totalHours = avgHours * projectCount;

    setColor(15);
    cout << string(66, '-') << endl;
    cout << left
         << setw(20) << "TOTALS"
         << setw(12) << fixed << setprecision(2) << totalHours
         << setw(12) << fixed << setprecision(3) << totalFilament
         << endl;

    setColor(14);
    cout << "\n  Avg print time per project : "
         << fixed << setprecision(2) << avgHours << " hrs" << endl;

    // struct array processing: show project with most filament used
    int topIndex = findMostFilamentUsed(projects, projectCount);
    cout << "  Most filament used          : "
         << projects[topIndex].name << " ("
         << fixed << setprecision(3) << projects[topIndex].filamentKg
         << " kg)" << endl;

    // daily log stats (array)
    if (hoursLogged)
    {
        cout << "\n--- Daily Print-Time Log Stats ---" << endl;
        cout << "  Days tracked : " << DAYS << endl;
        cout << "  Average      : "
             << fixed << setprecision(2) << calcAverage(dailyHours, DAYS) << " hrs/day" << endl;
        cout << "  Highest day  : "
             << fixed << setprecision(2) << findHighest(dailyHours, DAYS) << " hrs" << endl;
        cout << "  Lowest day   : "
             << fixed << setprecision(2) << findLowest(dailyHours, DAYS) << " hrs" << endl;
    }
    setColor(15);
}

//  saveReportToFile – writes formatted data to the given file
void PrintTracker::saveReportToFile(const string& filename) const
{
    ofstream outFile(filename.c_str());
    if (!outFile)
    {
        setColor(12);
        cout << "  ERROR: Could not open " << filename << " for writing." << endl;
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

    double totalFilament = 0.0;

    for (int i = 0; i < projectCount; i++)
    {
        outFile << left
                << setw(20) << projects[i].name
                << setw(12) << fixed << setprecision(2) << projects[i].printHours
                << setw(14) << fixed << setprecision(3) << projects[i].filamentKg
                << setw(8)  << projects[i].numColors
                << setw(14) << difficultyLabel(projects[i].difficulty)
                << endl;
        totalFilament += projects[i].filamentKg;
    }

    double avgHours   = computeAverageDuration();
    double totalHours = avgHours * projectCount;

    outFile << string(68, '-') << endl;
    outFile << left
            << setw(20) << "TOTALS"
            << setw(12) << fixed << setprecision(2) << totalHours
            << setw(14) << fixed << setprecision(3) << totalFilament
            << endl;

    outFile << endl;
    outFile << "  Avg print time per project : "
            << fixed << setprecision(2) << avgHours << " hrs" << endl;

    // array contents in file
    if (hoursLogged)
    {
        outFile << endl;
        outFile << "--- Daily Print-Time Log ---" << endl;
        for (int i = 0; i < DAYS; i++)
        {
            outFile << "  Day " << setw(3) << (i + 1) << ": "
                    << fixed << setprecision(2) << dailyHours[i] << " hrs" << endl;
        }
        outFile << endl;
        outFile << "  Average : "
                << fixed << setprecision(2) << calcAverage(dailyHours, DAYS) << " hrs/day" << endl;
        outFile << "  Highest : "
                << fixed << setprecision(2) << findHighest(dailyHours, DAYS) << " hrs" << endl;
        outFile << "  Lowest  : "
                << fixed << setprecision(2) << findLowest(dailyHours, DAYS) << " hrs" << endl;
    }

    outFile.close();
    setColor(10);
    cout << "  Report saved to " << filename << "!" << endl;
    setColor(15);
}

//  runMenu – displays the main menu and processes choices
//  (do-while loop keeps running until the user picks Exit)
void PrintTracker::runMenu()
{
    bool running = true;

    do
    {
        int choice = displayMenu();

        switch (choice)
        {
            case 1: // Add projects
                addSession();
                break;

            case 2: // View all projects
                showAllSessions();
                break;

            case 3: // Summary report to console
                showReport();
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
                    saveReportToFile("report.txt");
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
}

//  main
int main()
{
    // PrintTracker object - declared locally; its private struct
    // array and daily-hours array live safely inside it, set up
    // by the constructor above.
    PrintTracker tracker;

    displayBanner();

    // collect daily hours array first (required array feature)
    cout << "First, let's log your recent daily printing activity." << endl;
    tracker.logDailyHours();

    tracker.runMenu();

    return 0;
}