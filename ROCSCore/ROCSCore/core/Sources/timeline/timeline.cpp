#include "core/timeline/timeline.h"
#include "exlib/cout_buffer.h"
using namespace std;

namespace TL
{

Timeline::Timeline()
    :
    midiStartTicks_(0),
    midiEndTicks_(0),
    audioStartTicks_(0),
    audioEndTicks_(0),
    tempos_(TempoSeqPtrT(new TempoSeqT())),
    bars_beats_(BarsBeatsSeqPtrT(new BarsBeatsSeqT())),
    keys_(KeySeqPtrT(new KeySeqT())),
    page_nums_(PageNumSeqPtrT(new PageNumSeqT())),
    meters_(MeterSeqPtrT(new MeterSeqT()))
{
    this->make_all_seqs();
}


Timeline::Timeline(const Timeline& other)
    :
    midiStartTicks_(other.midiStartTicks_),
    midiEndTicks_(other.midiEndTicks_),
    audioStartTicks_(other.audioStartTicks_),
    audioEndTicks_(other.audioEndTicks_),
    tempos_(other.tempos_),
    bars_beats_(other.bars_beats_),
    keys_(other.keys_),
    page_nums_(other.page_nums_),
    meters_(other.meters_)
{
    this->make_all_seqs();
}


Timeline& Timeline::operator=(const Timeline& other)
{   
    this->midiStartTicks_ = other.midiStartTicks_;
    this->midiEndTicks_ = other.midiEndTicks_;
    this->audioStartTicks_ = other.audioStartTicks_;
    this->audioEndTicks_ = other.audioEndTicks_;

    this->tempos_.reset();
    this->bars_beats_.reset();
    this->keys_.reset();
    this->page_nums_.reset();
    this->meters_.reset();

    this->tempos_ = other.tempos_;
    this->bars_beats_ = other.bars_beats_;
    this->keys_ = other.keys_;
    this->page_nums_ = other.page_nums_;
    this->meters_ = other.meters_;
   
    this->all_seqs_.clear();
    this->make_all_seqs();
    
    return *this;
}

Timeline::Timeline(istream &is, const cmn::FileVersion &fileVersion)
{
    if (fileVersion >= cmn::FileVersion(2, 0))
    {   
        this->midiStartTicks_ = ex::read<UInt32>(is);
        this->midiEndTicks_ = ex::read<UInt32>(is);
        this->audioStartTicks_ = ex::read<UInt32>(is);
        this->audioEndTicks_ = ex::read<UInt32>(is);
    }
 
    UInt8 seq_count = is.get();
    for (; seq_count > 0; seq_count--)
    {
        switch(is.peek())
        {
            case codes::tempo:
                this->tempos_ = TempoSeqPtrT(new TempoSeqT(is, fileVersion));
                break;
            case codes::bars_beats:
                this->bars_beats_ = BarsBeatsSeqPtrT(new BarsBeatsSeqT(is, fileVersion));
                break;
            case codes::key:
                this->keys_ = KeySeqPtrT(new KeySeqT(is, fileVersion));
                break;
            case codes::page_num:
                this->page_nums_ = PageNumSeqPtrT(new PageNumSeqT(is, fileVersion));
                break;
            case codes::meter:
                this->meters_ = MeterSeqPtrT(new MeterSeqT(is, fileVersion));
                break;
            default:
                throw UnknownSequenceType(
                    ex::format( "Timeline found unknown event code: %#X at %u",
                                is.peek(),
                                is.tellg()));
        }
    }

    this->make_all_seqs();
}


Timeline::Timeline(const std_midi::MIDIFile& midiFile)
    :
    midiStartTicks_(0),
    midiEndTicks_(0),
    audioStartTicks_(0),
    audioEndTicks_(0),
    tempos_(new TempoSeqT()),
    bars_beats_(new BarsBeatsSeqT()),
    keys_(new KeySeqT()),
    page_nums_(new PageNumSeqT()),
    meters_(new MeterSeqT())
{
    /* The conductor track must specify the following at abs_time 0:
            Key, Tempo, Meter */
    auto conductor = midiFile.GetConductorPackets();
    if (!conductor.size())
    {
        throw MissingEvents(ex::format(
            "%s appears to be missing critical meta events...",
            midiFile.GetFilename().c_str()));
    }
    
    UInt32 abs_time = 0;
    
    if (conductor.at(0).delta_time() > 0)
    {
        throw MissingEvents(ex::format(
                "%s does not begin with required Tempo, Key, and Meter events",
                midiFile.GetFilename().c_str()));
    }
    
    // process zero_th events
    process_meta_messages(conductor.at(0).meta_messages(), abs_time);
    vector<string> missing;

    // Check for missing zeroth events
    if (!tempos_->size())
    {
        warnings << midiFile.GetFilename()
                << " is missing Tempo event as time 0. Using default 120 bpm." << endl;
        this->tempos_->push_back(Tempo(0, 500000));
    }
    
    if (!keys_->size())
    {
        warnings << midiFile.GetFilename()
                << " is missing KeySignature event as time 0. Using default C major."
                << endl;
        this->keys_->push_back(Key(0, 0, 0));
    }

    if (!meters_->size())
    {
        warnings << midiFile.GetFilename()
                << " is missing TimeSignature event as time 0. Using default 4/4."
                << endl;
        this->meters_->push_back(Meter(0, 4, 4, 24, 8));
    }


    // Process remaining Tempo, Key, and Time Signature Events
    for (auto it = conductor.begin() + 1; it != conductor.end(); it++)
    {
        abs_time += it->delta_time();
        process_meta_messages(it->meta_messages(), abs_time);
    }

    try
    {
        create_bars_beats(midiFile, meters_, bars_beats_);
    } catch (CL::ArgError& e)
    {
        throw CL::ArgError(ex::format(
            "%s in %s",
            e.what(),
            midiFile.GetFilename().c_str()));
    }

    read_pdf_track(
        midiFile,
        this->page_nums_,
        this->audioStartTicks_, 
        this->audioEndTicks_);

    this->make_all_seqs();
}



void Timeline::WriteString(ostream &os) const
{
    os << "Timeline(";
    os << "midiStartTicks=" << this->midiStartTicks_ << endl;
    os << "midiEndTicks=" << this->midiEndTicks_ << endl;
    os << "audioStartTicks=" << this->audioStartTicks_ << endl;
    os << "audioEndTicks=" << this->audioEndTicks_ << endl;
    os << "seq_count=" << this->all_seqs_.size();
    os << ")";
}

void Timeline::WriteStringDetailed(ostream &os, int indent) const
{
    os << string(indent, '\t');
    this->WriteString(os);
    os << endl;
    for (auto it : this->all_seqs_)
    {
        it->WriteStringEvents(os, indent+1);
    }
}

void Timeline::WriteBinary(ostream &os, const cmn::FileVersion &fileVersion) const
{
    if (fileVersion >= cmn::FileVersion(2, 0))
    {
        ex::write(os, this->midiStartTicks_);
        ex::write(os, this->midiEndTicks_);
        ex::write(os, this->audioStartTicks_);
        ex::write(os, this->audioEndTicks_);
    }

    os.put(this->all_seqs_.size());
    for (auto it: this->all_seqs_)
    {
        it->WriteBinary(os, fileVersion);
    }
}

void Timeline::make_all_seqs()
{
    #ifndef NDEBUG
    if (!(this->tempos_))
    {
        cout << "tempos_ has not been initialized." << endl;
        this->tempos_ = TempoSeqPtrT(new TempoSeqT);
    }

    if (!(this->bars_beats_)) {
        cout << "bars_beats_ has not been initialized." << endl;
        this->bars_beats_ = BarsBeatsSeqPtrT(new BarsBeatsSeqT);
    }

    if (!(this->keys_)) {
        cout << "keys_ has not been initialized." << endl;
        this->keys_ = KeySeqPtrT(new KeySeqT);
    }

    if (!(this->page_nums_)) {
        cout << "page_nums_ has not been initialized." << endl;
        this->page_nums_ = PageNumSeqPtrT(new PageNumSeqT);
    }

    if (!(this->meters_)) {
        cout << "meters_ has not been initialized." << endl;
        this->meters_ = MeterSeqPtrT(new MeterSeqT);
    }
    #endif
    
    this->all_seqs_.push_back(this->tempos_);
    this->all_seqs_.push_back(this->bars_beats_);
    this->all_seqs_.push_back(this->keys_);
    this->all_seqs_.push_back(this->page_nums_);
    this->all_seqs_.push_back(this->meters_);
}

void Timeline::process_meta_messages(
    const std_midi::MetaMsgPtrVecT& msg_vec,
    UInt32 abs_time)
{
    using namespace std_midi::sb;
    for (auto it : msg_vec)
    {
        switch(it->status())
        {
            case tempo:
                this->tempos_->push_back(Tempo(abs_time, it->microseconds()));
                break;
            case key_signature:
                this->keys_->push_back(Key(abs_time, it->sharps(), it->mode()));
                break;
            case time_signature:
                this->meters_->push_back(Meter(abs_time, &(it->bytes()[0])));
                break;
            default:
                continue;
        }
    }
}

UInt32 Timeline::GetBarOne(unsigned int pulsesPerQuarterNote) const
{
    Meter m(meters_->front());
    return static_cast<UInt32>(
        m.beats_per_measure() * m.ticks_per_beat(pulsesPerQuarterNote));
}

ROCS_CORE_API bool operator==(const Timeline &lhs, const Timeline &rhs)
{
    if (!(lhs.GetTempos() == rhs.GetTempos())) return false;
    if (lhs.GetBarsBeats() != rhs.GetBarsBeats()) return false;
    if (lhs.GetKeys() != rhs.GetKeys()) return false;
    if (lhs.GetPageNums() != rhs.GetPageNums()) return false;
    return lhs.GetMeters() == rhs.GetMeters();
}

} // End namespace TL
