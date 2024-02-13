

#include <any>
#include <iostream>
#include <map>
#include <ostream>
#include <utility>
#include "any.hpp"

int main(int argc, char const *argv[])
{
    Any a(32);
    Any aa(32);
    Any b(23.3213);
    std::cout << a.callva("toString").as<string>() + '+' + b.callva("toString").as<string>() << std::endl;
    return 0;
}
