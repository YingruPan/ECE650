#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <stdio.h>
#include <cstdlib>

using namespace std;

void handle_child_termination(int sig) {
    exit(0); // terminate parent process
}

int main(int argc, char *argv[]){

    signal(SIGCHLD, handle_child_termination);

    // pipe1 is rgen to a1, pipe2 is a1 to a2
    int pipe1[2], pipe2[2];
    pipe(pipe1);
    pipe(pipe2);

    // rgen process
    pid_t pid_rgen = fork();
    if(pid_rgen < 0){
        cerr << "Error: failed to fork rgen" << endl;
        return 1;
    }
    else if(pid_rgen == 0){
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe1[0]);
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[1]);
        char *newargv[argc + 1];
        for (int i = 0; i < argc; ++i) {
            newargv[i] = argv[i];
        }
        newargv[argc] = NULL;
        execv("./rgen", newargv);
        cerr << "Error: execv failed for rgen" << endl;
        exit(1);
    }

    // a1 process
    pid_t pid_a1 = fork();
    if(pid_a1 < 0){
        cerr << "Error: failed to fork a1" << endl;
        return 1;
    }
    else if(pid_a1 == 0){
        close(pipe1[1]);
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);
        close(pipe2[0]);
        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe2[1]);
        char *args[] = {"python3", "./ece650-a1.py", NULL};
        execvp(args[0], args); 
        cerr << "Error: execv failed for a1" << endl;
        exit(1);
    }

    // a2 process
    pid_t pid_a2 = fork();
    if(pid_a2 < 0){
        cerr << "Error: failed to fork a2" << endl;
        return 1;
    }
    else if(pid_a2 == 0){
        close(pipe1[1]);
        close(pipe1[0]);
        close(pipe2[1]);
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[0]);
        execv("./ece650-a2", NULL);
        cerr << "Error: execv failed for a2" << endl;
        exit(1);
    }

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    dup2(pipe2[1], STDOUT_FILENO);
    close(pipe2[1]);
    string sline;
    while(getline(cin, sline)){
        if (sline[0] == 's'){
            cout << sline << endl;
        }
    }

    return 0;
}
