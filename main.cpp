#include <iostream>
#include "ground.hpp"

int main(int argc, char *argv[])
{
    //wczytanie argumentu (ilosci kulek)
    int x= atoi(argv[argc-1]);
    Ground ground(x);
    ground.Start();
    return 0;
}