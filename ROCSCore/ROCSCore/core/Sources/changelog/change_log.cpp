#include "core/changelog/change_log.h"

using namespace std;

namespace CL
{

ROCS_CORE_API
std::string GetNewLogNameFromVersion(
    const std::string &existingName, 
    const cmn::ROCSVersion &version)
{
    auto nameParts = ex::split_string(existingName, "-");
#ifdef WIN32
	// Visual Studio does not know the difference between an
	//initializer_list and a vector.
    nameParts.back() = ex::join_string(
		    std::vector<std::string>({version.GetAsString(), "logdata"}), ".");
#else
    nameParts.back() = ex::join_string({version.GetAsString(), "logdata"}, ".");
#endif

    return ex::join_string(nameParts, "-");
}

ChangeLog::ChangeLog(const ChangeLog &other)
    :
    show_name_(other.show_name_),
    customer_name_(other.customer_name_),
    license_id_(other.license_id_),
    version_(other.version_)
{
    for (auto it: other.GetSongLogs())
    {
        this->song_logs_[it.first] = SongLogPtrT(new SongLog(*it.second));
    }
}

ChangeLog & ChangeLog::operator=(const ChangeLog &other)
{
    this->show_name_ = other.show_name_;
    this->customer_name_ = other.customer_name_;
    this->license_id_ = other.license_id_;
    this->version_ = other.version_;
    
    for (auto it: other.GetSongLogs())
    {
        this->song_logs_[it.first] = SongLogPtrT(new SongLog(*it.second));
    }
    
    return *this;
}

ChangeLog::ChangeLog(std::istream &is)
{
    this->LoadIstream(is);
}

void ChangeLog::LoadIstream(std::istream &is)
{
    cmn::FileVersion fileVersion; 
    is.exceptions(istream::failbit|istream::badbit);
    if (!cmn::check_file_header(
        is,
        "ROCS",
        "CHLG",
        MinimumFileVersion,
        fileVersion))
    {
        // This input is less than the minimum supported version
        throw ChangeLogVersionError("Unsupported ShowData Version");
    }

    this->show_name_ = ex::ReadString(is);
    this->customer_name_ = ex::ReadString(is);
    this->license_id_ = ex::UUID(is);
    this->version_ = cmn::ROCSVersion(is, fileVersion);

    UInt16 log_count;
    is.read(reinterpret_cast<char *>(&log_count), sizeof(log_count));

    SongLogPtrT log;
    for (; log_count > 0; log_count--)
    {
        log = SongLogPtrT(new SongLog(is, fileVersion));
        this->song_logs_[log->GetSongNumber()] = log;
        log.reset();
    }
}

void ChangeLog::DummyLoader()
{
    this->SetShowName("All Shook Up");
    SongLogPtrT log(new SongLog());
    log->SetSongNumber("01");
    log->SetSongName("Jailhouse Rock");
    log->GetVamps().push_back(Vamp(0, 1920, 4));
    log->GetRepeats().push_back(Repeat(0, 3810, 2));
    log->GetMarkers().push_back(Marker(65748, "Fusistance is Retile!"));
    this->AddSongLog(log);
}

void ChangeLog::WriteBinary(std::ostream &os, const cmn::FileVersion &fileVersion) const
{
    cmn::ROCSFileHeader hdr(
        "ROCS",
        "CHLG",
        fileVersion.GetMajorVersion(),
        fileVersion.GetMinorVersion());
    os.write((char *)&hdr, sizeof(hdr));
    ex::WriteString(os, this->GetShowName());
    ex::WriteString(os, this->GetCustomerName());

    this->license_id_.WriteBinary(os);
    this->version_.WriteBinary(os, fileVersion);

    UInt16 log_count = song_logs_.size();
    os.write((char *)&log_count, sizeof(log_count));

    // SongLogMapT::const_iterator it;

    for (auto it: this->song_logs_)
    {
        it.second->WriteBinary(os, fileVersion);
    }
}

void ChangeLog::WriteString(ostream &os) const
{
    os  << "ChangeLog("
        << "show_name=" << this->show_name_ << ", "
        << "customer_name=" << this->customer_name_ << ", "
        << "license_id = " << this->license_id_.uuid_string() << ", "
        << "version = " << this->version_.GetAsString() << ", "
        << "song_count = " << this->song_logs_.size()
        << ")";
}

SongLog & ChangeLog::GetSongLog(const std::string &song_num)
{
    try
    {
        return *this->song_logs_.at(song_num);
    } catch (out_of_range&)
    {
        try
        {
            return *this->song_logs_.at(ex::to_lower(song_num));
        } catch (out_of_range&)
        {
            return *this->song_logs_.at(ex::to_upper(song_num));
        }
    }
}

const SongLog& ChangeLog::GetSongLog(const std::string& song_num) const
{
    return *this->song_logs_.at(song_num);
}


void ChangeLog::AddSongLog(SongLogPtrT song_log_ptr)
{
    string song_num(song_log_ptr->GetSongNumber());
    this->song_logs_.erase(song_num);
    this->song_logs_[song_num] = song_log_ptr;

}

std::string ChangeLog::GetFileName() const
{
    vector<string> name = 
    {
        cmn::filter_filename(this->GetCustomerName()),
        cmn::filter_filename(this->GetShowName()),
        this->GetLicenseId().uuid_string(),
        this->GetVersion().GetAsString()
    };
    
    if (this->GetCustomerName().size() > 0)
    {
        return ex::join_string(name, "-") + ".logdata";
    } else {
        return ex::join_string(name.begin() + 1, name.end(), "-") + ".logdata";
    }
}

bool operator==(const ChangeLog &lhs, const ChangeLog &rhs)
{
    if (lhs.GetShowName() != rhs.GetShowName()) return false;
    if (lhs.GetCustomerName() != rhs.GetCustomerName()) return false;
    if (lhs.GetLicenseId() != rhs.GetLicenseId()) return false;
    if (lhs.GetVersion() != rhs.GetVersion()) return false;
    return equal(
        lhs.song_logs_begin(),
        lhs.song_logs_end(),
        rhs.song_logs_begin(),
        [] (const SongLog& x, const SongLog& y)->bool
        {
            return x == y;
        });
}

} // end namespace CL
