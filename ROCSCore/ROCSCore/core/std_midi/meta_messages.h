#pragma once

#include "core/win32/declspec.h"

#include <string>
#include <istream>
#include <ostream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>

#include <memory>

#include <stdexcept>
#include <sstream>
#include <cassert>
#include <cmath>
#include <cstring>

#include "exlib/xplatform_types.h"
#include "exlib/format.h"
#include "core/common/key_sigs.h"
#include "core/common/rocs_exception.h"
#include "core/common/file_version.h"
#include "core/std_midi/status_bytes.h"

namespace std_midi {

class ROCS_CORE_API MetaMessage;

typedef std::shared_ptr<MetaMessage> MetaMsgPtrT;
typedef std::vector<MetaMsgPtrT> MetaMsgPtrVecT;
ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<MetaMsgPtrT>);

// http://msdn.microsoft.com/en-us/library/3tdb471s.aspx
// C4275 can be ignored in Visual C++ if you are deriving from a type in the Standard C++ Library
MSC_DISABLE_WARNING(4275);
class ROCS_CORE_API MetaMessageError : public std::runtime_error
{
public:
    MetaMessageError(const std::string& what): std::runtime_error(what) {}
};
MSC_RESTORE_WARNING(4275);

/** Factory Functions
    return values of MetaMsgPtrT are shared_ptr's. **/
ROCS_CORE_API MetaMsgPtrT read_meta_message(std::istream &); // throw(MetaMessageError);
ROCS_CORE_API MetaMsgPtrT create_meta_message(const Byte status); // throw(MetaMessageError);

ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<Byte>);

class ROCS_CORE_API MetaMessage
{
public:
    MetaMessage(Byte status): status_(status), length_(0) {}
    MetaMessage(Byte status, UInt8 size);
    MetaMessage(std::istream &);
    virtual ~MetaMessage() {}

    Byte status() const                         { return status_; }
    Byte length() const                         { return length_; }
    const std::vector<Byte>& bytes() const      { return bytes_; }

    // Virtual Accessors
    virtual std::string message() const         { return std::string(); }
    virtual UInt32 microseconds() const         { return 0; }
    virtual UInt8 numerator() const             { return 0; }
    virtual UInt8 denominator() const           { return 0; }
    virtual UInt8 clocksPerClick() const        { return 0; }
    virtual UInt8 notated32nds() const          { return 0; }
    virtual SInt8 sharps() const                { return 0; }
    virtual UInt8 mode() const                  { return 0; }

    virtual void WriteString(std::ostream& o) const;
    virtual void WriteBinary(std::ostream &, const cmn::FileVersion &) const;

    bool operator==(MetaMessage& rhs)
	{
		return ((status_ == rhs.status_)
	        && (length_ == rhs.length_)
			&& (bytes_ == rhs.bytes_));
    }
    
	bool operator!=(MetaMessage& rhs) { return (!(*this==rhs)); }

protected:
    Byte status_;
    Byte length_;
    std::vector<Byte> bytes_;
};


class ROCS_CORE_API StringMetaMessage : public MetaMessage {
public:
    StringMetaMessage(const Byte status) : MetaMessage(status) {}
    StringMetaMessage(std::istream &s) : MetaMessage(s) {}

    std::string message() const { return std::string(bytes_.begin(), bytes_.end()); }
    void message(const std::string& s);
    virtual void WriteString(std::ostream& o) const;
};


class ROCS_CORE_API TrackName : public StringMetaMessage {
public:
    TrackName(): StringMetaMessage(sb::track_name) {}
    TrackName(std::istream &s): StringMetaMessage(s) {}
};


class ROCS_CORE_API Marker : public StringMetaMessage {
public:
    Marker(): StringMetaMessage(sb::marker) {}
    Marker(std::istream &s): StringMetaMessage(s) {}
};


class ROCS_CORE_API Tempo : public MetaMessage {
public:
    Tempo(): MetaMessage(sb::tempo, 3) {}
    Tempo(std::istream &s): MetaMessage(s) {}
    UInt32 microseconds() const;
    void microseconds(UInt32 tmpo);
    void WriteString(std::ostream& o) const;
};


class ROCS_CORE_API TimeSignature : public MetaMessage {
public:
    TimeSignature(): MetaMessage(sb::time_signature, 4) {}
    TimeSignature(std::istream &s): MetaMessage(s) {}

    UInt8 numerator() const { return bytes_[0]; }
    void numerator(UInt8 val) { bytes_[0] = val; }

    UInt8 denominator() const { return bytes_[1]; }
    void denominator(UInt8 val) { bytes_[1] = val; }

    // clocks per metronome click
    UInt8 clocksPerClick() const { return bytes_[2]; }
    void clocksPerClick(UInt8 val) { bytes_[2] = val; }

    // Notated 32nd notes per quarter note
    UInt8 notated32nds() const { return bytes_[3]; }
    void notated32nds(UInt8 val) { bytes_[3] = val; }
    void WriteString(std::ostream& o) const;
};

class ROCS_CORE_API KeySignature : public MetaMessage {
public:
    KeySignature(): MetaMessage(sb::key_signature, 2) {}
    KeySignature(std::istream &s): MetaMessage(s) {}

    SInt8 sharps() const { return bytes_[0]; }
    void sharps(SInt8 val) { bytes_[0] = val; }

    UInt8 mode() const { return bytes_[1]; }
    void mode(UInt8 val) { bytes_[1] = val; }
    void WriteString(std::ostream& o) const;
};

typedef std::shared_ptr<TrackName> TrackNamePtrT;
typedef std::shared_ptr<Marker> MarkerPtrT;
typedef std::shared_ptr<Tempo> TempoPtrT;
typedef std::shared_ptr<TimeSignature> TimeSignaturePtrT;
typedef std::shared_ptr<KeySignature> KeySignaturePtrT;

}
