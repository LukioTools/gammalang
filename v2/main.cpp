
#include <initializer_list>
#include <iostream>
#include <memory>
#include <span>
#include <utility>
#include <variant>
#include <vector>
#include "types/any.hpp"


Any fn(const std::span<Any>& args){
    if(args.empty())
        std::cout << "No arguments were provided :(" << std::endl;
    for(auto e : args)
        std::cout << e << std::endl;
    return {};
}

int main(int argc, char const *argv[])
{
    Any a = fn;

    std::cout << a << std::endl;

    a.as<function>()({});

    return 0;
}
