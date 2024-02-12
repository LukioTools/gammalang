

#include <any>
#include <iostream>
#include <map>
#include <utility>
#include "any.hpp"

int main(int argc, char const *argv[])
{
    Any a(map({std::make_pair(Any(1), Any(2.46574657))}));

    auto m = a.as<map>();
    
    std::cout << m[Any(1)].as<decimal>() << std::endl;

    return 0;
}
