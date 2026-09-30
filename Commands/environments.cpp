#include "environments.h"
#include <cstdlib>
#include <string>
#include<cctype>
using namespace std;

string expandVariables(const std::string& input) {
    string result;
    for (size_t i = 0; i < input.size(); i++) {
        if (input[i] == '$') {
            size_t j = i + 1;
            while (j < input.size() &&
                   (std::isalnum(input[j]) || input[j] == '_')) {
                j++;
            }

            string variableName = input.substr(i + 1, j - (i + 1));
            const char* value = getenv(variableName.c_str());
            if (value != nullptr) {
                result += value;
            }
            i = j - 1;
        }
        else {
            result += input[i];
        }
    }
    return result;
}