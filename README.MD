# CLI To Do List

A feature-rich command-line task management application built in C++ with an animated welcome banner, priority tracking, and undo/redo functionality.

## Features

- **Task Management**: Add, edit, delete, and view tasks
- **Priority Levels**: Low, Medium, and High priority classification
- **Due Dates**: Optional due date tracking
- **Search**: Find tasks by keyword
- **Sorting**: Organize tasks by priority or completion status
- **Undo/Redo**: Revert or reapply recent changes
- **Persistent Storage**: Tasks saved to JSON file
- **Animated UI**: Gradient color-shifting welcome banner

## Requirements

- C++11 or later
- Windows OS (uses Windows console API)
- [nlohmann/json](https://github.com/nlohmann/json) library

## Installation

1. Download or clone the repository
2. Include the `json.hpp` header file from nlohmann/json library
3. Compile the program:
   ```bash
   g++ -std=c++11 main.cpp -o ToDoList
   ```

## Usage

Run the executable:
```bash
./ToDoList
```

### Menu Options

1. **Add Task** - Create a new task with title, priority, and optional due date
2. **View Tasks** - Display all tasks with their status
3. **Edit Task** - Modify existing task details
4. **Delete Task** - Remove a task (with confirmation)
5. **Search Tasks** - Find tasks by keyword
6. **Sort Tasks** - Organize by priority or completion
7. **Undo** - Revert the last change
8. **Redo** - Reapply an undone change
9. **Exit** - Save and close the application

### Priority Levels

- `[L]` - Low (Green)
- `[M]` - Medium (Yellow)
- `[H]` - High (Red)

### Task Status

- `[ ]` - Incomplete
- `[V]` - Completed

## Data Storage

Tasks are automatically saved to `tasks.json` in the same directory as the executable. The file is created automatically on first run.

## Notes

- ANSI color codes require a terminal that supports virtual terminal sequences
- The animated banner runs for approximately 12 seconds on startup
- Duplicate task titles are not allowed                                                                             
