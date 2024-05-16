#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <sys/wait.h> 
#include <signal.h>

// (STDIN)regen(STDOUT)--(write end)rgenOne(read end)--(STDIN)a1(STDOUT)--
// --(write end)oneTwo(read end)--(STDIN)a2(STDOUT)

// (STDIN)a3(STDOUT)--(write end)oneTwo(read end)--(STDIN)a2(STDOUT)

// execute the rgen to get random streets.
int execrgen(int argc, char **argv){
    argv[0] = (char *) "rgen";
    int exresult = execv("rgen", argv);
    /*
    if (exresult != 0) {
        exit(1);
    }
    */
    return exresult;
}

/*
int spInput(int thePipe[2]) {
    while (!std::cin.eof()) {
        std::string line;
        std::getline(std::cin, line);

        if (line.empty()) {
            // inputFile.close(); // Close the file
            exit(0); // Terminate the program
        }

        // std::cout << line << std::endl;
        FILE* stream;
        close(thePipe[0]);
        stream = fdopen(thePipe[1], "w");
        fprintf(stream, "%s", line.c_str());
        close(thePipe[0]);
        fflush(stream);
    }
    return 0;
}
*/

int spInput() {
    while (!std::cin.eof()) {
        std::string line;
        std::getline(std::cin, line);

        if (line.empty()) {
            // inputFile.close(); // Close the file
            return 0;
        }

        std::cout << line << std::endl;
    }
    return 0;
}

int main (int argc, char **argv) {

    std::vector<pid_t> kids;
    
    pid_t child;
    
    // pipe between rgen and a1
    int rgenOne[2];
    pipe(rgenOne);

    // pipe between a1 and a2
    int oneTwo[2];
    pipe(oneTwo);

    // child process: rgen
    child = fork();

    if (child < 0) {
        fprintf(stderr, "Error: fork the driver program to rgen failed!\n");
    } else if (child == (pid_t) 0) {
        // This is the child process.
        close(rgenOne[0]);
        // redirect stdout of the child process(rgen) to the write end of the pipe between rgen and a1 
        dup2(rgenOne[1], STDOUT_FILENO);
        close(rgenOne[1]);
        return execrgen(argc, argv);
        /*
        if (exresult != 0) {
            exit(1);
        }
        */
    } 

    kids.push_back(child);

    // child process: a1
    child = fork();

    if (child < 0) {
        fprintf(stderr, "Error: fork the driver program to a1 failed!\n");
    } else if (child == (pid_t) 0) {
        // This is the child process.
        close(rgenOne[1]);
        // redirect stdin of the child process(a1) to the read end of the pipe between rgen and a1 
        dup2(rgenOne[0], STDIN_FILENO);
        close(rgenOne[0]);
        
        close(oneTwo[0]);
        // redirect stdout of the child process(a1) to the write end of the pipe between a1 and a2 
        dup2(oneTwo[1], STDOUT_FILENO);
        close(oneTwo[1]);
        char* argvOne[3];
        argvOne[0] = strdup("python3");
        argvOne[1] = strdup("streets.py");
        argvOne[2] = NULL;
        return execvp("python3", argvOne);
    } 

    kids.push_back(child);

    // child process: a2
    child = fork();

    if (child < 0) {
        fprintf(stderr, "Error: fork the driver program to a2 failed!\n");
    } else if (child == (pid_t) 0) {
        // This is the child process.  
        close(oneTwo[1]);
        // redirect stdin of the child process(a2) to the read end of the pipe between a1 and a2
        dup2(oneTwo[0], STDIN_FILENO);
        close(oneTwo[0]);
        char* argvTwo[2];
        argvTwo[0] = strdup("find-shortest-paths");
        argvTwo[1] = NULL;
        return execv("find-shortest-paths", argvTwo);
    }     
    kids.push_back(child);

    child = 0;
    close(oneTwo[0]);
    // redirect stdout of the parent process to the write end of the pipe between a1 and a2
    // so that inputs like "s 1 7" could be directed to a2 for further process
    dup2(oneTwo[1], STDOUT_FILENO);
    close(oneTwo[1]);

    int result = spInput();

    for (pid_t kid : kids) {
        int status;
        kill(kid, SIGTERM);
        waitpid(kid, &status, 0);
        /*
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            exit(1);
        }
        */
    }

    return result;
}
