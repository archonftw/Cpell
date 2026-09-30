#include<iostream>
#include<vector>
#include<string>
#include "./Parsing/parser.h"
#include "./Execute/execute.h"
#include "./utils/code.h"
using namespace std;



int main(){
    while(true){
        cout << "[~/" << getPathFromHome() << "]$ ";
        string input;
        getline(cin,input);

        vector<string> parsedInput = parser(input);
        cout<<executeCommand(parsedInput)<<endl;
    }
    return 0;
}