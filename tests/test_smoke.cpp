#include <cstdlib>
#include <iostream>

int main()
{
    if(1+1==2)
    {
        std::cerr<<'good';
        return EXIT_SUCCESS;
    }
    else
    {
        std::cerr<<'bad';
        return EXIT_FAILURE;
    }

}