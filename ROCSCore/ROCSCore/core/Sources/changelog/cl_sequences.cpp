#include "core/changelog/cl_sequences.h"

using namespace std;

namespace CL {

MarkerSequence::MarkerSequence(std::istream &is, const cmn::FileVersion &fileVersion)
{
    UInt8 event_type = is.get();
    if (event_type != this->code()) throw logic_error("event_type does not match container type.");
    UInt32 obj_count;
    is.read((char *)&obj_count, sizeof(obj_count));
    while (obj_count > 0) {
        this->events_.push_back(Marker(is, fileVersion));
        obj_count--;
    }
}

void MarkerSequence::WriteBinary(ostream &os, const cmn::FileVersion& fileVersion) const
{
    os.put(this->code());
    UInt32 obj_count = this->events_.size();
    os.write((char *)&obj_count, sizeof(obj_count));
    
    for (auto it: this->events_)
    {
        it.WriteBinary(os, fileVersion);
    }
}

} // namespace CL
