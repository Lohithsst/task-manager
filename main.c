// Import necessary libraries
#include <ctype.h>     // Character handling functions
#include <direct.h>    // Directory creation/modification (Windows)
#include <stdio.h>     // Standard I/O functions
#include <stdlib.h>    // Memory allocation, process control
#include <string.h>    // String manipulation functions
#include <sys/stat.h>  // File status and permissions
#include <sys/types.h> // Data types for system calls
#include <windows.h>   // Windows API services

// Create structs for date and tasks
typedef struct
{
  int day;   // Day of the month
  int month; // Month of the year
  int year;  // Year
} Date;      // Structure to store date information

typedef struct
{
  char name[256];      // Array to store task name, 255 characters plus null terminator
  char category[9999]; // Array to store task category or description
  char progress[60];   // Array to store task progress status
  int duration;        // Integer to store task duration
  Date dueDate;        // Structure to store task due date
} Task;                // Structure to store information about a task

// Function prototypes
int createFolder();
void start_message();
int inputValidation(const char *inputType, const char *input);
int validateDate(int day, int month, int year);
void printTask(const char *filePath);
int showTasks();
int deleteFile(const char *taskName);
void storeFile(Task *task);
void editTask(Task *task);
void createTask(Task *task);
int loadTask(const char *taskName, Task *task);
char **taskNames(int *taskCount);
int searchTasks();
int workDuration();

// Main function
int main(void)
{
  printf("Welcome to the Todo Program!\n");
  int choice, c;
  Task task;

  createFolder(); // Create the "tasks" folder if it doesn't exist

  while (1)
  {
    system("cls"); // Clear the terminal screen

    // Display total work done in minutes
    int duration = workDuration();
    printf("\nTotal work done (in min): %d\n", duration);

    start_message();          // Show the main menu options
    choice = getchar() - '0'; // Convert the first character of input to an integer
    while ((c = getchar()) != '\n' && c != EOF)
      ; // Ignore the rest of the line

    if (choice == 1)
    {
      showTasks(); // Show all tasks
      printf("\nPress any key to continue...\n");
      getchar(); // Wait for a key press
    }
    else if (choice == 2)
    {
      createTask(&task); // Create a new task
      printf("\nPress any key to continue...\n");
      getchar(); // Wait for a key press
    }
    else if (choice == 3)
    {
      // List tasks for editing
      WIN32_FIND_DATA findFileData; // Create variable of type WIN32_FIND_DATA to store file information

      // Create variable hFind of type HANDLE (type of ID windows uses to represent files)
      // FindFirstFile("tasks\\*.txt", &findFileData) --> Searches in tasks folder for .txt files, if found, stores valid HANDLE value in hFind
      HANDLE hFind = FindFirstFile("tasks\\*.txt", &findFileData);

      // If no files are found, hFind will be an INVALID_HANDLE_VALUE
      if (hFind == INVALID_HANDLE_VALUE)
      {
        printf("\nNo tasks found\n");
      }
      else
      {
        printf("List of tasks:\n");
        do
        {
          char *dot = strrchr(findFileData.cFileName, '.'); // Searches for '.' in file name
          if (dot && strcmp(dot, ".txt") == 0)              // If the found '.' is part of .txt
          {
            *dot = '\0'; // Removes the ".txt" extension
          }
          printf("- %s\n", findFileData.cFileName); // Print task name
        } while (FindNextFile(hFind, &findFileData) != 0); // Loop until no txt files are left
        FindClose(hFind); // Close the handle

        char taskName[256]; // Create variable to store taskName, 256 because max char limit of windows is 255 + '\n'
        printf("Enter task name to edit: ");
        fgets(taskName, sizeof(taskName), stdin); // Read task name
        taskName[strcspn(taskName, "\n")] = '\0'; // Remove newline

        if (loadTask(taskName, &task) == 0)
        {
          editTask(&task); // Edit the selected task
        }
      }
      printf("\nPress any key to continue...\n");
      getchar(); // Wait for a key press
    }
    else if (choice == 4)
    {
      // List tasks for deletion
      WIN32_FIND_DATA findFileData; // Create variable of type WIN32_FIND_DATA to store file information

      // Create variable hFind of type HANDLE (type of ID windows uses to represent files)
      // FindFirstFile("tasks\\*.txt", &findFileData) --> Searches in tasks folder for .txt files, if found, stores valid HANDLE value in hFind
      HANDLE hFind = FindFirstFile("tasks\\*.txt", &findFileData);

      // If no files are found, hFind will be an INVALID_HANDLE_VALUE
      if (hFind == INVALID_HANDLE_VALUE)
      {
        printf("\nNo tasks found\n");
      }
      else
      {
        printf("List of tasks:\n");
        do
        {
          char *dot = strrchr(findFileData.cFileName, '.'); // Searches for '.' in file name
          if (dot && strcmp(dot, ".txt") == 0)              // If the found '.' is part of .txt
          {
            *dot = '\0'; // Removes the ".txt" extension
          }
          printf("- %s\n", findFileData.cFileName); // Print task name
        } while (FindNextFile(hFind, &findFileData) != 0); // Loop until no txt files are left
        FindClose(hFind); // Close the handle

        char taskName[256]; // Create variable to store taskName, 256 because max char limit of windows is 255 + '\n'
        printf("Enter task name to delete: ");
        fgets(taskName, sizeof(taskName), stdin); // Read task name
        taskName[strcspn(taskName, "\n")] = '\0'; // Remove newline

        if (deleteFile(taskName) == 0)
        {
          printf("Deletion successful.\n");
        }
      }
      printf("\nPress any key to continue...\n");
      getchar(); // Wait for a key press
    }
    else if (choice == 5)
    {
      searchTasks(); // Search for tasks
      printf("\nPress any key to continue...\n");
      getchar(); // Wait for a key press
    }
    else if (choice == 6)
    {
      break; // Exit the program
    }
    else
    {
      printf("Invalid Input\n"); // Invalid menu option
      printf("\nPress any key to continue...\n");
      getchar(); // Wait for a key press
    }
  }
  return 0; // Program ends
}

// Checks if the "tasks" folder exists. If not, it creates one.
int createFolder()
{
  struct _stat st = {0}; // Struct to store file/directory info, _stat structure format is provided by sys/stat.h

  // Check if the "tasks" folder exists
  if (_stat("tasks", &st) == -1)
  {
    // If the folder does not exist, create it
    if (_mkdir("tasks") == -1)
    {
      printf("Error creating tasks directory\n");
      return 1; // Error occurred
    }
    printf("Tasks folder not found\n");
    printf("Created tasks folder\n");
  }
  return 0; // Success
}

// Message to be printed when program is run
void start_message()
{
  printf("\nYou have the following options:\n");
  printf("1. Show tasks - Display all the tasks in your list.\n");
  printf("2. Add tasks - Add a new task to your list.\n");
  printf("3. Edit tasks - Modify an existing task.\n");
  printf("4. Delete tasks - Remove a task from your list.\n");
  printf("5. Search - Find a task in your list.\n");
  printf("6. Quit - Quit the program.\n");
  printf("Please enter the number of your chosen option: ");
}

// Function to do input validation
int inputValidation(const char *inputType, const char *input)
{
  // Check for empty input
  if (strlen(input) == 0)
  {
    printf("Invalid input: Empty input.\n");
    return 1; // Error: Empty input
  }

  // Validation for "name"
  if (strcmp(inputType, "name") == 0)
  {
    // Reserved device names
    const char *reservedNames[] = {
        "con", "prn", "aux", "nul",
        "com1", "com2", "com3", "com4", "com5", "com6", "com7", "com8", "com9",
        "lpt1", "lpt2", "lpt3", "lpt4", "lpt5", "lpt6", "lpt7", "lpt8", "lpt9"};

    // Convert input to lowercase for case-insensitive comparison
    char lowerInput[256];
    strncpy(lowerInput, input, sizeof(lowerInput));
    lowerInput[sizeof(lowerInput) - 1] = '\0'; // Ensure null-termination

    int i = 0;
    while (lowerInput[i])
    {
      lowerInput[i] = tolower(lowerInput[i]);
      i++;
    }

    // Check for reserved names
    i = 0;
    while (i < sizeof(reservedNames) / sizeof(reservedNames[0])) // total memory / memory of 1 pointer = num of pointers
    {
      if (strcmp(lowerInput, reservedNames[i]) == 0)
      {
        printf("Invalid input: Reserved name '%s'.\n", input);
        return 1; // Error: Reserved name
      }
      i++;
    }

    // Reserved characters not allowed in file names
    const char *invalidChars = "\\/:*?\"<>|";
    // Check for invalid characters in input
    i = 0;
    while (input[i] != '\0')
    {
      if (strchr(invalidChars, input[i]) != NULL)
      {
        printf("Invalid input: Character '%c' is not allowed.\n", input[i]);
        return 1; // Error: Invalid characters
      }
      i++;
    }
  }

  // Validation for "category"
  else if (strcmp(inputType, "category") == 0)
  {
    // Reserved character not allowed in category names
    const char invalidChar = '\"';
    // Check for invalid character in input
    int i = 0;
    while (input[i] != '\0')
    {
      if (input[i] == invalidChar)
      {
        printf("Invalid input: Character '%c' is not allowed.\n", input[i]);
        return 1; // Error: Invalid character
      }
      i++;
    }
  }

  // Validation for "progress"
  else if (strcmp(inputType, "progress") == 0)
  {
    // Validate progress: Must match specific options
    if (strcmp(input, "Not Started") != 0 &&
        strcmp(input, "In Progress") != 0 && strcmp(input, "Completed") != 0)
    {
      printf("Invalid input: Choose 'Not Started', 'In Progress', or 'Completed'. Case sensitive.\n");
      return 1; // Error: Invalid progress input
    }
  }

  // Validation for "duration"
  else if (strcmp(inputType, "duration") == 0)
  {
    // Validate duration: Must be a positive integer
    int i = 0;
    while (input[i] != '\0')
    {
      if (!isdigit(input[i]))
      {
        printf("Invalid input: Please only enter numbers.\n");
        return 1; // Error: Non-digit characters found
      }
      i++;
    }
    if (atoi(input) <= 0)
    {
      printf("Invalid input: Please enter a positive non-zero number.\n");
      return 1; // Error: Non-positive number
    }
  }

  // Validation for "yn" (yes/no)
  else if (strcmp(inputType, "yn") == 0)
  {
    // Validate yes/no input: Must be 'y', 'Y', 'n', or 'N'
    if (strcmp(input, "y") != 0 && strcmp(input, "Y") != 0 &&
        strcmp(input, "n") != 0 && strcmp(input, "N") != 0)
    {
      printf("Invalid input: Choose 'y', 'Y', 'n', or 'N'.\n");
      return 1; // Error: Invalid yes/no input
    }
  }

  // Unknown input type
  else
  {
    printf("Invalid input: Unexpected input type.\n");
    return 1; // Error: Unexpected type
  }

  return 0; // Input is valid
}

// Function to validate date
int validateDate(int day, int month, int year)
{
  // Check if the year is within a reasonable range
  if (year < 1 || year > 9999)
  {
    printf("Invalid year: Please enter a year between 1 and 9999.\n");
    return 1; // Error: Invalid year
  }

  // Check if the month is valid
  if (month < 1 || month > 12)
  {
    printf("Invalid month: Please enter a month between 1 and 12.\n");
    return 1; // Error: Invalid month
  }

  // Days in each month
  int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // Determine and adjust for leap year
  if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
  {
    daysInMonth[1] = 29; // February has 29 days in a leap year
  }

  // Check if the day is valid for the given month
  if (day < 1 || day > daysInMonth[month - 1])
  {
    printf("Invalid day: Please enter a day between 1 and %d.\n", daysInMonth[month - 1]);
    return 1; // Error: Invalid day
  }

  // All checks passed, date is valid
  return 0; // Valid date
}

// Function to print out task information
void printTask(const char *filePath)
{
  // Open file for reading
  FILE *file = fopen(filePath, "r");

  // Check if file was opened successfully
  if (file != NULL)
  {
    // Variables to store task information
    char name[256], category[9999], progress[60];
    int duration;
    int dueDay, dueMonth, dueYear;

    // Read and print task details from the file
    int itemsRead = fscanf(file, "\"%255[^\"]\",\"%9998[^\"]\",\"%59[^\"]\",%d,%d-%d-%d\n",
                           name, category, progress, &duration,
                           &dueDay, &dueMonth, &dueYear);
    if (itemsRead == 7)
    {
      printf("\nTask Name: %s\n", name);
      printf("Progress: %s\n", progress);
      printf("Category: %s\n", category);
      printf("Duration: %d\n", duration);
      printf("Due Date: %02d-%02d-%04d\n", dueDay, dueMonth, dueYear);
    }
    else
    {
      printf("Could not parse the task details from file %s. Items read: %d\n", filePath, itemsRead);
    }

    // Close the file
    fclose(file);
  }
  else
  {
    // Error if the file cannot be opened
    printf("Could not open the file %s.\n", filePath);
  }
}

// Function to open tasks folder and get all tasks. Uses printTask function to print
int showTasks()
{
  WIN32_FIND_DATA findFileData;                                // Create variable of type WIN32_FIND_DATA to store file information
  HANDLE hFind = FindFirstFile("tasks\\*.txt", &findFileData); // Find .txt files in "tasks" folder

  // If no files are found, hFind will be an INVALID_HANDLE_VALUE
  if (hFind == INVALID_HANDLE_VALUE)
  {
    printf("\nNo tasks found\n");
    return 1; // Error: No tasks found
  }

  int fileCount = 0; // Counter for number of .txt files

  // First pass: Count the number of .txt files
  do
  {
    // Check if it's a file (not a directory)
    if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
      fileCount++; // Increment the file count
    }
  } while (FindNextFile(hFind, &findFileData) != 0);

  FindClose(hFind);                                     // Close the handle to reset search
  hFind = FindFirstFile("tasks\\*.txt", &findFileData); // Reopen the handle for a fresh search

  // Check again if no files were found
  if (fileCount == 0)
  {
    printf("\nNo tasks found\n");
    FindClose(hFind);
    return 1; // Error: No tasks found
  }

  // Allocate memory for file names based on file count
  char **fileNames = malloc(sizeof(char *) * fileCount);
  if (fileNames == NULL)
  {
    printf("Memory allocation failed\n");
    FindClose(hFind);
    return 1; // Error: Memory allocation failed
  }

  int index = 0; // Index to store file names

  // Second pass: Store file names in allocated memory
  do
  {
    // Check if it's a file (not a directory)
    if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
      // Allocate memory for the file name string
      fileNames[index] = malloc(strlen(findFileData.cFileName) + 1);

      // If malloc fails, free all used memory so far
      if (fileNames[index] == NULL)
      {
        printf("Memory allocation failed\n");
        // Free previously allocated memory for file names
        int j = 0;
        while (j < index)
        {
          free(fileNames[j]);
          j++;
        }
        free(fileNames);
        FindClose(hFind);
        return 1; // Error: Memory allocation failed
      }
      // Copy the file name into allocated memory
      strcpy(fileNames[index], findFileData.cFileName);
      index++; // Move to the next index
    }
  } while (FindNextFile(hFind, &findFileData) != 0);

  FindClose(hFind); // Close the find handle

  int i = 0;
  // Print each task and free allocated memory for file names
  while (i < fileCount)
  {
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "tasks\\%s", fileNames[i]);
    printTask(filePath); // Call function to print task details
    free(fileNames[i]);  // Free memory for the file name
    i++;
  }
  free(fileNames); // Free memory for the file name array

  return 0; // Success
}

// Function to delete text file (delete a task)
int deleteFile(const char *taskName)
{
  char filename[256]; // Array to hold the file path

  // Construct the file path
  snprintf(filename, sizeof(filename), "tasks/%s.txt", taskName);

  // Attempt to delete the file
  if (remove(filename) == 0)
  {
    return 0;
  }
}

// Function to create text file with task information (store tasks)
void storeFile(Task *task)
{
  char filename[256]; // Array to hold the file path
  FILE *file;         // File pointer

  // Construct the file path with the task name
  snprintf(filename, sizeof(filename), "tasks/%s.txt", task->name);

  // Open the file for writing
  file = fopen(filename, "w");

  // Check if file was opened successfully
  if (file == NULL)
  {
    printf("Error opening file: %s\n", filename); // Provide detailed error message
    return;                                       // Exit function if file can't be opened
  }

  // Store task information in the text file
  fprintf(file,
          "\"%s\",\"%s\",\"%s\",%d,%02d-%02d-%04d\n",
          task->name, task->category, task->progress, task->duration,
          task->dueDate.day, task->dueDate.month, task->dueDate.year);
  fclose(file); // Close the file
}

// Function to edit task information. Uses deleteFile and storeFile to delete old tasks and store edited tasks
void editTask(Task *task)
{
  char oldName[256];  // Variable to store the original task name
  char filename[256]; // Variable for constructing file paths
  FILE *file;         // File pointer
  int choice, c;      // User's choice of field to edit

  // Save the old name before making changes
  strcpy(oldName, task->name);

  printf("What would you like to edit?\n");
  printf("[1] Name\n");
  printf("[2] Category\n");
  printf("[3] Progress\n");
  printf("[4] Duration\n");
  printf("[5] Due Date\n");
  printf("Enter choice: ");

  // Get valid input
  while (1)
  {
    c = getchar(); // Read the first character
    if (c >= '1' && c <= '5')
    {
      choice = c - '0'; // Convert character to integer
      // Consume the rest of the line
      while ((c = getchar()) != '\n' && c != EOF)
        ;
      break; // Exit the loop if input is valid
    }
    else
    {
      // Invalid input; consume the rest of the line
      while ((c = getchar()) != '\n' && c != EOF)
        ;
      printf("\nInvalid choice. Please enter a number between 1 and 5.\n");
      printf("Enter choice: ");
    }
  }

  // Editing name
  if (choice == 1)
  {
    while (1)
    {
      printf("\nEnter task name (max 255 characters): ");
      if (!fgets(task->name, sizeof(task->name), stdin))
      {
        fprintf(stderr, "Error reading input.\n");
        continue;
      }
      if (task->name[strlen(task->name) - 1] != '\n')
      {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
          ;
        printf("Invalid input, please enter a task name with a maximum of 255 characters.\n");
        continue;
      }
      task->name[strcspn(task->name, "\n")] = '\0';

      while (inputValidation("name", task->name))
      {
        printf("Please enter a valid task name (max 255 characters): ");
        if (!fgets(task->name, sizeof(task->name), stdin))
        {
          fprintf(stderr, "Error reading input.\n");
          continue;
        }
        if (task->name[strlen(task->name) - 1] != '\n')
        {
          int c;
          while ((c = getchar()) != '\n' && c != EOF)
            ;
          printf("Invalid input, please enter a task name with a maximum of 255 characters.\n");
          continue;
        }
        task->name[strcspn(task->name, "\n")] = '\0';
      }

      snprintf(filename, sizeof(filename), "tasks/%s.txt", task->name);
      file = fopen(filename, "r");

      if (file != NULL)
      {
        fclose(file);
        char overwrite[10];
        do
        {
          printf("A task with this name already exists. Overwrite? (y/n): ");
          if (!fgets(overwrite, sizeof(overwrite), stdin))
          {
            fprintf(stderr, "Error reading input.\n");
            continue;
          }
          if (overwrite[strlen(overwrite) - 1] != '\n')
          {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
              ;
            printf("Invalid input, please enter 'y' or 'n'.\n");
            continue;
          }
          overwrite[strcspn(overwrite, "\n")] = '\0';

          if (inputValidation("yn", overwrite) == 0)
          {
            break;
          }
        } while (1);

        if (overwrite[0] == 'n' || overwrite[0] == 'N')
        {
          continue;
        }
      }
      break;
    }
  }
  else if (choice == 2) // Editing category
  {
    while (1)
    {
      printf("Enter task category (max 9998 characters): ");
      if (!fgets(task->category, sizeof(task->category), stdin))
      {
        fprintf(stderr, "Error reading input.\n");
      }
      else
      {
        if (task->category[strlen(task->category) - 1] != '\n')
        {
          int c;
          while ((c = getchar()) != '\n' && c != EOF)
            ;
          printf("Invalid input, please enter a task category with a maximum of 9998 characters.\n");
          continue;
        }
        task->category[strcspn(task->category, "\n")] = '\0';

        if (!inputValidation("category", task->category))
        {
          break;
        }
      }
    }
  }
  else if (choice == 3) // Editing progress
  {
    while (1)
    {
      printf("Enter new progress (Not Started, In Progress, Completed): ");
      if (!fgets(task->progress, sizeof(task->progress), stdin))
      {
        fprintf(stderr, "Error reading input.\n");
        continue;
      }
      if (task->progress[strlen(task->progress) - 1] != '\n')
      {
        while ((c = getchar()) != '\n' && c != EOF)
          ;
        printf("Invalid input, please enter a valid progress state.\n");
        continue;
      }
      task->progress[strcspn(task->progress, "\n")] = '\0';

      if (!inputValidation("progress", task->progress))
        break;
    }
  }
  else if (choice == 4) // Editing Duration
  {
    while (1)
    {
      printf("Enter task duration (min) (max 8 characters): ");
      char durationInput[10];
      if (!fgets(durationInput, sizeof(durationInput), stdin))
      {
        fprintf(stderr, "Error reading input.\n");
      }
      else
      {
        if (durationInput[strlen(durationInput) - 1] != '\n')
        {
          printf("Please enter a valid duration (max 10 characters)\n");
          int c;
          while ((c = getchar()) != '\n' && c != EOF)
            ;
          continue;
        }
        durationInput[strcspn(durationInput, "\n")] = '\0';

        if (!inputValidation("duration", durationInput))
        {
          task->duration = atoi(durationInput);
          break;
        }
      }
    }
  }
  else if (choice == 5) // Editing Date
  {
    while (1)
    {
      printf("Enter due date (dd-mm-yyyy): ");
      if (scanf("%d-%d-%d", &task->dueDate.day, &task->dueDate.month, &task->dueDate.year) != 3)
      {
        fprintf(stderr, "Invalid date format. Please enter as dd-mm-yyyy.\n");
        while (getchar() != '\n')
          ;
      }
      else if (!validateDate(task->dueDate.day, task->dueDate.month, task->dueDate.year))
      {
        break;
      }
      while (getchar() != '\n')
        ;
    }
  }

  // Print updated task details
  printf("Updated task details:\n Name: %s\n Progress: %s\n Category: %s\n Duration: %d\n Due Date: %02d-%02d-%04d\n",
         task->name, task->progress, task->category, task->duration,
         task->dueDate.day, task->dueDate.month, task->dueDate.year);

  // Delete old file and save updated task
  deleteFile(oldName);
  storeFile(task);
}

// Function to get task information. Uses editTask and storeFile to edit and store new tasks
void createTask(Task *task)
{
  char filename[256];
  FILE *file;

  strcpy(task->progress, "Not Started");

  while (1) // Name
  {
    printf("\nEnter task name (max 255 characters): ");
    if (!fgets(task->name, sizeof(task->name), stdin))
    {
      fprintf(stderr, "Error reading input.\n");
      continue;
    }
    if (task->name[strlen(task->name) - 1] != '\n')
    {
      int c;
      while ((c = getchar()) != '\n' && c != EOF)
        ;
      printf("Invalid input, please enter a task name with a maximum of 255 characters.\n");
      continue;
    }
    task->name[strcspn(task->name, "\n")] = '\0';

    while (inputValidation("name", task->name))
    {
      printf("Please enter a valid task name (max 255 characters): ");
      if (!fgets(task->name, sizeof(task->name), stdin))
      {
        fprintf(stderr, "Error reading input.\n");
        continue;
      }
      if (task->name[strlen(task->name) - 1] != '\n')
      {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
          ;
        printf("Invalid input, please enter a task name with a maximum of 255 characters.\n");
        continue;
      }
      task->name[strcspn(task->name, "\n")] = '\0';
    }

    snprintf(filename, sizeof(filename), "tasks/%s.txt", task->name);
    file = fopen(filename, "r");

    if (file != NULL)
    {
      fclose(file);
      char overwrite[10];
      do
      {
        printf("A task with this name already exists. Overwrite? (y/n): ");
        if (!fgets(overwrite, sizeof(overwrite), stdin))
        {
          fprintf(stderr, "Error reading input.\n");
          continue;
        }
        if (overwrite[strlen(overwrite) - 1] != '\n')
        {
          int c;
          while ((c = getchar()) != '\n' && c != EOF)
            ;
          printf("Invalid input, please enter 'y' or 'n'.\n");
          continue;
        }
        overwrite[strcspn(overwrite, "\n")] = '\0';

        if (inputValidation("yn", overwrite) == 0)
        {
          break;
        }
      } while (1);

      if (overwrite[0] == 'n' || overwrite[0] == 'N')
      {
        continue;
      }
    }
    break;
  }

  while (1) // Category
  {
    printf("Enter task category (max 9998 characters): ");
    if (!fgets(task->category, sizeof(task->category), stdin))
    {
      fprintf(stderr, "Error reading input.\n");
    }
    else
    {
      if (task->category[strlen(task->category) - 1] != '\n')
      {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
          ;
        printf("Invalid input, please enter a task category with a maximum of 9998 characters.\n");
        continue;
      }
      task->category[strcspn(task->category, "\n")] = '\0';

      if (!inputValidation("category", task->category))
      {
        break;
      }
    }
  }

  while (1) // Duration
  {
    printf("Enter task duration (min) (max 8 characters): ");
    char durationInput[10];
    if (!fgets(durationInput, sizeof(durationInput), stdin))
    {
      fprintf(stderr, "Error reading input.\n");
    }
    else
    {
      if (durationInput[strlen(durationInput) - 1] != '\n')
      {
        printf("Please enter a valid duration (max 10 characters)\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
          ;
        continue;
      }
      durationInput[strcspn(durationInput, "\n")] = '\0';

      if (!inputValidation("duration", durationInput))
      {
        task->duration = atoi(durationInput);
        break;
      }
    }
  }

  while (1) // Date
  {
    printf("Enter due date (dd-mm-yyyy): ");
    if (scanf("%d-%d-%d", &task->dueDate.day, &task->dueDate.month, &task->dueDate.year) != 3)
    {
      fprintf(stderr, "Invalid date format. Please enter as dd-mm-yyyy.\n");
      while (getchar() != '\n')
        ;
    }
    else if (!validateDate(task->dueDate.day, task->dueDate.month, task->dueDate.year))
    {
      break;
    }
    while (getchar() != '\n')
      ;
  }

  getchar(); // Remove newline

  printf("You entered:\n Name: %s\n Progress: %s\n Category: %s\n Duration: %d\n Due Date: %02d-%02d-%04d\n",
         task->name, task->progress, task->category, task->duration,
         task->dueDate.day, task->dueDate.month, task->dueDate.year);

  char edit[10];
  do
  {
    printf("Would you like to edit? (y/n): ");
    if (!fgets(edit, sizeof(edit), stdin))
    {
      fprintf(stderr, "Error reading input.\n");
    }
    else
    {
      if (edit[strlen(edit) - 1] != '\n')
      {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
          ;
        printf("Invalid input, please enter 'y' or 'n'.\n");
        continue;
      }
      edit[strcspn(edit, "\n")] = '\0';

      if (inputValidation("yn", edit) == 0)
      {
        break;
      }
    }
  } while (1);

  if (edit[0] == 'y' || edit[0] == 'Y')
  {
    editTask(task);
  }
  else
  {
    storeFile(task);
  }
}

// Function to open text file and store task information in task struct
int loadTask(const char *taskName, Task *task)
{
  char filename[256];                                             // Buffer to hold the file path
  snprintf(filename, sizeof(filename), "tasks/%s.txt", taskName); // Construct the file path

  FILE *file = fopen(filename, "r"); // Open the file for reading
  if (file == NULL)
  {
    printf("Task not found!\n"); // Error if the file does not exist
    return 1;                    // Return error code
  }

  // Read task information from the file
  fscanf(file, "\"%255[^\"]\",\"%9998[^\"]\",\"%59[^\"]\",%d,%d-%d-%d",
         task->name, task->category, task->progress, &task->duration,
         &task->dueDate.day, &task->dueDate.month, &task->dueDate.year);

  fclose(file); // Close the file
  return 0;     // Return success code
}

// Function to create an array of task names
char **taskNames(int *taskCount)
{
  WIN32_FIND_DATA findFileData; // Struct to store file data
  HANDLE hFind;
  char **taskNames = NULL; // Array to store task names

  *taskCount = 0; // Initialize task count

  // Count the number of .txt files in the tasks directory
  hFind = FindFirstFile("tasks\\*.txt", &findFileData);
  if (hFind == INVALID_HANDLE_VALUE)
  {
    return NULL; // Return NULL if no files found
  }

  // Increment taskCount for each .txt file found
  do
  {
    (*taskCount)++;
  } while (FindNextFile(hFind, &findFileData) != 0);
  FindClose(hFind); // Close the handle

  // Allocate memory for task names
  taskNames = (char **)malloc((*taskCount) * sizeof(char *));
  if (taskNames == NULL)
  {
    return NULL; // Return NULL if memory allocation fails
  }

  *taskCount = 0; // Reset counter for storing names

  // Re-open the directory to retrieve and store the task names
  hFind = FindFirstFile("tasks\\*.txt", &findFileData);
  if (hFind != INVALID_HANDLE_VALUE)
  {
    do
    {
      // Remove the ".txt" extension from the file name
      char *dot = strrchr(findFileData.cFileName, '.');
      if (dot && strcmp(dot, ".txt") == 0)
      {
        *dot = '\0'; // Terminate string at the "."
      }
      // Allocate memory for the task name
      taskNames[*taskCount] = (char *)malloc((strlen(findFileData.cFileName) + 1) * sizeof(char));
      if (taskNames[*taskCount] == NULL)
      {
        // Free previously allocated memory
        int i = 0;
        while (i < *taskCount)
        {
          free(taskNames[i]);
          i++;
        }
        free(taskNames);
        FindClose(hFind);
        return NULL; // Return NULL if memory allocation fails
      }
      strcpy(taskNames[*taskCount], findFileData.cFileName); // Store the task name
      (*taskCount)++;                                        // Increment the count of task names
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind); // Close the handle
  }

  return taskNames; // Return the array of task names
}

// Function to search for tasks based on input
int searchTasks()
{
  int taskCount = 0;
  char **tasks = taskNames(&taskCount); // Get the list of task names

  if (tasks == NULL)
  {
    printf("\nNo tasks found.\n");
    return 1; // Return error if no tasks are found
  }

  int choice, c;
  char searchTerm[9999];

  printf("What would you like to search?\n");
  printf("[1] Name\n");
  printf("[2] Category\n");
  printf("[3] Progress\n");
  printf("[4] Duration\n");
  printf("[5] Due Date\n");
  printf("Enter choice: ");
  while (1)
  {
    c = getchar(); // Read the first character
    if (c >= '1' && c <= '5')
    {
      choice = c - '0'; // Convert character to integer
      // Consume the rest of the line
      while ((c = getchar()) != '\n' && c != EOF)
        ;
      break; // Exit the loop if input is valid
    }
    else
    {
      // Invalid input; consume the rest of the line
      while ((c = getchar()) != '\n' && c != EOF)
        ;
      printf("Invalid choice. Please enter a number between 1 and 5.\n");
      printf("Enter choice: ");
    }
  }

  // Get the search term based on user choice
  while (1)
  {
    if (choice == 1)
    {
      printf("Enter task name: ");
    }
    else if (choice == 2)
    {
      printf("Enter category: ");
    }
    else if (choice == 3)
    {
      printf("Enter progress (Not Started, In Progress, Completed): ");
    }
    else if (choice == 4)
    {
      printf("Enter duration: ");
    }
    else if (choice == 5)
    {
      int day, month, year;
      printf("Enter due date (dd-mm-yyyy): ");
      if (scanf("%d-%d-%d", &day, &month, &year) == 3)
      {
        if (validateDate(day, month, year) == 0)
        {
          snprintf(searchTerm, sizeof(searchTerm), "%02d-%02d-%d", day, month, year);
          while ((c = getchar()) != '\n' && c != EOF)
            ;
          break;
        }
      }
      while (getchar() != '\n')
        ;
      continue;
    }

    fgets(searchTerm, sizeof(searchTerm), stdin);
    if (searchTerm[strlen(searchTerm) - 1] != '\n')
    {
      // Input overflow, clear the buffer
      while ((c = getchar()) != '\n' && c != EOF)
        ;
      printf("Input too long. Please enter up to 9998 characters.\n");
      continue;
    }

    searchTerm[strcspn(searchTerm, "\n")] = '\0'; // Remove newline

    if (inputValidation(choice == 1 ? "name" : choice == 2 ? "category"
                                           : choice == 3   ? "progress"
                                                           : "duration",
                        searchTerm) == 0)
    {
      break;
    }
  }

  // Convert searchTerm to lowercase for case-insensitive search
  int i = 0;
  while (searchTerm[i])
  {
    searchTerm[i] = tolower(searchTerm[i]);
    i++;
  }

  printf("\nTasks matching the search criteria:\n");
  i = 0;
  while (i < taskCount)
  {
    Task task;
    if (loadTask(tasks[i], &task) == 0)
    {
      int match = 0;
      // Search by chosen field
      if (choice == 1)
      {
        // Search by Name
        char taskNameLower[256];
        strcpy(taskNameLower, task.name);
        int j = 0;
        while (taskNameLower[j])
        {
          taskNameLower[j] = tolower(taskNameLower[j]);
          j++;
        }
        match = (strstr(taskNameLower, searchTerm) != NULL);
      }
      else if (choice == 2)
      {
        // Search by Category
        char taskCategoryLower[9999];
        strncpy(taskCategoryLower, task.category, sizeof(taskCategoryLower) - 1);
        taskCategoryLower[sizeof(taskCategoryLower) - 1] = '\0'; // Ensure null-termination
        int j = 0;
        while (taskCategoryLower[j])
        {
          taskCategoryLower[j] = tolower(taskCategoryLower[j]);
          j++;
        }
        match = (strstr(taskCategoryLower, searchTerm) != NULL);
      }
      else if (choice == 3)
      {
        // Search by Progress
        char taskProgressLower[60];
        strcpy(taskProgressLower, task.progress);
        int j = 0;
        while (taskProgressLower[j])
        {
          taskProgressLower[j] = tolower(taskProgressLower[j]);
          j++;
        }
        match = (strstr(taskProgressLower, searchTerm) != NULL);
      }
      else if (choice == 4)
      {
        // Search by Duration
        char durationStr[10];
        snprintf(durationStr, sizeof(durationStr), "%d", task.duration);
        match = (strstr(durationStr, searchTerm) != NULL);
      }
      else if (choice == 5)
      {
        // Search by Due Date
        char dueDateStr[20];
        snprintf(dueDateStr, sizeof(dueDateStr), "%02d-%02d-%d", task.dueDate.day, task.dueDate.month, task.dueDate.year);
        match = (strstr(dueDateStr, searchTerm) != NULL);
      }

      if (match)
      {
        printf("\nTask Name: %s\n", task.name);
        printf("Category: %s\n", task.category);
        printf("Progress: %s\n", task.progress);
        printf("Duration: %d\n", task.duration);
        printf("Due Date: %02d-%02d-%d\n", task.dueDate.day, task.dueDate.month, task.dueDate.year);
      }
    }
    i++;
  }

  // Free memory for tasks array
  i = 0;
  while (i < taskCount)
  {
    free(tasks[i]);
    i++;
  }
  free(tasks);

  return 0;
}

// Function to calculate total time of Completed tasks
int workDuration()
{
  int taskCount = 0;
  char **tasks = taskNames(&taskCount); // Get the list of task names

  if (tasks == NULL)
  {
    return 0; // Return 0 if no tasks are found
  }

  int totalDuration = 0;

  // Iterate through all tasks
  int i = 0;
  while (i < taskCount)
  {
    Task task;
    if (loadTask(tasks[i], &task) == 0)
    {
      // Check if the task progress is "Completed"
      if (strcmp(task.progress, "Completed") == 0)
      {
        totalDuration += task.duration; // Add duration to total
      }
    }
    i++;
  }

  // Free memory for tasks array
  i = 0;
  while (i < taskCount)
  {
    free(tasks[i]);
    i++;
  }
  free(tasks);

  return totalDuration;
}

