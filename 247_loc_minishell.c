// Improved mini-shell.c file, incorporating the requested features such as signal handling, command history, pipelining, and redirection support.
/* Key Features Added
Signal Handling:

Handles Ctrl+C gracefully with sigint_handler.
Command History:

Uses the GNU Readline library for command history and interactive input.
Piping:

Supports | for command pipelining with execute_piped_commands.
Output Redirection:

Handles > and >> for output redirection in execute_command.
Improved Parsing:

Handles parsing for piping and redirection.
Compilation Instructions

Compile the program with the -lreadline flag to link the GNU Readline library:
```bash
gcc mini-shell.c -o mini-shell -lreadline
```
Testing the Shell
Run commands like:
Built-ins: cd, pwd, ls.
Piped commands: ls | grep mini.
Redirection: ls > output.txt, ls >> output.txt.
This enhanced shell should provide a rich and user-friendly Linux experience!
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

void sigint_handler(int sig) {
    printf("\nUse 'exit' to quit the shell.\n");
    printf("\033[1;32mmini-shell$ \033[0m");
    fflush(stdout);
}

void read_input(char *buffer) {
    char *input = readline("\033[1;32mmini-shell$ \033[0m");
    if (!input) {
        exit(EXIT_FAILURE);
    }
    if (*input) {  // If not empty, add to history
        add_history(input);
    }
    strncpy(buffer, input, MAX_LINE);
    free(input);
}

void parse_input(char *input, char **args) {
    char *token = strtok(input, " \t\n");
    int i = 0;
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

int detect_piping(char *input, char **left, char **right) {
    char *pipe_pos = strchr(input, '|');
    if (pipe_pos) {
        *pipe_pos = '\0';
        parse_input(input, left);
        parse_input(pipe_pos + 1, right);
        return 1;
    }
    return 0;
}

int detect_redirection(char **args, char **redirect_file, int *redirect_type) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            *redirect_type = O_WRONLY | O_CREAT | O_TRUNC;
            *redirect_file = args[i + 1];
            args[i] = NULL;
            return 1;
        } else if (strcmp(args[i], ">>") == 0) {
            *redirect_type = O_WRONLY | O_CREAT | O_APPEND;
            *redirect_file = args[i + 1];
            args[i] = NULL;
            return 1;
        }
    }
    return 0;
}

void execute_piped_commands(char **args1, char **args2) {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return;
    }
    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        execvp(args1[0], args1);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[1]);
        execvp(args2[0], args2);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void execute_command(char **args) {
    char *redirect_file = NULL;
    int redirect_type = 0;

    if (detect_redirection(args, &redirect_file, &redirect_type)) {
        int fd = open(redirect_file, redirect_type, 0644);
        if (fd < 0) {
            perror("open");
            return;
        }
        pid_t pid = fork();
        if (pid == 0) {
            dup2(fd, STDOUT_FILENO);
            close(fd);
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        close(fd);
        waitpid(pid, NULL, 0);
    } else {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {  // Child process
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {  // Parent process
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void handle_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("chdir");
        }
    }
}

void handle_pwd() {
    char cwd[MAX_LINE];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd");
    }
}

void handle_clear() {
    printf("\033[H\033[J");  // ANSI escape code to clear the screen
}

void handle_ls() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    } else {
        perror("opendir");
    }
}

int main() {
    signal(SIGINT, sigint_handler);  // Handle Ctrl+C

    char input[MAX_LINE];
    char *args[MAX_ARGS];
    char *left_args[MAX_ARGS], *right_args[MAX_ARGS];

    while (1) {
        read_input(input);

        if (strlen(input) == 0) {
            continue;  // Ignore empty input
        }

        if (detect_piping(input, left_args, right_args)) {
            execute_piped_commands(left_args, right_args);
            continue;
        }

        parse_input(input, args);

        if (args[0] == NULL) {
            continue;  // Empty command
        }

        if (strcmp(args[0], "exit") == 0) {
            break;
        } else if (strcmp(args[0], "cd") == 0) {
            handle_cd(args);
        } else if (strcmp(args[0], "pwd") == 0) {
            handle_pwd();
        } else if (strcmp(args[0], "clear") == 0) {
            handle_clear();
        } else if (strcmp(args[0], "ls") == 0) {
            handle_ls();
        } else {
            execute_command(args);
        }
    }

    return 0;
}
