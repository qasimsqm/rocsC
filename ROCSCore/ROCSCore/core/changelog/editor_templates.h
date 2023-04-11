#pragma once
#include <algorithm>
#include <iostream>

namespace CL
{

template<class SequenceT, class EventT>
EventT &FindEvent(SequenceT &sequence, const EventT &event)
{
    auto evtIt = std::find(
        sequence.begin(),
        sequence.end(),
        event);

    if (evtIt == sequence.end())
    {
        throw EditorEventNotFound("");
    } else
    {
        return *evtIt;
    }
}

template<class SequenceT>
typename SequenceT::iterator CreateNewEventEmplace(
    SequenceT &sequence,
    const cmn::ROCSEvent &newEvent)
{
    return sequence.emplace(
        std::upper_bound(sequence.begin(), sequence.end(), newEvent.start()),
        newEvent);
}

template<class SequenceT, class EventT>
void DeleteEvent(SequenceT &sequence, const EventT &eventToDelete)
{
    auto evtIt = std::find(sequence.begin(), sequence.end(), eventToDelete);
    sequence.erase(evtIt);
}

template<class SequenceT>
void HolePuncher(
    SequenceT &sequence,
    const CL::Cut &cut,
    EditorHistory &activeHistory)
{
    // Find all events that lie under the cut
    std::list<typename SequenceT::value_type> eventsUnderCut;
    
    // Copy
    std::copy_if(
        sequence.begin(),
        sequence.end(),
        back_inserter(eventsUnderCut),
        [&](const typename SequenceT::value_type &event)
        {
            return cut.bounded(event.abs_time());
        });

    for (auto it : eventsUnderCut)
    {
        ::CL::DeleteEvent(sequence, it);
        activeHistory.AddDeletedEventToCurrentAction(it);
    }
}

template<class SequenceT>
void PairedHolePuncher(
    SequenceT &sequence,
    const CL::Cut &cut,
    EditorHistory &activeHistory)
{
    /*
    Case1: Trim the start of Event2
    <*****Event1*****>      <*****Event2*****>       <******Event3*****>
                        <*****Cut*****>

    Case2: // Do nothing.
    <*****Event1*****>      <*****Event2*****>       <******Event3*****>
                                <***Cut***>
    
    Case3: Trim the end of Event2
    <*****Event1*****>      <*****Event2*****>       <******Event3*****>
                                        <**Cut***>
    
    Case4: Delete Event2
    <*****Event1*****>      <*****Event2*****>       <******Event3*****>
                            <******Cut*******>
    
    Case5: Delete Event2
    <*****Event1*****>      <*****Event2*****>       <******Event3*****>
                        <***************Cut*******>
    
    Case6: Trim the end of Event1, Delete Event2, Trim the start of Event3
    <*****Event1*****>      <*****Event2*****>       <******Event3*****>
                <*******************Cut*********************>
    
    Case7: Do Nothing
    <*****Event1*****>                               <******Event3*****>
                         <**********Cut**********>
    
    Case8: Trim the end of Event2, Trim the start of Event3
    <*****Event1*****>      <*****Event2*****>       <******Event3*****>
                                     <***************Cut*******>
    */

    // Find all events that lie under the cut
    std::list<typename SequenceT::value_type> eventsUnderCut;

    // Get an iterator to the first event that has its end after cut.start()
    auto holeBeginIt = std::upper_bound(
        sequence.begin(),
        sequence.end(),
        cut.start(),
        [](UInt32 val, const typename SequenceT::value_type &event)
        {
            return val < event.end(); 
        });
    
    if (holeBeginIt == sequence.end())
    {
        // There are no events to punch holes in.
        return;
    }

    // Get an iterator to one beyond the last event that starts before cut.end()
    auto holeEndIt = std::upper_bound(
        holeBeginIt,
        sequence.end(),
        cut.end());
    
    std::copy(holeBeginIt, holeEndIt, back_inserter(eventsUnderCut));
    
    if (!eventsUnderCut.size())
    {
        // Nothing to do.
        return;
    }

    for (auto it : eventsUnderCut)
    {
        if (it.start() < cut.start()
            && it.end() > cut.start()
            && it.end() <= cut.end())
        {
            // The tail end of this event lies under the cut.
            // Trim it.
            auto &editedEvent = FindEvent(sequence, it);
            editedEvent.end(cut.start());
            activeHistory.AddEditedEventToCurrentAction(it, editedEvent);
        } else if(it.start() >= cut.start() &&
            it.end() <= cut.end())
        {
            // This event is entirely inside the cut.
            // Remove it.
            DeleteEvent(sequence, it);
            activeHistory.AddDeletedEventToCurrentAction(it);
        } else if (it.start() >= cut.start()
            && it.start() < cut.end()
            && it.end() > cut.end())
        {
            // The start end of this event lies under the cut.
            // Trim it.
            auto &editedEvent = FindEvent(sequence, it);
            editedEvent.start(cut.end());
            activeHistory.AddEditedEventToCurrentAction(it, editedEvent);
        }
    }
}

} // end namespace CL
