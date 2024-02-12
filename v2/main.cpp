
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

/*
//can be loosely typed if you just leave the type away
            ///tripledots are va_args                     //strongly typed args like this
string function fn_name(arg1, arg2 = "default constructors", string: arg3 = "where ever", arg4...){ //notice brackets
    ///the functioning shit here.....
    //access to variables defined before function.. somehow :3

    --- insert code herea ---

    return arg2 + " world";
}   //no forced return, but can if one wants to

*/

/*
so that function would expand to something like so:

//or string?
(Any | string) fn_name(const std::span<Any>& args){
    Any arg1;
    Any arg2;
    (Any | string) arg3;
    (Any | array) arg4;
    
    if(span.size() > 0)
        arg1 = span[0];
    if(span.size() > 1)
        arg2 = span[1];
    if(span.size() > 2)
        arg3 = span[2].has_value()? span[2] : "where ever";
            //or how ever spans work, if dont just a for loop and append
    (Any | array) arg4 = span.subspan(3);

    --insert code here--

    return arg2.as<string>() + string(" world");
}


*/


int main(int argc, char const *argv[])
{
    Any a = fn;

    std::cout << a << std::endl;

    a.as<function>()({});

    return 0;
}
