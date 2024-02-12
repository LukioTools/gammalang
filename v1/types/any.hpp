#pragma once

#include "integer.hpp"
#include "method.hpp"
#include "string.hpp"
#include <any>
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <system_error>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
class Any
{
protected:
    std::shared_ptr<std::any> _data = std::make_shared<std::any>();
public:
    static std::unordered_map<size_t, void(*)(std::ostream&, const Any&)> any_printable;
    static std::unordered_map<size_t , std::unordered_map<String, Method>> any_methods;

    std::shared_ptr<std::any> data() const{
        return _data;
    }
    std::any& any() const {
        return *_data;
    }
    std::unordered_map<String, Method>& methods(){
        return any_methods[code()];
    }
    Method method(String m){
        return methods()[m];
    }
    void method(String m, Method fn){
        methods()[m] = fn;
    }
    template<typename ...Args>
    Any call(String s, Args... a){
        auto fn = methods()[s];
        if(fn){
            std::vector<Any> v  = {*this, Any(a)...};
            return fn(v);
        }
        return {};
    }


    template<typename T>
    void operator=(const T& e){
        _data = e;
    }
    void operator=(const Any& e){
        _data = e._data;
    }
    
    template<typename T>
    void operator=(std::shared_ptr<T>) = delete;



    template<typename T>
    T& as()const {
        return std::any_cast<T&>(*_data.get());
    }
    const std::type_info& type() const {
        return any().type();
    }
    size_t code() const{
        return any().type().hash_code();
    }
    friend std::ostream& operator<<(std::ostream& os, const Any& a){
        auto it = any_printable.find(a.code());
        if(it != any_printable.end() && it->second)
            it->second(os, a);
        else
            std::cout << "Couldnt find print for type: " << a.type().name() << std::endl;
        return os;
    }

    static Any dupe(const Any a){
        return a;
    }
    static Any copy(const Any a){
        return a.any();
    }
    operator std::shared_ptr<std::any>(){
        return _data;
    }
    Any(){}
    Any(std::shared_ptr<std::any> ptr) : _data(ptr){}
    Any(const std::any& a) : _data(std::make_unique<std::any>(a)){}
    ~Any() {}
};

template<typename Type>
class TypedAny : public Any
{
private:
public:
    void operator=(const Any& a){
        if(a.code() != typeid(Type).hash_code())
            throw std::bad_any_cast();
        _data = a.data();
    }
    
    Type& as(){
        return std::any_cast<Type&>(any());
    }
    TypedAny(){}
    TypedAny(Type i) : Any(i){}
};

