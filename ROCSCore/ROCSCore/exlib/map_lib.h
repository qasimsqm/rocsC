#pragma once

#include "exlib/win32/declspec.h"

#include <map>
#include <vector>
#include "exlib/ptr_map.h"

namespace ex {

template <class Key, class T, class Compare, class Allocator>
std::vector<Key> get_keys(const std::map<Key, T, Compare, Allocator>& m)
{
    std::vector<Key> keys;
    typename std::map<Key, T, Compare, Allocator>::const_iterator it;
    for (auto it: m)
        keys.push_back(it.first);
    return keys;
}

template <class Key, class T, class Compare, class Allocator>
std::vector<T> get_values(const std::map<Key, T, Compare, Allocator>& m)
{
    std::vector<T> values;
    typename std::map<Key, T, Compare, Allocator>::const_interator it;
    for (auto it: m)
        values.push_back(it.second);
    return values;
}

template <class Key, class T, class Compare, class Allocator>
std::vector<Key> get_keys(const ptr_map<Key, T, Compare, Allocator>& m)
{
    std::vector<Key> keys;
    typename ptr_map<Key, T, Compare, Allocator>::const_iterator it;
    for (auto it: m)
        keys.push_back(it.first);
    return keys;
}

template <class Key, class T, class Compare, class Allocator>
std::vector<T> get_values(const ptr_map<Key, T, Compare, Allocator>& m)
{
    std::vector<T> values;
    typename ptr_map<Key, T, Compare, Allocator>::const_interator it;
    for (auto it: m)
        values.push_back(it.second);

    return values;
}

}
