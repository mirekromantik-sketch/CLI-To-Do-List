#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <limits>
#include <stack>
#include <algorithm>
#include <thread>
#include <chrono>
#include <windows.h>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// ═══════════════════════════════════════════════════════════════════
// ANSI COLOR CODES
// ═══════════════════════════════════════════════════════════════════

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BRIGHT_RED     "\033[91m"
#define BRIGHT_GREEN   "\033[92m"
#define BRIGHT_YELLOW  "\033[93m"
#define BRIGHT_BLUE    "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN    "\033[96m"

// ═══════════════════════════════════════════════════════════════════
// DATA STRUCTURES
// ═══════════════════════════════════════════════════════════════════

enum class Priority { Low, Medium, High };

string priorityToString(Priority p) {
    if (p == Priority::High) return "H";
    if (p == Priority::Medium) return "M";
    return "L";
}

Priority intToPriority(int v) {
    if (v == 3) return Priority::High;
    if (v == 2) return Priority::Medium;
    return Priority::Low;
}

struct Task {
    string title;
    bool completed = false;
    Priority priority = Priority::Medium;
    string dueDate;
};

// ═══════════════════════════════════════════════════════════════════
// GLOBAL STATE
// ═══════════════════════════════════════════════════════════════════

stack<vector<Task>> undoStack;
stack<vector<Task>> redoStack;

// ═══════════════════════════════════════════════════════════════════
// UI DISPLAY FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

void setupConsole() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

string getGradientColor(int lineIndex, int frame) {
    const string colors[] = {
        "\033[38;5;196m", // Bright Red
        "\033[38;5;202m", // Orange-Red
        "\033[38;5;208m", // Orange
        "\033[38;5;214m", // Golden Orange
        "\033[38;5;220m", // Yellow-Orange
        "\033[38;5;226m", // Yellow
        "\033[38;5;190m", // Yellow-Green
        "\033[38;5;154m", // Light Green
        "\033[38;5;118m", // Green
        "\033[38;5;82m",  // Cyan-Green
        "\033[38;5;51m",  // Cyan
        "\033[38;5;45m",  // Bright Cyan
        "\033[38;5;39m",  // Light Blue
        "\033[38;5;33m",  // Blue
        "\033[38;5;27m",  // Deep Blue
        "\033[38;5;57m",  // Blue-Purple
        "\033[38;5;93m",  // Purple
        "\033[38;5;129m", // Magenta-Purple
        "\033[38;5;165m", // Magenta
        "\033[38;5;201m"  // Bright Magenta
    };
    int colorIndex = (lineIndex * 3 + frame) % 20;
    return colors[colorIndex];
}

void displayWelcomeBanner() {
    const int frames = 120;
    const int delay_ms = 100;
    
    for (int frame = 0; frame < frames; ++frame) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        
        cout << "\n";
        cout << "  " << getGradientColor(0, frame) << "╔═══════════════════════════════════════════════════════════════════════╗" << RESET << "\n";
        cout << "  " << getGradientColor(0, frame) << "║" << RESET << "                                                                       " << getGradientColor(0, frame) << "║" << RESET << "\n";
        
        cout << "  " << getGradientColor(1, frame) << "║" << RESET << "    " << getGradientColor(1, frame) << "████████╗ █████╗ ███████╗██╗  ██╗    ███╗   ███╗ █████╗ ███████╗" << RESET << " " << getGradientColor(1, frame) << "  ║" << RESET << "\n";
        cout << "  " << getGradientColor(2, frame) << "║" << RESET << "    " << getGradientColor(2, frame) << "╚══██╔══╝██╔══██╗██╔════╝██║ ██╔╝    ████╗ ████║██╔══██╗██╔════╝" << RESET << " " << getGradientColor(2, frame) << "  ║" << RESET << "\n";
        cout << "  " << getGradientColor(3, frame) << "║" << RESET << "       " << getGradientColor(3, frame) << "██║   ███████║███████╗█████╔╝     ██╔████╔██║███████║███████╗" << RESET << " " << getGradientColor(3, frame) << "  ║" << RESET << "\n";
        cout << "  " << getGradientColor(4, frame) << "║" << RESET << "       " << getGradientColor(4, frame) << "██║   ██╔══██║╚════██║██╔═██╗     ██║╚██╔╝██║██╔══██║╚════██║" << RESET << " " << getGradientColor(4, frame) << "  ║" << RESET << "\n";
        cout << "  " << getGradientColor(5, frame) << "║" << RESET << "       " << getGradientColor(5, frame) << "██║   ██║  ██║███████║██║  ██╗    ██║ ╚═╝ ██║██║  ██║███████║" << RESET << " " << getGradientColor(5, frame) << "  ║" << RESET << "\n";
        cout << "  " << getGradientColor(6, frame) << "║" << RESET << "       " << getGradientColor(6, frame) << "╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝    ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝" << RESET << " " << getGradientColor(6, frame) << "  ║" << RESET << "\n";
        
        cout << "  " << getGradientColor(7, frame) << "║" << RESET << "                                                                       " << getGradientColor(7, frame) << "║" << RESET << "\n";
        cout << "  " << getGradientColor(8, frame) << "║" << RESET << "              Advanced Task Management System                          " << getGradientColor(8, frame) << "║" << RESET << "\n";
        cout << "  " << getGradientColor(9, frame) << "║" << RESET << "           Priority Tracking • Undo/Redo • Search & Sort               " << getGradientColor(9, frame) << "║" << RESET << "\n";
        cout << "  " << getGradientColor(10, frame) << "║" << RESET << "                                                                       " << getGradientColor(10, frame) << "║" << RESET << "\n";
        cout << "  " << getGradientColor(11, frame) << "╚═══════════════════════════════════════════════════════════════════════╝" << RESET << "\n";
        
        this_thread::sleep_for(chrono::milliseconds(delay_ms));
    }
}

void displayMenu() {
    cout << "\n";
    cout << "  ============================= MAIN MENU =============================\n";
    cout << "  1. [+] Add Task             5. [?] Search Tasks\n";
    cout << "  2. [=] View Tasks           6. [*] Sort Tasks\n";
    cout << "  3. [~] Edit Task            7. [<] Undo\n";
    cout << "  4. [X] Delete Task          8. [>] Redo\n";
    cout << "                                9. [!] Exit\n";
    cout << "  ====================================================================\n";
}

// ═══════════════════════════════════════════════════════════════════
// INPUT HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

int readInt(const string& prompt) {
    int v;
    while (true) {
        cout << "  " << prompt;
        cin >> v;
        if (!cin.fail()) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return v;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  " << RED << "[X]" << RESET << " Invalid number. Please try again.\n";
    }
}

string readLine(const string& prompt) {
    cout << "  " << prompt;
    string s;
    getline(cin, s);
    return s;
}

void saveState(const vector<Task>& tasks) {
    undoStack.push(tasks);
    while (!redoStack.empty()) redoStack.pop();
}

// ═══════════════════════════════════════════════════════════════════
// FILE STORAGE FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

void saveTasks(const vector<Task>& tasks) {
    json j;
    j["tasks"] = json::array();

    for (const auto& t : tasks) {
        j["tasks"].push_back({
            {"title", t.title},
            {"completed", t.completed},
            {"priority", (int)t.priority},
            {"dueDate", t.dueDate}
        });
    }

    ofstream file("tasks.json");
    if (file.is_open()) {
        file << j.dump(4);
    }
}

void loadTasks(vector<Task>& tasks) {
    ifstream file("tasks.json");
    if (!file.is_open()) return;

    json j;

    try {
        file >> j;
    } catch (...) {
        cout << "  " << YELLOW << "[!]" << RESET << " Invalid JSON file. Starting fresh.\n";
        return;
    }

    if (j.contains("tasks") && j["tasks"].is_array()) {
        for (const auto& item : j["tasks"]) {
            Task task;
            task.title = item.value("title", "");
            task.completed = item.value("completed", false);
            task.priority = intToPriority(item.value("priority", 2));
            task.dueDate = item.value("dueDate", "");

            if (!task.title.empty()) {
                tasks.push_back(task);
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// UTILITY FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

bool taskExists(const vector<Task>& tasks, const string& title) {
    for (const auto& t : tasks) {
        if (t.title == title) {
            return true;
        }
    }
    return false;
}

string getPrioritySymbol(Priority p) {
    if (p == Priority::High) return RED "[H]" RESET;
    if (p == Priority::Medium) return YELLOW "[M]" RESET;
    return GREEN "[L]" RESET;
}

// ═══════════════════════════════════════════════════════════════════
// CORE FEATURE FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

void displayTasks(const vector<Task>& tasks) {
    if (tasks.empty()) {
        cout << "\n  [i] No tasks found. Add one to get started!\n";
        return;
    }

    cout << "\n  ========================== YOUR TASKS ==========================\n\n";

    for (size_t i = 0; i < tasks.size(); ++i) {
        cout << "  " << i + 1 << ". ";
        
        if (tasks[i].completed) {
            cout << "[V] ";
        } else {
            cout << "[ ] ";
        }
        
        cout << getPrioritySymbol(tasks[i].priority) << " " << tasks[i].title;
        
        if (!tasks[i].dueDate.empty()) {
            cout << " (Due: " << tasks[i].dueDate << ")";
        }
        cout << '\n';
    }
    
    cout << "\n  ================================================================\n";
}

void addTask(vector<Task>& tasks) {
    cout << "\n  ========================= ADD NEW TASK =========================\n\n";

    string title = readLine("Task title: ");
    
    if (title.empty()) {
        cout << "  " << RED << "[X]" << RESET << " Task title cannot be empty.\n";
        return;
    }
    
    if (taskExists(tasks, title)) {
        cout << "  " << RED << "[X]" << RESET << " A task with this title already exists.\n";
        return;
    }

    int p = readInt("Priority (1 = Low, 2 = Medium, 3 = High): ");
    string due = readLine("Due date (YYYY-MM-DD or leave empty): ");

    saveState(tasks);
    tasks.push_back({title, false, intToPriority(p), due});
    saveTasks(tasks);
    
    cout << "  " << GREEN << "[V]" << RESET << " Task added successfully!\n";
}

void deleteTask(vector<Task>& tasks) {
    if (tasks.empty()) {
        cout << "\n  [i] No tasks to delete.\n";
        return;
    }

    cout << "\n  ========================== DELETE TASK ==========================\n\n";

    displayTasks(tasks);
    
    int idx = readInt("Delete which task (number): ") - 1;
    
    if (idx < 0 || idx >= (int)tasks.size()) {
        cout << "  " << RED << "[X]" << RESET << " Invalid task number.\n";
        return;
    }

    string confirm = readLine("Confirm delete (y/n): ");
    if (confirm.empty() || tolower(confirm[0]) != 'y') {
        cout << "  " << RED << "[X]" << RESET << " Deletion cancelled.\n";
        return;
    }

    saveState(tasks);
    tasks.erase(tasks.begin() + idx);
    saveTasks(tasks);
    
    cout << "  " << GREEN << "[V]" << RESET << " Task deleted successfully!\n";
}

void editTask(vector<Task>& tasks) {
    if (tasks.empty()) {
        cout << "\n  [i] No tasks to edit.\n";
        return;
    }

    cout << "\n  =========================== EDIT TASK ===========================\n\n";

    displayTasks(tasks);
    
    int idx = readInt("Edit which task (number): ") - 1;
    
    if (idx < 0 || idx >= (int)tasks.size()) {
        cout << "  " << RED << "[X]" << RESET << " Invalid task number.\n";
        return;
    }

    saveState(tasks);
    Task& t = tasks[idx];

    cout << "\n  Current title: " << t.title << '\n';
    string nt = readLine("New title (leave empty to keep): ");
    if (!nt.empty() && !taskExists(tasks, nt)) {
        t.title = nt;
    }

    string c = readLine("Completed (y/n/leave empty to keep): ");
    if (!c.empty()) {
        t.completed = (tolower(c[0]) == 'y');
    }

    int np = readInt("Priority (1 = Low, 2 = Medium, 3 = High, 0 = keep): ");
    if (np >= 1 && np <= 3) {
        t.priority = intToPriority(np);
    }

    string nd = readLine("Due date (YYYY-MM-DD or leave empty): ");
    if (!nd.empty()) {
        t.dueDate = nd;
    }
    
    saveTasks(tasks);
    cout << "  " << GREEN << "[V]" << RESET << " Task updated successfully!\n";
}

void searchTasks(const vector<Task>& tasks) {
    if (tasks.empty()) {
        cout << "\n  [i] No tasks to search.\n";
        return;
    }

    cout << "\n  ========================== SEARCH TASKS =========================\n\n";

    string query = readLine("Search query: ");
    
    if (query.empty()) {
        cout << "  " << RED << "[X]" << RESET << " Search query cannot be empty.\n";
        return;
    }

    cout << "\n  Search results:\n\n";
    
    bool found = false;
    for (size_t i = 0; i < tasks.size(); ++i) {
        if (tasks[i].title.find(query) != string::npos) {
            cout << "  " << i + 1 << ". ";
            
            if (tasks[i].completed) {
                cout << "[V] ";
            } else {
                cout << "[ ] ";
            }
            
            cout << getPrioritySymbol(tasks[i].priority) << " " << tasks[i].title;
            
            if (!tasks[i].dueDate.empty()) {
                cout << " (Due: " << tasks[i].dueDate << ")";
            }
            cout << '\n';
            found = true;
        }
    }
    
    if (!found) {
        cout << "  [i] No tasks found matching \"" << query << "\"\n";
    }
    cout << '\n';
}

void sortTasks(vector<Task>& tasks) {
    if (tasks.empty()) {
        cout << "\n  [i] No tasks to sort.\n";
        return;
    }

    cout << "\n  =========================== SORT TASKS ===========================\n\n";

    int opt = readInt("Sort by (1 = Priority, 2 = Completion): ");
    
    saveState(tasks);

    if (opt == 1) {
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.priority > b.priority;
        });
        cout << "  " << GREEN << "[V]" << RESET << " Tasks sorted by priority!\n";
    } else if (opt == 2) {
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.completed < b.completed;
        });
        cout << "  " << GREEN << "[V]" << RESET << " Tasks sorted by completion status!\n";
    } else {
        cout << "  " << RED << "[X]" << RESET << " Invalid option.\n";
        undoStack.pop();
        return;
    }

    saveTasks(tasks);
}

void undo(vector<Task>& tasks) {
    if (undoStack.empty()) {
        cout << "\n  " << CYAN << "[i]" << RESET << " Nothing to undo.\n";
        return;
    }
    
    redoStack.push(tasks);
    tasks = undoStack.top();
    undoStack.pop();
    saveTasks(tasks);
    
    cout << "  " << GREEN << "[V]" << RESET << " Undo successful!\n";
}

void redo(vector<Task>& tasks) {
    if (redoStack.empty()) {
        cout << "\n  " << CYAN << "[i]" << RESET << " Nothing to redo.\n";
        return;
    }
    
    undoStack.push(tasks);
    tasks = redoStack.top();
    redoStack.pop();
    saveTasks(tasks);
    
    cout << "  " << GREEN << "[V]" << RESET << " Redo successful!\n";
}

// ═══════════════════════════════════════════════════════════════════
// MAIN FUNCTION
// ═══════════════════════════════════════════════════════════════════

int main() {
    setupConsole();
    
    vector<Task> tasks;
    loadTasks(tasks);

    displayWelcomeBanner();

    while (true) {
        displayMenu();

        int choice = readInt("Choice: ");

        switch (choice) {
            case 1:
                addTask(tasks);
                break;

            case 2:
                displayTasks(tasks);
                break;

            case 3:
                editTask(tasks);
                break;

            case 4:
                deleteTask(tasks);
                break;

            case 5:
                searchTasks(tasks);
                break;

            case 6:
                sortTasks(tasks);
                break;

            case 7:
                undo(tasks);
                break;

            case 8:
                redo(tasks);
                break;

            case 9:
                cout << "\n";
                cout << "  ============================================================\n";
                cout << "             Thank you for using Task Manager!\n";
                cout << "             Stay organized, stay productive!\n";
                cout << "  ============================================================\n";
                cout << "\n";
                saveTasks(tasks);
                return 0;

            default:
                cout << "  " << RED << "[X]" << RESET << " Invalid option. Please choose 1-9.\n";
                break;
        }
    }

    return 0;
}