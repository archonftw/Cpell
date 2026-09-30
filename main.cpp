#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

#include <readline/readline.h>
#include <readline/history.h>

#include "./Parsing/parser.h"
#include "./Execute/execute.h"
#include "./utils/code.h"

using namespace std;

int main() {

    // Enable autocomplete
    rl_attempted_completion_function = autocomplete;

    while (true) {

        string prompt = "[~/" + getPathFromHome() + "]$ ";

        // Read input using readline
        char* rawInput = readline(prompt.c_str());

        // Ctrl + D
        if (rawInput == nullptr) {
            cout << endl;
            break;
        }

        string input = rawInput;

        // Add command to history
        if (!input.empty()) {
            add_history(rawInput);
        }

        free(rawInput);

        vector<string> parsedInput = parser(input);

        // Empty input
        if (parsedInput.empty()) {
            continue;
        }

        // Exit
        if (parsedInput[0] == "exit") {
            break;
        }

        cout << executeCommand(parsedInput) << endl;
    }

    return 0;
}