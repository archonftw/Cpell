#include "execute.h"
#include "../Commands/cd.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <signal.h>
#include <cstdlib>

using namespace std;

string executeCommand(vector<string> input) {

    if (input.empty()) {
        return "";
    }

    // cd
    if (input[0] == "cd") {
        if (input.size() == 1) {
            return "Path not provided...\n";
        }

        changeDir(input[1]);
        return "";
    }

    // help
    if (input[0] == "help") {
        return R"(Cpell - A simple C++ shell

        Built-in commands:
          cd <directory>     Change the current directory
          pwd                Print the current working directory
          help               Show this help message
          exit               Exit Cpell
          export NAME=value  Set an environment variable
          unset NAME         Remove an environment variable
          env                Show environment variables

        External commands:
          Cpell can execute external programs available in your PATH.

        Examples:
          ls
          ls -la
          cd /home/archon
          pwd
          cat file.txt

        )";
    }

    // export
    if (input[0] == "export") {

        if (input.size() != 2) {
            return "Usage: export NAME=value\n";
        }

        string assignment = input[1];

        size_t equalPos = assignment.find('=');

        if (equalPos == string::npos) {
            return "Usage: export NAME=value\n";
        }

        string name = assignment.substr(0, equalPos);
        string value = assignment.substr(equalPos + 1);

        if (name.empty()) {
            return "Invalid variable name\n";
        }

        setenv(name.c_str(), value.c_str(), 1);

        return "";
    }

    // unset
    if (input[0] == "unset") {

        if (input.size() != 2) {
            return "Usage: unset NAME\n";
        }

        unsetenv(input[1].c_str());

        return "";
    }

    // env
    if (input[0] == "env") {

        extern char** environ;

        for (char** env = environ; *env != nullptr; env++) {
            cout << *env << '\n';
        }

        return "";
    }

    // External commands
    pid_t pid = fork();

    if (pid == 0) {
        signal(SIGINT,SIG_DFL);


        for (size_t i = 0; i < input.size(); i++) {
            if (input[i] == ">") {
                if (i + 1 >= input.size()) {
                    cerr << "Cpell: expected filename after >\n";
                    exit(1);
                }
                int fd = open(
                    input[i + 1].c_str(),
                    O_WRONLY | O_CREAT | O_TRUNC,
                    0644
                );
                if (fd == -1) {
                    perror("open");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
                input.erase(input.begin() + i, input.begin() + i + 2);
                i--;
            }

            else if (input[i] == ">>") {
                if (i + 1 >= input.size()) {
                    cerr << "Cpell: expected filename after >>\n";
                    exit(1);
                }
                int fd = open(
                    input[i + 1].c_str(),
                    O_WRONLY | O_CREAT | O_APPEND,
                    0644
                );
                if (fd == -1) {
                    perror("open");
                    exit(1);
                }

                dup2(fd, STDOUT_FILENO);
                close(fd);
                input.erase(input.begin() + i, input.begin() + i + 2);
                i--;
            }
            else if (input[i] == "<") {
                if (i + 1 >= input.size()) {
                    cerr << "Cpell: expected filename after <\n";
                    exit(1);
                }
                int fd = open(
                    input[i + 1].c_str(),
                    O_RDONLY
                );
                if (fd == -1) {
                    perror("open");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
                input.erase(input.begin() + i, input.begin() + i + 2);
                i--;
            }
        }

        char* args[input.size() + 1];
        for (size_t i = 0; i < input.size(); i++) {
            args[i] = const_cast<char*>(input[i].c_str());
        }
        args[input.size()] = nullptr;

        execvp(args[0], args);

        // execvp only returns if something went wrong
        perror("execvp failed");
        exit(1);
    }

    // Parent
    else if (pid > 0) {

        int status;

        waitpid(pid, &status, 0);
    }

    // fork failed
    else {

        perror("fork failed");
    }

    return "";
}