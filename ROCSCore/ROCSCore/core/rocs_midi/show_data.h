#pragma once

#include "core/win32/declspec.h"

#include <map>
#include <vector>
#include <string>
#include <istream>
#include <iostream>
#include <stdexcept>

#include <memory>
#include <algorithm>

#include "exlib/map_lib.h" // get_keys
#include "exlib/binary_string_io.h" // ex::ReadString, ex::WriteString
#include "exlib/ptr_map.h"
#include "exlib/numeric_string_compare.h"
#include "exlib/string_lib.h"
#include "exlib/map_iters.h"
#include "core/common/filter_filename.h"
#include "core/common/rocs_exception.h"
#include "core/common/rocs_file_header.h"
#include "core/common/file_version.h"
#include "core/rocs_midi/song_data.h"
#include "core/rocs_midi/show_data_version.h"

namespace rocs_midi
{

class ROCS_CORE_API ShowDataError : public cmn::ROCSException
{
public:
    ShowDataError(const std::string& what): cmn::ROCSException(what) {}
};

class ROCS_CORE_API ShowDataVersionError : public ShowDataError
{
public:
    ShowDataVersionError(const std::string& what): ShowDataError(what) {}
};

class ROCS_CORE_API ShowDataNonExistError : public ShowDataError
{
public:
    ShowDataNonExistError(const std::string& what): ShowDataError(what) {}
};

typedef ex::SortedStringMapT SongNameBySongNumberT;

typedef ex::ptr_map<
    std::string,
	SongData,
	ex::NumericStringCompare> SongDataBySongNumberT;


// The map type wrapped by ex::ptr_map
ROCS_CORE_TEMPLATE_DECLSPEC(std::map <
	std::string,
	std::shared_ptr<SongData>,
	ex::NumericStringCompare,
	std::allocator < std::pair<const std::string, SongData>>>);

ROCS_CORE_TEMPLATE_DECLSPEC(ex::ptr_map<
    std::string,
    SongData,
    ex::NumericStringCompare,
	std::allocator < std::pair<const std::string, SongData>>>);

typedef ex::MapIters<SongDataBySongNumberT>::value_iterator SongDataIteratorT;

class ROCS_CORE_API ShowData
{
public:
    ShowData() {}

    ShowData(const std::string & show_name)
        :
        show_name_(show_name)
    {}

    ShowData(std::istream &);

    void WriteBinary(std::ostream &os, const cmn::FileVersion &) const;

    void WriteString(std::ostream &os) const;

    std::string GetShowName() const { return this->show_name_; }

    void SetShowName(const std::string &show_name) { this->show_name_ = show_name; }

    std::vector<std::string> GetSongNumbers() const
    {
        return ex::get_keys(this->songDataBySongNumber_);
    }
    
    SongNameBySongNumberT GetSongNamesBySongNumber() const
    {
        return this->songNameBySongNumber_;
    }

    const SongData& GetSongData(const std::string &song_num) const;

    void AddSongData(SongDataPtrT song_data_ptr);

    const SongDataBySongNumberT & GetSongs() const { return this->songDataBySongNumber_; }

    SongDataBySongNumberT& GetSongs() { return this->songDataBySongNumber_; }

    SongDataIteratorT GetSongDataBegin() const
    {
        return SongDataIteratorT(this->songDataBySongNumber_.begin());
    }

    SongDataIteratorT GetSongDataEnd() const
    {
        return SongDataIteratorT(this->songDataBySongNumber_.end());
    }

    std::string sr_file_name() const;

    std::string fh_file_name() const;

protected:
	MSC_DISABLE_WARNING(4251);
	std::string show_name_;
	MSC_RESTORE_WARNING(4251);
    SongDataBySongNumberT songDataBySongNumber_; 
    SongNameBySongNumberT songNameBySongNumber_; 
};

ROCS_CORE_API bool operator==(const ShowData& lhs, const ShowData& rhs);

typedef std::shared_ptr<ShowData> ShowDataPtrT;

}
