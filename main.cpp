#include <iostream>
#include <string>
#include <vector>

using namespace std;


void addTask(vector<string>& tasks) {
    string task;
    char addMore = 'y';

    while (addMore == 'y' || addMore == 'Y') {
        cout << "Enter your task: ";
        getline(cin, task);

        if(!task.empty()) {
            tasks.push_back(task);
            cout << "Task added! \n";
        } else {
            cout << "Task cannot be empty. \n";
        }

        cout << "Add another task? (y/n): ";
        cin >> addMore;
        cin.ignore();
    } 
}
int main() {
    vector<string> tasks;

    while (true) {
        cout << "   ==============================" << endl;
        cout << " Welcome to a Simple CLI To do list " << endl;
        cout << "   ==============================" << endl;

        cout << "1. Add a task" << endl;
        cout << "2. View tasks" << endl;
        cout << "3. Exit" << endl; 
        cout << "Please select an option (1-3): ";

        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: {
                addTask(tasks);
                break;
            } 
            case 2: {
                if (tasks.empty()) {
                    cout << "No tasks found \n";
                } else {
                    cout << "Your tasks: \n";
                    for (size_t i = 0; i < tasks.size(); ++i) {
                        cout << i + 1 << ". " << tasks[i] << '\n';
                    }
                }
                break;
            }

            case 3:
            cout << "Exiting the program";
            return 0;

            default:
                cout << "invalid option. \n";
        }
    }
    return 0;
}