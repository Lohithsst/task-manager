# Task Manager

A to-do list program in C for the Windows console. Each task is saved as its own text file in a `tasks` folder, so tasks are kept between runs.

## Features

- Add, show, edit and delete tasks. Each task has a name, category, progress, duration in minutes and due date.
- Search by any field. Searches ignore case and match part of a value.
- The home screen shows the total minutes of work in completed tasks.
- The `tasks` folder is created on the first run if it does not exist.

## Input validation

- Task names become file names, so names that Windows reserves (CON, PRN, AUX, NUL, COM1 to COM9, LPT1 to LPT9) and characters that file names cannot contain are rejected.
- Progress must be Not Started, In Progress or Completed.
- Duration must be a positive whole number.
- Due dates must be real calendar dates, including 29 February in leap years.
- Input that is too long for its field is rejected, and the leftover characters are cleared so they do not spill into the next prompt.
- Saving a task under a name that already exists asks before overwriting it.

Task lists are built with `malloc` and freed after each use.

## Constraints

The brief banned `for` loops, `switch` and `goto`. So every loop is a `while` or `do`-`while`, and the menus are `if`/`else if` chains.

## Running it

Windows only, as it uses the Windows API to list the task files. Compile with MinGW:

```
gcc main.c -o task-manager.exe
```
