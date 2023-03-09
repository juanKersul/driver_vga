#include "types.h"
#include "stat.h"
#include "user.h"


int main (int argc, char **argv){
    if (argc == 6){
        plotrectangle(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]),atoi(argv[5]));
    }
    exit();
}