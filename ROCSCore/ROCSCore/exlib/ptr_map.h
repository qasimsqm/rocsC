#pragma once

#include "exlib/win32/declspec.h"

#include <memory>
#include <map>
#include <utility>
#include <iostream>

namespace ex {

// #define EXLIB_PTR_MAP_CONTAINER_T(Key, T, ...) std::map<Key, T, __VA_ARGS__>

template<
	class Key,
	class T,
	class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<const Key,std::shared_ptr<T>>>>
class ptr_map
{
public:
    typedef std::shared_ptr<T>                      PtrT;
    typedef std::map<Key, PtrT, Compare, Allocator> MapT;
    typedef std::pair<Key, PtrT>                    value_type;
    typedef T&                                      reference;
    typedef const T&                                const_reference;
    typedef typename MapT::difference_type          difference_type;
    typedef typename MapT::size_type                size_type;
    typedef typename MapT::allocator_type           allocator_type;
    typedef typename MapT::key_type                 key_type;
    typedef typename MapT::key_compare              key_compare;
    typedef typename MapT::value_compare            value_compare;
    typedef PtrT                                    mapped_type;
    typedef T&                                      mapped_reference;
    typedef const T&                                const_mapped_reference;
    typedef typename MapT::iterator                 iterator;
    typedef typename MapT::const_iterator           const_iterator;
    typedef typename MapT::reverse_iterator         reverse_iterator;
    typedef typename MapT::const_reverse_iterator   const_reverse_iterator;


public: // constructors
    ptr_map() {}

    ptr_map& operator=(const ptr_map& other) { map_ = other.map_; return *this; }

public: // observers
    key_compare     key_comp() const    { return map_.key_comp(); }
    value_compare   value_comp() const  { return map_.value_comp(); }

public: // capacity
    size_type   size() const            { return map_.size(); }
    size_type   max_size() const        { return map_.max_size(); }
    bool        empty() const           { return map_.empty(); }

public: // modifiers
    void                        clear() { map_.clear(); }
    std::pair<iterator, bool>   insert(const value_type& x) { return map_.insert(x); }

    std::pair<iterator, bool>   insert(const key_type& k, T* x)
                                            { return map_.insert(std::make_pair(k, PtrT(x))); }
    std::pair<iterator, bool>   insert(const key_type& k, PtrT x)
                                            { return map_.insert(std::make_pair(k , x)); }
    template< typename InputIterator >
    void        insert(InputIterator first, InputIterator last) { map_.insert(first, last); }

    void        erase(iterator position) { map_.erase(position); }
    size_type   erase(const key_type& x) { return map_.erase(x); }
    void        erase(iterator first, iterator last) { map_.erase(first, last); }

public: // lookup
    T&       operator[](const key_type& key) {
                        create_new_entry(key);
                        return *map_[key];
                    }


    T&       at(const key_type& key) {
                        return *map_.at(key);
                    }

    const T& at(const key_type& key) const   {  return *map_.at(key); }

public: // ptr access
    PtrT pat(const key_type& key) { return map_.at(key); }
    PtrT pat(const key_type& key) const { return map_.at(key); }

public: // algorithms
    iterator        find(const key_type& key) { return map_.find(key); }
    const_iterator  find(const key_type& key) const { return map_.find(key); }
    size_type       count(const key_type& key) const { return map_.count(key); }

public: // iterators
    iterator                begin()         { return map_.begin(); }
    const_iterator          begin() const   { return map_.begin(); }
    iterator                end()           { return map_.end(); }
    const_iterator          end() const     { return map_.end(); }
    reverse_iterator        rbegin()        { return map_.rbegin(); }
    const_reverse_iterator  rbegin() const  { return map_.rbegin(); }
    reverse_iterator        rend()          { return map_.rend(); }
    const_reverse_iterator  rend() const    { return map_.rend(); }

private:
	MSC_DISABLE_WARNING(4251);
    MapT map_;
	MSC_RESTORE_WARNING(4251);

    void create_new_entry(const key_type& key)
    {
        if (!map_.count(key)) {
            PtrT _ptr(new T);
            map_.insert(std::make_pair(key, _ptr));
        }
    }

};

} // namespace ex
