#pragma once


#include "func.hpp"
#include <any>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <variant>
#include <vector>


struct any;
class map;



typedef std::vector<std::shared_ptr<any>> array;
class string : public std::string
{
public:
    using std::string::basic_string;
};
typedef long integer;
typedef double decimal;

static inline std::ostream& operator<<(std::ostream& os, const array& arr);
static inline std::ostream& operator<<(std::ostream& os, const map& m);
static inline std::ostream& operator<<(std::ostream& os, const string& m);

template <>
struct std::hash<::string> {
    std::size_t operator()(const ::string& obj) const {
        std::hash<std::string> h;
        return h(obj);
    }
};


struct any ///object container, cannot be copied unless especially told so
{
public:
    enum any_e : uint_fast8_t{
        UNKNOWN,
        INTEGER,
        DECIMAL,
        STRING,
        ARRAY,
        MAP,
        FUNCTION,
    } _type = UNKNOWN;
    struct any_t //add custom shit :3
    {
        const std::type_info& type = typeid(void);
        void(*constructor)(any*, void* data) = nullptr;
        void(*destructor)(any*) = nullptr;
    };
    static std::vector<any_t> any_map;
    void* _data = nullptr;

protected:
    void overrite(any_e new_type){
        if(_data){
            if(new_type == _type)
                return;
            else
                metadata().destructor(this);
        }
        any_map[new_type].constructor(this, nullptr);
    }

public:                 //any_e
    any_e get_e()const{
        return _type;
    }
    std::size_t hash() const{
        if(_type == INTEGER)
            return asr<long>();
        if(_type == DECIMAL)
            return *reinterpret_cast<long*>(_data);
        if(_type == STRING){
            std::hash<string> hasher;
            return hasher(asr<string>());
        }
        return std::variant_npos - _type;
    }
    template<typename T>
    T* asp() const{
        if(typeid(T) != any_map[_type].type)
            throw std::bad_cast();
        return reinterpret_cast<T*>(_data);
    }
    template<typename T>
    T& asr() const{
        if(typeid(T) != any_map[_type].type)
            throw std::bad_cast();
        return *reinterpret_cast<T*>(_data);
    }
    void* data()const{
        return _data;
    }

    any_t& metadata(){
        return any_map[_type];
    }
    template<typename T, typename std::enable_if_t<std::is_integral_v<T>>>
    void operator=(T l){
        overrite(INTEGER);
        *reinterpret_cast<long*>(_data) = l;
    }
    template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>>>
    void operator=(T l){
        overrite(DECIMAL);
        asr<double>() = l;
    }
    void operator=(const string& l){
        overrite(STRING);
        asr<string>() = l;
    }
    void operator=(const array& l){
        overrite(ARRAY);
        asr<array>() = l;
    }
    void operator=(const map& l){
        overrite(MAP);
        any_map[MAP].constructor(this, (void*) &l);
    }

    bool operator<(const any& a) const{
        return hash() < a.hash();
    }

    bool operator==(const any& a)const{
        if(a._type != _type)
            return false;
        switch (_type) {
            case INTEGER:
                return a.asr<long>() == asr<long>();
            case DECIMAL:
                return a.asr<double>() == asr<double>();
            case STRING:
                return a.asr<string>() == asr<string>();
            case FUNCTION:
                return a.asr<function>() == asr<function>();
            case ARRAY:
            case MAP:
            default:
                return true;
        }
    }
    any copy() const{
        switch (_type) {
            case INTEGER:
                return any(asr<long>());
            case DECIMAL:
                return any(asr<double>());
            case STRING:
                return any(asr<string>());
            case ARRAY:
                return any(asr<array>());
            case MAP:
                return any(asr<map>());
            case FUNCTION:
                return any(asr<function>());
            default:
                return any();
        }
    }
    void swap(any& a){
        auto t = _type;
        _type = a._type;
        a._type = t;
        auto d = _data;
        _data = a._data;
        a._data = d;
    }

    void clear(){
        if(_data)
            any_map[_type].destructor(this);
    }

    bool has_value()const{
        return _data && _type;
    }

    const std::type_info& type() const{
        if(!_data)
            return typeid(void);
        return any_map[_type].type;
    }
    any(){};
    template<typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = 0>
    any(T i) {_data = new long(i); _type = INTEGER;}
    template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>, bool> = 0>
    any(T f) {_data = new double(f); _type = DECIMAL;}
    any(const char* str){_data = new std::string(str); _type = STRING;}
    any(const string& str){_data = new std::string(str); _type = STRING;}
    any(const array& arr){_data = new array(arr); _type = ARRAY;}
    any(const std::initializer_list<std::shared_ptr<any>>& arr){_data = new array(arr); _type = ARRAY;}
    any(const map& m){any_map[MAP].constructor(this, (void*) &m);}
    any(const function& f){_data = new function(f); _type = FUNCTION;}
        //creates a copy
    any(const any& a){auto an = a.copy(); _data =  an._data; _type = an._type;}
        //moves
    any(any&& a){_data =  a._data; _type = a._type; a._data = nullptr; a._type = UNKNOWN;}

    ~any(){
        if(!_data)
            return;
        //any_map[_type].destructor(this);
    }
};

// Specialize std::hash template for MyVariant
template <>
struct std::hash<::any> {
    std::size_t operator()(const ::any& obj) const {
        return obj.hash();
    }
};
// Specialize std::hash template for MyVariant
template <>
struct std::hash<std::shared_ptr<::any>> {
    std::size_t operator()(const ::std::shared_ptr<::any>& obj) const {
        if(obj)
            return obj->hash();
        return variant_npos;
    }
};

class map : public std::unordered_map<std::shared_ptr<any>, std::shared_ptr<any>>
{
public:
    using std::unordered_map<std::shared_ptr<any>, std::shared_ptr<any>>::unordered_map;
};

#define any_map_template(tname, tid) {typeid(tname), [](any* p, void* vp){p->_data = vp ? new tname(*reinterpret_cast<tname*>(vp)) : new tname; p->_type = tid;},[](any* p){delete reinterpret_cast<tname*>(p->_data); p->_data = nullptr; p->_type = UNKNOWN;}}
inline std::vector<any::any_t> any::any_map = {
    {
        {
            typeid(void),
            [](any* p, void*){},
            [](any* p){free(p->_data); p->_data = nullptr; p->_type = UNKNOWN;}
        },
        any_map_template(long, INTEGER),
        any_map_template(double, DECIMAL),
        any_map_template(string, STRING),
        any_map_template(array, ARRAY),
        any_map_template(map, MAP),
        any_map_template(function, FUNCTION),
    }
};

    //this object always contains a handle
class Any
{
private:
    std::shared_ptr<any> handle = std::make_shared<any>();
public:
    Any ref() const{
        return Any{handle};
    }
    Any copy() const{
        return std::make_shared<any>(handle->copy());
    }
    template<typename T>
    T& as() const{
        return handle->asr<T>();
    }
    any::any_t& metadata() const{
        return handle->metadata();
    }
    any::any_e e_type() const{
        return handle->_type;
    }
    void clear(){
        handle = std::make_shared<any>();
    }
    bool has_value(){
        return handle->has_value();
    }

    template<typename T>
    T* as_ptr() const{
        return handle->asp<T>();
    }
    template<typename T, typename std::enable_if_t<!std::is_same_v<T, Any>, bool> = nullptr>
    T& operator=(const T& o){
        *handle = o;
        return handle->asr<T>();
    }
    Any& operator=(Any&& mv){
        handle = mv.handle;
        mv.clear();
        return *this;
    }

    operator std::shared_ptr<any>() const{
        return handle;
    }

    friend std::ostream& operator<<(std::ostream& os, const Any& a){
        switch (a.e_type()) {
        case any::INTEGER:
            os << a.as<integer>();
            break;
        case any::DECIMAL:
            os << a.as<decimal>();
            break;
        case any::STRING:
            os << a.as<string>();
            break;
        case any::FUNCTION:
            os << "function: " << (void*) a.as<function>();
            break;
        case any::ARRAY:
            os << a.as<array>();
            break;
        case any::MAP:
            os << a.as<map>();
            break;
        case any::UNKNOWN:
        default:
            break;
        }
        return os;
    }

    Any() {}
    Any(const Any& p) :handle(p.handle) {}
    Any(Any&& p) :handle(p.handle) {p.handle = nullptr;}
    Any(std::shared_ptr<any> a) : handle(a){}
    template<typename T>
    Any(T d) : handle(std::make_shared<any>(d)) {}
    ~Any() {}
};




static inline std::ostream& operator<<(std::ostream& os, const array& arr){
    os << '['; 
    for (size_t i = 0; i < arr.size(); i++)
    {
        os << Any(arr[i]);
        if(i != arr.size()-1) os<<',';
    }
    os << ' ' << ']';
    return os;
};
static inline std::ostream& operator<<(std::ostream& os, const map& m){
    os << '{' << '\n';
    for (auto& e : m) {
        os << Any(e.first) << ":" << Any(e.second) << '\n';
    }
    os << '}';
    return os;
};
static inline std::ostream& operator<<(std::ostream& os, const string& m){
    os << '"' << std::string(m) << '"';
    return os;
};
