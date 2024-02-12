




#include "types/any.hpp"
#include "types/array.hpp"
#include "types/float.hpp"
#include "types/integer.hpp"
#include "types/map.hpp"
#include "types/method.hpp"
#include "types/string.hpp"
#include <any>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <variant>



std::unordered_map<size_t, void(*)(std::ostream&, const Any&)> Any::any_printable{
    std::make_pair(typeid(Integer).hash_code(), [](std::ostream& o, const Any& a){
        o << a.as<Integer>();
    }),
    std::make_pair(typeid(String).hash_code(), [](std::ostream& o, const Any& a){
        o << a.as<String>();
    })
};
std::unordered_map<size_t , std::unordered_map<String, Method>> Any::any_methods;


enum variant_e{
    STRING,
    INTEGER,
    FLOAT,
    ARRAY,
};
typedef std::variant<String, Integer, Float, Array> a;
int main(int argc, char const *argv[])
{

    a var = 22L;//String("Hello!");
    
    std::map<a, int> a;




    /*
    Any e(Integer(33));
    e.methods()["print"] = [](std::vector<Any>& args){
        for(auto& e : args)
            std::cout << e.as<Integer>() << ' ';
        return Any();
    };
    e.call("print");
    */

    //std::map<std::any, std::shared_ptr<std::any>> map;
    //map[String("hello")] = Any(Integer(343));
    //std::cout << Any(map[String("hello")]) << std::endl;



    return 0;
}
