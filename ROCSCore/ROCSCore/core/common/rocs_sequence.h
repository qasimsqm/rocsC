#pragma once

/** ROCSSequence is the virtual base class for all meta tracks of ROCSEvents.  Every event
    in a sequence will have the same type.  The next class down in the hierarchy is
    the templated class SequenceTemplate.  All meta tracks of ROCSEvents will be
    subclassed from this template.  By using both an abstract base class and a
    templated base class in succession, it is hoped that the best of both static and
    dynamic polymorphism can be combined. **/

#include "core/win32/declspec.h"

#include <istream>
#include <ostream>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include <iostream> // for debug printing
#include <cassert>

#include <memory>

#include "exlib/xplatform_types.h"
#include "core/common/codes.h"
#include "core/common/file_version.h"

#define ROCS_SEQUENCE_TEMPLATE(x) ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<x>)

namespace cmn {

class ROCS_CORE_API ROCSSequence;

typedef std::shared_ptr<ROCSSequence> ROCSSeqPtrT;
typedef std::vector<ROCSSeqPtrT> ROCSSeqPtrVecT;
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<ROCSSequence>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<ROCSSeqPtrT>);

class ROCS_CORE_API ROCSSequence
{
public:
    virtual UInt8 code() const = 0;
    virtual void WriteBinary(std::ostream &, const cmn::FileVersion &) const = 0;
    virtual void WriteString(std::ostream&) const = 0;
    virtual void WriteStringEvents(std::ostream&, int indent) const = 0;
};


template< class ROCSEventT >
class SequenceTemplate : public ROCSSequence
{
public: // typedefs
    typedef typename std::vector<ROCSEventT> VectorT;
    typedef typename VectorT::iterator iterator;
    typedef typename VectorT::const_iterator const_iterator;
    typedef typename VectorT::reverse_iterator reverse_iterator;
    typedef typename VectorT::const_reverse_iterator const_reverse_iterator;
    typedef typename VectorT::size_type size_type;
    typedef typename VectorT::value_type value_type;

public: // Construction
    SequenceTemplate() {}

    SequenceTemplate(std::istream &, const cmn::FileVersion &);

    explicit SequenceTemplate(size_type to_reserve) { events_.reserve(to_reserve); }

    template<class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
        this->events_.assign(first, last);
    }

    template<class InputRange>
    void assign(const InputRange& ir) { this->events_.assign(ir); }

    virtual ~SequenceTemplate() {}

public: // Additional functions beyond std::vector
    virtual UInt8 code() const { return ROCSEventT::evt_code; }

    virtual void WriteString(std::ostream&) const;

    virtual void WriteStringEvents(std::ostream&, int indent=0) const;

    size_type size() const { return events_.size(); }

public: // element access
    ROCSEventT& front() { return events_.front(); }

    const ROCSEventT& front() const { return events_.front(); }

    ROCSEventT& back() { return events_.back(); }

    const ROCSEventT& back() const { return events_.back(); }

    ROCSEventT& operator[](size_type n) { return events_[n]; }

    const ROCSEventT& operator[](size_type n) const { return events_[n]; }

    ROCSEventT& at(size_type n) { return events_.at(n); }

    const ROCSEventT& at(size_type n) const { return events_.at(n); }

public: // modifers
    void push_back(const ROCSEventT& evt) { events_.push_back(evt); }

    void pop_back() { events_.pop_back(); }

    void clear() { events_.clear(); }
    
    template <class... Args>
    iterator emplace(const_iterator position, Args&&... args)
    {
        return this->events_.emplace(position, std::forward<Args>(args)...);
    }

    iterator erase(const_iterator position) { return this->events_.erase(position); }

    iterator erase(const_iterator first, const_iterator last)
    {
        return this->events_.erase(first, last);
    }

    void reserve(size_type size) { this->events_.reserve(size); }

public:    // iterators
    iterator begin() { return events_.begin(); }

    const_iterator begin() const { return events_.begin(); }

    iterator end() { return events_.end(); }

    const_iterator end() const { return events_.end(); }

    reverse_iterator rbegin() { return events_.rbegin(); }

    const_reverse_iterator rbegin() const { return events_.rbegin(); }

    reverse_iterator rend() { return events_.rend(); }

    const_reverse_iterator rend() const { return events_.rend(); }
    
public:     
    // Read-only direct access
    const VectorT& events() const { return events_; } 
    
protected:
    VectorT events_;
};


template <class ROCSEventT>
SequenceTemplate<ROCSEventT>::SequenceTemplate(
    std::istream &is,
    const cmn::FileVersion &fileVersion)
{
    UInt8 event_type = is.get();
    if (event_type != this->code())
    {
        throw std::logic_error("event_type does not match container type.");
    }

    UInt32 obj_count;
    is.read((char *)&obj_count, sizeof(UInt32));
    
    if (!obj_count)
    {
        return;
    }

    events_.reserve(obj_count);
    while(obj_count-- > 0)
    {
        events_.push_back(ROCSEventT(is, fileVersion));
    }
}


template <class ROCSEventT>
void SequenceTemplate<ROCSEventT>::WriteString(std::ostream &os) const
{
    os  << codes::code_names[this->code()] << "Sequence("
        << "code=" << std::hex << std::showbase
        << static_cast<UInt16>(this->code()) << ", "
        << std::dec << std::noshowbase
        << "event count = " << this->events_.size()
        << ")";
}

template< class ROCSEventT >
void SequenceTemplate<ROCSEventT>::WriteStringEvents(std::ostream& os, int indent) const
{
    os << std::string(indent, '\t');
    std::string indentString(indent + 1, '\t');
    this->WriteString(os);
    os << std::endl;
    const_iterator it;
    for (auto it: this->events_) {
        os << indentString;
        it.WriteString(os);
        os << std::endl;
    }
}

template< class T >
bool operator==(const cmn::SequenceTemplate<T> &lhs, const cmn::SequenceTemplate<T> &rhs)
{
    return equal(
        lhs.begin(),
        lhs.end(),
        rhs.begin(),
        [] (const T& e1, const T& e2)
        {
            return e1 == e2;
        });
}

template< class T >
bool operator!=(const cmn::SequenceTemplate<T> &lhs, const cmn::SequenceTemplate<T> &rhs)
{
    return !(lhs == rhs);
}

}
