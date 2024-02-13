

#include <any>
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

class Any;
class map;
class array;
class string : public std::string
{
public:
    using std::string::basic_string;
};
typedef long integer;
typedef double decimal;
typedef Any(*function)(const std::span<Any>&);

class Any 
{
private:
    std::shared_ptr<std::any> ptr = std::make_shared<std::any>();
public:
    static std::unordered_map<size_t, std::unordered_map<std::string, function>> any_methods;

    enum AnyE{
        NONE,
        INTEGER,
        DECIMAL,
        STRING,
        ARRAY,
        MAP,
        FUNCTION,
    };

    std::size_t hash()const;

    template<typename T>
    T& as()const{
        try {
        return std::any_cast<T&>(*ptr);
        } catch (const std::bad_any_cast& bad_cast) {
            std::cout << "bad cast between: " << typeid(T).name() << " and " << ptr->type().name() << std::endl;
            throw std::bad_cast();
        }
    }



    Any call(const string& name, array& span)const;
    template<class ...Args>
    Any callva(const string& name, Args ...a)const;
    template<class T>
    static void add_method(const string& name, function fn){
        any_methods[typeid(T).hash_code()][name] = fn;
    }

    void operator=(const Any& a);
    template<class T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    void operator=(const T& a){*ptr = (integer)a;};
    template<class T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = 0>
    void operator=(const T& a){*ptr = (decimal)a;};
    void operator=(const function& a);
    void operator=(const string& a);
    void operator=(const array& a);
    void operator=(const map& a);

    bool operator==(const Any& a)const;
    template<class T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    bool operator==(const T& a)const{
        return as<integer>() == (integer)a;
    };
    template<class T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = 0>
    bool operator==(const T& a)const{
        return as<decimal>() == (decimal)a;
    };
    bool operator==(const function& a)const;
    bool operator==(const string& a)const;
    bool operator==(const array& a)const;
    bool operator==(const map& a)const;

    operator integer(){
        return as<integer>();
    };
    operator decimal(){
        return as<decimal>();
    };
    operator function(){
        return as<function>();
    };
    operator string&(){
        return as<string>();
    };
    operator array&(){
        return as<array>();
    };
    operator map&(){
        return as<map>();
    };

    Any() = default;
    template<class T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    Any(const T l) : ptr(std::make_shared<std::any>((integer)l)){}
    //template<class T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    template<class T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = 0>
    Any(const T d) : ptr(std::make_shared<std::any>((decimal)d)){}
    Any(const array& arr) : ptr(std::make_shared<std::any>(arr)){}
    Any(const map& m) : ptr(std::make_shared<std::any>(m)){}
    Any(const string& str) : ptr(std::make_shared<std::any>(str)){}
    Any(const function fn) : ptr(std::make_shared<std::any>(fn)){}
    ~Any() = default;
};

template<>
struct std::hash<Any>{
    std::size_t operator()(const Any& a) const{
        return 1;
    }
    std::size_t operator()(Any& a) const{
        return 1;
    }
};

class map : public std::unordered_map<Any, Any>
{
private:
public:
    using std::unordered_map<Any, Any>::unordered_map;
};


class array : public std::vector<Any>
{
public:
    using std::vector<Any>::vector;
};

inline std::unordered_map<std::size_t, std::unordered_map<std::string, function>> Any::any_methods({
    {
        std::make_pair(
            typeid(void).hash_code(),
            std::unordered_map<std::string, function>({
            std::make_pair("=", [](const std::span<Any>& span){
                    if(span.size() < 2)
                        return Any(false);
                    auto& a = span[0];
                    auto& b = span[1];
                    a.ptr = b.ptr;
                    return Any(a);
                }),
            })),
        std::make_pair(
            typeid(integer).hash_code(),
            std::unordered_map<std::string, function>({
                std::make_pair("==", [](const std::span<Any>& span){
                    if(span.size() < 2)
                        return Any(false);
                    auto& a = span[0];
                    auto& b = span[1];
                    if(a.ptr->type() != b.ptr->type() || a.ptr->type()!=typeid(integer))
                        return Any(false);
                    return Any(a.as<integer>() == b.as<integer>());
                }),
                std::make_pair("toString", [](const std::span<Any>& span){
                    if(span.empty())
                        return Any();
                    auto& a = span[0];
                    return Any(std::to_string(a.as<integer>()).c_str());
                }),
            })),
        std::make_pair(
            typeid(double).hash_code(),
            std::unordered_map<std::string, function>({
                std::make_pair("==", [](const std::span<Any>& span){
                    if(span.size() < 2)
                        return Any(false);
                    auto& a = span[0];
                    auto& b = span[1];
                    if(a.ptr->type() != b.ptr->type() || a.ptr->type()!=typeid(double))
                        return Any(false);
                    return Any(a.as<double>() == b.as<double>());
                }),
                std::make_pair("toString", [](const std::span<Any>& span){
                    if(span.empty())
                        return Any();
                    return Any(std::to_string(span[0].as<decimal>()).c_str());
                }),
            })),
    }
});


inline bool Any::operator==(const Any& a)const{
    auto& t = ptr->type();
    if(a.ptr->type() != t)
        return false;
    if(t.hash_code() == typeid(integer).hash_code())
        return a.as<integer>() == as<integer>();
    if(t.hash_code() == typeid(decimal).hash_code())
        return a.as<decimal>() == as<decimal>();
    if(t.hash_code() == typeid(string).hash_code())
        return a.as<string>() == as<string>();
    if(t.hash_code() == typeid(function).hash_code())
        return a.as<function>() == as<function>();
    return true;
}

inline void Any::operator=(const Any& a){
    ptr = a.ptr;
}
inline void Any::operator=(const function& a){
    *ptr = (function) a;
};
inline void Any::operator=(const string& a){
    *ptr = (string) a;
};
inline void Any::operator=(const array& a){
    *ptr = (array) a;
};
inline void Any::operator=(const map& a){
    *ptr = (map) a;
};
inline bool Any::operator==(const function& a)const{
    if(ptr->type().hash_code() == typeid(function).hash_code())
        return as<function>() == (function) a;
    return false;
};
inline bool Any::operator==(const string& a)const{
    if(ptr->type().hash_code() == typeid(string).hash_code())
        return as<string>() == (string) a;
    return false;
};
inline bool Any::operator==(const array& a)const{
    if(ptr->type().hash_code() == typeid(array).hash_code())
        return as<array>() == (array) a;
    return false;
};
inline bool Any::operator==(const map& a) const{
    if(ptr->type().hash_code() == typeid(map).hash_code())
        return as<map>() == (map) a;
    return false;
};  ///XD ITS NOT ACTUALLY CONST :333
inline Any Any::call(const string& name, array& args)const{
    function fn = nullptr;
    if(any_methods.contains(ptr->type().hash_code())){
        if(any_methods[ptr->type().hash_code()].contains(name)){
            fn = any_methods[ptr->type().hash_code()][name];
            goto end;
        }
    }
    if(any_methods.contains(typeid(void).hash_code())){
        if(any_methods[typeid(void).hash_code()].contains(name)){
            fn = any_methods[typeid(void).hash_code()][name];
            goto end;
        }
    }
end:

    if(!fn) return {};

    args.insert(args.begin(), *this);
    return fn(args);
}
template<class ...Args>
inline Any Any::callva(const string &name, Args ...a)const{
    array arr = {Any(a)...};
    return call(name, arr);
}
inline std::size_t Any::hash() const{
    if(ptr->type() == typeid(integer)){
        return  as<integer>();
    }
    if(ptr->type() == typeid(decimal)){
        return  as<decimal>();
    }
    if(ptr->type() == typeid(string)){
        std::hash<std::string> hasher;
        return  hasher(as<string>());
    }
    try {
    return callva("hash", *this).as<integer>();
    } catch (...) {
    return ptr->type().hash_code();
    }
}