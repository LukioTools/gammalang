

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
#include <vector>

class Any;
class map;
class array;
class string;
typedef long integer;
typedef double decimal;
typedef Any(*function)(const std::span<Any>&);

class Any 
{
private:
    std::shared_ptr<std::any> ptr = std::make_shared<std::any>();
public:

    enum AnyE{
        NONE,
        INTEGER,
        DECIMAL,
        STRING,
        ARRAY,
        MAP,
        FUNCTION,
    };

    std::size_t hash()const{
        return 0;
    }

    template<typename T>
    T& as()const{
        try {
        return std::any_cast<T&>(*ptr);
        } catch (const std::bad_any_cast& bad_cast) {
            std::cout << "bad cast between: " << typeid(T).name() << " and " << ptr->type().name() << std::endl;
            throw std::bad_cast();
        }
    }

    bool operator==(const Any& a)const;

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

class string : public std::string
{
public:
    using std::string::basic_string;
};
class array : public std::vector<Any>
{
public:
    using std::vector<Any>::vector;
};

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