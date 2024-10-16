#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include <cmath>

#define STACK_SIZE 100
#define MAX_VARS 100
#define MAX_VAR_NAME 32
using namespace std;
// Variable structure to hold name and value
typedef struct {
    char name[MAX_VAR_NAME];
    int value;
} Variable;

// Stack for the interpreter
int stack[STACK_SIZE];
int sp = 0;

// Array of variables
Variable variables[MAX_VARS];
int var_count = 0;

// Find a variable by name
int find_variable(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Set or update a variable
void set_variable(const char *name, int value) {
    int index = find_variable(name);
    if (index != -1) {
        variables[index].value = value;
    } else {
        if (var_count >= MAX_VARS) {
            printf("Error: Too many variables\n");
            exit(1);
        }
        strncpy(variables[var_count].name, name, MAX_VAR_NAME - 1);
        variables[var_count].value = value;
        var_count++;
    }
}

// Get the value of a variable
int get_variable(const char *name) {
    int index = find_variable(name);
    if (index == -1) {
        printf("Error: Undefined variable '%s'\n", name);
        exit(1);
    }
    return variables[index].value;
}

// Evaluate a token (either a number or a variable)
int evaluate_token(char *token) {
    if (isdigit(token[0])) {
        return atoi(token);
    } else {
        return get_variable(token);
    }
}

// Execute a single command
void execute(char *line) {
    // Handle print() function
    if (strncmp(line, "print(", 6) == 0) {
        char *expr = line + 6;
        char *end = strchr(expr, ')');
        string text = "";
        if (end) {
            *end = '\0';  // Null-terminate the expression
            printf("%d\n", evaluate_token(expr));
        } else {
            printf("Error: Syntax error in print statement\n");
        }
        return;
    }

    // Handle variable assignment (e.g., x = 10 or result = x + y)
    char *equals = strchr(line, '=');
    if (equals) {
        *equals = '\0'; // Split the line at '='
        char *var_name = strtok(line, " ");
        char *expr = equals + 1;

        // Handle simple arithmetic expressions with +, -, *, /
        char *token = strtok(expr, " ");
        int value = evaluate_token(token);

        while ((token = strtok(NULL, " ")) != NULL) {
            char op = token[0];
            token = strtok(NULL, " ");
            int operand = evaluate_token(token);

            switch (op) {
                case '+':
                    value += operand;
                    break;
                case '-':
                    value -= operand;
                    break;
                case '*':
                    value *= operand;
                    break;
                case '^':
                    value *= operand;
                    break;
                case '/':
                    if (operand == 0) {
                        printf("Error: Division by zero\n");
                        exit(1);
                    }
                    value /= operand;
                    break;
                default:
                    printf("Error: Unknown operator '%c'\n", op);
                    exit(1);
            }
        }

        // Set the result to the variable
        set_variable(var_name, value);
        return;
    }

    printf("Error: Unknown command '%s'\n", line);
}

// Read and execute commands from a file
void run_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file '%s'\n", filename);
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';
        execute(line);
    }

    fclose(file);
}

// Read and execute commands interactively
void repl() {
    char line[256];
    printf("Xi Script\n");

    while (1) {
        printf("--> ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';
        execute(line);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        // Run a file with .calcl extension
        char *filename = argv[1];
        if (strstr(filename, ".xis") != NULL) {
            run_file(filename);
        } else {
            printf("Error: File must have a .xis extension\n");
            return 1;
        }
    } else {
        // Run in interactive mode (REPL)
        repl();
    }

    return 0;
}
