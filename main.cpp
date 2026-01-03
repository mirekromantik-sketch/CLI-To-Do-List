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
#define RED     "\033[38;5;131m"  // Muted red
#define GREEN   "\033[38;5;108m"  // Sage green
#define YELLOW  "\033[38;5;179m"  // Muted gold
#define BLUE    "\033[38;5;67m"   // Slate blue
#define MAGENTA "\033[38;5;132m"  // Dusty magenta
#define CYAN    "\033[38;5;73m"   // Muted cyan
#define BRIGHT_RED     "\033[38;5;167m"  // Soft coral
#define BRIGHT_GREEN   "\033[38;5;114m"  // Soft green
#define BRIGHT_YELLOW  "\033[38;5;185m"  // Soft yellow
#define BRIGHT_BLUE    "\033[38;5;74m"   // Soft blue
#define BRIGHT_MAGENTA "\033[38;5;139m"  // Soft mauve
#define BRIGHT_CYAN    "\033[38;5;80m"   // Soft teal

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

string getGradientColor(int lineIndex, int colIndex, int frame) {
    const string colors[] = {
        "\033[38;5;131m", // Muted red
        "\033[38;5;137m", // Terracotta
        "\033[38;5;173m", // Soft orange
        "\033[38;5;179m", // Muted gold
        "\033[38;5;185m", // Soft yellow
        "\033[38;5;186m", // Pale yellow
        "\033[38;5;150m", // Yellow-sage
        "\033[38;5;114m", // Soft green
        "\033[38;5;108m", // Sage
        "\033[38;5;72m",  // Teal-green
        "\033[38;5;73m",  // Muted cyan
        "\033[38;5;80m",  // Soft teal
        "\033[38;5;74m",  // Soft blue
        "\033[38;5;67m",  // Slate blue
        "\033[38;5;61m",  // Deeper slate
        "\033[38;5;97m",  // Soft purple
        "\033[38;5;103m", // Lavender
        "\033[38;5;139m", // Mauve
        "\033[38;5;132m", // Dusty magenta
        "\033[38;5;168m"  // Soft rose
    };
    // 45-degree diagonal: both lineIndex and colIndex contribute equally
    int colorIndex = (lineIndex + colIndex + frame) % 20;
    return colors[colorIndex];
}

void displayWelcomeBanner() {
    const int frames = 40;
    const int delay_ms = 80;
    
    for (int frame = 0; frame < frames; ++frame) {
        // Use ANSI cursor positioning instead of cls to reduce flicker
        cout << "\033[H\033[2J"; // Clear screen and move to home
        
        cout << "\n";
        cout << "  " << getGradientColor(0, 0, frame) << "╔════════════════════════════════════════════════════════════════════════════════════════════════╗" << RESET << "\n";
        cout << "  " << getGradientColor(0, 0, frame) << "║" << RESET << "                                                                                                " << getGradientColor(0, 95, frame) << "║" << RESET << "\n";
        
        cout << "  " << getGradientColor(1, 0, frame) << "║" << RESET << "    " << getGradientColor(1, 4, frame) << "████████╗ █████╗ ███████╗██╗  ██╗    ███╗   ███╗ █████╗ ███████╗████████╗███████╗██████╗ " << RESET << " " << getGradientColor(1, 95, frame) << "  ║" << RESET << "\n";
        cout << "  " << getGradientColor(2, 0, frame) << "║" << RESET << "    " << getGradientColor(2, 4, frame) << "╚══██╔══╝██╔══██╗██╔════╝██║ ██╔╝    ████╗ ████║██╔══██╗██╔════╝╚══██╔══╝██╔════╝██╔══██╗" << RESET << " " << getGradientColor(2, 95, frame) << "  ║" << RESET << "\n";
        cout << "  " << getGradientColor(3, 0, frame) << "║" << RESET << "       " << getGradientColor(3, 7, frame) << "██║   ███████║███████╗█████╔╝     ██╔████╔██║███████║███████╗   ██║   █████╗  ██████╔╝" << RESET << " " << getGradientColor(3, 95, frame) << "  ║" << RESET << "\n";
        cout << "  " << getGradientColor(5, 0, frame) << "║" << RESET << "       " << getGradientColor(5, 7, frame) << "██║   ██╔══██║╚════██║██╔═██╗     ██║╚██╔╝██║██╔══██║╚════██║   ██║   ██╔══╝  ██╔══██╗" << RESET << " " << getGradientColor(5, 95, frame) << "  ║" << RESET << "\n";
        cout << "  " << getGradientColor(6, 0, frame) << "║" << RESET << "       " << getGradientColor(6, 7, frame) << "██║   ██║  ██║███████║██║  ██╗    ██║ ╚═╝ ██║██║  ██║███████║   ██║   ███████╗██║  ██║" << RESET << " " << getGradientColor(6, 95, frame) << "  ║" << RESET << "\n";
        cout << "  " << getGradientColor(7, 0, frame) << "║" << RESET << "       " << getGradientColor(7, 7, frame) << "╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝    ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝   ╚═╝   ╚══════╝╚═╝  ╚═╝" << RESET << " " << getGradientColor(7, 95, frame) << "  ║" << RESET << "\n";
        cout << "  " << getGradientColor(8, 0, frame) << "║" << RESET << "                                                                                                " << getGradientColor(8, 95, frame) << "║" << RESET << "\n";
        cout << "  " << getGradientColor(9, 0, frame) << "║" << RESET << "                                 " << getGradientColor(9, 33, frame) << "Advanced Task Management System" << RESET << "                                " << getGradientColor(9, 95, frame) << "║" << RESET << "\n";
        cout << "  " << getGradientColor(10, 0, frame) << "║" << RESET << "                           " << getGradientColor(10, 27, frame) << "Priority Tracking • Undo/Redo • Search & Sort" << RESET << "                        " << getGradientColor(10, 95, frame) << "║" << RESET << "\n";
        cout << "  " << getGradientColor(11, 0, frame) << "║" << RESET << "                                                                                                " << getGradientColor(11, 95, frame) << "║" << RESET << "\n";
        cout << "  " << getGradientColor(12, 0, frame) << "╚════════════════════════════════════════════════════════════════════════════════════════════════╝" << RESET << "\n";
        
        cout << flush;
        this_thread::sleep_for(chrono::milliseconds(delay_ms));
    }
    
    // Display final static banner with last frame colors
    cout << "\033[H\033[2J"; // Clear screen
    cout << "\n";
    cout << "  " << getGradientColor(0, 0, frames-1) << "╔════════════════════════════════════════════════════════════════════════════════════════════════╗" << RESET << "\n";
    cout << "  " << getGradientColor(0, 0, frames-1) << "║" << RESET << "                                                                                                " << getGradientColor(0, 95, frames-1) << "║" << RESET << "\n";
    
    cout << "  " << getGradientColor(1, 0, frames-1) << "║" << RESET << "    " << getGradientColor(1, 4, frames-1) << "████████╗ █████╗ ███████╗██╗  ██╗    ███╗   ███╗ █████╗ ███████╗████████╗███████╗██████╗ " << RESET << " " << getGradientColor(1, 95, frames-1) << "  ║" << RESET << "\n";
    cout << "  " << getGradientColor(2, 0, frames-1) << "║" << RESET << "    " << getGradientColor(2, 4, frames-1) << "╚══██╔══╝██╔══██╗██╔════╝██║ ██╔╝    ████╗ ████║██╔══██╗██╔════╝╚══██╔══╝██╔════╝██╔══██╗" << RESET << " " << getGradientColor(2, 95, frames-1) << "  ║" << RESET << "\n";
    cout << "  " << getGradientColor(3, 0, frames-1) << "║" << RESET << "       " << getGradientColor(3, 7, frames-1) << "██║   ███████║███████╗█████╔╝     ██╔████╔██║███████║███████╗   ██║   █████╗  ██████╔╝" << RESET << " " << getGradientColor(3, 95, frames-1) << "  ║" << RESET << "\n";
    cout << "  " << getGradientColor(5, 0, frames-1) << "║" << RESET << "       " << getGradientColor(5, 7, frames-1) << "██║   ██╔══██║╚════██║██╔═██╗     ██║╚██╔╝██║██╔══██║╚════██║   ██║   ██╔══╝  ██╔══██╗" << RESET << " " << getGradientColor(5, 95, frames-1) << "  ║" << RESET << "\n";
    cout << "  " << getGradientColor(6, 0, frames-1) << "║" << RESET << "       " << getGradientColor(6, 7, frames-1) << "██║   ██║  ██║███████║██║  ██╗    ██║ ╚═╝ ██║██║  ██║███████║   ██║   ███████╗██║  ██║" << RESET << " " << getGradientColor(6, 95, frames-1) << "  ║" << RESET << "\n";
    cout << "  " << getGradientColor(7, 0, frames-1) << "║" << RESET << "       " << getGradientColor(7, 7, frames-1) << "╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝    ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝   ╚═╝   ╚══════╝╚═╝  ╚═╝" << RESET << " " << getGradientColor(7, 95, frames-1) << "  ║" << RESET << "\n";
    cout << "  " << getGradientColor(8, 0, frames-1) << "║" << RESET << "                                                                                                " << getGradientColor(8, 95, frames-1) << "║" << RESET << "\n";
    cout << "  " << getGradientColor(9, 0, frames-1) << "║" << RESET << "                                 " << getGradientColor(9, 33, frames-1) << "Advanced Task Management System" << RESET << "                                " << getGradientColor(9, 95, frames-1) << "║" << RESET << "\n";
    cout << "  " << getGradientColor(10, 0, frames-1) << "║" << RESET << "                           " << getGradientColor(10, 27, frames-1) << "Priority Tracking • Undo/Redo • Search & Sort" << RESET << "                        " << getGradientColor(10, 95, frames-1) << "║" << RESET << "\n";
    cout << "  " << getGradientColor(11, 0, frames-1) << "║" << RESET << "                                                                                                " << getGradientColor(11, 95, frames-1) << "║" << RESET << "\n";
    cout << "  " << getGradientColor(12, 0, frames-1) << "╚════════════════════════════════════════════════════════════════════════════════════════════════╝" << RESET << "\n";
}

void displayMenu() {
    cout << "\n";
    cout << "  ============================= MAIN MENU =============================\n";
    cout << "  1. [+] Add Task             5. [?] Search Tasks\n";
    cout << "  2. [=] View Tasks           6. [*] Sort Tasks\n";
    cout << "  3. [~] Edit Task            7. [<] Undo\n";
    cout << "  4. [X] Delete Task          8. [>] Redo\n";
    cout << "                              9. [!] Exit\n";
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