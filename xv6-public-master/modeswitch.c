#include "types.h"
#include "stat.h"
#include "user.h"


int main (int argc, char **argv){
    if (argc == 2){
        modeswitch(atoi(argv[1]));
    }
    exit();
}
    
