#include "core/changelog/editor.h"
#include "core/changelog/ensure_non_overlap.h"
#include "core/changelog/copy_rocs_event.h"
#include "core/changelog/editor_templates.h"


namespace CL
{

Editor::Editor(ChangeLog &changeLog, const rocs_midi::ShowData &showData)
    :
    changeLog_(changeLog),
    showData_(showData),
    activeSongNumber_(*changeLog.song_nums_begin())
{
    std::lock_guard<std::recursive_mutex> lock(this->GetActiveSongLog().GetLock());
    this->CreateBeatTickIndices_();
}

void Editor::SelectSongLogForEditing(const std::string &songNumber)
{
    if (this->activeSongNumber_ == songNumber)
    {
        // This song is already the active song.
        return;
    }

    this->activeSongNumber_ = songNumber;

    std::lock_guard<std::recursive_mutex> lock(this->GetActiveSongLog().GetLock());
    this->CreateBeatTickIndices_();
}

const cmn::ROCSEvent & Editor::AddEvent(const cmn::ROCSEvent &evt)
{
    std::lock_guard<std::recursive_mutex> lock(this->GetActiveSongLog().GetLock());
    cmn::ROCSEvtPtrT newEvent = CopyRocsEvent(evt);
    auto &activeHistory = this->GetActiveHistory();
    activeHistory.BeginAction();
    this->ValidateEdit_(*newEvent);
    activeHistory.AddNewEventToCurrentAction(*newEvent);
    activeHistory.EndAction();
    cmn::ROCSEvent &addedEvent = this->AddEvent_(*newEvent);
    this->FinishEdit_(evt);
    return addedEvent;
}

void Editor::DeleteEvent_(const cmn::ROCSEvent &eventToDelete)
{
    switch (eventToDelete.code())
    {
        case codes::vamp:
            ::CL::DeleteEvent(
                this->GetActiveSongLog().GetVamps(), CL::Vamp(eventToDelete));
            break; 
        case codes::repeat:
            ::CL::DeleteEvent(
                this->GetActiveSongLog().GetRepeats(),
                CL::Repeat(eventToDelete));
            break;
        case codes::cut:
            ::CL::DeleteEvent(
                this->GetActiveSongLog().GetCuts(),
                CL::Cut(eventToDelete));
            break;
        case codes::transpose:
            ::CL::DeleteEvent(
                this->GetActiveSongLog().GetTranspositions(),
                CL::Transpose(eventToDelete));
            break;
        case codes::tempo_scale:
            ::CL::DeleteEvent(
                this->GetActiveSongLog().GetTempoScales(),
                CL::TempoScale(eventToDelete));
            break;
        case codes::marker:
            ::CL::DeleteEvent(
                this->GetActiveSongLog().GetMarkers(),
                CL::Marker(eventToDelete));
            break;
        case codes::click:
            ::CL::DeleteEvent(
                this->GetActiveSongLog().GetClicks(),
                CL::Click(eventToDelete));
            break;
        case codes::caesura:
            ::CL::DeleteEvent(
                this->GetActiveSongLog().GetCaesuras(),
                CL::Caesura(eventToDelete));
            break;
        case codes::fermata:
            ::CL::DeleteEvent(
                this->GetActiveSongLog().GetFermatas(),
                CL::Fermata(eventToDelete));
            break;
        default:
            throw std::logic_error(ex::format(
                "Unsupported event code: %i",
                eventToDelete.code()));
    };
}

void Editor::DeleteEvent(const cmn::ROCSEvent &eventToDelete)
{
    std::lock_guard<std::recursive_mutex> lock(this->GetActiveSongLog().GetLock());
    this->DeleteEvent_(eventToDelete);
    auto &activeHistory = this->GetActiveHistory();
    activeHistory.BeginAction();
    activeHistory.AddDeletedEventToCurrentAction(eventToDelete);
    activeHistory.EndAction();
    this->FinishEdit_(eventToDelete);
}

const cmn::ROCSEvent & Editor::EditEventIntValue(
    const cmn::ROCSEvent &evt,
    SInt32 newValue)
{
    std::lock_guard<std::recursive_mutex> lock(this->GetActiveSongLog().GetLock());
    cmn::ROCSEvent &editedEvent = *this->FindEvent_(evt);
    editedEvent.int_value(newValue);
    EditorHistory &activeHistory = this->GetActiveHistory();
    activeHistory.BeginAction();
    activeHistory.AddEditedEventToCurrentAction(evt, editedEvent);
    activeHistory.EndAction();
    this->FinishEdit_(editedEvent);
    return editedEvent;
}

const cmn::ROCSEvent & Editor::EditEventFloatValue(
    const cmn::ROCSEvent &evt,
    double newValue)
{
    std::lock_guard<std::recursive_mutex> lock(this->GetActiveSongLog().GetLock());
    cmn::ROCSEvent &editedEvent = *this->FindEvent_(evt);
    editedEvent.float_value(newValue);
    EditorHistory &activeHistory = this->GetActiveHistory();
    activeHistory.BeginAction();
    activeHistory.AddEditedEventToCurrentAction(evt, editedEvent);
    activeHistory.EndAction();
    this->FinishEdit_(editedEvent); 
    return editedEvent;
}

const cmn::ROCSEvent & Editor::EditEventStringValue(
    const cmn::ROCSEvent &evt,
    const std::string &newValue)
{
    std::lock_guard<std::recursive_mutex> lock(this->GetActiveSongLog().GetLock());
    cmn::ROCSEvent &editedEvent = *this->FindEvent_(evt);
    editedEvent.value(newValue);
    auto &activeHistory = this->GetActiveHistory();
    activeHistory.BeginAction();
    activeHistory.AddEditedEventToCurrentAction(evt, editedEvent);
    activeHistory.EndAction();
    this->FinishEdit_(editedEvent);
    return editedEvent;
}

const cmn::ROCSEvent & Editor::MoveEvent(
    const cmn::ROCSEvent &evt,
    UInt32 newStartingPosition)
{
    std::lock_guard<std::recursive_mutex> lock(this->GetActiveSongLog().GetLock());
    cmn::ROCSEvtPtrT eventCopyForValidation = CopyRocsEvent(evt);
    SInt32 originalStart = evt.start();
    SInt32 originalEnd = evt.end();
    SInt32 movedTickCount =
        static_cast<SInt32>(newStartingPosition) - originalStart;
    eventCopyForValidation->start(newStartingPosition);
    eventCopyForValidation->end(originalEnd + movedTickCount);
    auto &activeHistory = this->GetActiveHistory();
    activeHistory.BeginAction();
    cmn::ROCSEvent &editedEvent = *this->FindEvent_(evt);
    this->ValidateEdit_(*eventCopyForValidation, &editedEvent);
    editedEvent.start(eventCopyForValidation->start());
    editedEvent.end(eventCopyForValidation->end()); 
    activeHistory.AddEditedEventToCurrentAction(evt, editedEvent);
    activeHistory.EndAction();
    const cmn::ROCSEvent &sortedEventRef = this->SortEditedVector_(editedEvent);
    this->FinishEdit_(sortedEventRef);
    return sortedEventRef;
}

const cmn::ROCSEvent & Editor::TrimEvent(
    const cmn::ROCSEvent &evt,
    UInt32 newStartingPosition,
    UInt32 newEndingPosition)
{
    std::lock_guard<std::recursive_mutex> lock(this->GetActiveSongLog().GetLock());
//    assert(evt.IsPairedEvent());
    cmn::ROCSEvtPtrT eventCopyForValidation = CopyRocsEvent(evt);
    eventCopyForValidation->start(newStartingPosition);
    eventCopyForValidation->end(newEndingPosition);
    auto &activeHistory = this->GetActiveHistory();
    activeHistory.BeginAction();
    cmn::ROCSEvent &editedEvent = *this->FindEvent_(evt);
    this->ValidateEdit_(*eventCopyForValidation, &editedEvent);
    #ifdef EDITOR_VERBOSE_LOGGING
    std::cout << "ValidateEdit_ finished with start=" << eventCopyForValidation->start()
        << ", end=" << eventCopyForValidation->end() << std::endl;
    #endif
    editedEvent.start(eventCopyForValidation->start());
    editedEvent.end(eventCopyForValidation->end());
    
    #ifdef EDITOR_VERBOSE_LOGGING
    std::cout << "Copied values to editEvent start=" << editedEvent.start()
        << ", end=" << editedEvent.end() << std::endl;
    #endif
    activeHistory.AddEditedEventToCurrentAction(evt, editedEvent);
    activeHistory.EndAction();
    const cmn::ROCSEvent &sortedEventRef = this->SortEditedVector_(editedEvent);
    this->FinishEdit_(sortedEventRef);
    return sortedEventRef;
}


EditorHistory::HistoryActionT Editor::Undo()
{
    std::lock_guard<std::recursive_mutex> lock(this->GetActiveSongLog().GetLock());
    auto action = this->GetActiveHistory().PopUndoAction();
    for (auto it: action)
    {
        if (it.OriginalEvent() == nullptr)
        {
            // This edit was an add.
            // Delete the added event
            this->DeleteEvent_(*it.EditedEvent());
        } else if (it.EditedEvent() == nullptr)
        {
            // This edit was a delete.
            // Restore the original event.
            this->AddEvent_(*it.OriginalEvent());
        } else
        {
            // This edit neither created nor deleted an event.
            this->DeleteEvent_(*it.EditedEvent());
            this->AddEvent_(*it.OriginalEvent());
        }
    }

    this->CreateBeatTickIndices_();
    return action;
}
 
bool Editor::CanUndo()
{
    return this->GetActiveHistory().CanUndo();
}

EditorHistory::HistoryActionT Editor::Redo()
{
    std::lock_guard<std::recursive_mutex> lock(this->GetActiveSongLog().GetLock());
    auto action = this->GetActiveHistory().PopRedoAction();
    for (auto it: action)
    {
        if (it.OriginalEvent() == nullptr)
        {
            // This edit was an add.
            // Re-add it.
            this->AddEvent_(*it.EditedEvent());
        } else if (it.EditedEvent() == nullptr)
        {
            // This edit was a delete.
            // Re-delete it.
            this->DeleteEvent_(*it.OriginalEvent());
        } else
        {
            // This edit neither created nor deleted an event.
            this->DeleteEvent_(*it.OriginalEvent());
            this->AddEvent_(*it.EditedEvent());
        }
    }
    
    this->CreateBeatTickIndices_();
    return action;
}
    
bool Editor::CanRedo()
{
    return this->GetActiveHistory().CanRedo();
}

EditorHistory::HistoryActionT Editor::GetEditedEvents()
{
    return this->GetActiveHistory().PeekUndoAction();
}

// Private Methods
cmn::ROCSEvent & Editor::AddEvent_(const cmn::ROCSEvent &newEvent)
{
    switch (newEvent.code())
    {
        case codes::vamp:
            return *CreateNewEventEmplace(this->GetActiveSongLog().GetVamps(), newEvent);
        case codes::repeat:
            return *CreateNewEventEmplace(
                this->GetActiveSongLog().GetRepeats(),
                newEvent);
        case codes::cut:
            return *CreateNewEventEmplace(this->GetActiveSongLog().GetCuts(), newEvent);
        case codes::transpose:
            return *CreateNewEventEmplace(
                this->GetActiveSongLog().GetTranspositions(),
                newEvent);
        case codes::tempo_scale:
            return *CreateNewEventEmplace(
                this->GetActiveSongLog().GetTempoScales(),
                newEvent);
        case codes::marker:
            return *CreateNewEventEmplace(
                this->GetActiveSongLog().GetMarkers(),
                newEvent);
        case codes::click:
            return *CreateNewEventEmplace(
                this->GetActiveSongLog().GetClicks(),
                newEvent);
        case codes::caesura:
            return *CreateNewEventEmplace(
                this->GetActiveSongLog().GetCaesuras(),
                newEvent);
        case codes::fermata:
            return *CreateNewEventEmplace(
                this->GetActiveSongLog().GetFermatas(),
                newEvent);
        default:
            throw std::logic_error(ex::format(
                "Unsupported event code: %i",
                newEvent.code()));
    }
}

void Editor::AlignEventWithBeat_(cmn::ROCSEvent &event)
{
    // Use the active timeline and any relevant Click events to adjust event bounds
    // Move event start and, if it is a paired event, event end to the nearest
    // beat boundary.
    
    event.start(this->GetBeatAlignedTickIndex_(event.start()));

    // Do the same thing for end, if event is paired
    if (event.IsPairedEvent())
    {
        event.end(this->GetBeatAlignedTickIndex_(event.end()));    
    }
}

void Editor::CreateBeatTickIndices_()
{
    this->beatTickIndices_.clear();
    auto& barsBeats = this->GetActiveTimeline().GetBarsBeats(); 
    auto& clicks = this->GetActiveSongLog().GetClicks();
    auto& meters = this->GetActiveTimeline().GetMeters();
    
    for (auto it: barsBeats)
    {
        this->beatTickIndices_.push_back(it.abs_time());
    }

    for (auto it: clicks)
    {
        // Find the meter that is active at the start of the click
        auto meterIt = upper_bound(
            meters.begin(),
            meters.end(),
            it.start(),
            [&] (UInt32 tickIndex, const TL::Meter &meter)
            {
                return tickIndex < meter.abs_time();
            });
        
        // Because I have changed to upper_bound, meterIt may be meters.end(),
        // but it should never be meters.begin()
        assert(meterIt != meters.begin());
        
        // We have found the meters.end() or the meterIt that comes one after
        // it.start().
        // Either way, meterIt-- will give us the meter that is at or before
        // it.start().
        meterIt--;

        UInt32 clocksPerClick = it.ClocksPerClick(
            meterIt->notated_32_per_quarter());
        
        UInt32 ticksPerQuarterNote = 
            this->showData_.GetSongData(this->activeSongNumber_)
                .GetPulsesPerQuarterNote();
        
        UInt32 clocksPerQuarterNote = 24; 
        UInt32 ticksPerClock = ticksPerQuarterNote / clocksPerQuarterNote;
        UInt32 ticksPerClick = ticksPerClock * clocksPerClick;
        
        // Assuming that every Click must begin and end on a beat
        // Do not add it.start() or it.end(), just the extra beats in between.
        UInt32 clickTickIndex = it.start() + ticksPerClick;
        while (clickTickIndex < it.end())
        {
            this->beatTickIndices_.push_back(clickTickIndex);
            clickTickIndex += ticksPerClick;
        }
    }

    this->beatTickIndices_.sort();
}


cmn::ROCSEvent * Editor::FindEvent_(const cmn::ROCSEvent &eventToFind)
{
    switch (eventToFind.code())
    {
        case codes::vamp:
            return &FindEvent(
                this->GetActiveSongLog().GetVamps(),
                CL::Vamp(eventToFind));
        case codes::repeat:
            return &FindEvent(
                this->GetActiveSongLog().GetRepeats(),
                CL::Repeat(eventToFind));
        case codes::cut:
            return &FindEvent(
                this->GetActiveSongLog().GetCuts(),
                CL::Cut(eventToFind));
        case codes::transpose:
            return &FindEvent(
                this->GetActiveSongLog().GetTranspositions(),
                CL::Transpose(eventToFind));
        case codes::tempo_scale:
            return &FindEvent(
                this->GetActiveSongLog().GetTempoScales(),
                CL::TempoScale(eventToFind));
        case codes::marker:
            return &FindEvent(
                this->GetActiveSongLog().GetMarkers(),
                CL::Marker(eventToFind));
        case codes::click:
            return &FindEvent(
                this->GetActiveSongLog().GetClicks(),
                CL::Click(eventToFind));
        case codes::caesura:
            return &FindEvent(
                this->GetActiveSongLog().GetCaesuras(),
                CL::Caesura(eventToFind));
        case codes::fermata:
            return &FindEvent(
                this->GetActiveSongLog().GetFermatas(),
                CL::Fermata(eventToFind));
        default:
            throw std::logic_error(ex::format(
                "Unsupported event code: %i",
                eventToFind.code()));
    };
}

void Editor::FinishEdit_(const cmn::ROCSEvent &evt)
{
    if (evt.code() == codes::click)
    {
        this->CreateBeatTickIndices_();
    }
}

EditorHistory &Editor::GetActiveHistory()
{
    return this->editorHistoryBySongNumber_[this->activeSongNumber_];   
}

const TL::Timeline &Editor::GetActiveTimeline() const
{
    return this->showData_.GetSongData(this->activeSongNumber_).GetTimeline();
}

CL::SongLog &Editor::GetActiveSongLog()
{
    return this->changeLog_.GetSongLog(this->activeSongNumber_);
}

UInt32 Editor::GetBeatAlignedTickIndex_(UInt32 tickIndex)
{
    // Assumes that this->beatTickIndices_ is up-to-date
    // Find beat nearest to tickIndex 
    auto beatTickIt = std::upper_bound(
        this->beatTickIndices_.begin(),
        this->beatTickIndices_.end(),
        tickIndex);

    UInt32 nearestBeatIndexBefore, nearestBeatIndexAfter;
   
    if (beatTickIt != this->beatTickIndices_.end())
    {
        nearestBeatIndexAfter = *beatTickIt;
    } else
    {
        nearestBeatIndexAfter = 0; 
    }
    
    if (beatTickIt == this->beatTickIndices_.begin())
    {
        throw std::logic_error("beatTickIt should never be begin().");
    }

    beatTickIt--;
    nearestBeatIndexBefore = *beatTickIt;
    
    if (tickIndex == nearestBeatIndexBefore)
    {
        return tickIndex;
    } else 
    {
        // tickIndex is not already aligned with a beat.
        if (nearestBeatIndexAfter == 0)
        {
            return nearestBeatIndexBefore;
        } else
        {
            // Pick the closer of the two beats
            UInt32 frontDiff = tickIndex - nearestBeatIndexBefore;
            UInt32 backDiff = nearestBeatIndexAfter - tickIndex;
            if (frontDiff < backDiff)
            {
                return nearestBeatIndexBefore;
            } else
            {
                return nearestBeatIndexAfter;
            }
        }
    }
}


/* Trims or deletes events to make room for cut */
void Editor::PunchHoleForCut_(const CL::Cut &cutEvent)
{
    auto &activeHistory = this->GetActiveHistory();
    PairedHolePuncher(this->GetActiveSongLog().GetVamps(), cutEvent, activeHistory);    
    PairedHolePuncher(this->GetActiveSongLog().GetRepeats(), cutEvent, activeHistory);    
    PairedHolePuncher(
        this->GetActiveSongLog().GetTranspositions(),
        cutEvent,
        activeHistory);
    PairedHolePuncher(this->GetActiveSongLog().GetTempoScales(), cutEvent, activeHistory);
    PairedHolePuncher(this->GetActiveSongLog().GetClicks(), cutEvent, activeHistory);    
    PairedHolePuncher(this->GetActiveSongLog().GetFermatas(), cutEvent, activeHistory);
    HolePuncher(this->GetActiveSongLog().GetMarkers(), cutEvent, activeHistory);    
    HolePuncher(this->GetActiveSongLog().GetCaesuras(), cutEvent, activeHistory);    
}

template<class SequenceT>
const cmn::ROCSEvent &SequenceSorter(SequenceT &sequence, const cmn::ROCSEvent &editedEvent)
{
    typename SequenceT::value_type tracker(editedEvent);
    std::sort(sequence.begin(), sequence.end());
    return FindEvent(sequence, tracker);
}

const cmn::ROCSEvent & Editor::SortEditedVector_(const cmn::ROCSEvent &editedEvent)
{
    auto &activeLog = this->GetActiveSongLog();
    switch (editedEvent.code())
    {
        case codes::vamp:
            return SequenceSorter(activeLog.GetVamps(), editedEvent);
        case codes::repeat:
            return SequenceSorter(activeLog.GetRepeats(), editedEvent);
        case codes::cut:
            return SequenceSorter(activeLog.GetCuts(), editedEvent);
        case codes::transpose:
            return SequenceSorter(activeLog.GetTranspositions(), editedEvent);
        case codes::tempo_scale:
            return SequenceSorter(activeLog.GetTempoScales(), editedEvent);
        case codes::marker:
            return SequenceSorter(activeLog.GetMarkers(), editedEvent);
        case codes::click:
            return SequenceSorter(activeLog.GetClicks(), editedEvent);
        case codes::caesura:
            return SequenceSorter(activeLog.GetCaesuras(), editedEvent);
        case codes::fermata:
            return SequenceSorter(activeLog.GetFermatas(), editedEvent);
        default:
            throw std::logic_error(ex::format(
                "Unsupported event code: %i",
                editedEvent.code()));
    };
}

void Editor::ValidateEdit_(
    cmn::ROCSEvent &newEvent,
    const cmn::ROCSEvent *originalEvent)
{
    this->AlignEventWithBeat_(newEvent);
    
    if (newEvent.IsPairedEvent())
    {
        if (newEvent.code() == codes::cut)
        {
            this->ValidateCut_(newEvent, originalEvent);
        } else
        {
            this->ValidatePairedExceptCut_(newEvent, originalEvent);
            #ifdef EDITOR_VERBOSE_LOGGING
            std::cout << "ValidatePairedExceptCut_ finshed with newEvent.start()="
                << newEvent.start() << ", newEvent.end()=" << newEvent.end() << std::endl;
            #endif

            if (newEvent.code() == codes::repeat
                || newEvent.code() == codes::vamp)
            {
                this->ValidateVampOrRepeat_(newEvent);
                #ifdef EDITOR_VERBOSE_LOGGING
                std::cout << "ValidateVampOrRepeat_ finished with newEvent.start()="
                    << newEvent.start() << ", end=" << newEvent.end() << std::endl;
                #endif
            } else if (newEvent.code() == codes::click)
            {
                this->ValidateClick_(newEvent);
            }
        }
    } else
    {
        this->ValidateNonPairedEvent_(newEvent, originalEvent);
    }
}

/* Ensures non overlap, then uses PunchHoleForCut_ to edit any events that may
 * be overlapping with the cut.
 */
void Editor::ValidateCut_(cmn::ROCSEvent &cut, const cmn::ROCSEvent *originalEvent)
{
    assert(cut.code() == codes::cut);
    EnsureNonOverlap(this->GetActiveSongLog().GetCuts(), cut, originalEvent); 
    this->PunchHoleForCut_(CL::Cut(cut));
}

/* Checks that start() and end() do not occur within a cut.
 * If both start() and end() are within the same cut, the edit is not allowed
 * to proceed. Throws exception EditorEditNotAllowed.
 * Checks that there are no other events of the same type that overlap, and
 * makes adjustments to newEvent if necessary.
 * Only Repeats are allowed to nest with themselves.

Allowed for repeat, but for nothing else:
<*****Repeat1*****>
   <***Repeat2****>

Not allowed anywhere, where EventX is the same type of event:
<*****EventX1*****>
           <*****EventX2*****>
 *
 *
 */
void Editor::ValidatePairedExceptCut_(
    cmn::ROCSEvent &newEvent,
    const cmn::ROCSEvent *originalEvent)
{
    assert(newEvent.code() != codes::cut);
    EnsureNonOverlap(this->GetActiveSongLog().GetCuts(), newEvent);

    switch (newEvent.code())
    {
        case (codes::vamp):
            EnsureNonOverlap(
                this->GetActiveSongLog().GetVamps(),
                newEvent,
                originalEvent);
            break;
        case (codes::repeat):
            EnsureNonOverlapAtEndPoints(
                this->GetActiveSongLog().GetRepeats(),
                newEvent,
                originalEvent);
            break;
        case (codes::transpose):
            EnsureNonOverlap(
                this->GetActiveSongLog().GetTranspositions(),
                newEvent, 
                originalEvent);
            break;
        case (codes::tempo_scale):
            EnsureNonOverlap(
                this->GetActiveSongLog().GetTempoScales(),
                newEvent,
                originalEvent);
            break;
        case (codes::click):
            EnsureNonOverlap(
                this->GetActiveSongLog().GetClicks(),
                newEvent,
                originalEvent);
            break;
        case (codes::fermata):
            EnsureNonOverlap(
                this->GetActiveSongLog().GetFermatas(),
                newEvent,
                originalEvent);
            break;
        default:
            throw std::logic_error(ex::format(
                "Unsupported event code within ValidatePairedExceptCut_: %i",
                newEvent.code()));
    };
}

/* Already checked that Vamp or Repeat do not overlap with themselves. 
 * Check that they do not overlap with each other.
 
 Not allowed:
 <****Repeat****>
     <****Vamp*****>

 Allowed:
 <****Repeat*****>
    <****Vamp****>

 <****Repeat*****>
 <****Vamp****>

 <****Repeat********>
   <****Vamp****>
 
 */
void Editor::ValidateVampOrRepeat_(cmn::ROCSEvent &newEvent)
{
    assert((newEvent.code() == codes::vamp)
        || (newEvent.code() == codes::repeat));
    
    if (newEvent.code() == codes::vamp)
    {
        EnsureNonOverlapAtEndPoints(
            this->GetActiveSongLog().GetRepeats(),
            newEvent);
    } else if (newEvent.code() == codes::repeat)
    {
        EnsureNonOverlapAtEndPoints(
            this->GetActiveSongLog().GetVamps(),
            newEvent);
    }
}


void Editor::ValidateClick_(cmn::ROCSEvent &newEvent)
{
    // May not span a change in Meter
    // May not span another click
    assert(newEvent.code() == codes::click);
    
    // Check for meter changes that may be bounded by newEvent
    // A TL::Meter may occur at the start or end of newEvent, but not in
    // between.
    auto meterIt = upper_bound(
        this->GetActiveTimeline().GetMeters().begin(),
        this->GetActiveTimeline().GetMeters().end(),
        newEvent.start());
    
    assert(meterIt != this->GetActiveTimeline().GetMeters().begin());
    meterIt--;

    while (meterIt != this->GetActiveTimeline().GetMeters().end())
    {
        if (meterIt->abs_time() > newEvent.end())
        {
            // newEvent ends before the meter change
            break;
        }

        if ((newEvent.start() < meterIt->abs_time())
            && newEvent.end() > meterIt->abs_time())
        {
            newEvent.end(meterIt->abs_time());
            break; 
        }

        meterIt++;
    }
}

void Editor::ValidateNonPairedEvent_(cmn::ROCSEvent &newEvent, const cmn::ROCSEvent *originalEvent)
{
    assert(!(newEvent.IsPairedEvent()));
    
    // Make sure that it doesn't fall within a cut.
    for (auto cutIt : this->GetActiveSongLog().GetCuts())
    {
        if (cutIt.bounded(newEvent.abs_time()))
        {
            throw EditorEditNotAllowed("Cannot create edit inside of cut.");
        }
    }

    // Make sure it does not coincide with another event of the same type.
    if (newEvent.code() == codes::marker)
    {
        auto existingIt = std::find_if(
            this->GetActiveSongLog().GetMarkers().begin(),
            this->GetActiveSongLog().GetMarkers().end(),
            [&] (const CL::Marker &event)
            {
                return event.abs_time() == newEvent.abs_time();
            });
        
        if (existingIt != this->GetActiveSongLog().GetMarkers().end())
        {
            if (&(*existingIt) != originalEvent)
            {
                throw EditorEditNotAllowed("Marker already exists at this tick index.");
            }
        }
    } else if (newEvent.code() == codes::caesura)
    {
        auto existingIt = std::find_if(
            this->GetActiveSongLog().GetCaesuras().begin(),
            this->GetActiveSongLog().GetCaesuras().end(),
            [&] (const CL::Caesura &event)
            {
                return event.abs_time() == newEvent.abs_time();
            });
        
        if (existingIt != this->GetActiveSongLog().GetCaesuras().end())
        {
            if (&(*existingIt) != originalEvent)
            {
                throw EditorEditNotAllowed("Caesura already exists at this tick index.");
            }
        }
    }
}

} // end namespace CL
