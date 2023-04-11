#pragma once

/** ptr_vector, for convenience, provides a reference to the stored object when you
    dereference the iterator.  For this reason, it is not ideal as a container of
    virtual base classes.  If you need to use any std algorithms, like find_if, the
    iterator is dereferenced and it becomes impossible to access the derived
    implementation class.  However, find_if problems aside, you can still access
    virtual methods of the derived class by using it->virtual_method_name().
    To fully preserve polymorphic behavior of virtual classes, use
    std::vector< std::shared_ptr<T> >

    This ptr_vector differs from boost::ptr_vector in that each stored object is a
    shared_ptr.
    **/

#include "exlib/win32/declspec.h"

#include <memory>
#include <vector>
#include <iostream>

namespace ex
{

#define EXLIB_PTR_VECTOR_CONTAINER_T(x) std::vector<std::shared_ptr<x>>

template< class T >
class ptr_vector
{
public: // typdefs
    typedef std::shared_ptr<T>                      PtrT;
    typedef typename std::vector<PtrT>              VecT;
    typedef PtrT                                    value_type;
    typedef T&                                      reference;
    typedef const T&                                const_reference;
    typedef typename VecT::difference_type          difference_type;
    typedef typename VecT::size_type                size_type;
    typedef typename VecT::allocator_type           allocator_type;
    typedef typename VecT::iterator                 std_iterator;
    typedef typename VecT::const_iterator           std_const_iterator;
    typedef typename VecT::reverse_iterator         std_reverse_iterator;
    typedef typename VecT::const_reverse_iterator   std_const_reverse_iterator;
    template< class U >
    class PtrVecIterator : public U
    {
    public:
        PtrVecIterator(const U& i): U(i) {}
        T& operator*()              { return *U::operator*(); }
        PtrT operator->()           { return *U::operator->(); }
        const T& operator*() const  { return *U::operator*(); }
    };

    typedef PtrVecIterator<typename VecT::iterator> iterator;
    typedef PtrVecIterator<typename VecT::const_iterator> const_iterator;
    typedef PtrVecIterator<typename VecT::reverse_iterator> reverse_iterator;
    typedef PtrVecIterator<typename VecT::const_reverse_iterator> const_reverse_iterator;


public: // construction
    ptr_vector() {}
    explicit ptr_vector(size_type to_reserve) { vec.reserve(to_reserve); }
    ~ptr_vector() {}
    template< class InputIterator >
    void assign( InputIterator first, InputIterator last ) { vec.assign(first, last); }

public: // capacity
    size_type   capacity() const        { return vec.capacity(); }
    size_type   size() const            { return vec.size(); }
    void        reserve(size_type n)    { vec.reserve(n); }

public: // element access
    T&          front()                         { return *vec.front(); }
    const T&    front() const                   { return *vec.front(); }
    T&          back()                          { return *vec.back(); }
    const T&    back() const                    { return *vec.back(); }
    T&          operator[](size_type n)         { return *vec[n]; }
    const T&    operator[](size_type n) const   { return *vec[n]; }
    T&          at(size_type n)                 { return *vec.at(n); }
    const T&    at(size_type n) const           { return *vec.at(n); }

public: // ptr element access
    PtrT        pfront()                        { return vec.front(); }
    PtrT        pback()                         { return vec.back(); }
    PtrT        pat(size_type n)                { return vec.at(n); }

public: // iterators
    iterator                begin()         { return vec.begin(); }
    const_iterator          begin() const   { return vec.begin(); }
    iterator                end()           { return vec.end(); }
    const_iterator          end() const     { return vec.end(); }
    reverse_iterator        rbegin()        { return vec.rbegin(); }
    const_reverse_iterator  rbegin() const  { return vec.rbegin(); }
    reverse_iterator        rend()          { return vec.rend(); }
    const_reverse_iterator  rend() const    { return vec.rend(); }

public: // std_iterators
    std_iterator                std_begin()         { return vec.begin(); }
    std_const_iterator          std_begin() const   { return vec.begin(); }
    std_iterator                std_end()           { return vec.end(); }
    std_const_iterator          std_end() const     { return vec.end(); }
    std_reverse_iterator        std_rbegin()        { return vec.rbegin(); }
    std_const_reverse_iterator  std_rbegin() const  { return vec.rbegin(); }
    std_reverse_iterator        std_rend()          { return vec.rend(); }
    std_const_reverse_iterator  std_rend() const    { return vec.rend(); }

public: // modifiers
    void push_back(T* x)                { vec.push_back(PtrT(x)); }
    void push_back(PtrT x)              { vec.push_back(x); }
    void pop_back()                     { vec.pop_back(); }
    void resize(size_type n)            { vec.resize(n); }

    void erase(iterator first, iterator last)   { vec.erase(first, last); }
    void erase(iterator position)               { vec.erase(position); }

public:  // algorithms
    // void sort()                                 { vec.sort(); }
    // void sort(iterator first, iterator last)    { vec.sort(first, last); }
    // template< class Compare >
    // void sort(Compare comp)                     { vec.sort(comp); }
    // template< class Compare >
    // void sort(iterator begin, iterator end, Compare comp) { vec.sort(begin, end, comp); }

private:
	MSC_DISABLE_WARNING(4251);
    VecT vec;
	MSC_RESTORE_WARNING(4251);
};

}
