### Linux MiniShell (linux_minishell)

### Overview
Linux MiniShell is a lightweight custom shell written in C that supports basic Linux shell functionalities such as command execution, command history, piping, and output redirection. It also includes built-in commands like cd, pwd, clear, and ls.

### Features
Signal Handling: Handles Ctrl+C gracefully without terminating the shell.

Command History: Uses GNU Readline to provide command history and interactive input.

Piping: Supports | for chaining commands (e.g., ls | grep mini).

Output Redirection: Handles > and >> for output redirection to files.

Built-in Commands: Includes cd, pwd, clear, and ls.

### Compilation
Ensure you have the GNU Readline library installed. Use the following command:
```bash
gcc linux_minishell.c -o linux_minishell -lreadline
```
### Usage
Run the MiniShell executable:
```bash
./linux_minishell
```
###Example Commands:

Built-in commands:
cd /path/to/directory - Change directory.
pwd - Print the current working directory.
clear - Clear the terminal screen.
ls - List files in the current directory.

Piped commands:
ls | grep mini - Pipe output of ls to grep.

Redirection:
ls > output.txt - Redirect output to a file.
ls >> output.txt - Append output to a file.

Exiting:
Use 'exit' to terminate the shell.

### Requirements
GCC compiler
GNU Readline library (install with sudo apt-get install libreadline-dev on Debian-based systems)

### Notes
Use Ctrl+C to interrupt running commands, but the shell will remain active.
This shell does not include advanced features like job control or background execution.

### Author
Ben Santora 
