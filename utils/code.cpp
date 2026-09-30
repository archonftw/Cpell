#include "code.h"

#include <filesystem>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

#include <readline/readline.h>

using namespace std;
namespace fs = std::filesystem;


// Built-in commands
vector<string> commands = {
    "cd",
    "pwd",
    "help",
    "exit"
};


// Get current directory name
string getPathFromHome() {

    string str = filesystem::current_path().string();

    return str.substr(str.find_last_of("/") + 1);
}


// Generate possible command completions
char* commandGenerator(const char* text, int state) {

    static vector<string> matches;
    static size_t index;

    if (state == 0) {

        matches.clear();
        index = 0;

        string prefix(text);

        // Add built-in commands
        for (const string& command : commands) {

            if (command.rfind(prefix, 0) == 0) {
                matches.push_back(command);
            }
        }


        // Search executables in PATH
        const char* pathEnv = getenv("PATH");

        if (pathEnv != nullptr) {

            string path(pathEnv);
            size_t start = 0;

            while (start < path.size()) {

                size_t end = path.find(':', start);

                if (end == string::npos) {
                    end = path.size();
                }

                string directory = path.substr(start, end - start);

                try {

                    for (const auto& entry : fs::directory_iterator(directory)) {

                        string filename =
                            entry.path().filename().string();

                        if (
                            filename.rfind(prefix, 0) == 0 &&
                            fs::is_regular_file(entry.path()) &&
                            (fs::status(entry.path()).permissions()
                             & fs::perms::owner_exec) != fs::perms::none
                        ) {

                            matches.push_back(filename);
                        }
                    }

                }
                catch (const fs::filesystem_error&) {
                    // Ignore directories we cannot access
                }

                start = end + 1;
            }
        }
    }


    // Return next match
    if (index < matches.size()) {

        return strdup(matches[index++].c_str());
    }

    return nullptr;
}


// Called by readline when TAB is pressed
char** autocomplete(const char* text, int start, int end) {

    // Only autocomplete the first word
    if (start == 0) {

        return rl_completion_matches(
            text,
            commandGenerator
        );
    }

    return nullptr;
}