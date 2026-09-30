#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include<pwd.h>
#include<unistd.h>
#include<signal.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "./Parsing/parser.h"
#include "./Execute/execute.h"
#include "./utils/code.h"
#include "./Commands/environments.h"

using namespace std;

int main() {
    initShell();

    // Enables autocomplete modes
    rl_attempted_completion_function = autocomplete;
    while (true) {
        uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);    
        string user = pw->pw_name;
        string capitalizedUser = user;
        capitalizedUser[0] = toupper(capitalizedUser[0]);
        string prompt = "\033[36m" +
                user + "@" +
                capitalizedUser + ":/" +
                getPathFromHome() +
                "$ \033[0m";
        char* rawInput = readline(prompt.c_str());
        if (rawInput == nullptr) {
            cout << endl;
            break;
        }
        string input = rawInput;

        if (!input.empty()) {
            add_history(rawInput);
        }
        free(rawInput);

        vector<string> parsedInput = parser(input);

        // Empty input
        if (parsedInput.empty()) continue;
        // Exit
        if (parsedInput[0] == "exit") break;

        for(string& arg:parsedInput) arg = expandVariables(arg);

        cout << executeCommand(parsedInput) << endl;
    }

    return 0;
}