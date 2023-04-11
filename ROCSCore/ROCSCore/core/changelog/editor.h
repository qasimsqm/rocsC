/* Spencer,
 * Some notable changes from this first round of implementation:
 * The ctor for Editor now requires ShowData as a second argument.
 * GetEditedEvents now returns HistoryActionT 
 *
 * -- Jonathan, 20 Jan 2014
 *
 *
 * Additional Changes to the API -- 25 Mar 2014:
 *  Undo/Redo now return the HistoryActionT that was just processed, rather
 *  than a bool.
 *
 *
 *
 */

#pragma once

#include "core/changelog/change_log.h"
#include "core/rocs_midi/show_data.h"
#include "core/changelog/editor_history.h"

// #define EDITOR_VERBOSE_LOGGING

namespace CL
{

/** class Editor

For all Editor methods accepting 'const ROCSEvent &' as the first argument,
pass a reference to the event to be edited.

The edit will be checked for beat alignment or conflicting events, and the
requested parameters will be adjusted as needed.  The return value will be a
reference to the event that was actually added/edited.

When adding or editing a Cut, overlapping events may be automatically edited.
Likewise, performing Undo/Redo actions may affect more than one event.  After
each edit command, you may call GetEditedEvents() to get a list of events that
were touched by the last edit. Each item in the list will be a HistoryItem.
This may be useful for only redrawing affected events in the GUI timeline,
rather than perform a full refresh each time.

If a Cut is added that adjoins or overlaps another cut, the two cuts will be
merged into the same event.

<****existingCut****>
               <***addedCut***>
<*********resultantCut********>

If a Cut is added that covers any other editable events, they will be trimmed
or deleted so that nothing happens inside of the cut.

<*******existingVamp********>
      <****existingRepeat***>
                    <*****addedCut*****>

Result:
<***editedVamp*****>
      <editedRepeat>
                    <*****addedCut*****>

If a Vamp or Repeat is added that overlaps with a cut, it will be trimmed so
that there is no overlap with the cut.
<****existingCut****>
            <****addedVamp****>

Result:
<****existingCut****>
                     <*newVamp>

Vamps may occur within repeats, and vice-versa, but they may not overlap

Not allowed:
<***Vamp***>
      <***Vamp***>

Allowed:
<***Vamp***>
            <Vamp>

Not allowed:
<*******Repeat********>
                <***Vamp***>

Allowed:
<*******Repeat********>
      <***Vamp***>


A Click edit is not allowed to span a change in Meter.

A Click edit must align with a Time Signature beat. This one will be taken care of by not allowing a Click to span another Click.

If an edit is requested that is not allowed, the Editor will try to make
adjustments to the new edit so that it is legal.  If this is not possible, an
EditorEditNotAllowed exception will be thrown.

**/

class ROCS_CORE_API EditorEventNotFound: public std::logic_error
{
public:
    EditorEventNotFound(const std::string &what): std::logic_error(what) {}
};

class ROCS_CORE_API EditorError: public std::runtime_error
{
public:
    EditorError(const std::string &what): std::runtime_error(what) {}
};

class ROCS_CORE_API EditorEditNotAllowed: public EditorError 
{
public:
    EditorEditNotAllowed(const std::string &what): EditorError(what) {}
};

class ROCS_CORE_API Editor
{
public:
    typedef std::pair<const cmn::ROCSEvent &, cmn::ROCSEvent &> EditedPairT;
    typedef std::list<EditedPairT> EditedListT;
    typedef std::map<std::string, EditorHistory> EditorHistoryBySongNumberT; 
public:
    Editor(ChangeLog &changeLog, const rocs_midi::ShowData &showData);

    void SelectSongLogForEditing(const std::string &songNumber);
    
    /** Returns a reference to the added event.
        The starting/ending positions of the added event may be changed  
        because of required beat alignment, or because of conflicts with other
        events. **/
    const cmn::ROCSEvent & AddEvent(const cmn::ROCSEvent &evt);
    
    /** Pass in a reference to the evt that will be deleted. **/
    void DeleteEvent(const cmn::ROCSEvent &evt);

    /** Returns a reference to the edited event. **/
    const cmn::ROCSEvent & EditEventIntValue(
        const cmn::ROCSEvent &evt,
        SInt32 newValue);

    /** Returns a reference to the edited event. **/
    const cmn::ROCSEvent & EditEventFloatValue(
        const cmn::ROCSEvent &evt,
        double newValue);

    /** Returns a reference to the edited event. **/
    const cmn::ROCSEvent & EditEventStringValue(
        const cmn::ROCSEvent &evt,
        const std::string &newValue);

    /** Returns a reference to the moved event.
        The starting position of the moved event may not be newStartingPosition
        because of required beat alignment, or because of conflicts with other
        events. **/
    const cmn::ROCSEvent & MoveEvent(
        const cmn::ROCSEvent &evt,
        UInt32 newStartingPosition);
    
    /** Returns a reference to the trimmed event.
        The bounds of the trimmed event may not be equal to the requested bounds, 
        because of required beat alignment, or because of conflicts with other
        events. **/
    const cmn::ROCSEvent & TrimEvent(
        const cmn::ROCSEvent &evt,
        UInt32 newStartingPosition,
        UInt32 newEndingPosition);

    /** Returns HistoryActionT that was just undone.
        If there are no actions to undo, throws EditorHistoryNoActionError 
        If any other exception is thrown, it is due to an internal error, and
        it should be considered fatal. */
    EditorHistory::HistoryActionT Undo();
    
    /** returns true if there are any edits to undo. **/
    bool CanUndo();
    
    /** Returns HistoryActionT that was just redone.
        If there are no actions to undo, throws EditorHistoryNoActionError 
        If any other exception is thrown, it is due to an internal error, and
        it should be considered fatal. */
    EditorHistory::HistoryActionT Redo();
   
    /** returns true if there are any edits to redo. **/
    bool CanRedo();

    EditorHistory::HistoryActionT GetEditedEvents();
    
protected:
    cmn::ROCSEvent &AddEvent_(const cmn::ROCSEvent &newEvent);
    
    void AlignEventWithBeat_(cmn::ROCSEvent &event);

    void CreateBeatTickIndices_();

    void DeleteEvent_(const cmn::ROCSEvent &eventToDelete);

    cmn::ROCSEvent *FindEvent_(const cmn::ROCSEvent &eventToFind);
    
    void FinishEdit_(const cmn::ROCSEvent &evt);

    EditorHistory &GetActiveHistory();

    const TL::Timeline &GetActiveTimeline() const;
    
    CL::SongLog &GetActiveSongLog();

    UInt32 GetBeatAlignedTickIndex_(UInt32 tickIndex);

    void PunchHoleForCut_(const CL::Cut &cutEvent);
       
    const cmn::ROCSEvent & SortEditedVector_(const cmn::ROCSEvent &editedEvent);

    // A dispatcher to all validation work.
    void ValidateEdit_(cmn::ROCSEvent &newEvent, const cmn::ROCSEvent *original = NULL);
    
    void ValidateCut_(cmn::ROCSEvent &newEvent, const cmn::ROCSEvent *original = NULL);

    void ValidatePairedExceptCut_(  
        cmn::ROCSEvent &newEvent,
        const cmn::ROCSEvent *original = NULL);

    void ValidateVampOrRepeat_(cmn::ROCSEvent &newEvent);

    void ValidateClick_(cmn::ROCSEvent &newEvent);

    void ValidateNonPairedEvent_(
        cmn::ROCSEvent &newEvent,
        const cmn::ROCSEvent *original = NULL);

protected:
    ChangeLog &changeLog_;
    const rocs_midi::ShowData &showData_;
    std::string activeSongNumber_;
    EditorHistoryBySongNumberT editorHistoryBySongNumber_;
    std::list<UInt32> beatTickIndices_;
};

} // end namespace CL
