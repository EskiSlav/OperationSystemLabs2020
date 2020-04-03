#include <iostream>
#include <string>
// Required by for routine
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
// Declaration for exit()
using namespace std;

int globalVariable = 2;

int main()
{
    string sIdentifier;
    int iStackVariable = 20;
    pid_t pID;
    pID = fork();
    if (pID == 0)
    {
        // Code only executed by child process
        sIdentifier = "Child Process: ";
        sleep(1);
        globalVariable++;
        iStackVariable++;
        execl("/bin/ps", "/bin/ps", "-U", "eski", 0);
        _exit(EXIT_SUCCESS);
        
    }
    else if (pID < 0)
    {
        cerr << "Failed to fork" << endl;
        exit(1);
        // Throw exception
    }
    else
    {
        // Code only executed by parent process
        sIdentifier = "Parent Process:";
    }
    // Code executed by both parent and child.
    cout << sIdentifier;
    cout << " Global variable: " << globalVariable;
    cout << " Stack variable: " << iStackVariable 
        << " Pid: " << getpid() << endl;
}