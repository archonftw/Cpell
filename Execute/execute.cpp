#include "execute.h"
#include "../Commands/cd.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>
#include <errno.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include<algorithm>

using namespace std;

struct Job {
    int id;
    pid_t pgid;
    string command;
    bool running;
};

vector<Job> jobs;
int nextJobId = 1;
pid_t shellPgid;
int shellTerminal;

void sigchldHandler(int) {}

void initShell() {
    shellTerminal = STDIN_FILENO;

    while (tcgetpgrp(shellTerminal) != (shellPgid = getpgrp()))
        kill(-shellPgid, SIGTTIN);

    shellPgid = getpid();

    if (setpgid(shellPgid, shellPgid) < 0) {
        perror("setpgid");
        exit(1);
    }

    tcsetpgrp(shellTerminal, shellPgid);

    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, sigchldHandler);
}

void updateJobs() {
    for (auto it = jobs.begin(); it != jobs.end();) {
        int status;
        pid_t r = waitpid(-it->pgid, &status,
                          WNOHANG | WUNTRACED | WCONTINUED);

        if (r == 0) {
            ++it;
            continue;
        }

        if (r == -1) {
            if (errno == ECHILD) it = jobs.erase(it);
            else ++it;
            continue;
        }

        if (WIFSTOPPED(status)) it->running = false;
        if (WIFCONTINUED(status)) it->running = true;
        if (WIFEXITED(status) || WIFSIGNALED(status))
            it = jobs.erase(it);
        else
            ++it;
    }
}

Job* findJob(int id) {
    for (auto& job : jobs)
        if (job.id == id) return &job;
    return nullptr;
}

void printJobs() {
    updateJobs();

    for (const auto& job : jobs)
        cout << "[" << job.id << "] "
             << (job.running ? "Running" : "Stopped")
             << "    " << job.command << '\n';
}

string executeCommand(vector<string> input) {
    if (input.empty()) return "";

    if (input[0] == "jobs") {
        printJobs();
        return "";
    }

    if (input[0] == "bg") {
        if (input.size() != 2) return "Usage: bg JOB_ID\n";

        Job* job = findJob(stoi(input[1]));
        if (!job) return "Cpell: job not found\n";

        kill(-job->pgid, SIGCONT);
        job->running = true;

        cout << "[" << job->id << "] "
             << job->command << " &\n";
        return "";
    }

    if (input[0] == "fg") {
        if (input.size() != 2) return "Usage: fg JOB_ID\n";

        Job* job = findJob(stoi(input[1]));
        if (!job) return "Cpell: job not found\n";

        tcsetpgrp(shellTerminal, job->pgid);
        kill(-job->pgid, SIGCONT);
        job->running = true;

        int status;
        waitpid(-job->pgid, &status, WUNTRACED);

        tcsetpgrp(shellTerminal, shellPgid);

        if (WIFSTOPPED(status))
            job->running = false;
        else if (WIFEXITED(status) || WIFSIGNALED(status))
            jobs.erase(
                remove_if(jobs.begin(), jobs.end(),
                    [&](const Job& j) { return j.id == job->id; }),
                jobs.end()
            );

        return "";
    }

    if (input[0] == "cd") {
        if (input.size() == 1) return "Path not provided...\n";
        changeDir(input[1]);
        return "";
    }

    if (input[0] == "help")
        return "Cpell commands:\n"
               "  cd <dir>\n"
               "  help\n"
               "  export NAME=value\n"
               "  unset NAME\n"
               "  env\n"
               "  jobs\n"
               "  bg JOB_ID\n"
               "  fg JOB_ID\n";

    if (input[0] == "export") {
        if (input.size() != 2) return "Usage: export NAME=value\n";

        string assignment = input[1];
        size_t pos = assignment.find('=');

        if (pos == string::npos)
            return "Usage: export NAME=value\n";

        string name = assignment.substr(0, pos);
        string value = assignment.substr(pos + 1);

        if (name.empty()) return "Invalid variable name\n";

        setenv(name.c_str(), value.c_str(), 1);
        return "";
    }

    if (input[0] == "unset") {
        if (input.size() != 2) return "Usage: unset NAME\n";
        unsetenv(input[1].c_str());
        return "";
    }

    if (input[0] == "env") {
        extern char** environ;
        for (char** e = environ; *e; e++)
            cout << *e << '\n';
        return "";
    }

    bool background = !input.empty() && input.back() == "&";
    if (background) input.pop_back();

    pid_t pid = fork();

    if (pid == 0) {
        setpgid(0, 0);

        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        for (size_t i = 0; i < input.size(); i++) {
            int fd = -1;

            if (input[i] == ">") {
                if (i + 1 >= input.size()) {
                    cerr << "Cpell: expected filename after >\n";
                    exit(1);
                }

                fd = open(input[i + 1].c_str(),
                          O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            else if (input[i] == ">>") {
                if (i + 1 >= input.size()) {
                    cerr << "Cpell: expected filename after >>\n";
                    exit(1);
                }

                fd = open(input[i + 1].c_str(),
                          O_WRONLY | O_CREAT | O_APPEND, 0644);
            }
            else if (input[i] == "<") {
                if (i + 1 >= input.size()) {
                    cerr << "Cpell: expected filename after <\n";
                    exit(1);
                }

                fd = open(input[i + 1].c_str(), O_RDONLY);
            }

            if (fd != -1) {
                int target = input[i] == "<" ? STDIN_FILENO : STDOUT_FILENO;

                if (dup2(fd, target) == -1) {
                    perror("dup2");
                    exit(1);
                }

                close(fd);
                input.erase(input.begin() + i, input.begin() + i + 2);
                i--;
            }
        }

        vector<char*> args;

        for (auto& arg : input)
            args.push_back(const_cast<char*>(arg.c_str()));

        args.push_back(nullptr);

        execvp(args[0], args.data());

        perror("execvp failed");
        exit(1);
    }

    if (pid < 0) {
        perror("fork failed");
        return "";
    }

    setpgid(pid, pid);

    if (background) {
        Job job{nextJobId++, pid, "", true};

        for (const auto& arg : input)
            job.command += arg + " ";

        jobs.push_back(job);

        cout << "[" << job.id << "] " << pid << '\n';
        return "";
    }

    tcsetpgrp(shellTerminal, pid);

    int status;
    waitpid(-pid, &status, WUNTRACED);

    tcsetpgrp(shellTerminal, shellPgid);

    if (WIFSTOPPED(status)) {
        Job job{nextJobId++, pid, "", false};

        for (const auto& arg : input)
            job.command += arg + " ";

        jobs.push_back(job);

        cout << "\n[" << job.id << "]+  Stopped    "
             << job.command << '\n';
    }

    return "";
}