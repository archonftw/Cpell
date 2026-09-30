#include<filesystem>
#include<string>
#include "code.h"
using namespace std;

string getPathFromHome(){
    string str = filesystem::current_path().string();
    return str.substr(str.find_last_of("/")+1);

}