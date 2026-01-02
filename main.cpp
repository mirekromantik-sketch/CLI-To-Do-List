#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <limits>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

struct Task {
    string title;
    bool completed = false;
};

void saveTasks(const vector<Task>& tasks) {
    json j;
    j["tasks"] = json::array();

    for (const auto& task : tasks) {
        j["tasks"].push_back({
            {"title", task.title},
            {"completed", task.completed}
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
        cout << "Invalid JSON file. Starting fresh.\n";
        return;
    }

    if (j.contains("tasks") && j["tasks"].is_array()) {
        for (const auto& item : j["tasks"]) {
            Task task;
            task.title = item.value("title", "");
            task.completed = item.value("completed", false);

            if (!task.title.empty()) {
                tasks.push_back(task);
            }
        }
    }
}

void addTask(vector<Task>& tasks) {
    string task;
    char addMore = 'y';

    while (addMore == 'y' || addMore == 'Y') {
        cout << "Enter your task: ";
        getline(cin, task);

        if (!task.empty()) {
            tasks.push_back({task, false});
            saveTasks(tasks);
            cout << "Task added!\n";
        } else {
            cout << "Task cannot be empty.\n";
        }

        cout << "Add another task? (y/n): ";
        cin >> addMore;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void completeTask(vector<Task>& tasks) {
    size_t index;
    cout << "Enter task number to complete: ";
    cin >> index;

    if (cin.fail() || index == 0 || index > tasks.size()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid task number.\n";
        return;
    }

    tasks[index - 1].completed = true;
    saveTasks(tasks);
    cout << "Task marked as completed!\n";
}

void displayMenu() {
    cout << "\n==============================\n";
    cout << "  Simple CLI To-Do List\n";
    cout << "==============================\n";
    cout << "1. Add a task\n";
    cout << "2. View tasks\n";
    cout << "3. Mark task as complete\n";
    cout << "4. Delete task\n";
    cout << "5. Exit\n";
    cout << "\nPlease select an option (1-5): ";
}

void displayTasks(const vector<Task>& tasks) {
    cout << "\nYour tasks:\n\n";
    for (size_t i = 0; i < tasks.size(); ++i) {
        cout << i + 1 << ". "
             << (tasks[i].completed ? "[X] " : "[ ] ")
             << tasks[i].title << '\n';
    }
    cout << '\n';
}

void deleteTask(vector<Task>& tasks) {
    if (tasks.empty()) {
        cout << "No tasks to delete! \n";
        return;
    }

    for(size_t i = 0; i < tasks.size(); ++i)
        cout << i + 1 << ". " << (tasks[i].completed ? "[x] " : "[ ] ") << tasks[i].title << '\n';

    size_t index;
    cout << "Enter task number to delete :";
    cin >> index;

    if (cin.fail() || index == 0 || index > tasks.size()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid task number.\n";
        return;
    }

    tasks.erase(tasks.begin() + index -1);
    saveTasks(tasks);
    cout << "Task deleted! \n";
}

int main() {
    vector<Task> tasks;
    loadTasks(tasks);

    while (true) {
        displayMenu();

        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                addTask(tasks);
                break;

            case 2:
                if (tasks.empty()) {
                    cout << "\nNo tasks found.\n";
                } else {
                    displayTasks(tasks);
                }
                break;

            case 3:
                if (tasks.empty()) {
                    cout << "\nNo tasks to complete.\n";
                } else {
                    displayTasks(tasks);
                    completeTask(tasks);
                }
                break;

            case 4:
                if (tasks.empty()){
                    cout << "\nNo tasks to complete. \n";
                } else {
                    deleteTask(tasks);
                }
                break;

            case 5:
                cout << "\nExiting program...\n";
                return 0;

            default:
                cout << "\nInvalid option.\n";
                break;
        }
    }

    return 0;
}