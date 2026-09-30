#include"execute.h"
#include<unistd.h>
#include<sys/wait.h>
#include<iostream>

string executeCommand(vector<string> input){
    if(input.empty()) return "";            //If the input is empty
    
    pid_t pid = fork();
    if(pid==0){
        
        char* args[input.size()+1];
        for(size_t i=0;i<input.size();i++){
            args[i]=const_cast<char*>(input[i].c_str());
        }
        args[input.size()]=NULL;

        execvp(args[0],args);
        perror("execvp failed");
        exit(1);
    }else if(pid>0){
        int status;
        waitpid(pid,&status,0);
    }else{
        perror("Fork failed");
    }
    return "";



}