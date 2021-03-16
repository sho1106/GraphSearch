#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <memory>
#include <functional>

namespace Graph
{
    template<class T>
    using vector = std::vector<T>;

    template<class T1, class T2>
    using UnorderedMap = std::unordered_map<T1, T2>;

    template<class T>
    using UnorderedSet = std::unordered_set<T>;

    template<class T1, class T2>
    using Pair = std::pair<T1, T2>;

    using string = std::string;

    template<class T>
    using SharedPtr = std::shared_ptr<T>;

    template<class T>
    using Comp = std::function<const bool(const T&, const T&)>;

#define RUNTIME_ERROR(x) std::runtime_error(string(x) + " in " + string(__FUNCTION__))
}