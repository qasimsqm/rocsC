#include "exlib/id3.h"

using namespace std;

#define ID3_UNSYNCHRONIZATION 0x80
#define ID3_EXTENDED_HEADER 0x40
#define ID3_EXPERIMENTAL 0x20

namespace ex {

string ID3Header::id_str() const
{
    // cout << "ID3Header::id_str()" << endl;
    // cout << " " << id[0] << " " << id[1] << " " << id[2] << endl;
    return string(&id[0], 3);
}

UInt32 ID3Header::get_size() const
{
    UInt32 tag_size = 0;
    for (int i=0; i < 4; i++) {
        tag_size |= size[i] << (3-i) * 7;
    }
    return tag_size;
}

bool ID3Header::unsynchronisation() const
{
    return (flags & ID3_UNSYNCHRONIZATION) != 0;
}

bool ID3Header::extended_header() const
{
    return (flags & ID3_EXTENDED_HEADER) != 0;
}

bool ID3Header::experimental() const
{
    return (flags & ID3_EXPERIMENTAL) != 0;
}

EXLIB_API ExtendedHeader::ExtendedHeader(istream& is)
{
    is.read((char *)&size_, sizeof(size_));
    is.read((char *)&flags[0], 2);
    is.read((char *)&padding_size_, sizeof(padding_size_));
    if (size() > 6) {
        is.seekg(size() - 6, ios_base::cur);
    }
}

UInt32 ExtendedHeader::size() const
{
    return ex::reverse_byte_order_const<UInt32>(size_);
}


EXLIB_API FrameHeader::FrameHeader(istream& is)
{
    is.read((char *)&id[0], 4);
    is.read((char *)&size_, sizeof(size_));
    is.read((char *)&flags[0], 2);
}

UInt32 FrameHeader::size() const
{
    return ex::reverse_byte_order_const<UInt32>(size_);
}

string FrameHeader::id_str() const
{
    return string(&id[0], 4);
}

EXLIB_API COMMFrame::COMMFrame(istream& is)
{
    // cout << "COMMFrame FrameHeader(is)..." << endl;
    hdr = FrameHeader(is);
    if (hdr.id_str() != "COMM") {
        // cout << "not a COMM frame. Advancing is past this frame." << endl;
        is.seekg(hdr.size(), ios_base::cur);
        throw NotACOMM("Not a COMM frame.");
    }
    streampos frame_end = is.tellg() + streampos(hdr.size());
    // cout << "is a COMM frame." << endl;
    is.read((char *)&encoding, 1);
    is.read((char *)&language[0], 3);

    if (encoding == 0) {
        getline(is, short_description, '\0');
        // Apparently I cannot count on the comment being null-terminated.
        int comment_length = static_cast<int>(frame_end - is.tellg());
        if (comment_length > 0) {
            while (comment_length) {
                comment.push_back(is.get());
                comment_length--;
            }
        }
    } else {
        throw NotACOMM("Only encoding == 0 is supported.");
    }
}

string COMMFrame::id_str() const
{
    return hdr.id_str();
}

EXLIB_API string get_comm(istream& is)
{
    // cout << "get_comm" << endl;
    ID3Header id3_hdr;
    ExtendedHeader ext_hdr;
    // cout << "reading id3_hdr.." << endl;
    is.read((char *)&id3_hdr, sizeof(id3_hdr));
    // cout << "checking for valid hdr..." << endl;
    // cout << "id3_hdr.id[0]=" << id3_hdr.id[0] << endl;
    if (id3_hdr.id_str() != "ID3") {
        throw NoID3Found(string("No ID3 tag found. ") + id3_hdr.id_str());
    }
    // cout << "hdr is valid." << endl;
    if (id3_hdr.extended_header()) {
        ext_hdr = ExtendedHeader(is);
    }

    COMMFrame comm;
    // cout << "iterating over frames..." << endl;
    while (true) {
        // cout << "is.tellg() = " << is.tellg() << endl;
        if (is.tellg() > id3_hdr.get_size() + 10) {
            break;
        }
        if (is.tellg() > 10000) break;

        try {
            comm = COMMFrame(is);
        } catch (NotACOMM&) {
            // cout << "looking for COMMFrame at is.tellg() = " << is.tellg() << endl;
            continue;
        }

        return comm.comment;
    }

    return "";
}

}
