#include "execute.h"
#include "../Commands/cd.h"

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <cstdlib>

using namespace std;

string executeCommand(vector<string> input) {

    if (input.empty()) {
        return "";
    }
    if (input[0] == "cd") {
        if (input.size() == 1) {
            return "Path not provided...\n";
        }
        changeDir(input[1]);
        return "";
    }

    if (input[0] == "help") {
        return R"(Cpell - A simple C++ shell Built-in commands:
          cd <directory>     Change the current directory
          pwd                Print the current working directory
          help               Show this help message
          exit               Exit Cpell

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

    // External commands
    pid_t pid = fork();
    if (pid == 0) {
        char* args[input.size() + 1];
        for (size_t i = 0; i < input.size(); i++) {
            args[i] = const_cast<char*>(input[i].c_str());
        }
        args[input.size()] = nullptr;
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);

    } 
    else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } 
    else {
        perror("fork failed");
    }
    return "";
}