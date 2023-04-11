#include <iostream>
#include "core/timeline/midi_loaders.h"

using namespace std;

namespace TL {

ROCS_CORE_API void read_pdf_track(
    const std_midi::MIDIFile& midiFile,
    PageNumSeqPtrT seq_ptr,
    UInt32 &audioStartTicks,
    UInt32 &audioEndTicks)
{
    UInt32 abs_time = 0;
    map<int, int> notes;
    notes[20] = 0;
    notes[21] = 127;
    notes[22] = 254;
    notes[23] = 381;
    auto pdf_track = find_if(
        midiFile.GetTracks().begin(),
        midiFile.GetTracks().end(),
        [] (const std_midi::MIDITrack& t)
        {
            return (t.GetTrackName() == "PDF" || t.GetTrackName() == "pdf" );
        });

    bool foundFirstCC25 = false;
    bool foundSecondCC25 = false;

    if (pdf_track != midiFile.GetTracks().end())
    {
        map<int, int> ccs;
        for (auto it : pdf_track->GetPackets())
        {
            abs_time += it.delta_time();
            auto voice_events = it.voice_filter(std_midi::sb::control_change);
            if (!voice_events.size()) continue;
            ccs.clear();

            for (auto evt : voice_events)
            {
                // evt should be a std_midi::VoiceMessage
                if (!notes.count(evt.GetData1()))
                {
                    if (evt.GetData1() == 25)
                    {
                        cout << "Found cc25 at abs_time = " << abs_time << endl;
                        if (foundFirstCC25)
                        {
                            foundSecondCC25 = true;
                            audioEndTicks = abs_time;
                        } else
                        {
                            foundFirstCC25 = true;
                            audioStartTicks = abs_time;
                        }
                        continue;
                    } else
                    {
                        throw PageNumberError(ex::format(
                            "Invalid CC %d encountered at %d in PDF track of %s",
                            evt.GetData1(),
                            abs_time, 
                            midiFile.GetFilename().c_str()));
                    }
                } else
                {
                    ccs[evt.GetData1()] = evt.GetData2();
                }
            }

            if (foundSecondCC25)
            {
                break; // No events are allowed after end cc
            }

            if (!ccs.size())
            {
                // No need to proceed. There are no page number ccs here.
                continue;
            }

            PageNum page;
            for (auto p : ccs)
            {
                if (p.second > 0)
                {
                    if (page.page_number() > 0)
                    {
                    // This is the second CC at this time.
                        throw PageNumberError(ex::format(
                            "Multiple page number ccs found at %d in %s",
                            abs_time,
                            midiFile.GetFilename().c_str()));
                    } else
                    {
                        page.abs_time(abs_time);
                        page.page_number(notes[p.first] + p.second);
                    }
                }
            }

            if (page.page_number() == 0)
            {
                throw PageNumberError(ex::format(
                    "cc messages at %d in %s specify invalid page number (0)",
                    abs_time,
                    midiFile.GetFilename().c_str()));
            }

            seq_ptr->push_back(page);
        }
    } else
    {
        throw PageNumberError("Missing pdf track in " + midiFile.GetFilename());
    }

    cout << "read_pdf_track found audioStartTicks = " << audioStartTicks
        << ", and audioEndTicks = " << audioEndTicks << endl;
}


typedef BarsBeatsSeqT::iterator bb_iterator_t;
typedef CL::CustomBarSeqT::iterator cb_iterator;

/* use rename_bars to renumber bars after a custom bar number. */
bb_iterator_t rename_bars(
    bb_iterator_t start_it,
    bb_iterator_t end_it,
    SInt32 start_num)
{
    if (start_it == end_it)
    {
        throw logic_error("Cannot rename_bars beyond the end of bars_beats!");
    }

    start_it->bar_number(start_num);
    start_it++;

    for (; start_it != end_it; start_it++)
    {
        if (start_it->beat_number() == 1)
        {
            start_num++;
        }
        
        start_it->bar_number(start_num);
    }
    
    return start_it;
};

/* use rename_measure to add a custom name to an entire measure. */
bb_iterator_t rename_measure(bb_iterator_t start_it, bb_iterator_t end_it, const std::string& custom_name) {

    do 
    {
        start_it->bar_number(custom_name);
        start_it++;
        if (start_it == end_it) break;
    } while (start_it->beat_number() != 1);

    return start_it;
};


ROCS_CORE_API void create_bars_beats(
    const std_midi::MIDIFile& midiFile,
    MeterSeqPtrT meters,
    BarsBeatsSeqPtrT bars_beats )
{
    /* Using meters, create all of the bars_beats events */
    auto ppqn = midiFile.GetDivision();
    auto conductor = midiFile.GetConductorPackets();
    Meter current_meter = meters->front();
    Meter next_meter;
    UInt32 current_time = current_meter.abs_time();
    UInt32 next_time;

    SInt32 bar_number = 0;
    int whole_beats_per_measure;
    int remainder_ticks;
    int ticks_per_beat;

    auto calc_beats_and_ticks = [&]()
    {
        ticks_per_beat = current_meter.ticks_per_beat(ppqn);
        double intpart = 0;
        double remainder = modf(current_meter.beats_per_measure(), &intpart);
        whole_beats_per_measure = static_cast<int>(intpart);
        remainder_ticks = static_cast<int>(remainder * ppqn);
    };

    auto process_range = [&]()
    {
        while (current_time < next_time)
        {
            for (auto i = 0; i < whole_beats_per_measure; i++)
            {
                bars_beats->push_back(BarsBeats(current_time, bar_number, i + 1));
                current_time += ticks_per_beat;
            }

            if (remainder_ticks)
            {
                bars_beats->push_back(BarsBeats(
                    current_time,
                    bar_number,
                    whole_beats_per_measure + 1));
                
                current_time += remainder_ticks; 
                /* remainder ticks will be nonzero when the meter is irregular,
                 * e.g. 5/8, 7/8 */
            }

            ++bar_number;
        }
    };

    calc_beats_and_ticks();

    if (meters->size() > 1)
    {
        for (auto it = meters->begin() + 1; it != meters->end(); it++)
        {
            next_meter = *it;
            next_time = next_meter.abs_time();
            process_range();
            current_meter = next_meter;
            calc_beats_and_ticks();
        }
    }

    /* Now process bars_beats to the end of the song. */
    next_time = midiFile.GetLength() + 1;
    process_range();

    /* Now, using Custom Bar numbers, renumber the bars in bars_beats */
    UInt32 abs_time = 0;
    CL::CustomBarSeqT custom_bars;
    for (auto it: conductor)
    {
        abs_time += it.delta_time();
        auto msg_ptr = it.meta_filter(std_midi::sb::marker);
        if (msg_ptr)
        {
            auto mkr_ptr = std::dynamic_pointer_cast<std_midi::Marker>(msg_ptr);
            auto cbar_ptr = CL::read_marker_single<CL::CustomBar>(abs_time, *mkr_ptr);
            if (cbar_ptr)
            {
                custom_bars.push_back(*cbar_ptr);
                cbar_ptr.reset();
            }

            mkr_ptr.reset();
        }

        msg_ptr.reset();
    }

    if (!(custom_bars.size()))
    {
        return;
    }
    
    /* check for alignment on measure boundary. */
    auto check_cbar = [&](bb_iterator_t bb_it, cb_iterator cb_it)
    {
        if (bb_it != bars_beats->end())
        {
            /* aligned with a beat, because cb_it is the result of a call to
             * find_if, which compares custom bars to measure boundaries.
             * find_if return end if criteria is not met. */
            
            if (bb_it->beat_number() == 1)
            {
                // aligned with a measure boundary
                return;
            }
        }

        throw CustomBarError(ex::format(
            "CustomBar %s at %d in %s does not align with measure boundary",
            cb_it->value().c_str(),
            cb_it->abs_time(),
            midiFile.GetFilename().c_str()));
    };

    cb_iterator cbar_it = custom_bars.begin();

    /* Find the BarsBeats that matches the time of the first CustomBar */
    bb_iterator_t current_bb_it = find_if(
        bars_beats->begin(),
        bars_beats->end(),
        [&](const BarsBeats& b)
        {
            return cbar_it->abs_time() == b.abs_time();
        });

    /* Check that the first CustomBar is on a measure boundary */
    check_cbar(current_bb_it, cbar_it);

    /* current_bar_str may be convertible to an int, or it may contain alphas
     * */
    string current_bar_str = cbar_it->value();

    /* rename_measure will rename all of the BarsBeats with current_bar_str,
     * until it encounters beat 1 (the next measure) or bars_beats->end(). The
     * return value is an iterator to the next measure. rename_measure changes
     * the bar_number of each beat to match the custom bar value.
     */
    current_bb_it = rename_measure(current_bb_it, bars_beats->end(), current_bar_str);

    /* advance cbar_it. After advancing, it may be at the same time as current_bb_it,
        and it may be later. */
    cbar_it++;

    for (; cbar_it != custom_bars.end(); cbar_it++)
    {
        /* Find the BarsBeats that matches the time of cbar_it */
        auto next_bb_it = find_if(
            current_bb_it,
            bars_beats->end(),
            [&](const BarsBeats& b)
            {
                return cbar_it->abs_time() == b.abs_time();
            });
        
        /* Check that cbar_it aligns with measure boundary */
        check_cbar(next_bb_it, cbar_it);

        if (next_bb_it != current_bb_it)
        {
            if (ex::str_is_int(current_bar_str))
            {
                /* If current_bar_str is an integer, then renumber all of the
                 * measures up to the next CustomBar. */
                current_bb_it = rename_bars(
                    current_bb_it,
                    next_bb_it,
                    ex::str_to_num<int>(current_bar_str) + 1);
            } else
            {
                /* A non-integer CustomBar was not followed by another
                 * CustomBar, so I do not know which integer value to start
                 * numbering from.  This is an error. */
                throw CustomBarError(ex::format(
                    "%s %s at %d in %s. Next CustomBar: %s at %u",
                    "Expected CustomBar not found following",
                    current_bar_str.c_str(),
                    current_bb_it->abs_time(),
                    midiFile.GetFilename().c_str(),
                    cbar_it->value().c_str(),
                    next_bb_it->abs_time()));
            }
        }

        /* At this point, current_bb_it should be equal to next_bb_it */
        if (current_bb_it != next_bb_it)
        {
            throw logic_error("current_bb_it is not next_bb_it");
        }

        /* It is possible that current_bb_it == bars_beats.end() */
        if (current_bb_it == bars_beats->end())
        {
            break;
        }

        current_bar_str = cbar_it->value();
        current_bb_it = rename_measure(
            current_bb_it,
            bars_beats->end(),
            current_bar_str);
    }

    // Rename bars to the end of the song
    // cbar_it == custom_bars.end()
    cbar_it--;
    if (current_bb_it != bars_beats->end())
    {
        if (!ex::str_is_int(current_bar_str))
        {
            throw CustomBarError(ex::format(
                    "Expected CustomBar not found following %s at %d in %s",
                    current_bar_str.c_str(),
                    current_bb_it->abs_time(),
                    midiFile.GetFilename().c_str()));
        } else
        {
            rename_bars(
                current_bb_it,
                bars_beats->end(),
                ex::str_to_num<int>(current_bar_str) + 1);
        }
    }
}

} // namespace TL
