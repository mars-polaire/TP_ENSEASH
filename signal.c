#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        // Par défaut, SIGSEGV
        raise(SIGSEGV);
        return 0;
    }
    
    int signal_num = atoi(argv[1]);
    raise(signal_num);
    
    return 0;
}
