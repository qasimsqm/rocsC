#include "exlib/path.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << "usage: path_test <path to create>" << endl;
        return -1;
    }
    
    ex::path::makedirs(argv[1]);
    return 0;
}
