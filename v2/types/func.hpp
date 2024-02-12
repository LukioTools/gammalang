#pragma once
//todo add le functions to the program


#include <functional>
#include <span>


class Any;
typedef Any(*function)(const std::span<Any>&) ;


template <>
struct std::hash<::function> {
    std::size_t operator()(const ::function& obj) const {
        return (long)obj;
    }
};
