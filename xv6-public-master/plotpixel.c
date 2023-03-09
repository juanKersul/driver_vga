#include "types.h"
#include "stat.h"
#include "user.h"


int main (int argc, char **argv){
    if (argc == 4){
        plotpixel(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    }
    exit();
}