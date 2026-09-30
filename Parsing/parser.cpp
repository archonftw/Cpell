#include "parser.h"
#include<sstream>

vector<string> parser(string s){
    stringstream ss(s);
    vector<string> inputParsed;
    string word;

    while(ss>>word){
        inputParsed.push_back(word);
    }
    return inputParsed;
}