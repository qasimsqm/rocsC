#pragma once

#include "core/win32/declspec.h"

#include <istream>
#include <ostream>
#include <string>
#include <map>
#include <utility>
#include <mutex>
#include <memory>

#include <algorithm>
#include "exlib/binary_string_io.h"
#include "exlib/xplatform_types.h"
#include "exlib/uuid.h"
#include "exlib/numeric_string_compare.h"
#include "exlib/string_lib.h"
#include "exlib/map_iters.h"
#include "core/common/rocs_file_header.h"
#include "core/common/rocs_version.h"
#include "core/common/filter_filename.h"
#include "core/changelog/change_log_version.h"
#include "core/changelog/change_log_exception.h"
#include "core/changelog/song_log.h"


namespace CL {

class ROCS_CORE_API ChangeLogVersionError : public ChangeLogException
{
public:
    ChangeLogVersionError(const std::string& what): ChangeLogException(what) {}
};

class ROCS_CORE_API ChangeLogNonExistError : public ChangeLogException
{
public:
    ChangeLogNonExistError(const std::string& what): ChangeLogException(what) {}
};

typedef std::map<std::string, SongLogPtrT, ex::NumericStringCompare> SongLogMapT;
ROCS_CORE_TEMPLATE_DECLSPEC(std::map<std::string, SongLogPtrT, ex::NumericStringCompare>);
typedef ex::MapIters<SongLogMapT>::key_iterator SongNumIterator;
typedef ex::MapIters<SongLogMapT>::value_iterator SongLogIterator;

ROCS_CORE_API 
std::string GetNewLogNameFromVersion(
    const std::string &existingName, 
    const cmn::ROCSVersion &version);

class ROCS_CORE_API ChangeLog
{
public:
    ChangeLog() {}

    ChangeLog(
        const std::string& show_name,
        const std::string& customer_name,
        const ex::UUID& license_id = ex::UUID(),
        const cmn::ROCSVersion& version = cmn::ROCSVersion())
        :
        show_name_(show_name),
        customer_name_(customer_name),
        license_id_(license_id),
        version_(version) 
    {
    
    }

    ChangeLog(std::istream &);

    ChangeLog(const ChangeLog& other);

    ChangeLog& operator=(const ChangeLog& other);

    ~ChangeLog() {}

    void LoadIstream(std::istream &);

    void DummyLoader();

    void WriteBinary(std::ostream &, const cmn::FileVersion &) const;

    void WriteString(std::ostream&) const;

    std::string GetShowName() const { return this->show_name_; }

    void SetShowName(const std::string& show_name) { this->show_name_ = show_name; }

    std::string GetCustomerName() const { return this->customer_name_; }

    void SetCustomerName(const std::string& customer_name)
    {
        this->customer_name_ = customer_name;
    }

    ex::UUID& GetLicenseId() { return this->license_id_; }

    const ex::UUID& GetLicenseId() const { return this->license_id_; }

    void SetLicenseId(const ex::UUID &license_id) { this->license_id_ = license_id; }

    cmn::ROCSVersion& GetVersion() { return this->version_; }

    const cmn::ROCSVersion& GetVersion() const { return this->version_; }

    void SetVersion(const cmn::ROCSVersion &version) { this->version_ = version; }

    SongLog & GetSongLog(const std::string &song_number);

    const SongLog & GetSongLog(const std::string& song_number) const;

    void AddSongLog(SongLogPtrT song_log_ptr);

    std::string GetFileName() const;

    const SongLogMapT& GetSongLogs() const { return this->song_logs_; }

    // Iterate over just the song_numbers.
    // SongNumIterator dereferences to std::string const &
    SongNumIterator song_nums_begin() const { return SongNumIterator(song_logs_.begin());}

    SongNumIterator song_nums_end() const { return SongNumIterator(song_logs_.end()); }

    /** Iterator over just the tracks.
        The SongLogIterator dereferences to a SongLog const& **/
    SongLogIterator song_logs_begin() const { return SongLogIterator(song_logs_.begin());}

    SongLogIterator song_logs_end() const { return SongLogIterator(song_logs_.end()); }
    
private:
	std::string show_name_;
    std::string customer_name_;
    ex::UUID license_id_;
    cmn::ROCSVersion version_;
    SongLogMapT song_logs_;
};

ROCS_CORE_API bool operator==(const ChangeLog& lhs, const ChangeLog& rhs);

typedef std::shared_ptr<ChangeLog> ChangeLogPtrT;

}
