#pragma once
#include <any>
#include <memory>
#include <vector>
class Any;

typedef Any(*Method)(std::vector<Any>& args);
