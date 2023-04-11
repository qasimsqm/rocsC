#include "core/changelog/song_log.h"

using namespace std;

namespace CL
{

SongLog::SongLog()
    :
    song_number_(),
    song_name_(),
    ppqn_(0),
    song_length_(0),
    attacca_state_(0),
    vamps_(VampSeqPtrT(new VampSeqT())),
    repeats_(RepeatSeqPtrT(new RepeatSeqT())),
    cuts_(CutSeqPtrT(new CutSeqT())),
    transpositions_(TransposeSeqPtrT(new TransposeSeqT())),
    tempo_scales_(TempoScaleSeqPtrT(new TempoScaleSeqT())),
    markers_(MarkerSeqPtrT(new MarkerSeqT())),
    clicks_(ClickSeqPtrT(new ClickSeqT())),
    fermatas_(FermataSeqPtrT(new FermataSeqT())),    
    caesuras_(CaesuraSeqPtrT(new CaesuraSeqT())),
    showReadyStartTicks_(0),
    showReadyEndTicks_(0),
    fillHarmonicStartTicks_(0),
    fillHarmonicEndTicks_(0),
    hasShowReadyStartTicks_(false),
    hasShowReadyEndTicks_(false),
    hasFillHarmonicStartTicks_(false),
    hasFillHarmonicEndTicks_(false)
{
    make_all_seqs();
}

SongLog::SongLog(const SongLog& other)
    :
    song_number_(other.song_number_),
    song_name_(other.song_name_),
    ppqn_(other.ppqn_),
    song_length_(other.song_length_),
    attacca_state_(other.attacca_state_),
    vamps_(VampSeqPtrT(new VampSeqT(*other.vamps_))),
    repeats_(RepeatSeqPtrT(new RepeatSeqT(*other.repeats_))),
    cuts_(CutSeqPtrT(new CutSeqT(*other.cuts_))),
    transpositions_(TransposeSeqPtrT(new TransposeSeqT(*other.transpositions_))),
    tempo_scales_(TempoScaleSeqPtrT(new TempoScaleSeqT(*other.tempo_scales_))),
    markers_(MarkerSeqPtrT(new MarkerSeqT(*other.markers_))),
    clicks_(ClickSeqPtrT(new ClickSeqT(*other.clicks_))),
    fermatas_(FermataSeqPtrT(new FermataSeqT(*other.fermatas_))),
    caesuras_(CaesuraSeqPtrT(new CaesuraSeqT(*other.caesuras_))),
    showReadyStartTicks_(other.showReadyStartTicks_),
    showReadyEndTicks_(other.showReadyEndTicks_),
    fillHarmonicStartTicks_(other.fillHarmonicStartTicks_),
    fillHarmonicEndTicks_(other.fillHarmonicEndTicks_),
    hasShowReadyStartTicks_(other.hasShowReadyStartTicks_),
    hasShowReadyEndTicks_(other.hasShowReadyEndTicks_),
    hasFillHarmonicStartTicks_(other.hasFillHarmonicStartTicks_),
    hasFillHarmonicEndTicks_(other.hasFillHarmonicEndTicks_)
{
    this->make_all_seqs();
}

SongLog::SongLog(
    const std::string &song_number_,
    const std::string &song_name_,
    UInt16 ppqn_,
    UInt32 song_length_)
    :
    song_number_(song_number_),
    song_name_(song_name_),
    ppqn_(ppqn_),
    song_length_(song_length_),
    attacca_state_(0),
    vamps_(VampSeqPtrT(new VampSeqT())),
    repeats_(RepeatSeqPtrT(new RepeatSeqT())),
    cuts_(CutSeqPtrT(new CutSeqT())),
    transpositions_(TransposeSeqPtrT(new TransposeSeqT())),
    tempo_scales_(TempoScaleSeqPtrT(new TempoScaleSeqT())),
    markers_(MarkerSeqPtrT(new MarkerSeqT())),
    clicks_(ClickSeqPtrT(new ClickSeqT())),
    fermatas_(FermataSeqPtrT(new FermataSeqT())),
    caesuras_(CaesuraSeqPtrT(new CaesuraSeqT())),
    showReadyStartTicks_(0),
    showReadyEndTicks_(0),
    fillHarmonicStartTicks_(0),
    fillHarmonicEndTicks_(0),
    hasShowReadyStartTicks_(false),
    hasShowReadyEndTicks_(false),
    hasFillHarmonicStartTicks_(false),
    hasFillHarmonicEndTicks_(false)
{
    this->make_all_seqs();
}

SongLog & SongLog::operator=(const SongLog &other)
{
    if (this == &other) throw logic_error("SongLog::operator= Copy to self");

    this->song_number_ = other.song_number_;
    this->song_name_ = other.song_name_;
    this->ppqn_ = other.ppqn_;
    this->song_length_ = other.song_length_;
    this->attacca_state_ = other.attacca_state_;
    
    this->vamps_ = VampSeqPtrT(new VampSeqT(*other.vamps_)),
    this->repeats_ = RepeatSeqPtrT(new RepeatSeqT(*other.repeats_)),
    this->cuts_ = CutSeqPtrT(new CutSeqT(*other.cuts_)),
    this->transpositions_ = TransposeSeqPtrT(new TransposeSeqT(*other.transpositions_)),
    this->tempo_scales_ = TempoScaleSeqPtrT(new TempoScaleSeqT(*other.tempo_scales_)),
    this->markers_ = MarkerSeqPtrT(new MarkerSeqT(*other.markers_)),
    this->clicks_ = ClickSeqPtrT(new ClickSeqT(*other.clicks_)),
    this->fermatas_ = FermataSeqPtrT(new FermataSeqT(*other.fermatas_)),
    this->caesuras_ = CaesuraSeqPtrT(new CaesuraSeqT(*other.caesuras_)),
    
    this->showReadyStartTicks_ = other.showReadyStartTicks_;
    this->showReadyEndTicks_ = other.showReadyEndTicks_;
    this->fillHarmonicStartTicks_ = other.fillHarmonicStartTicks_;
    this->fillHarmonicEndTicks_ = other.fillHarmonicEndTicks_;
    this->hasShowReadyStartTicks_ = other.hasShowReadyStartTicks_;
    this->hasShowReadyEndTicks_ = other.hasShowReadyEndTicks_;
    this->hasFillHarmonicStartTicks_ = other.hasFillHarmonicStartTicks_;
    this->hasFillHarmonicEndTicks_ = other.hasFillHarmonicEndTicks_;
    
    this->all_seqs_.clear();
    this->make_all_seqs();

    return *this;
}

SongLog::SongLog(istream &is, const cmn::FileVersion &fileVersion)
{
    this->song_number_ = ex::ReadString(is);
    this->song_name_ = ex::ReadString(is);
    is.read((char *)&this->ppqn_, sizeof(this->ppqn_));
    is.read((char *)&this->song_length_, sizeof(this->song_length_));
    this->attacca_state_ = is.get();
    UInt8 seq_count = is.get();
    
    for (; seq_count > 0; seq_count--)
    {
        switch(is.peek())
        {
            case codes::marker:
                this->markers_ = MarkerSeqPtrT(new MarkerSeqT(is, fileVersion));
                break;
            case codes::caesura:
                this->caesuras_ = CaesuraSeqPtrT(new CaesuraSeqT(is, fileVersion));
                break;
            case codes::cut:
                this->cuts_ = CutSeqPtrT(new CutSeqT(is, fileVersion));
                break;
            case codes::vamp:
                this->vamps_ = VampSeqPtrT(new VampSeqT(is, fileVersion));
                break;
            case codes::repeat:
                this->repeats_ = RepeatSeqPtrT(new RepeatSeqT(is, fileVersion));
                break;
            case codes::transpose:
                this->transpositions_ = TransposeSeqPtrT(
                    new TransposeSeqT(is, fileVersion));
                break;
            case codes::click:
                this->clicks_ = ClickSeqPtrT(new ClickSeqT(is, fileVersion));
                break;
            case codes::tempo_scale:
                this->tempo_scales_ = TempoScaleSeqPtrT(
                    new TempoScaleSeqT(is, fileVersion));
                break;
            case codes::fermata:
                this->fermatas_ = FermataSeqPtrT(new FermataSeqT(is, fileVersion));
                break; 
            default:
                throw UnknownSequenceType(ex::format(
                    "SongLog found unknown event code: %#X at %u",
                    is.peek(),
                    is.tellg()));
        }
    }

    this->make_all_seqs();
}

typedef std::shared_ptr<Vamp> VampPtrT;
typedef std::shared_ptr<Repeat> RepeatPtrT;
typedef std::shared_ptr<Fermata> FermataPtrT;

SongLog::SongLog(const std_midi::MIDIFile &midiFile):
    vamps_(new VampSeqT()),
    repeats_(new RepeatSeqT),
    cuts_(new CutSeqT),
    transpositions_(new TransposeSeqT()),
    tempo_scales_(new TempoScaleSeqT()),
    markers_(new MarkerSeqT()),
    clicks_(new ClickSeqT()),
    fermatas_(new FermataSeqT()),
    caesuras_(new CaesuraSeqT()),
    showReadyStartTicks_(0),
    showReadyEndTicks_(0),
    fillHarmonicStartTicks_(0),
    fillHarmonicEndTicks_(0),
    hasShowReadyStartTicks_(false),
    hasShowReadyEndTicks_(false),
    hasFillHarmonicStartTicks_(false),
    hasFillHarmonicEndTicks_(false)
{
    this->ppqn_ = midiFile.GetDivision();
    this->song_length_ = midiFile.GetLength();

    cmn::parse_filename(
        midiFile.GetFilename(),
        this->song_number_,
        this->song_name_,
        this->attacca_state_);

    // Vamp and Repeat are the only PairedEvent's that can be encoded in MIDI file.
    // Added fermatas
    Vamp prev_vamp;
    VampPtrT curr_vamp;
    Repeat prev_repeat;
    RepeatPtrT curr_repeat;
    Fermata prev_fermata;
    FermataPtrT curr_fermata;
    std::list<cmn::ROCSEvtPtrT> evts;
    std_midi::MarkerPtrT mkr_ptr;

    std_midi::MIDIPacketVecT packets = midiFile.GetTracks().at(0).GetPackets();
    UInt32 abs_time = 0;

    for (auto p_it: midiFile.GetConductorPackets())
    {
        abs_time += p_it.delta_time();
        auto meta_ptr = p_it.meta_filter(std_midi::sb::marker);
        if (!meta_ptr)
        {
            continue;
        } else
        {
            mkr_ptr = dynamic_pointer_cast<std_midi::Marker>(meta_ptr);
        }
        
        evts = read_marker(abs_time, *mkr_ptr);
        std::shared_ptr<InPoint> inPoint;
        std::shared_ptr<OutPoint> outPoint;
        
        for (auto evt_it : evts)
        {
            switch(evt_it->code())
            {
                case codes::vamp:
                    curr_vamp = dynamic_pointer_cast<Vamp>(evt_it);
                    if (prev_vamp.is_start())
                    {
                        // The next event must be a vamp end.
                        if (!curr_vamp->is_end())
                        {
                            throw MIDIFileLoopError(
                                ex::format("Found @vs at %d. Expected @ve", abs_time));
                        }

                        prev_vamp.end(curr_vamp->end());
                        this->vamps_->push_back(prev_vamp);
                        prev_vamp.reset();
                    } else if (curr_vamp->is_start())
                    {
                        prev_vamp = *curr_vamp;
                    } else
                    {
                        // The next event must be a vamp start.
                        throw MIDIFileLoopError(
                            ex::format("Found @ve at %d. Expected @vs", abs_time));
                    }

                    curr_vamp.reset();
                    break;

                case codes::repeat:
                    curr_repeat = dynamic_pointer_cast<Repeat>(evt_it);
                    if (prev_repeat.is_start())
                    {
                        if (!curr_repeat->is_end())
                        {
                            throw MIDIFileLoopError(
                                ex::format("Found @rs at %d. Expected @re", abs_time));
                        }

                        prev_repeat.end(curr_repeat->end());
                        this->repeats_->push_back(prev_repeat);
                        prev_repeat.reset();
                    } else if ( curr_repeat->is_start())
                    {
                        prev_repeat = *curr_repeat;
                    } else
                    {
                        // The next event must be a repeat start.
                        throw MIDIFileLoopError(
                            ex::format("Found @re at %d. Expected @rs", abs_time));
                    }

                    curr_repeat.reset();
                    break;

                case codes::fermata:
                    curr_fermata = dynamic_pointer_cast<Fermata>(evt_it);
                    if (prev_fermata.is_start())
                    {
                        if (!curr_fermata->is_end())
                        {
                            throw MIDIFileLoopError(
                                ex::format("Found @f at %d. Expected @j", abs_time));
                        }

                        prev_fermata.end(curr_fermata->end());
                        this->fermatas_->push_back(prev_fermata);
                        prev_fermata.reset();
                    } else if (curr_fermata->is_start())
                    {
                        prev_fermata = *curr_fermata;
                    } else
                    {
                        // The next event must be a fermata start.
                        throw MIDIFileLoopError(
                            ex::format("Found @j at %d. Expected @f", abs_time));
                    }

                    curr_fermata.reset();
                    break;

                case codes::marker:
                    this->markers_->push_back(
                        *dynamic_pointer_cast<Marker>(evt_it));
                    break;

                case codes::caesura:
                    this->caesuras_->push_back(
                        *dynamic_pointer_cast<Caesura>(evt_it));
                    break;

                case codes::inPoint:
                    inPoint = dynamic_pointer_cast<InPoint>(evt_it);
                    if (inPoint->VersionTarget() == INOUT_TARGETS_ALL_MIDI)
                    {
                        this->hasShowReadyStartTicks_ = true;
                        this->hasFillHarmonicStartTicks_ = true;
                        this->showReadyStartTicks_ = inPoint->abs_time();
                        this->fillHarmonicStartTicks_ = inPoint->abs_time();
                    } else if (inPoint->VersionTarget() == INOUT_TARGETS_SHOWREADY)
                    {
                        this->hasShowReadyStartTicks_ = true;
                        this->showReadyStartTicks_ = inPoint->abs_time();
                    } else if (inPoint->VersionTarget() == INOUT_TARGETS_FILLHARMONIC)
                    {
                        this->hasFillHarmonicStartTicks_ = true;
                        this->fillHarmonicStartTicks_ = inPoint->abs_time();
                    }

                    break;

                case codes::outPoint:
                    outPoint = dynamic_pointer_cast<OutPoint>(evt_it);
                    if (outPoint->VersionTarget() == INOUT_TARGETS_ALL_MIDI)
                    {
                        this->hasShowReadyEndTicks_ = true;
                        this->hasFillHarmonicEndTicks_ = true;
                        this->showReadyEndTicks_ = outPoint->abs_time();
                        this->fillHarmonicEndTicks_ = outPoint->abs_time();
                    } else if (outPoint->VersionTarget() == INOUT_TARGETS_SHOWREADY)
                    {
                        this->hasShowReadyEndTicks_ = true;
                        this->showReadyEndTicks_ = outPoint->abs_time();
                    } else if (outPoint->VersionTarget() == INOUT_TARGETS_FILLHARMONIC)
                    {
                        this->hasFillHarmonicEndTicks_ = true;
                        this->fillHarmonicEndTicks_ = outPoint->abs_time();
                    }

                    break;
            }
        }
    }

    // There should not be any dangling vamp start or repeat start events, or fermatas
    if (prev_vamp.is_start())
    {
        throw MIDIFileLoopError(
            ex::format("Found @vs at %d without corresponding @ve", prev_vamp.start()));
    }
    
    if (prev_repeat.is_start())
    {
        throw MIDIFileLoopError(
            ex::format("Found @rs at %d without corresponding @re", prev_repeat.start())
        );
    }

    if (prev_fermata.is_start())
    {
        throw MIDIFileLoopError(
            ex::format("Found @f at %d without corresponding @j", prev_fermata.start()));
    }

    this->make_all_seqs();
}


void SongLog::WriteString(ostream &os) const
{
    os  << "SongLog("
        << "song_number=" << this->GetSongNumber() << ", "
        << "song_name=" << this->GetSongName() << ", "
        << "ppqn=" << this->GetPulsesPerQuarterNote() << ", "
        << "song_length=" << this->GetSongLength() << ", "
        << "attacca_state=" << attacca::names[this->attacca_state_]
        << ")";
}


void SongLog::WriteStringDetailed(ostream &os, int indent) const
{
    os << string(indent, '\t');
    WriteString(os);
    os << endl;
    for (auto it : all_seqs_)
    {
        it->WriteStringEvents(os, indent+1);
    }
}


void SongLog::WriteBinary(ostream &os, const cmn::FileVersion &fileVersion) const
{
    ex::WriteString(os, this->GetSongNumber());
    ex::WriteString(os, this->GetSongName());
    os.write((char *)&this->ppqn_, sizeof(this->ppqn_));
    os.write((char *)&this->song_length_, sizeof(this->song_length_));
    os.put(this->attacca_state_);
    os.put(this->all_seqs_.size());

    for (auto it : this->all_seqs_)
    {
        it->WriteBinary(os, fileVersion);
    }
}


void SongLog::make_all_seqs()
{
    if (!(this->vamps_)) throw;
    if (!(this->repeats_)) throw;
    if (!(this->cuts_)) throw;
    if (!(this->transpositions_)) throw;
    if (!(this->tempo_scales_)) throw;
    if (!(this->markers_)) throw;
    if (!(this->clicks_)) throw;
    if (!(this->caesuras_)) throw;
    if (!(this->fermatas_)) throw;
    this->all_seqs_.push_back(this->vamps_);
    this->all_seqs_.push_back(this->repeats_);
    this->all_seqs_.push_back(this->cuts_);
    this->all_seqs_.push_back(this->transpositions_);
    this->all_seqs_.push_back(this->tempo_scales_);
    this->all_seqs_.push_back(this->markers_);
    this->all_seqs_.push_back(this->clicks_);
    this->all_seqs_.push_back(this->caesuras_);
    this->all_seqs_.push_back(this->fermatas_);
}

CaesuraSeqT SongLog::GetDisplayCaesuras()
{
    CaesuraSeqT displayCaesuras;
    displayCaesuras.reserve(this->caesuras_->size());

    for (auto &it : *this->caesuras_)
    {
        if (!(it.VersionTarget() == CL::GetCaesuraIgnoredVersionTarget()))
        {
            displayCaesuras.push_back(it);
        }
    }

    return displayCaesuras;
}

bool operator==(const SongLog& lhs, const SongLog& rhs)
{
    if (lhs.GetSongNumber() != rhs.GetSongNumber()) return false;
    if (lhs.GetSongName() != rhs.GetSongName()) return false;
    if (lhs.GetPulsesPerQuarterNote() != rhs.GetPulsesPerQuarterNote()) return false;
    if (lhs.GetSongLength() != rhs.GetSongLength()) return false;
    if (lhs.GetAttaccaState() != rhs.GetAttaccaState()) return false;
    if (lhs.GetVamps() != rhs.GetVamps()) return false;
    if (lhs.GetRepeats() != rhs.GetRepeats()) return false;
    if (lhs.GetCuts() != rhs.GetCuts()) return false;
    if (lhs.GetTranspositions() != rhs.GetTranspositions()) return false;
    if (lhs.GetTempoScales() != rhs.GetTempoScales()) return false;
    if (lhs.GetMarkers() != rhs.GetMarkers()) return false;
    if (lhs.GetClicks() != rhs.GetClicks()) return false;
    if (lhs.GetFermatas() != rhs.GetFermatas()) return false;
    return lhs.GetCaesuras() == rhs.GetCaesuras();
}

} // End namespace CL
