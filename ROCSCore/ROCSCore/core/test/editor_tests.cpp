#include "gtest/gtest.h"
#include "core/test/test_editor.h"
#include "core/changelog/change_log.h"
#include "core/rocs_midi/show_data.h"

#include <string>
#include <utility>
#include <fstream>
#include <memory>
#include <iostream>

const std::string changelogFilename("test/the_addams_family.logdata");
const std::string showdataFilename("test/the_addams_family.showdata");
using namespace CL;


void PrintHistoryItem(const HistoryItem &historyItem, int indentSpaces = 0)
{
    historyItem.WriteString(std::cout, indentSpaces);
}

void PrintHistoryAction(const EditorHistory::HistoryActionT &action)
{
    int index = 0;
    std::cout << "HistoryAction:\n{" << std::endl;
    for (auto &it: action)
    {
        std::cout << "  [" << index << "] = " << std::endl;
        PrintHistoryItem(it, 4);
        std::cout << "," << std::endl;
        index++;
    }
    std::cout << "}" << std::endl;
}

void PrintRocsEvtPtr(cmn::ROCSEvtPtrT ptr)
{
    WriteRocsEvtPtrAsString(ptr, std::cout);
    std::cout << std::endl;
}

void PrintRocsEvtPtrList(const std::list<cmn::ROCSEvtPtrT> &rocsEvtPtrList)
{
    WriteRocsEvtPtrListAsString(rocsEvtPtrList, std::cout);
    std::cout << std::endl;
}

class EditorTest
    :
    public ::testing::Test
{
public:
    EditorTest()
    {
        std::ifstream changeLogIstream(
            changelogFilename,
            std::ios::in | std::ios::binary);
        
        std::ifstream showDataIstream(
            showdataFilename,
            std::ios::in | std::ios::binary);
        
        this->changeLog_ = std::unique_ptr<CL::ChangeLog>(
            new CL::ChangeLog(changeLogIstream));
        
        this->showData_ = std::unique_ptr<rocs_midi::ShowData>(
            new rocs_midi::ShowData(showDataIstream));
        
        this->editor_ = std::unique_ptr<CL::TestEditor>(
            new CL::TestEditor(*this->changeLog_, *this->showData_));

        this->SelectSongForEditing("01");
    }

    virtual ~EditorTest() {}
    
    CL::TestEditor &GetEditor() { return *this->editor_; }

    CL::ChangeLog &GetChangeLog() { return *this->changeLog_; }

    CL::SongLog &GetActiveSongLog()
    {
        return this->changeLog_->GetSongLog(this->activeSongNumber_);
    }
    
    rocs_midi::ShowData GetShowData() { return *this->showData_; }
    
    UInt32 GetBeatIndex(
        UInt32 *outPreviousBeatSpacing = NULL,
        UInt32 *outNextBeatSpacing = NULL)
    {
        auto timeline = 
            this->GetShowData().GetSongData(this->activeSongNumber_).GetTimeline();
        auto barsBeats = timeline.GetBarsBeats();
        auto middleIndex = barsBeats.size() / 2;
        UInt32 resultTickIndex = barsBeats[middleIndex].abs_time(); 
        
        if (outPreviousBeatSpacing != NULL)
        {
            *outPreviousBeatSpacing = resultTickIndex - 
                barsBeats[middleIndex - 1].abs_time();
        }
        
        if (outNextBeatSpacing != NULL)
        {
            *outNextBeatSpacing = barsBeats[middleIndex + 1].abs_time()
                - resultTickIndex;
        }

        return resultTickIndex;
    }
    
    struct BeatAlignedEndPoints 
    {
        UInt32 start;
        UInt32 end;
        UInt32 beatSpacingBeforeStart;
        UInt32 beatSpacingAfterStart;
        UInt32 beatSpacingBeforeEnd;
        UInt32 beatSpacingAfterEnd;
        
        UInt32 SizeTickCount()
        {
            assert(end >= start);
            return end - start;
        }
    };

    BeatAlignedEndPoints GetTwoBeatIndices(SInt32 beatCount = 8)
    {
        BeatAlignedEndPoints result; 
        auto timeline = 
            this->GetShowData().GetSongData(this->activeSongNumber_).GetTimeline();
        auto barsBeats = timeline.GetBarsBeats();
        auto middleIndex = barsBeats.size() / 2;
        result.start = barsBeats[middleIndex].abs_time();
        
        while (middleIndex + beatCount > (barsBeats.size() - 2))
        {
            beatCount--;
        }

        assert(beatCount > 0);
        result.end = barsBeats[middleIndex + beatCount].abs_time();
        
        result.beatSpacingBeforeStart =
            result.start - barsBeats[middleIndex - 1].abs_time();

        
        result.beatSpacingAfterStart = 
                barsBeats[middleIndex + 1].abs_time() - result.start;
        
        result.beatSpacingBeforeEnd = 
                result.end - barsBeats[middleIndex + beatCount - 1].abs_time();
        
        result.beatSpacingAfterEnd =
                barsBeats[middleIndex + beatCount + 1].abs_time() - result.end;
        
        return result;
    }

    void SelectSongForEditing(const std::string &songNumber)
    {
        this->activeSongNumber_ = songNumber;
        this->GetEditor().SelectSongLogForEditing(songNumber);
    }


    struct EndPoints
    {
        EndPoints() {}

        EndPoints(UInt32 startArg, UInt32 endArg)
            :
            start(startArg),
            end(endArg)
        {}
        
        UInt32 SizeTickCount()
        {
            assert(end >= start);
            return end - start;
        }

        UInt32 start;
        UInt32 end;
    };

    EndPoints GetOverlapStart(
        const cmn::ROCSEvent &event,
        int startOffsetBeats = 2,
        int endOffsetBeats = 2)
    {
        return EndPoints(
            this->GetBeatTickIndexBefore(event.start(), startOffsetBeats),
            this->GetBeatTickIndexAfter(event.start(), endOffsetBeats));
    }
    
    EndPoints GetOverlapEnd(
        const cmn::ROCSEvent &event,
        int startOffsetBeats = 2,
        int endOffsetBeats = 2)
    {
        return EndPoints(
            this->GetBeatTickIndexBefore(event.end(), startOffsetBeats),
            this->GetBeatTickIndexAfter(event.end(), endOffsetBeats));
    }

    EndPoints GetOverlapOutside(
        const cmn::ROCSEvent &event,
        int startOffsetBeats = 2,
        int endOffsetBeats = 2)
    {
        return EndPoints(
            this->GetBeatTickIndexBefore(event.start(), startOffsetBeats),
            this->GetBeatTickIndexAfter(event.end(), endOffsetBeats));
    }

    EndPoints GetOverlapInside(
        const cmn::ROCSEvent &event,
        int startOffsetBeats = 2,
        int endOffsetBeats = 2)
    {
        return EndPoints(
            this->GetBeatTickIndexAfter(event.start(), startOffsetBeats),
            this->GetBeatTickIndexBefore(event.end(), endOffsetBeats));
    }

    EndPoints GetAdjacentToStart(
        const cmn::ROCSEvent &event,
        int startOffsetBeats = 8)
    {
        return EndPoints(
            this->GetBeatTickIndexBefore(event.start(), startOffsetBeats),
            event.start()); 
    }

    EndPoints GetAdjacentToEnd(
        const cmn::ROCSEvent &event,
        int endOffsetBeats = 8)
    {
        return EndPoints(
            event.end(),
            this->GetBeatTickIndexAfter(event.end(), endOffsetBeats));
    }

    EndPoints GetEndPointsBefore(
        const cmn::ROCSEvent &event,
        int lengthBeatCount,
        int offsetBeatCount)
    {
        return EndPoints(
            this->GetBeatTickIndexBefore(
                event.start(),
                offsetBeatCount + lengthBeatCount),
            this->GetBeatTickIndexBefore(
                event.start(),
                offsetBeatCount));
    }
    
    EndPoints GetEndPointsAfter(
        const cmn::ROCSEvent &event,
        int lengthBeatCount,
        int offsetBeatCount)
    {
        return EndPoints(
            this->GetBeatTickIndexAfter(
                event.end(),
                offsetBeatCount),
            this->GetBeatTickIndexAfter(
                event.end(),
                offsetBeatCount + lengthBeatCount));
    }

    UInt32 GetBoundedBeatTickIndex(
        const cmn::ROCSEvent &event,
        int offsetBeatsFromStart = 2)
    {
        UInt32 boundedBeatTickIndex;
        if (offsetBeatsFromStart < 0)
        {
            int offsetBeatsFromEnd = -1 * offsetBeatsFromStart;
            boundedBeatTickIndex = 
                this->GetBeatTickIndexBefore(event.end(), offsetBeatsFromEnd);

            while (boundedBeatTickIndex < event.start())
            {
                if (offsetBeatsFromEnd < 0)
                {
                    // event.end() may be equal to event.start()
                    break;
                }
                
                offsetBeatsFromEnd--;
                boundedBeatTickIndex = 
                    this->GetBeatTickIndexAfter(event.end(), offsetBeatsFromEnd);
            }
        } else
        {
            boundedBeatTickIndex = 
                this->GetBeatTickIndexAfter(event.start(), offsetBeatsFromStart);

            while (boundedBeatTickIndex >= event.end())
            {
                // boundedBeatTickIndex is not bounded.
                if (offsetBeatsFromStart <= 0)
                {
                    // event.end() may be equal to event.start()
                    break;
                }
                
                offsetBeatsFromStart--;
                boundedBeatTickIndex = 
                    this->GetBeatTickIndexAfter(event.start(), offsetBeatsFromStart);
            }
        }

        return boundedBeatTickIndex;
    }
    
    
protected:
    UInt32 GetBeatTickIndexBefore(UInt32 tickIndex, int count = 1)
    {
        auto &barsBeats = this->GetEditor().TestGetActiveTimeline().GetBarsBeats();
        auto beatIt = std::lower_bound(
            barsBeats.begin(),
            barsBeats.end(),
            tickIndex);
        
        if (beatIt == barsBeats.begin())
        {
            throw std::out_of_range("No earlier bars beats.");
        }
        
        while (count > 0)
        {
            beatIt--;
            if (beatIt == barsBeats.begin())
            {
                throw std::out_of_range("No earlier bars beats.");
            }

            count--;
        }

        return beatIt->abs_time();
    }
    
    UInt32 GetBeatTickIndexAfter(UInt32 tickIndex, int count = 1)
    {
        auto &barsBeats = this->GetEditor().TestGetActiveTimeline().GetBarsBeats();
        auto beatIt = std::upper_bound(
            barsBeats.begin(),
            barsBeats.end(),
            tickIndex);
        
        auto endIt = barsBeats.end();
        if (beatIt == endIt)
        {
            throw std::out_of_range("No later bars beats.");
        }
        
        // upper_bound already returns 1 ahead
        assert(beatIt->abs_time() > tickIndex); 

        count -= 1;
        
        while (count > 0)
        {
            beatIt++;
             
            if (beatIt == endIt)
            {
                throw std::out_of_range("No later bars beats.");
            }

            count--;
        }
        
        return beatIt->abs_time();
    }

protected:
    std::unique_ptr<CL::TestEditor> editor_;
    std::unique_ptr<CL::ChangeLog> changeLog_;
    std::unique_ptr<rocs_midi::ShowData> showData_;
    std::string activeSongNumber_;
};

class AlignWithBeat : public EditorTest {};

TEST_F(AlignWithBeat, NonPairedExact)
{
    UInt32 alignedBeat = GetBeatIndex();
    CL::Marker testMarker(alignedBeat, "Test Marker.");
    GetEditor().TestAlignEventWithBeat_(testMarker);
    EXPECT_EQ(alignedBeat, testMarker.abs_time());
}

TEST_F(AlignWithBeat, NonPairedRoundUp)
{
    UInt32 previousBeatSpacing, alignedBeat;
    alignedBeat = GetBeatIndex(&previousBeatSpacing, NULL);
    UInt32 tickIndex = alignedBeat - (previousBeatSpacing / 4);  
    CL::Marker testMarker(tickIndex, "Test Marker.");
    GetEditor().TestAlignEventWithBeat_(testMarker);
    EXPECT_EQ(alignedBeat, GetEditor().TestGetBeatAlignedTickIndex_(tickIndex));
    EXPECT_EQ(alignedBeat, testMarker.abs_time());
}

TEST_F(AlignWithBeat, NonPairedRoundDown)
{
    UInt32 nextBeatSpacing, alignedBeat;
    alignedBeat = GetBeatIndex(&nextBeatSpacing, NULL);
    UInt32 tickIndex = alignedBeat + (nextBeatSpacing / 4);
    CL::Marker testMarker(tickIndex, "Test Marker.");
    GetEditor().TestAlignEventWithBeat_(testMarker);
    EXPECT_EQ(alignedBeat, testMarker.abs_time());
}


TEST_F(AlignWithBeat, PairedExact)
{
    auto endPoints = GetTwoBeatIndices();
    CL::Repeat testRepeat(endPoints.start, endPoints.end);
    GetEditor().TestAlignEventWithBeat_(testRepeat);
    EXPECT_EQ(endPoints.start, testRepeat.start());
    EXPECT_EQ(endPoints.end, testRepeat.end());
}

TEST_F(AlignWithBeat, PairedRoundUpStart)
{
    auto endPoints = GetTwoBeatIndices();
    UInt32 startTickIndex = endPoints.start - (endPoints.beatSpacingBeforeStart / 4);
    CL::Repeat testRepeat(startTickIndex, endPoints.end);
    GetEditor().TestAlignEventWithBeat_(testRepeat);
    EXPECT_EQ(endPoints.start, testRepeat.start());
    EXPECT_EQ(endPoints.end, testRepeat.end());
}

TEST_F(AlignWithBeat, PairedRoundDownStart)
{
    auto endPoints = GetTwoBeatIndices();
    UInt32 startTickIndex = endPoints.start + (endPoints.beatSpacingAfterStart / 4);
    CL::Repeat testRepeat(startTickIndex, endPoints.end);
    GetEditor().TestAlignEventWithBeat_(testRepeat);
    EXPECT_EQ(endPoints.start, testRepeat.start());
    EXPECT_EQ(endPoints.end, testRepeat.end());
}


TEST_F(AlignWithBeat, PairedRoundUpEnd)
{
    auto endPoints = GetTwoBeatIndices();
    UInt32 endTickIndex = endPoints.end - (endPoints.beatSpacingBeforeEnd / 4);
    CL::Repeat testRepeat(endPoints.start, endTickIndex);
    GetEditor().TestAlignEventWithBeat_(testRepeat);
    EXPECT_EQ(endPoints.start, testRepeat.start());
    EXPECT_EQ(endPoints.end, testRepeat.end());
}

TEST_F(AlignWithBeat, PairedRoundDownEnd)
{
    auto endPoints = GetTwoBeatIndices();
    UInt32 endTickIndex = endPoints.end + (endPoints.beatSpacingAfterEnd / 4);
    CL::Repeat testRepeat(endPoints.start, endTickIndex);
    GetEditor().TestAlignEventWithBeat_(testRepeat);
    EXPECT_EQ(endPoints.start, testRepeat.start());
    EXPECT_EQ(endPoints.end, testRepeat.end());
}


TEST_F(AlignWithBeat, PairedRoundUpBoth)
{
    auto endPoints = GetTwoBeatIndices();
    UInt32 startTickIndex = endPoints.start - (endPoints.beatSpacingBeforeStart / 4);
    UInt32 endTickIndex = endPoints.end - (endPoints.beatSpacingBeforeEnd / 4);
    CL::Repeat testRepeat(startTickIndex, endTickIndex);
    GetEditor().TestAlignEventWithBeat_(testRepeat);
    EXPECT_EQ(endPoints.start, testRepeat.start());
    EXPECT_EQ(endPoints.end, testRepeat.end());
}

TEST_F(AlignWithBeat, PairedRoundDownBoth)
{
    auto endPoints = GetTwoBeatIndices();
    UInt32 startTickIndex = endPoints.start + (endPoints.beatSpacingAfterStart / 4);
    UInt32 endTickIndex = endPoints.end + (endPoints.beatSpacingAfterEnd / 4);
    CL::Repeat testRepeat(startTickIndex, endTickIndex);
    GetEditor().TestAlignEventWithBeat_(testRepeat);
    EXPECT_EQ(endPoints.start, testRepeat.start());
    EXPECT_EQ(endPoints.end, testRepeat.end());
}

TEST_F(AlignWithBeat, PairedRoundUpStartRoundDownEnd)
{
    auto endPoints = GetTwoBeatIndices();
    UInt32 startTickIndex = endPoints.start - (endPoints.beatSpacingBeforeStart / 4);
    UInt32 endTickIndex = endPoints.end + (endPoints.beatSpacingAfterEnd / 4);
    CL::Repeat testRepeat(startTickIndex, endTickIndex);
    GetEditor().TestAlignEventWithBeat_(testRepeat);
    EXPECT_EQ(endPoints.start, testRepeat.start());
    EXPECT_EQ(endPoints.end, testRepeat.end());
}

TEST_F(AlignWithBeat, PairedRoundDownStartRoundUpEnd)
{
    auto endPoints = GetTwoBeatIndices();
    UInt32 startTickIndex = endPoints.start + (endPoints.beatSpacingAfterStart / 4);
    UInt32 endTickIndex = endPoints.end - (endPoints.beatSpacingBeforeEnd / 4);
    CL::Repeat testRepeat(startTickIndex, endTickIndex);
    GetEditor().TestAlignEventWithBeat_(testRepeat);
    EXPECT_EQ(endPoints.start, testRepeat.start());
    EXPECT_EQ(endPoints.end, testRepeat.end());
}

class AddSingleEvent
    : 
    public EditorTest
{
public:
    template<class EventType>
    struct AddSingleResult
    {
        AddSingleResult(
            const EventType &newEventArg,
            const cmn::ROCSEvent &validatedReferenceArg)
            :
            newEvent(newEventArg),
            validatedEvent(validatedReferenceArg),
            validatedReference(validatedReferenceArg),
            newEventStart(newEvent.start()),
            newEventEnd(newEvent.end()),
            validatedEventStart(validatedEvent.start()),
            validatedEventEnd(validatedEvent.end())
        {}

        EventType newEvent;
        EventType validatedEvent; 
        const cmn::ROCSEvent &validatedReference;

        UInt32 newEventStart;
        UInt32 newEventEnd;
        UInt32 validatedEventStart;
        UInt32 validatedEventEnd;
    };
    
    template<class EventType>
    typename std::enable_if
    <
        !std::is_base_of<CL::PairedEvent, EventType>::value, AddSingleResult<EventType>
    >::type
    AddSingle()
    {
        // We are adding a non-paired event
        EventType testEvent(this->GetBeatIndex());
        return AddSingleResult<EventType>(
            testEvent,
            this->GetEditor().AddEvent(testEvent));
    }
    
    template<class EventType>
    typename std::enable_if
    <
        std::is_base_of<CL::PairedEvent, EventType>::value, AddSingleResult<EventType>
    >::type
    AddSingle()
    {
        // We are adding a paired event
        auto endPoints = this->GetTwoBeatIndices();
        EventType testEvent(endPoints.start, endPoints.end);
        return AddSingleResult<EventType>(
            testEvent,
            this->GetEditor().AddEvent(testEvent));
    }
};

#define ADD_SINGLE_TESTER(eventType, sequenceGetter) \
TEST_F(AddSingleEvent, Add##eventType) \
{ \
    auto addResult = AddSingle<eventType>(); \
    EXPECT_EQ(addResult.newEvent, addResult.validatedEvent); \
    auto &songLogSequence = GetActiveSongLog().sequenceGetter(); \
    auto eventFromLog = std::find( \
        songLogSequence.begin(), \
        songLogSequence.end(), \
        addResult.validatedEvent); \
\
    ASSERT_NE(eventFromLog, songLogSequence.end()); \
    EXPECT_EQ(*eventFromLog, addResult.newEvent); \
    EXPECT_EQ(&(*eventFromLog), &addResult.validatedReference); \
}

ADD_SINGLE_TESTER(Marker, GetMarkers);
ADD_SINGLE_TESTER(Caesura, GetCaesuras);
ADD_SINGLE_TESTER(Vamp, GetVamps);
ADD_SINGLE_TESTER(Repeat, GetRepeats);
ADD_SINGLE_TESTER(Cut, GetCuts);
ADD_SINGLE_TESTER(Transpose, GetTranspositions);
ADD_SINGLE_TESTER(TempoScale, GetTempoScales);
ADD_SINGLE_TESTER(Click, GetClicks);
ADD_SINGLE_TESTER(Fermata, GetFermatas);

#undef ADD_SINGLE_PAIRED_TESTER

class AddOverlappingEvent
    : public AddSingleEvent
{
public:
    template<class EventT, class MasterT = EventT> 
    struct OverlapResult
    {
        OverlapResult(
            UInt32 targetStartTickIndexArg,
            UInt32 targetEndTickIndexArg,
            const EventT &newEventArg,
            const EventT &validatedEventArg,
            const MasterT &masterEventArg)
            :
            targetStartTickIndex(targetStartTickIndexArg),
            targetEndTickIndex(targetEndTickIndexArg),
            newEvent(newEventArg),
            validatedEvent(validatedEventArg),
            masterEvent(masterEventArg),
            newEventStart(newEvent.start()),
            newEventEnd(newEvent.end()),
            validatedEventStart(validatedEvent.start()),
            validatedEventEnd(validatedEvent.end()),
            masterEventStart(masterEvent.start()),
            masterEventEnd(masterEvent.end())
        {}
       
        UInt32 targetStartTickIndex;
        UInt32 targetEndTickIndex; 
        EventT newEvent;
        EventT validatedEvent;
        MasterT masterEvent;

        UInt32 newEventStart;
        UInt32 newEventEnd;
        UInt32 validatedEventStart;
        UInt32 validatedEventEnd;
        UInt32 masterEventStart;
        UInt32 masterEventEnd;
    };

public:
    template<class EventType>
    OverlapResult<EventType>
    AddOverlapStart(EventType event)
    {
        auto endPoints = this->GetOverlapStart(event, 4, 2);
        EventType newEvent(endPoints.start, endPoints.end);
        const cmn::ROCSEvent &result = this->GetEditor().AddEvent(newEvent);
        return OverlapResult<EventType>(
            endPoints.start,
            endPoints.end,
            newEvent,
            result,
            event);
    }
    

    template<class EventType>
    OverlapResult<EventType>
    AddOverlapEnd(EventType event)
    {
        auto endPoints = this->GetOverlapEnd(event, 2, 4);
        EventType newEvent(endPoints.start, endPoints.end);
        return OverlapResult<EventType>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            event);
    }
    
    template<class EventType>
    OverlapResult<EventType>
    AddOverlapExact(EventType event)
    {
        UInt32 startTickIndex = event.start();
        UInt32 endTickIndex = event.end();
        EventType newEvent(startTickIndex, endTickIndex);
        return OverlapResult<EventType>(
            startTickIndex,
            endTickIndex,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            event);
    }

    template<class EventType>
    OverlapResult<EventType>
    AddOverlapContained(EventType event)
    {
        auto endPoints = this->GetOverlapInside(event, 1, 1);
        EventType newEvent(endPoints.start, endPoints.end);
        return OverlapResult<EventType>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            event);
    }
};


/* Prototype for ADD_OVERLAP_START_TESTER
TEST_F(AddOverlappingEvent, AddOverlapStartVamp)
{
    // Get the first event.
    auto singleAddResult = AddSingle<Vamp>();

    // Add an event that overlaps.
    auto result = AddOverlapStart(singleAddResult.newEvent);
    
    // Check that the proper adjustments were made.
    // The end of the overlapping event should have been trimmed to the start
    // of the single event.
    EXPECT_EQ(result.masterEvent.start(), result.validatedEvent.end());
    
    // The start of the overlapping event should remain unchanged
    EXPECT_EQ(result.validatedEvent.start(), result.newEvent.start());

    // Let's do a sanity check to be sure the end of the overlapping event was
    // actually changed.
    EXPECT_LT(result.validatedEvent.end(), result.newEvent.end());
}
*/

#define ADD_OVERLAP_START_TESTER(eventType) \
TEST_F(AddOverlappingEvent, AddOverlapStart##eventType) \
{ \
    auto singleAddResult = AddSingle<eventType>(); \
    auto result = AddOverlapStart(singleAddResult.newEvent); \
    EXPECT_EQ(result.masterEvent.start(), result.validatedEvent.end()); \
    EXPECT_EQ(result.validatedEvent.start(), result.newEvent.start()); \
    EXPECT_LT(result.validatedEvent.end(), result.newEvent.end()); \
}

ADD_OVERLAP_START_TESTER(Vamp);
ADD_OVERLAP_START_TESTER(Repeat);
ADD_OVERLAP_START_TESTER(Cut);
ADD_OVERLAP_START_TESTER(Transpose);
ADD_OVERLAP_START_TESTER(TempoScale);
ADD_OVERLAP_START_TESTER(Click);
ADD_OVERLAP_START_TESTER(Fermata);


#define ADD_OVERLAP_END_TESTER(eventType) \
TEST_F(AddOverlappingEvent, AddOverlapEnd##eventType) \
{ \
    auto singleAddResult = AddSingle<eventType>(); \
    auto overlapAddResult = AddOverlapEnd(singleAddResult.newEvent); \
    EXPECT_EQ(singleAddResult.validatedEventEnd, overlapAddResult.validatedEventStart); \
    EXPECT_EQ(overlapAddResult.validatedEventEnd, overlapAddResult.newEventEnd); \
    EXPECT_GT(overlapAddResult.validatedEventStart, overlapAddResult.newEventStart); \
}

ADD_OVERLAP_END_TESTER(Vamp);
ADD_OVERLAP_END_TESTER(Repeat);
ADD_OVERLAP_END_TESTER(Cut);
ADD_OVERLAP_END_TESTER(Transpose);
ADD_OVERLAP_END_TESTER(TempoScale);
ADD_OVERLAP_END_TESTER(Click);
ADD_OVERLAP_END_TESTER(Fermata);


#define ADD_OVERLAP_EXACT_TESTER(eventType) \
TEST_F(AddOverlappingEvent, AddOverlapExact##eventType) \
{ \
    auto singleAddResult = AddSingle<eventType>(); \
    EXPECT_THROW(AddOverlapExact(singleAddResult.newEvent), CL::EditorEditNotAllowed); \
}

ADD_OVERLAP_EXACT_TESTER(Vamp);
ADD_OVERLAP_EXACT_TESTER(Repeat);
ADD_OVERLAP_EXACT_TESTER(Cut);
ADD_OVERLAP_EXACT_TESTER(Transpose);
ADD_OVERLAP_EXACT_TESTER(TempoScale);
ADD_OVERLAP_EXACT_TESTER(Click);
ADD_OVERLAP_EXACT_TESTER(Fermata);

#define ADD_OVERLAP_CONTAINED_TESTER(eventType) \
TEST_F(AddOverlappingEvent, AddOverlapContained##eventType) \
{ \
    auto singleAddResult = AddSingle<eventType>(); \
    EXPECT_THROW( \
        AddOverlapContained(singleAddResult.newEvent), \
        CL::EditorEditNotAllowed); \
}

ADD_OVERLAP_CONTAINED_TESTER(Vamp);
ADD_OVERLAP_CONTAINED_TESTER(Cut);
ADD_OVERLAP_CONTAINED_TESTER(Transpose);
ADD_OVERLAP_CONTAINED_TESTER(TempoScale);
ADD_OVERLAP_CONTAINED_TESTER(Click);
ADD_OVERLAP_CONTAINED_TESTER(Fermata);

TEST_F(AddOverlappingEvent, AddOverlapContainedRepeat)
{
    auto singleAddResult = AddSingle<Repeat>();
    UInt32 initialEventStart = singleAddResult.validatedEventStart;
    UInt32 initialEventEnd = singleAddResult.validatedEventEnd;
    auto addContainedResult = AddOverlapContained(singleAddResult.newEvent);
    EXPECT_GT(addContainedResult.validatedEventStart, initialEventStart);
    EXPECT_LT(addContainedResult.validatedEventEnd, initialEventEnd);
}

class TrimOverlappingEvent
    :
    public AddOverlappingEvent
{
public:
    template<class EventType>
    OverlapResult<EventType>
    TrimOverlapStart(EventType event)
    {
        // Create a new event that does not overlap.
        auto endPoints = this->GetEndPointsBefore(event, 4, 4);
        UInt32 targetTrimEndTickIndex = this->GetBoundedBeatTickIndex(event, 2);

        EventType newEvent(endPoints.start, endPoints.end);
        auto &intermediateResult = this->GetEditor().AddEvent(newEvent);
        
        // Verify that it was created without modification.
        assert(intermediateResult == newEvent);

        const cmn::ROCSEvent &result = this->GetEditor().TrimEvent(
            newEvent,
            newEvent.start(),
            targetTrimEndTickIndex);
        
        assert(newEvent.start() == result.start());
        return OverlapResult<EventType>(
            newEvent.start(),
            targetTrimEndTickIndex,
            newEvent,
            result,
            event);
    }

    template<class EventType>
    OverlapResult<EventType>
    TrimOverlapEnd(EventType event)
    {
        // Create a new event that does not overlap.
        auto endPoints = this->GetEndPointsAfter(event, 4, 4);
        UInt32 targetTrimStartTickIndex = this->GetBoundedBeatTickIndex(event, -2);

        EventType newEvent(endPoints.start, endPoints.end);
        auto &intermediateResult = this->GetEditor().AddEvent(newEvent);

        // Verify that it was created without modification.
        assert(intermediateResult == newEvent);
        
        const cmn::ROCSEvent &result = this->GetEditor().TrimEvent(
            newEvent,
            targetTrimStartTickIndex,
            newEvent.end());
        
        assert(newEvent.end() == result.end());
        
        return OverlapResult<EventType>(
            targetTrimStartTickIndex,
            newEvent.end(),
            newEvent,
            result,
            event);
    }
    
    template<class EventType>
    OverlapResult<EventType>
    TrimOverlapContainedStart(EventType event)
    {
        // Creates an event before 'event' that does not overlap.
        // Then moves the end of the new event to a point after event.
        // Expected behavior for all events except Repeat is that the new
        // end will be aligned with event.start()

        int newEventStartOffset = 8;
        int newEventEndOffset = 4;
        int trimEndOffset = 2;
        
        // Create a new event that does not overlap.
        UInt32 startTickIndex = event.start();
        UInt32 endTickIndex = event.end();
        UInt32 newEventStartTickIndex = this->GetBeatTickIndexBefore(
            startTickIndex, newEventStartOffset);
        UInt32 newEventEndTickIndex = this->GetBeatTickIndexBefore(
            startTickIndex, newEventEndOffset);
        UInt32 trimEventEndTickIndex = this->GetBeatTickIndexAfter(
            endTickIndex, trimEndOffset);

        EventType newEvent(newEventStartTickIndex, newEventEndTickIndex);
        auto &intermediateResult = this->GetEditor().AddEvent(newEvent);
        
        // Verify that it was created without modification.
        assert(intermediateResult == newEvent);

        const cmn::ROCSEvent &result = this->GetEditor().TrimEvent(
            newEvent,
            newEvent.start(),
            trimEventEndTickIndex);
        
        return OverlapResult<EventType>(
            newEvent.start(),
            trimEventEndTickIndex,
            newEvent,
            result,
            event);
    }
    
    template<class EventType>
    OverlapResult<EventType>
    TrimOverlapContainedEnd(EventType event)
    {
        // Creates an event after 'event' that does not overlap.
        // Then trims the start of the new event to a point before event.
        // Expected behavior for all events except Repeat is that the new
        // start will be aligned with event.end()

        int newEventStartOffset = 4;
        int newEventEndOffset = 8;
        int trimEndOffset = 2;
        
        // Create a new event that does not overlap.
        UInt32 startTickIndex = event.start();
        UInt32 endTickIndex = event.end();
        UInt32 newEventStartTickIndex = this->GetBeatTickIndexAfter(
            endTickIndex, newEventStartOffset);
        UInt32 newEventEndTickIndex = this->GetBeatTickIndexAfter(
            endTickIndex, newEventEndOffset);
        UInt32 trimEventStartTickIndex = this->GetBeatTickIndexBefore(
            startTickIndex, trimEndOffset);

        EventType newEvent(newEventStartTickIndex, newEventEndTickIndex);
        auto &intermediateResult = this->GetEditor().AddEvent(newEvent);
        
        // Verify that it was created without modification.
        assert(intermediateResult == newEvent);
        
        const cmn::ROCSEvent &result = this->GetEditor().TrimEvent(
            newEvent,
            trimEventStartTickIndex,
            newEvent.end());

        return OverlapResult<EventType>(
            trimEventStartTickIndex,
            newEvent.end(),
            newEvent,
            result,
            event);
    }

};


#define TRIM_OVERLAP_START_TESTER(eventType) \
TEST_F(TrimOverlappingEvent, TrimOverlapStart##eventType) \
{ \
    auto singleAddResult = AddSingle<eventType>(); \
    UInt32 initialEventStart = singleAddResult.validatedEventStart; \
    auto overlapTrimResult = TrimOverlapStart(singleAddResult.newEvent); \
    EXPECT_EQ(initialEventStart, overlapTrimResult.validatedEventEnd); \
    EXPECT_EQ(overlapTrimResult.validatedEventStart, overlapTrimResult.newEventStart); \
}

TRIM_OVERLAP_START_TESTER(Vamp);
TRIM_OVERLAP_START_TESTER(Repeat);
TRIM_OVERLAP_START_TESTER(Cut);
TRIM_OVERLAP_START_TESTER(Transpose);
TRIM_OVERLAP_START_TESTER(TempoScale);
TRIM_OVERLAP_START_TESTER(Click);
TRIM_OVERLAP_START_TESTER(Fermata);


#define TRIM_OVERLAP_END_TESTER(eventType) \
TEST_F(TrimOverlappingEvent, TrimOverlapEnd##eventType) \
{ \
    auto singleAddResult = AddSingle<eventType>(); \
    UInt32 initialEventEnd = singleAddResult.validatedEventEnd; \
    auto overlapTrimResult = TrimOverlapEnd(singleAddResult.newEvent); \
    EXPECT_EQ(initialEventEnd, overlapTrimResult.validatedEventStart); \
    EXPECT_EQ(overlapTrimResult.validatedEventEnd, overlapTrimResult.newEventEnd); \
}

TRIM_OVERLAP_END_TESTER(Vamp);
TRIM_OVERLAP_END_TESTER(Repeat);
TRIM_OVERLAP_END_TESTER(Cut);
TRIM_OVERLAP_END_TESTER(Transpose);
TRIM_OVERLAP_END_TESTER(TempoScale);
TRIM_OVERLAP_END_TESTER(Click);
TRIM_OVERLAP_END_TESTER(Fermata);

#define TRIM_OVERLAP_CONTAINED_START_TESTER(eventType) \
TEST_F(TrimOverlappingEvent, TrimOverlapContainedStart##eventType) \
{ \
    auto singleAddResult = AddSingle<eventType>(); \
    UInt32 initialEventStart = singleAddResult.validatedEventStart; \
    auto overlapTrimResult = TrimOverlapContainedStart(singleAddResult.newEvent); \
    ASSERT_EQ(overlapTrimResult.newEventStart, overlapTrimResult.validatedEventStart); \
    EXPECT_EQ(initialEventStart, overlapTrimResult.validatedEventEnd); \
}

TRIM_OVERLAP_CONTAINED_START_TESTER(Vamp);
TRIM_OVERLAP_CONTAINED_START_TESTER(Cut);
TRIM_OVERLAP_CONTAINED_START_TESTER(Transpose);
TRIM_OVERLAP_CONTAINED_START_TESTER(TempoScale);
TRIM_OVERLAP_CONTAINED_START_TESTER(Click);
TRIM_OVERLAP_CONTAINED_START_TESTER(Fermata);

TEST_F(TrimOverlappingEvent, TrimOverlapContainedStartRepeat)
{
    auto singleAddResult = AddSingle<Repeat>();
    UInt32 initialEventEnd = singleAddResult.validatedEventEnd;
    auto overlapTrimResult = TrimOverlapContainedStart(singleAddResult.newEvent);
    ASSERT_EQ(overlapTrimResult.newEventStart, overlapTrimResult.validatedEventStart);
    EXPECT_GT(overlapTrimResult.validatedEventEnd, initialEventEnd);
}

#define TRIM_OVERLAP_CONTAINED_END_TESTER(eventType) \
TEST_F(TrimOverlappingEvent, TrimOverlapContainedEnd##eventType) \
{ \
    auto singleAddResult = AddSingle<eventType>(); \
    UInt32 initialEventStart = singleAddResult.validatedEventStart; \
    auto overlapTrimResult = TrimOverlapContainedEnd(singleAddResult.newEvent); \
    EXPECT_LT(overlapTrimResult.validatedEventStart, initialEventStart); \
    EXPECT_EQ(overlapTrimResult.validatedEventEnd, initialEventStart); \
}

TRIM_OVERLAP_CONTAINED_END_TESTER(Vamp);
TRIM_OVERLAP_CONTAINED_END_TESTER(Cut);
TRIM_OVERLAP_CONTAINED_END_TESTER(Transpose);
TRIM_OVERLAP_CONTAINED_END_TESTER(TempoScale);
TRIM_OVERLAP_CONTAINED_END_TESTER(Click);
TRIM_OVERLAP_CONTAINED_END_TESTER(Fermata);

TEST_F(TrimOverlappingEvent, TrimOverlapContainedEndRepeat)
{
    auto singleAddResult = AddSingle<Repeat>();
    UInt32 eventStart = singleAddResult.validatedEventStart;
    UInt32 eventEnd = singleAddResult.validatedEventEnd;

    auto overlapTrimResult = TrimOverlapContainedEnd(singleAddResult.newEvent);
    EXPECT_LT(overlapTrimResult.validatedEventStart, eventStart);
    EXPECT_GT(overlapTrimResult.validatedEventEnd, eventEnd);
}


class MoveOverlappingEvent
    :
    public AddOverlappingEvent
{
public:
    
    template<class EventType>
    OverlapResult<EventType> 
    MoveOverlapStart(EventType event)
    {
        int newEventStartOffset = 4;
        int overlapStartOffsetTicks = 4;
        int overlapEndOffsetTicks = 2;
        
        UInt32 startTickIndex = event.start();

        // Calculate final position of moved event.
        UInt32 finalStartTickIndex = this->GetBeatTickIndexBefore(
            startTickIndex,
            overlapStartOffsetTicks);

        UInt32 finalEndTickIndex = this->GetBeatTickIndexAfter(
            startTickIndex,
            overlapEndOffsetTicks);
        
        assert(finalEndTickIndex > finalStartTickIndex);

        // Get size in ticks
        UInt32 newEventSizeTickCount = finalEndTickIndex - finalStartTickIndex;
        
        assert(startTickIndex > newEventSizeTickCount);

        // Calculate initial position for new event without overlap.
        UInt32 initialStartTickIndex = this->GetBeatTickIndexBefore(
            startTickIndex - newEventSizeTickCount,
            newEventStartOffset);
        
        UInt32 initialEndTickIndex = initialStartTickIndex + newEventSizeTickCount;

        EventType newEvent(initialStartTickIndex, initialEndTickIndex);
        auto &intermediateResult = this->GetEditor().AddEvent(newEvent);
        
        // Verify that it was created without modification.
        assert(intermediateResult == newEvent);

        const cmn::ROCSEvent &result = this->GetEditor().MoveEvent(
            newEvent,
            finalStartTickIndex);
        
        return OverlapResult<EventType>(
            finalStartTickIndex,
            finalEndTickIndex,
            newEvent,
            result,
            event);
    }

    template<class EventType>
    OverlapResult<EventType> 
    MoveOverlapEnd(EventType event)
    {
        int newEventStartOffset = 8;
        int newEventEndOffset = 4;
        int overlapOffsetTicks = 2;
        
        UInt32 startTickIndex = event.start();
        UInt32 endTickIndex = event.end();
        UInt32 newEventStartTickIndex = this->GetBeatTickIndexBefore(
            startTickIndex, newEventStartOffset);
        UInt32 newEventEndTickIndex = this->GetBeatTickIndexBefore(
            startTickIndex, newEventEndOffset);
        UInt32 moveToTickIndex = this->GetBeatTickIndexBefore(
            endTickIndex, overlapOffsetTicks);

        EventType newEvent(newEventStartTickIndex, newEventEndTickIndex);
        auto &intermediateResult = this->GetEditor().AddEvent(newEvent);

        // Verify that it was created without modification.
        assert(intermediateResult == newEvent);
        
        const cmn::ROCSEvent &result = this->GetEditor().MoveEvent(
            newEvent,
            moveToTickIndex);
        
        return OverlapResult<EventType>(
            moveToTickIndex,
            moveToTickIndex + (newEventEndTickIndex - newEventStartTickIndex), 
            newEvent,
            result,
            event);
    }

    template<class EventType>
    OverlapResult<EventType> 
    MoveOverlapInside(EventType event)
    {
        int newEventEndOffset = 4;
        int overlapOffsetTicks = 1;
        
        UInt32 startTickIndex = event.start();
        UInt32 endTickIndex = event.end();
        
        // Determine the start and end points for the final position of the
        // moved event.
        UInt32 movedFinalStart = this->GetBeatTickIndexAfter(
            startTickIndex,
            overlapOffsetTicks);

        UInt32 movedFinalEnd = this->GetBeatTickIndexBefore(
            endTickIndex,
            overlapOffsetTicks);
        
        assert(movedFinalEnd > movedFinalStart);

        // Calculate the size in ticks of the new event
        UInt32 newEventTickLength = movedFinalEnd - movedFinalStart;
        
        // Determine the initial positions for the new event
        UInt32 newEventEndTickIndex = this->GetBeatTickIndexBefore(
            startTickIndex, newEventEndOffset);
        UInt32 newEventStartTickIndex = newEventEndTickIndex - newEventTickLength;
         
        EventType newEvent(newEventStartTickIndex, newEventEndTickIndex);
        auto &intermediateResult = this->GetEditor().AddEvent(newEvent);

        // Verify that it was created without modification.
        assert(intermediateResult == newEvent);

        const cmn::ROCSEvent &result = this->GetEditor().MoveEvent(
            newEvent,
            movedFinalStart);
        
        return OverlapResult<EventType>(
            movedFinalStart,
            movedFinalEnd,
            newEvent,
            result,
            event);
    }
    
    template<class EventType>
    OverlapResult<EventType> 
    MoveOverlapOutside(EventType event)
    {
        int newEventEndOffset = 4;
        int overlapOffsetTicks = 1;
        
        UInt32 startTickIndex = event.start();
        UInt32 endTickIndex = event.end();
        
        // Determine the start and end points for the final position of the
        // moved event.
        UInt32 movedFinalStart = this->GetBeatTickIndexBefore(
            startTickIndex,
            overlapOffsetTicks);

        UInt32 movedFinalEnd = this->GetBeatTickIndexAfter(
            endTickIndex,
            overlapOffsetTicks);
        
        assert(movedFinalEnd > movedFinalStart);

        // Calculate the size in ticks of the new event
        UInt32 newEventTickLength = movedFinalEnd - movedFinalStart;
        
        // Determine the initial positions for the new event
        UInt32 newEventEndTickIndex = this->GetBeatTickIndexBefore(
            startTickIndex, newEventEndOffset);
        UInt32 newEventStartTickIndex = newEventEndTickIndex - newEventTickLength;
         
        EventType newEvent(newEventStartTickIndex, newEventEndTickIndex);
        auto &intermediateResult = this->GetEditor().AddEvent(newEvent);

        // Verify that it was created without modification.
        assert(intermediateResult == newEvent);

        const cmn::ROCSEvent &result = this->GetEditor().MoveEvent(
            newEvent,
            movedFinalStart);
        
        return OverlapResult<EventType>(
            movedFinalStart,
            movedFinalEnd,
            newEvent,
            result,
            event);
    }
};


#define MOVE_OVERLAP_START_TESTER(eventType) \
TEST_F(MoveOverlappingEvent, MoveOverlapStart##eventType) \
{ \
    auto singleAddResult = AddSingle<eventType>(); \
    auto result = MoveOverlapStart(singleAddResult.newEvent); \
    EXPECT_EQ(result.targetStartTickIndex, result.validatedEvent.start()); \
    EXPECT_EQ(result.masterEvent.start(), result.validatedEvent.end()); \
}

MOVE_OVERLAP_START_TESTER(Vamp);
MOVE_OVERLAP_START_TESTER(Repeat);
MOVE_OVERLAP_START_TESTER(Cut);
MOVE_OVERLAP_START_TESTER(TempoScale);
MOVE_OVERLAP_START_TESTER(Transpose);
MOVE_OVERLAP_START_TESTER(Click);
MOVE_OVERLAP_START_TESTER(Fermata);

#define MOVE_OVERLAP_END_TESTER(eventType) \
TEST_F(MoveOverlappingEvent, MoveOverlapEnd##eventType) \
{ \
    auto singleAddResult = AddSingle<eventType>(); \
    auto result = MoveOverlapEnd(singleAddResult.newEvent); \
    UInt32 expectedEndTickIndex = \
        result.targetStartTickIndex + (result.newEvent.end() - result.newEvent.start()); \
    EXPECT_EQ(expectedEndTickIndex, result.validatedEvent.end()); \
    EXPECT_EQ(result.masterEvent.end(), result.validatedEvent.start()); \
}


MOVE_OVERLAP_END_TESTER(Vamp);
MOVE_OVERLAP_END_TESTER(Repeat);
MOVE_OVERLAP_END_TESTER(Cut);
MOVE_OVERLAP_END_TESTER(TempoScale);
MOVE_OVERLAP_END_TESTER(Transpose);
MOVE_OVERLAP_END_TESTER(Click);
MOVE_OVERLAP_END_TESTER(Fermata);

#define MOVE_OVERLAP_INSIDE_TESTER(eventType) \
TEST_F(MoveOverlappingEvent, MoveOverlapInside##eventType) \
{ \
    auto singleAddResult = AddSingle<eventType>(); \
    EXPECT_THROW(MoveOverlapInside(singleAddResult.newEvent), CL::EditorEditNotAllowed); \
}

MOVE_OVERLAP_INSIDE_TESTER(Vamp);
MOVE_OVERLAP_INSIDE_TESTER(Cut);
MOVE_OVERLAP_INSIDE_TESTER(TempoScale);
MOVE_OVERLAP_INSIDE_TESTER(Transpose);
MOVE_OVERLAP_INSIDE_TESTER(Click);
MOVE_OVERLAP_INSIDE_TESTER(Fermata);

TEST_F(MoveOverlappingEvent, MoveOverlapInsideRepeat)
{
    auto singleAddResult = AddSingle<Repeat>();
    auto result = MoveOverlapInside(singleAddResult.newEvent);
    UInt32 expectedEndTickIndex =
        result.targetStartTickIndex + (result.newEvent.end() - result.newEvent.start());
    EXPECT_EQ(result.targetStartTickIndex, result.validatedEvent.start());
    EXPECT_EQ(expectedEndTickIndex, result.validatedEvent.end());
}

#define MOVE_OVERLAP_OUTSIDE_TESTER(eventType) \
TEST_F(MoveOverlappingEvent, MoveOverlapOutside##eventType) \
{ \
    auto singleAddResult = AddSingle<eventType>(); \
    auto result = MoveOverlapOutside(singleAddResult.newEvent); \
    EXPECT_EQ(result.targetStartTickIndex, result.validatedEvent.start()); \
    EXPECT_EQ(result.masterEvent.start(), result.validatedEvent.end()); \
}

MOVE_OVERLAP_OUTSIDE_TESTER(Vamp);
MOVE_OVERLAP_OUTSIDE_TESTER(Cut);
MOVE_OVERLAP_OUTSIDE_TESTER(TempoScale);
MOVE_OVERLAP_OUTSIDE_TESTER(Transpose);
MOVE_OVERLAP_OUTSIDE_TESTER(Click);
MOVE_OVERLAP_OUTSIDE_TESTER(Fermata);

TEST_F(MoveOverlappingEvent, MoveOverlapOutsideRepeat)
{
    auto singleAddResult = AddSingle<Repeat>();
    auto result = MoveOverlapOutside(singleAddResult.newEvent);
    UInt32 expectedEndTickIndex =
        result.targetStartTickIndex + (result.newEvent.end() - result.newEvent.start());
    EXPECT_EQ(result.targetStartTickIndex, result.validatedEvent.start());
    EXPECT_EQ(expectedEndTickIndex, result.validatedEvent.end());
}



class CutTester
    :
    public AddOverlappingEvent
{
public:
    
    struct MultiCutResult
    {
        std::list<cmn::ROCSEvtPtrT> originalEvents;
        CL::EditorHistory::HistoryActionT editedEvents;
        CL::Cut cutEvent;
    };

public:
    
    template<class EventType>
    MultiCutResult
    AddCutOverManyEvents()
    {
        MultiCutResult result; 
        
        // Create 5 non-overlapping events of type EventType
        cmn::ROCSEvtPtrT event1(new EventType(
            this->AddSingle<EventType>().validatedReference));
        
        auto endPoints2 = this->GetEndPointsAfter(*event1, 8, 1);
        cmn::ROCSEvtPtrT event2(new EventType(endPoints2.start, endPoints2.end));
        
        auto endPoints3 = this->GetEndPointsAfter(*event2, 8, 1);
        cmn::ROCSEvtPtrT event3(new EventType(endPoints3.start, endPoints3.end));
        
        auto endPoints4 = this->GetEndPointsAfter(*event3, 8, 1);
        cmn::ROCSEvtPtrT event4(new EventType(endPoints4.start, endPoints4.end));
        
        auto endPoints5 = this->GetEndPointsAfter(*event4, 8, 1);
        cmn::ROCSEvtPtrT event5(new EventType(endPoints5.start, endPoints5.end));
        
        // event1 was already added.
        // Add the others here.
        result.originalEvents.push_back(event1);
        std::list<cmn::ROCSEvtPtrT> desiredEvents({event2, event3, event4, event5});
        for (auto it : desiredEvents)
        {
            result.originalEvents.push_back(
                cmn::ROCSEvtPtrT(
                    new EventType(this->GetEditor().AddEvent(*it))));
        }

        // Start the cut halfway through event2, leaving event1 untouched.
        UInt32 cutStart = this->GetBeatTickIndexAfter(event2->start(), 4);
        
        // End the cut halfway through event4, leaving event5 untouched.
        UInt32 cutEnd = this->GetBeatTickIndexAfter(event4->start(), 4);

        result.cutEvent = CL::Cut(cutStart, cutEnd);
        this->GetEditor().AddEvent(result.cutEvent);
        
        result.editedEvents = this->GetEditor().GetEditedEvents();
        
        return result;
    }

    MultiCutResult
    AddCutOverlapStartManyDifferentEventTypes()
    {
        MultiCutResult result;
        
        cmn::ROCSEvtPtrT transposeEvent(new CL::Transpose(
            this->AddSingle<CL::Transpose>().validatedReference));

        cmn::ROCSEvtPtrT tempoScaleEvent(new CL::TempoScale(
            transposeEvent->start(),
            this->GetBeatTickIndexAfter(transposeEvent->end(), 1)));

        cmn::ROCSEvtPtrT clickEvent(new CL::Click(
            transposeEvent->start(),
            this->GetBeatTickIndexAfter(transposeEvent->end(), 2)));

        cmn::ROCSEvtPtrT fermataEvent(new CL::Fermata(
            transposeEvent->start(),
            this->GetBeatTickIndexAfter(transposeEvent->end(), 3)));

        cmn::ROCSEvtPtrT repeatEvent(new CL::Repeat(
            transposeEvent->start(),
            this->GetBeatTickIndexAfter(transposeEvent->end(), 4)));
        
        result.originalEvents.push_back(transposeEvent);
        
        result.originalEvents.push_back(
            cmn::ROCSEvtPtrT(
                new CL::TempoScale(this->GetEditor().AddEvent(*tempoScaleEvent))));

        result.originalEvents.push_back(
            cmn::ROCSEvtPtrT(
                new CL::Click(this->GetEditor().AddEvent(*clickEvent))));

        result.originalEvents.push_back(
            cmn::ROCSEvtPtrT(
                new CL::Fermata(this->GetEditor().AddEvent(*fermataEvent))));

        result.originalEvents.push_back(
            cmn::ROCSEvtPtrT(
                new CL::Repeat(this->GetEditor().AddEvent(*repeatEvent))));

        // Start the cut halfway through event2, leaving event1 untouched.
        UInt32 cutStart = this->GetBeatTickIndexBefore(transposeEvent->start(), 4);
        
        // End the cut halfway through event4, leaving event5 untouched.
        UInt32 cutEnd = this->GetBeatTickIndexAfter(transposeEvent->start(), 4);

        result.cutEvent = CL::Cut(cutStart, cutEnd);
        this->GetEditor().AddEvent(result.cutEvent);
        
        result.editedEvents = this->GetEditor().GetEditedEvents();
        
        return result;
    }
   
    // TODO: Test Undo/Redo

    template<class EventType>
    OverlapResult<EventType, CL::Cut>
    AddEventOverlappingCutStart()
    {
        CL::Cut masterEvent(this->AddSingle<CL::Cut>().validatedReference);
        auto endPoints = this->GetOverlapStart(masterEvent, 4, 2);
        EventType newEvent(endPoints.start, endPoints.end);
        return OverlapResult<EventType, CL::Cut>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            masterEvent);
    }

    template<class EventType>
    OverlapResult<EventType, CL::Cut>
    AddEventOverlappingCutEnd()
    {
        CL::Cut masterEvent(this->AddSingle<CL::Cut>().validatedReference);
        auto endPoints = this->GetOverlapEnd(masterEvent, 2, 4);
        EventType newEvent(endPoints.start, endPoints.end);
        return OverlapResult<EventType, CL::Cut>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            masterEvent);
    }

    template<class EventType>
    OverlapResult<EventType, CL::Cut>
    AddEventAdjacentToCutStart()
    {
        CL::Cut masterEvent(this->AddSingle<CL::Cut>().validatedReference);
        auto endPoints = this->GetAdjacentToStart(masterEvent);
        EventType newEvent(endPoints.start, endPoints.end);
        return OverlapResult<EventType, CL::Cut>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            masterEvent);
        
    }

    template<class EventType>
    OverlapResult<EventType, CL::Cut>
    AddEventAdjacentToCutEnd()
    {
        CL::Cut masterEvent(this->AddSingle<CL::Cut>().validatedReference);
        auto endPoints = this->GetAdjacentToEnd(masterEvent);
        EventType newEvent(endPoints.start, endPoints.end);
        return OverlapResult<EventType, CL::Cut>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            masterEvent);

    }

    template<class EventType>
    OverlapResult<EventType, CL::Cut>
    AddEventInsideCut()
    {
        CL::Cut masterEvent(this->AddSingle<CL::Cut>().validatedReference);
        auto endPoints = this->GetOverlapInside(masterEvent);
        EventType newEvent(endPoints.start, endPoints.end);
        return OverlapResult<EventType, CL::Cut>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            masterEvent);
    }

    template<class EventType>
    OverlapResult<EventType, CL::Cut>
    AddEventOverlappingOutsideCut()
    {
        CL::Cut masterEvent(this->AddSingle<CL::Cut>().validatedReference);
        auto endPoints = this->GetOverlapOutside(masterEvent, 4, 2);
        EventType newEvent(endPoints.start, endPoints.end);
        return OverlapResult<EventType, CL::Cut>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            masterEvent);
    }
    
    template<class EventType>
    OverlapResult<CL::Cut, EventType>
    AddCutOverlappingEventStart()
    {
        EventType masterEvent(this->AddSingle<EventType>().validatedReference);
        auto endPoints = this->GetOverlapStart(masterEvent, 4, 2);
        CL::Cut newEvent(endPoints.start, endPoints.end);
        return OverlapResult<CL::Cut, EventType>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            masterEvent);
    }
    
    template<class EventType>
    OverlapResult<CL::Cut, EventType>
    AddCutOverlappingEventEnd()
    {
        EventType masterEvent(this->AddSingle<EventType>().validatedReference);
        auto endPoints = this->GetOverlapEnd(masterEvent, 2, 4);
        CL::Cut newEvent(endPoints.start, endPoints.end);
        return OverlapResult<CL::Cut, EventType>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            masterEvent);

    }

    template<class EventType>
    OverlapResult<CL::Cut, EventType>
    AddCutAdjacentToEventStart()
    {
        EventType masterEvent(this->AddSingle<EventType>().validatedReference);
        auto endPoints = this->GetAdjacentToStart(masterEvent);
        CL::Cut newEvent(endPoints.start, endPoints.end);
        return OverlapResult<CL::Cut, EventType>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            masterEvent);
    }
    
    template<class EventType>
    OverlapResult<CL::Cut, EventType>
    AddCutAdjacentToEventEnd()
    {
        EventType masterEvent(this->AddSingle<EventType>().validatedReference);
        auto endPoints = this->GetAdjacentToEnd(masterEvent);
        CL::Cut newEvent(endPoints.start, endPoints.end);
        return OverlapResult<CL::Cut, EventType>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            masterEvent);
    }

    template<class EventType>
    OverlapResult<CL::Cut, EventType>
    AddCutInsideEvent()
    {
        EventType masterEvent(this->AddSingle<EventType>().validatedReference);
        auto endPoints = this->GetOverlapInside(masterEvent);
        CL::Cut newEvent(endPoints.start, endPoints.end);
        return OverlapResult<CL::Cut, EventType>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            masterEvent);
    }

    template<class EventType>
    OverlapResult<CL::Cut, EventType>
    AddCutOverlappingOutsideEvent()
    {
        EventType masterEvent(this->AddSingle<EventType>().validatedReference);
        auto endPoints = this->GetOverlapOutside(masterEvent, 4, 2);
        CL::Cut newEvent(endPoints.start, endPoints.end);
        return OverlapResult<CL::Cut, EventType>(
            endPoints.start,
            endPoints.end,
            newEvent,
            this->GetEditor().AddEvent(newEvent),
            masterEvent);
    }
};

#define CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_START(event_type) \
TEST_F(CutTester, AddEventOverlappingCutStart##event_type) \
{ \
    auto result = AddEventOverlappingCutStart<event_type>(); \
    EXPECT_EQ(result.masterEventStart, result.validatedEventEnd); \
    EXPECT_EQ(result.newEventStart, result.validatedEventStart); \
}

#define CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_END(event_type) \
TEST_F(CutTester, AddEventOverlappingCutEnd##event_type) \
{ \
    auto result = AddEventOverlappingCutEnd<event_type>(); \
    EXPECT_EQ(result.masterEventEnd, result.validatedEventStart); \
    EXPECT_EQ(result.newEventEnd, result.validatedEventEnd); \
}

#define CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_START(event_type) \
TEST_F(CutTester, AddEventAdjacentToCutStart##event_type) \
{ \
    auto result = AddEventAdjacentToCutStart<event_type>(); \
    EXPECT_EQ(result.newEventStart, result.validatedEventStart); \
    EXPECT_EQ(result.newEventEnd, result.validatedEventEnd); \
}

#define CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_END(event_type) \
TEST_F(CutTester, AddEventAdjacentToCutEnd##event_type) \
{ \
    auto result = AddEventAdjacentToCutEnd<event_type>(); \
    EXPECT_EQ(result.newEventStart, result.validatedEventStart); \
    EXPECT_EQ(result.newEventEnd, result.validatedEventEnd); \
}

#define CUT_TESTER_ADD_EVENT_INSIDE_CUT(event_type) \
TEST_F(CutTester, AddEventInsideCut##event_type) \
{ \
    EXPECT_THROW(AddEventInsideCut<event_type>(), CL::EditorEditNotAllowed); \
}

#define CUT_TESTER_ADD_EVENT_OVERLAPPING_OUTSIDE_CUT(event_type) \
TEST_F(CutTester, AddEventOverlappingOutsideCut##event_type) \
{ \
    auto result = AddEventOverlappingOutsideCut<event_type>(); \
    EXPECT_EQ(result.masterEventStart, result.validatedEventEnd); \
    EXPECT_EQ(result.newEventStart, result.validatedEventStart); \
}

// For AddCut tests, result.masterEvent is the paired event that the new cut
// may have affected.

// We expect the masterEvent to be trimmed at the start to make room for the
// cut.
#define CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_START(event_type) \
TEST_F(CutTester, AddCutOverlappingEventStart##event_type) \
{ \
    auto result = AddCutOverlappingEventStart<event_type>(); \
    EXPECT_EQ(result.newEventStart, result.validatedEventStart); \
    EXPECT_EQ(result.newEventEnd, result.validatedEventEnd); \
    EXPECT_THROW(GetEditor().TestFindEvent_(result.masterEvent), EditorEventNotFound); \
    event_type editedMasterEvent(result.validatedEventEnd, result.masterEventEnd); \
    EXPECT_NO_THROW(GetEditor().TestFindEvent_(editedMasterEvent)); \
}

#define CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_END(event_type) \
TEST_F(CutTester, AddCutOverlappingEventEnd##event_type) \
{ \
    auto result = AddCutOverlappingEventEnd<event_type>(); \
    EXPECT_EQ(result.newEventStart, result.validatedEventStart); \
    EXPECT_EQ(result.newEventEnd, result.validatedEventEnd); \
    EXPECT_THROW(GetEditor().TestFindEvent_(result.masterEvent), EditorEventNotFound); \
    event_type editedMasterEvent(result.masterEventStart, result.validatedEventStart); \
    EXPECT_NO_THROW(GetEditor().TestFindEvent_(editedMasterEvent)); \
}

#define CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_START(event_type) \
TEST_F(CutTester, AddCutAdjacentToEventStart##event_type) \
{ \
    auto result = AddCutAdjacentToEventStart<event_type>(); \
    EXPECT_EQ(result.newEventStart, result.validatedEventStart); \
    EXPECT_EQ(result.newEventEnd, result.validatedEventEnd); \
    EXPECT_NO_THROW(GetEditor().TestFindEvent_(result.masterEvent)); \
}

#define CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_END(event_type) \
TEST_F(CutTester, AddCutAdjacentToEventEnd##event_type) \
{ \
    auto result = AddCutAdjacentToEventEnd<event_type>(); \
    EXPECT_EQ(result.newEventStart, result.validatedEventStart); \
    EXPECT_EQ(result.newEventEnd, result.validatedEventEnd); \
    EXPECT_NO_THROW(GetEditor().TestFindEvent_(result.masterEvent)); \
}

#define CUT_TESTER_ADD_CUT_INSIDE_EVENT(event_type) \
TEST_F(CutTester, AddCutInsideEvent##event_type) \
{ \
    auto result = AddCutInsideEvent<event_type>(); \
    EXPECT_EQ(result.newEventStart, result.validatedEventStart); \
    EXPECT_EQ(result.newEventEnd, result.validatedEventEnd); \
    EXPECT_NO_THROW(GetEditor().TestFindEvent_(result.masterEvent)); \
}

#define CUT_TESTER_ADD_CUT_OVERLAPPING_OUTSIDE_EVENT(event_type) \
TEST_F(CutTester, AddCutOverlappingOutsideEvent##event_type) \
{ \
    auto result = AddCutOverlappingOutsideEvent<event_type>(); \
    EXPECT_EQ(result.newEventStart, result.validatedEventStart); \
    EXPECT_EQ(result.newEventEnd, result.validatedEventEnd); \
    EXPECT_THROW(GetEditor().TestFindEvent_(result.masterEvent), EditorEventNotFound); \
}

CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_START(Vamp);
CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_START(Repeat);
CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_START(Transpose);
CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_START(TempoScale);
CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_START(Click);
CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_START(Fermata);

CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_END(Vamp);
CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_END(Repeat);
CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_END(Transpose);
CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_END(TempoScale);
CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_END(Click);
CUT_TESTER_ADD_EVENT_OVERLAPPING_CUT_END(Fermata);

CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_START(Vamp);
CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_START(Repeat);
CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_START(Transpose);
CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_START(TempoScale);
CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_START(Click);
CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_START(Fermata);

CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_END(Vamp);
CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_END(Repeat);
CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_END(Transpose);
CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_END(TempoScale);
CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_END(Click);
CUT_TESTER_ADD_EVENT_ADJACENT_TO_CUT_END(Fermata);

CUT_TESTER_ADD_EVENT_INSIDE_CUT(Vamp);
CUT_TESTER_ADD_EVENT_INSIDE_CUT(Repeat);
CUT_TESTER_ADD_EVENT_INSIDE_CUT(Transpose);
CUT_TESTER_ADD_EVENT_INSIDE_CUT(TempoScale);
CUT_TESTER_ADD_EVENT_INSIDE_CUT(Click);
CUT_TESTER_ADD_EVENT_INSIDE_CUT(Fermata);

CUT_TESTER_ADD_EVENT_OVERLAPPING_OUTSIDE_CUT(Vamp);
CUT_TESTER_ADD_EVENT_OVERLAPPING_OUTSIDE_CUT(Repeat);
CUT_TESTER_ADD_EVENT_OVERLAPPING_OUTSIDE_CUT(Transpose);
CUT_TESTER_ADD_EVENT_OVERLAPPING_OUTSIDE_CUT(TempoScale);
CUT_TESTER_ADD_EVENT_OVERLAPPING_OUTSIDE_CUT(Click);
CUT_TESTER_ADD_EVENT_OVERLAPPING_OUTSIDE_CUT(Fermata);

CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_START(Vamp);      
CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_START(Repeat);    
CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_START(Transpose); 
CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_START(TempoScale);
CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_START(Click);     
CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_START(Fermata);   

CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_END(Vamp);      
CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_END(Repeat);    
CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_END(Transpose); 
CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_END(TempoScale);
CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_END(Click);     
CUT_TESTER_ADD_CUT_OVERLAPPING_EVENT_END(Fermata);   

CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_START(Vamp);      
CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_START(Repeat);    
CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_START(Transpose); 
CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_START(TempoScale);
CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_START(Click);     
CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_START(Fermata);   

CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_END(Vamp);      
CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_END(Repeat);    
CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_END(Transpose); 
CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_END(TempoScale);
CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_END(Click);     
CUT_TEST_ADD_CUT_ADJACENT_TO_EVENT_END(Fermata);   

CUT_TESTER_ADD_CUT_INSIDE_EVENT(Vamp);      
CUT_TESTER_ADD_CUT_INSIDE_EVENT(Repeat);    
CUT_TESTER_ADD_CUT_INSIDE_EVENT(Transpose); 
CUT_TESTER_ADD_CUT_INSIDE_EVENT(TempoScale);
CUT_TESTER_ADD_CUT_INSIDE_EVENT(Click);     
CUT_TESTER_ADD_CUT_INSIDE_EVENT(Fermata);   

CUT_TESTER_ADD_CUT_OVERLAPPING_OUTSIDE_EVENT(Vamp);
CUT_TESTER_ADD_CUT_OVERLAPPING_OUTSIDE_EVENT(Repeat);
CUT_TESTER_ADD_CUT_OVERLAPPING_OUTSIDE_EVENT(Transpose);
CUT_TESTER_ADD_CUT_OVERLAPPING_OUTSIDE_EVENT(TempoScale);
CUT_TESTER_ADD_CUT_OVERLAPPING_OUTSIDE_EVENT(Click);
CUT_TESTER_ADD_CUT_OVERLAPPING_OUTSIDE_EVENT(Fermata);
CUT_TESTER_ADD_CUT_OVERLAPPING_OUTSIDE_EVENT(Marker);
CUT_TESTER_ADD_CUT_OVERLAPPING_OUTSIDE_EVENT(Caesura);

TEST_F(CutTester, AddCutOverlapStartManyDifferentEventTypes)
{
    auto result = AddCutOverlapStartManyDifferentEventTypes();
    
    std::list<UInt8> editedCodes;
    for (auto it : result.originalEvents)
    {
        EXPECT_THROW(GetEditor().TestFindEvent_(*it), EditorEventNotFound);
        editedCodes.push_back(it->code());
    }

    for (auto it : result.editedEvents)
    {
        if (it.OriginalEvent() != nullptr && it.EditedEvent() != nullptr)
        {
            ASSERT_NE(it.OriginalEvent()->code(), codes::cut);
            EXPECT_EQ(result.cutEvent.end(), it.EditedEvent()->start());
            EXPECT_EQ(it.OriginalEvent()->end(), it.EditedEvent()->end());
            EXPECT_NO_THROW(GetEditor().TestFindEvent_(*it.EditedEvent()));
            editedCodes.remove(it.OriginalEvent()->code());
        }
    }

    EXPECT_EQ(static_cast<const unsigned long>(0), editedCodes.size());
};

#define CUT_TESTER_ADD_CUT_OVER_MANY_EVENTS(event_type) \
TEST_F(CutTester, AddCutOverManyEvents##event_type) \
{ \
    auto result = AddCutOverManyEvents<event_type>(); \
    for (auto originalIt : result.originalEvents) \
    { \
        if ((*originalIt == *result.originalEvents.back()) \
            || (*originalIt == *result.originalEvents.front())) \
        { \
            EXPECT_NO_THROW(GetEditor().TestFindEvent_(*originalIt)); \
        } else \
        { \
            EXPECT_THROW( \
                GetEditor().TestFindEvent_(*originalIt), \
                EditorEventNotFound); \
        }\
    } \
    \
    for (auto it: result.editedEvents) \
    { \
        if (it.OriginalEvent() != nullptr && it.EditedEvent() != nullptr) \
        { \
            ASSERT_NE(it.OriginalEvent()->code(), codes::cut); \
            if (it.OriginalEvent()->start() < result.cutEvent.start()) \
            { \
                if (it.OriginalEvent()->end() > result.cutEvent.start()) \
                { \
                    EXPECT_EQ(it.OriginalEvent()->start(), it.EditedEvent()->start()); \
                    EXPECT_EQ(result.cutEvent.start(), it.EditedEvent()->end()); \
                } \
            } else if (it.OriginalEvent()->end() > result.cutEvent.end()) \
            { \
                EXPECT_EQ(it.OriginalEvent()->end(), it.EditedEvent()->end()); \
                EXPECT_EQ(result.cutEvent.end(), it.EditedEvent()->start()); \
            } \
        } else \
        { \
            if (it.OriginalEvent() != nullptr) \
            { \
                EXPECT_NE(it.OriginalEvent()->code(), codes::cut); \
                EXPECT_GE(it.OriginalEvent()->start(), result.cutEvent.start()); \
                EXPECT_LE(it.OriginalEvent()->end(), result.cutEvent.end()); \
            } else \
            { \
                ASSERT_NE(it.EditedEvent(), nullptr); \
                EXPECT_EQ(codes::cut, it.EditedEvent()->code()); \
                EXPECT_EQ(result.cutEvent, *it.EditedEvent()); \
            } \
        } \
    } \
}

CUT_TESTER_ADD_CUT_OVER_MANY_EVENTS(Vamp);      
CUT_TESTER_ADD_CUT_OVER_MANY_EVENTS(Repeat);    
CUT_TESTER_ADD_CUT_OVER_MANY_EVENTS(Transpose); 
CUT_TESTER_ADD_CUT_OVER_MANY_EVENTS(TempoScale);
CUT_TESTER_ADD_CUT_OVER_MANY_EVENTS(Click);     
CUT_TESTER_ADD_CUT_OVER_MANY_EVENTS(Fermata);   

TEST_F(CutTester, UndoAddCutOverManyEventsVamp)
{
    auto result = AddCutOverManyEvents<Vamp>();
    ASSERT_TRUE(GetEditor().CanUndo());
    auto undoAction = GetEditor().Undo();
    for (auto originalIt : result.originalEvents)
    {
        EXPECT_NO_THROW(GetEditor().TestFindEvent_(*originalIt));
    }
    EXPECT_THROW(GetEditor().TestFindEvent_(result.cutEvent), EditorEventNotFound);
    ASSERT_TRUE(GetEditor().CanRedo());
    auto redoAction = GetEditor().Redo();
    EXPECT_NO_THROW(GetEditor().TestFindEvent_(result.cutEvent));
    for (auto editedIt : result.editedEvents)
    {
        if (editedIt.EditedEvent() != nullptr)
        {
            EXPECT_NO_THROW(GetEditor().TestFindEvent_(*editedIt.EditedEvent()));
        }
    }
};

TEST_F(CutTester, UndoAddCutOverManyEventsRepeat)
{
    auto result = AddCutOverManyEvents<Repeat>();
    ASSERT_TRUE(GetEditor().CanUndo());
    auto undoAction = GetEditor().Undo();
    for (auto originalIt : result.originalEvents)
    {
        EXPECT_NO_THROW(GetEditor().TestFindEvent_(*originalIt));
    }
    EXPECT_THROW(GetEditor().TestFindEvent_(result.cutEvent), EditorEventNotFound);
    ASSERT_TRUE(GetEditor().CanRedo());
    auto redoAction = GetEditor().Redo();
    EXPECT_NO_THROW(GetEditor().TestFindEvent_(result.cutEvent));
    for (auto editedIt : result.editedEvents)
    {
        if (editedIt.EditedEvent() != nullptr)
        {
            EXPECT_NO_THROW(GetEditor().TestFindEvent_(*editedIt.EditedEvent()));
        }
    }
};

TEST_F(CutTester, UndoAddCutOverManyEventsTranspose)
{
    auto result = AddCutOverManyEvents<Transpose>();
    ASSERT_TRUE(GetEditor().CanUndo());
    auto undoAction = GetEditor().Undo();
    for (auto originalIt : result.originalEvents)
    {
        EXPECT_NO_THROW(GetEditor().TestFindEvent_(*originalIt));
    }
    EXPECT_THROW(GetEditor().TestFindEvent_(result.cutEvent), EditorEventNotFound);
    ASSERT_TRUE(GetEditor().CanRedo());
    auto redoAction = GetEditor().Redo();
    EXPECT_NO_THROW(GetEditor().TestFindEvent_(result.cutEvent));
    for (auto editedIt : result.editedEvents)
    {
        if (editedIt.EditedEvent() != nullptr)
        {
            EXPECT_NO_THROW(GetEditor().TestFindEvent_(*editedIt.EditedEvent()));
        }
    }
};

TEST_F(CutTester, UndoAddCutOverManyEventsTempoScale)
{
    auto result = AddCutOverManyEvents<TempoScale>();
    ASSERT_TRUE(GetEditor().CanUndo());
    auto undoAction = GetEditor().Undo();
    for (auto originalIt : result.originalEvents)
    {
        EXPECT_NO_THROW(GetEditor().TestFindEvent_(*originalIt));
    }
    EXPECT_THROW(GetEditor().TestFindEvent_(result.cutEvent), EditorEventNotFound);
    ASSERT_TRUE(GetEditor().CanRedo());
    auto redoAction = GetEditor().Redo();
    EXPECT_NO_THROW(GetEditor().TestFindEvent_(result.cutEvent));
    for (auto editedIt : result.editedEvents)
    {
        if (editedIt.EditedEvent() != nullptr)
        {
            EXPECT_NO_THROW(GetEditor().TestFindEvent_(*editedIt.EditedEvent()));
        }
    }
};

TEST_F(CutTester, UndoAddCutOverManyEventsClick)
{
    auto result = AddCutOverManyEvents<Click>();
    ASSERT_TRUE(GetEditor().CanUndo());
    auto undoAction = GetEditor().Undo();
    for (auto originalIt : result.originalEvents)
    {
        EXPECT_NO_THROW(GetEditor().TestFindEvent_(*originalIt));
    }
    EXPECT_THROW(GetEditor().TestFindEvent_(result.cutEvent), EditorEventNotFound);
    ASSERT_TRUE(GetEditor().CanRedo());
    auto redoAction = GetEditor().Redo();
    EXPECT_NO_THROW(GetEditor().TestFindEvent_(result.cutEvent));
    for (auto editedIt : result.editedEvents)
    {
        if (editedIt.EditedEvent() != nullptr)
        {
            EXPECT_NO_THROW(GetEditor().TestFindEvent_(*editedIt.EditedEvent()));
        }
    }
};

TEST_F(CutTester, UndoAddCutOverManyEventsFermata)
{
    auto result = AddCutOverManyEvents<Fermata>();
    ASSERT_TRUE(GetEditor().CanUndo());
    auto undoAction = GetEditor().Undo();
    for (auto originalIt : result.originalEvents)
    {
        EXPECT_NO_THROW(GetEditor().TestFindEvent_(*originalIt));
    }
    EXPECT_THROW(GetEditor().TestFindEvent_(result.cutEvent), EditorEventNotFound);
    ASSERT_TRUE(GetEditor().CanRedo());
    auto redoAction = GetEditor().Redo();
    EXPECT_NO_THROW(GetEditor().TestFindEvent_(result.cutEvent));
    for (auto editedIt : result.editedEvents)
    {
        if (editedIt.EditedEvent() != nullptr)
        {
            EXPECT_NO_THROW(GetEditor().TestFindEvent_(*editedIt.EditedEvent()));
        }
    }
};



