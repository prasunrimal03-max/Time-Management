// time_management.c
// Console-based Time Management System in C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TASKS 100
#define MAX_DESC 256
#define QUOTE_COUNT 100

typedef struct {
    int number; // Task number
    char description[MAX_DESC];
    int hour, minute;
    int completed; // 0 = pending, 1 = done
    int carried;   // 1 = carried from previous day
} Task;

Task tasks[MAX_TASKS];
int taskCount = 0;

// Function declarations
void loadTasks();
void saveTasks();
void addTask();
void editTask();
void deleteTask();
void viewTasks(int completed);
void markTaskDone();
void showRandomQuote();
void carryOverTasks();
void checkReminders();

// Load tasks from file
void loadTasks() {
    FILE *fp = fopen("tasks.txt", "r");
    if (!fp) return;
    taskCount = 0;
    while (!feof(fp) && taskCount < MAX_TASKS) {
        Task t;
        char status[10], carried[10];
        fscanf(fp, "Task %d\n", &t.number);
        fscanf(fp, "Description: %[^\n]\n", t.description);
        fscanf(fp, "Time: %d:%d\n", &t.hour, &t.minute);
        fscanf(fp, "Status: %[^\n]\n", status);
        fscanf(fp, "Carried: %[^\n]\n", carried);
        fscanf(fp, "---\n");

        t.completed = (strcmp(status, "Done") == 0);
        t.carried = (strcmp(carried, "Yes") == 0);

        tasks[taskCount++] = t;
    }
    fclose(fp);
}

// Save tasks to file
void saveTasks() {
    FILE *fp = fopen("tasks.txt", "w");
    for (int i = 0; i < taskCount; i++) {
        fprintf
        (fp, "Task %d\nDescription: %s\nTime: %02d:%02d\nStatus: %s\nCarried: %s\n---\n",
                tasks[i].number,
                tasks[i].description,
                tasks[i].hour,
                tasks[i].minute,
                tasks[i].completed ? "Done" : "Pending",
                tasks[i].carried ? "Yes" : "No");
    }
    fclose(fp);
}

// Add new task
void addTask() {
    if (taskCount >= MAX_TASKS) {
        printf("Too many tasks!\n");
        return;
    }
    Task t;
    t.number = taskCount + 1;
    printf("Enter task description: ");
    getchar(); // clear input buffer
    fgets(t.description, MAX_DESC, stdin);
    t.description[strcspn(t.description, "\n")] = 0;

    // Time input with validation
    int hour, minute;
    char timeStr[6];
    while (1) {
        printf("Enter time (HH:MM): ");
        scanf("%5s", timeStr);
        if (sscanf(timeStr, "%2d:%2d", &hour, &minute) == 2 &&
            hour >= 0 && hour < 24 && minute >= 0 && minute < 60) {
            break;
        } else {
            printf("Invalid time format! Please enter in HH:MM format (e.g., 14:30).\n");
        }
    }

    t.hour = hour;
    t.minute = minute;
    t.completed = 0;
    t.carried = 0;
    tasks[taskCount++] = t;
    saveTasks();
    printf("Task added.\n");
}

// Edit task
void editTask() {
    int number;
    printf("Enter task number to edit: ");
    scanf("%d", &number);
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].number == number) {
            printf("New description: ");
            getchar(); // clear input buffer
            fgets(tasks[i].description, MAX_DESC, stdin);
            tasks[i].description[strcspn(tasks[i].description, "\n")] = 0;

            // Time input with validation
            int hour, minute;
            char timeStr[6];
            while (1) {
                printf("Enter new time (HH:MM): ");
                scanf("%5s", timeStr);
                if (sscanf(timeStr, "%2d:%2d", &hour, &minute) == 2 &&
                    hour >= 0 && hour < 24 && minute >= 0 && minute < 60) {
                    break;
                } else {
                    printf("Invalid time! Please use format HH:MM (e.g., 09:15).\n");
                }
            }

            tasks[i].hour = hour;
            tasks[i].minute = minute;
            saveTasks();
            printf("Task updated.\n");
            return;
        }
    }
    printf("Task not found.\n");
}

// Delete task
void deleteTask() {
    int number;
    printf("Enter task number to delete: ");
    scanf("%d", &number);
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].number == number) {
            for (int j = i; j < taskCount - 1; j++) {
                tasks[j] = tasks[j + 1];
                tasks[j].number = j + 1;
            }
            taskCount--;
            saveTasks();
            printf("Task deleted.\n");
            return;
        }
    }
    printf("Task not found.\n");
}

// View tasks (pending or completed)
void viewTasks(int completed) {
    printf("\n--- %s Tasks ---\n", completed ? "Completed" : "Today's");
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].completed == completed) {
            printf("Task %d\nDesc: %s\nTime: %02d:%02d %s\n\n",
                tasks[i].number,
                tasks[i].description,
                tasks[i].hour,
                tasks[i].minute,
                tasks[i].carried ? "(Carried)" : "");
        }
    }
}

// Mark task as done
void markTaskDone() {
    int number;
    printf("Enter task number to mark as done: ");
    scanf("%d", &number);
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].number == number) {
            tasks[i].completed = 1;
            saveTasks();
            FILE *log = fopen("log.txt", "a");
            fprintf(log, "Completed: %s at %02d:%02d\n",
                    tasks[i].description, tasks[i].hour, tasks[i].minute);
            fclose(log);
            printf("Task marked as done.\n");
            return;
        }
    }
    printf("Task not found.\n");
}

// Carry over uncompleted tasks
void carryOverTasks() {
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].completed == 0) {
            tasks[i].carried = 1;
        }
    }
    saveTasks();
}

// Show reminders
void checkReminders() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int currentHour = t->tm_hour;
    int currentMin = t->tm_min;

    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].completed == 0) {
            int taskAlertHour = tasks[i].hour;
            int taskAlertMin = tasks[i].minute - 15;
            if (taskAlertMin < 0) {
                taskAlertMin += 60;
                taskAlertHour--;
            }
            if (taskAlertHour == currentHour && taskAlertMin == currentMin) {
                printf("\nReminder: Task '%s' is due in 15 minutes!\n", tasks[i].description);
            }
        }
    }
}

// Show a random quote from quotes.txt
void showRandomQuote() {
    FILE *fp = fopen("quotes.txt", "r");
    if (!fp) return;
    char quotes[QUOTE_COUNT][MAX_DESC];
    int count = 0;
    while (fgets(quotes[count], MAX_DESC, fp) && count < QUOTE_COUNT) {
        quotes[count][strcspn(quotes[count], "\n")] = 0;
        count++;
    }
    fclose(fp);
    if (count > 0) {
        srand(time(NULL));
        int index = rand() % count;
        printf("\n*** MOTIVATION ***\n%s\n\n", quotes[index]);
    }
}

// Main menu with safe input
int main() {
    loadTasks();
    showRandomQuote();
    carryOverTasks();

    int choice;
    char input[10];

    while (1) {
        checkReminders();
        printf("\n--- Time Management System ---\n");
        printf("1. Add Task\n2. Edit Task\n3. Delete Task\n4. View Today's Tasks\n5. Mark Task Done\n6. View Completed Tasks\n7. Exit\nEnter choice: ");
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1: addTask(); break;
            case 2: editTask(); break;
            case 3: deleteTask(); break;
            case 4: viewTasks(0); break;
            case 5: markTaskDone(); break;
            case 6: viewTasks(1); break;
            case 7: printf("Goodbye!\n"); exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}
