# CLI To-Do List

A very simple command-line to-do list app in C++.

## What You Need

- C++ compiler
- `json.hpp` file from [nlohmann/json](https://github.com/nlohmann/json)

## How to Run

1. Download `json.hpp` and put it in the same folder as `main.cpp`
2. Compile:
   ```bash
   g++ -o todo main.cpp
   ```
3. Run:
   ```bash
   ./todo
   ```

## How to Use

The menu has 5 options:

1. **Add a task** - Type your task
2. **View tasks** - See all your tasks
3. **Mark complete** - Check off a task
4. **Delete task** - Remove a task
5. **Exit** - Quit the program

Tasks are saved automatically to `tasks.json`.

## Example

```
1. [ ] Buy milk
2. [X] Do homework
```

`[ ]` = not done  
`[X]` = done