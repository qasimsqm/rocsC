#include "core/common/file_version.h"
#include "core/common/rocs_file_header.h"

using namespace std;

namespace cmn
{

FileVersion get_showdata_version(istream& is)
{
    ROCSFileHeader fh;
    is.read((char *)&fh, sizeof(fh));
    
    if (fh.GetVendorId() != "ROCS")
    {
        return {0, 0};
    }

    if (fh.GetFileType() != "SDAT")
    {
        return {0, 0};
    } else
    {
        return fh.GetFileVersion();
    } 
    
    /*
    Conditions for a false positive.
    
    'R' = 82, 'O' = 79, 'C' = 67, 'S' = 83, 'D' = 68, 'A' = 65, 'T' = 84
        = 0x52    = 0x4f    = 0x43    = 0x53    = 0x44    = 0x41    = 0x54
        
    The beginning of a version 0.0 showdata file has
    
    UInt8 song_count;   --> 'R' = 0x52 = 82
    UInt32 data_size;   --> 'OCSS' = 0x4f 0x43 0x53 0x53 = 1397965647
    UInt8 song_number_size; --> 'D' = 0x44 = 68
    char song_number[song_number_size]; --> 'AT' 0x0 0x0
    
    So, the showdata file would have to contain 82 SongData objects, and the first
    SongData object would have be nearly 1.4 GB in size, not to mention the first
    SongData objects song number would have to be 68 characters long, beginning with 'AT',
    then two NULL bytes, which is impossible anyway.  I think we are safe. -- JP
    */
    
}

FileVersion get_logdata_version(istream& is)
{
    ROCSFileHeader fh;
    is.read((char *)&fh, sizeof(fh));
    
    //"CHLG", when read as a UInt32, is nearly 1.2 GB, so no false positives here either.
    if (fh.GetFileType() != "CHLG")
    {
        return {0, 0};
    } else
    {
        return fh.GetFileVersion();
    }
}


}
