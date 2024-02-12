
#include <initializer_list>
#include <iostream>
#include <memory>
#include <utility>
#include <variant>
#include <vector>
#include "types/any.hpp"

int main(int argc, char const *argv[])
{
    Any a;
    {
        Any arr {map{std::make_pair(Any("Hello"), Any("World")),std::make_pair(Any(3), Any(3.3))}};
        std::cout << arr << std::endl;
        a = std::move(arr);
    }
    return 0;
}
