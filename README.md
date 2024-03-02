# CShell
CShell is a command-line interprer that mimics a Linux/Unix command line made using C. This shell supports listing files and directories in the file system, history of executed commands, and environment variables. Upon calling an unsupported function, CShell will create a fork to a child process, where the command will be passed and executed by the normal Linux/Unix terminal. The commands include:
```
$VAR=foo
print $VAR
pwd
ls
whoami
theme red
log
exit
```
cshell support the following built-in commands

● exit, the shell terminates on this Command. This must be implemented for a clean exit of
the program.

● log, the shell prints history of executed commands with time and return code

● print, the shell prints argument given to this Command

● theme, the shell changes the color of and output

cshell also takes a non-built-in command (like ls, pwd, whoami), it is executed in the child
process, and Command's output is printed. cshell creates a child process using fork() system
call, then cshell waits for the child process to terminate via wait() system call. Child process
executes a non-built-in command using exec() and its analogues.

#

Our cshell will work in two modes: interactive mode and script mode

The interactive mode is activated when cshell starts without command line arguments.
Interactive mode is essentially the following loop:

● Print out a prompt

● Read line

● Parse line, and if the line is non-valid, print an error message

● If the line was valid, do what should be done.

```bash
./cshell
$cshell 
```

For script mode, it should start like ./cshell <filename> The following testScripts.txt is provided in the repo.

In script mode, the shell does the following for each line of the file:

● Read the line of the file

● Parse line, if the line is non-valid, print an error message

● If the line was valid, do what should be done.

● The program must exit cleanly after executing in script mode

```bash
./cshell testScripts.txt

foo 
/home/user/CShell
user
Sat Mar  2 02:40:40 2024
 $VAR=foo 0
Sat Mar  2 02:40:40 2024
 print 0
Sat Mar  2 02:40:40 2024
 pwd 0
Sat Mar  2 02:40:40 2024
 whoami 0
Sat Mar  2 02:40:40 2024
 theme 0
Bye!
```

## Makefile

Compile: 
	make all
Delete compiled file：
	make clean

## Disclaimer

A Linux/Unix environment is required to run this program successfully as it uses the Linux/Unix system calls to run certain functions. Windows users can run this program through [WSL]

