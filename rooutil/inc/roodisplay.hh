/*
Utility to call event display from within a root macro

to use:

  launch_display("mcs.mu2e.ensembleMDS2cMix1BBTriggered.MDC2020ba_best_v1_3.art", 15028,34,1201);
*/

#include <cstdlib> 
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>


int execute_command(const std::string& program, const std::vector<std::string>& args) {
    /*
   Executes a command by invoking the executable directly, avoiding shell injection risks.
   
   * program: The name of the executable.
   * args: The arguments to pass to the executable.
   * return int The exit status of the child process.
   * throws std::runtime_error if the command fails to execute.
   */
    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("fork() failed");
    }

    if (pid == 0) { // Child process
        std::vector<const char*> argv_c_str;
        argv_c_str.push_back(program.c_str());
        for (const auto& arg : args) {
            argv_c_str.push_back(arg.c_str());
        }
        argv_c_str.push_back(nullptr);

        execvp(program.c_str(), const_cast<char* const*>(argv_c_str.data()));
        _exit(127); // execvp failed
    }

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1; // Indicate a non-standard termination
}

void launch_display(std::string dataset, unsigned int event, unsigned int subrun, unsigned int run) {
    std::cout << "========== Launching Mu2e/EventDisplay ===============" << std::endl;
    
    // Command 1: pickEvent
    std::cout << "Extracting the data-set..." << std::endl;
    std::string run_subrun_event = std::to_string(run) + "/" + std::to_string(subrun) + "/" + std::to_string(event);
    std::vector<std::string> args1 = {"-e", "-v", dataset, run_subrun_event};
    
    if (execute_command("pickEvent", args1) != 0) {
        throw std::runtime_error("pickEvent command failed");
    }

    // Command 2: mu2e
    std::cout << "Data-set has been extracted, now running display." << std::endl;
    std::string art_file_name = dataset + "_" + std::to_string(run) + "_" + std::to_string(subrun) + "_" + std::to_string(event) + ".art";
    std::vector<std::string> args2 = {"-c", "EventDisplay/examples/nominal_example.fcl", art_file_name};
    
    if (execute_command("mu2e", args2) != 0) {
        throw std::runtime_error("mu2e command failed");
    }
    
    std::cout << "Display finished." << std::endl;
}

