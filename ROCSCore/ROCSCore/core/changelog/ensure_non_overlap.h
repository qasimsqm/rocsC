#pragma once
#include "core/win32/declspec.h"
#include "core/changelog/editor.h"
#include "core/changelog/editor_templates.h"

namespace CL
{

template<class MasterSequenceT, class EventT>
void EnsureNonOverlap(
    const MasterSequenceT &masterSequence,
    EventT &editEvent,
    const cmn::ROCSEvent *originalEvent = NULL)
{
    typedef typename MasterSequenceT::value_type MasterEventT;
    
    // Ensure that editEvent does not span any MasterEventT events.
    
    // masterSequence contains the existing PairedEvents
    // If possible, editEvent will be modified to make it permissible.
    // Otherwise, EditorEditNotAllowed will be thrown.

    /**
    Find the first masterEvent that ends after editEvent.start()
        masterEvent.end() > editEvent.start()

    'M' == masterEvent
    'E' == the editEvent that is being validated
    
    Three cases:
        A. The masterEvent (M) is shorter in length than the editEvent (E)
            masterEvent.GetLengthTicks() < editEvent.GetLengthTicks() 

            1. masterEvent.start() <= editEvent.start()
                && masterEvent.end() < editEvent.end()
            Fix it: E.start() = M.end()
            a.
            <*****M*****>
                <**********E**********>
            -- becomes -- 
            <*****M*****>
                         <****E*******>
            
            b.
                <*****M*****>
                <**********E**********>
                -- becomes -- 
                <*****M*****>
                             <**E*****>
            
            2. masterEvent.start() > editEvent.start()
                && masterEvent.start() < editEvent.end()
            Fix it: E.end() = M.start() 
                a. 
                    <*****M*****>
                <**********E**********>
                -- becomes --  
                <*E><*****M*****>

                b.
                          <*****M*****>
                <**********E**********>
                -- becomes --  
                          <*****M*****>
                <***E****>
                
                c.
                                <*****M*****>
                <**********E**********>
                -- becomes --            
                                <*****M*****>
                <******E*******>
            
            3. masterEvent.start() >= editEvent.end()
            Automatically Passes 
                                         <*****M*****>
                <**********E**********>

        B. The masterEvent (M) is the same length as E
            masterEvent.GetLengthTicks() == editEvent.GetLengthTicks()

            1. masterEvent.start() <= editEvent.start() 
                && masterEvent.end() < editEvent.end()
            Fix it: E.start() = M.end()
                    <*****M*****>
                           <*****E*****>
            -- becomes --
                    <*****M*****>
                                 <**E**>
                    
             
            2. masterEvent.start() == editEvent.start()
                && masterEvent.end() == editEvent.end()
            Automatically Fails
                           <*****M*****>
                           <*****E*****>

            
            3. masterEvent.start() > editEvent.start()
                && masterEvent.start() < editEvent.end()
                
            Fix it: E.end() == M.start()
                                  <*****M*****>
                           <*****E*****>
            
            4. masterEvent.start() >= editEvent.end()
            Automatically Passes
                                          <*****M*****>
                           <*****E*****>


        C. M is longer than E
            masterEvent.GetLengthTicks() > editEvent.GetLengthTicks()
            
            1. masterEvent.start() <= editEvent.start()
                && masterEvent.end() < editEvent.end()
            Fix it: E.start() == M.end()
            <**********M**********>
                            <*****E*****>
            -- becomes -- 
            <**********M**********>
                                   <**E*>

            2. masterEvent.start() < editEvent.start()
                && masterEvent.end() == editEvent.end()
            Automatically Fails 
                  <**********M**********>
                            <*****E*****>

            3. masterEvent.start() < editEvent.start()
                && masterEvent.end() > editEvent.start()
            Automatically Fails 
                       <**********M**********>
                            <*****E*****>

            4. masterEvent.start() == editEvent.start()
                && masterEvent.end() > editEvent.end()
            Automatically Fails 
                            <**********M**********>
                            <*****E*****>

            5. masterEvent.start() > editEvent.start()
                && masterEvent.start() < editEvent.end()
                && masterEvent.end() > editEvent.end()
            Fix it: E.end() == M.start()
                                  <**********M**********>
                            <*****E*****>
            -- becomes -- 
                                  <**********M**********>
                            <*E**>
            
            6. masterEvent.start() >= editEvent.end()
            Automatically passes
                                          <**********M**********>
                            <*****E*****>

    
    **/
    
    // Find the first MasterEventT that ends after editEvent.start()
    auto masterEventIt = std::upper_bound(
        masterSequence.begin(),
        masterSequence.end(),
        editEvent.start(),
        [] (const UInt32 &val, const MasterEventT& click)
        {
            return val < click.end();
        });

    if (masterEventIt == masterSequence.end())
    {
        // Either there are no other MasterEventT events, or they all end before
        // this editEvent starts.
        // This editEvent automatically passes.
        return;
    }

    // Idiot check
    assert(masterEventIt->end() > editEvent.start());

    UInt32 masterStart, masterEnd;
    const cmn::ROCSEvent *masterPtr;
    for (; masterEventIt != masterSequence.end(); masterEventIt++)
    {
        masterPtr = &(*masterEventIt);
        if (masterPtr == originalEvent)
        {
            continue;
        }
        
        masterStart = masterEventIt->start();
        masterEnd = masterEventIt->end();
        #ifdef EDITOR_VERBOSE_LOGGING
        std::cout << "EnsureNonOverlap master start=" << masterStart
            << ", end=" << masterEnd << std::endl;
        std::cout << "editEvent start=" << editEvent.start()
            << ", end=" << editEvent.end() << std::endl;
        #endif
        if (masterEventIt->start() >= editEvent.end())
        {
            // There is no overlap.
            // Automatically passes, and there are no other events to consider.
            // A.3, B.4, C.6
            // return;
            return;
        }
        
        // There is some form of overlap.
        // Determine whether an adjustment to editEvent is possible.
        if (masterEventIt->start() <= editEvent.start())
        {
            if (masterEventIt->end() < editEvent.end())
            {
                // We found a master event that overlaps the start of editEvent,
                // but does not overlap the end
                // A.1.a, A.1.b, B.1, C.1

                // editEvent overlaps the tail of masterEvent
                #ifdef EDITOR_VERBOSE_LOGGING
                std::cout << "EnsureNonOverlap setting editEvent start ("
                    << editEvent.start() << ") to master end (" << masterEnd << ")"
                    << std::endl;
                #endif
                editEvent.start(masterEventIt->end());
            } else
            {
                // masterEventIt->end() >= editEvent.end()
                // The masterEvent eclipses editEvent.
                // No adjustment possible
                // B.2, C.2, C.3, C.4
                throw EditorEditNotAllowed("New event is eclipsed by existing event.");
            }
        } else
        {
            // masterEventIt->start() > editEvent.start()
            
            // We should have already filtered out events whose start time occurs
            // on or after editEvent.end()
            assert(masterEventIt->start() < editEvent.end());
           
            // A.2.a, A.2.b, A.2.c, B.3, C.5
            #ifdef EDITOR_VERBOSE_LOGGING
            std::cout << "EnsureNonOverlap setting editEvent end ("
                << editEvent.end() << ") to master start (" << masterStart << ")"
                << std::endl;
            #endif

            editEvent.end(masterEventIt->start());
        }
    }
    
    #ifdef EDITOR_VERBOSE_LOGGING
    std::cout << "EnsureNonOverlap finished with editEvent start=" << editEvent.start()
        << ", end=" << editEvent.end() << std::endl;
    #endif
}

/**
    EnsureNonOverlapAtEndPoints allows nesting, where the inner event is completely contained by a larger outer event.  Overlapping at ends is not allowed.

    Allowed:
    <****Master****>
    <*********Edit*******>

       <****Master****>
    <*********Edit*******>
    
          <****Master****>
    <*********Edit*******>

    <*******Master********>
    <****Edit****>
    
    <*******Master********>
        <****Edit****>
    
    <*******Master********>
             <****Edit****>
    
    Not Allowed:
           <*******Master********>
    <****Edit****>
           
           <*******Master********>
                         <****Edit****>
    
    <*****Master*****>
    <*****Edit*******>
**/
template<class MasterSequenceT, class EventT>
void EnsureNonOverlapAtEndPoints(
    const MasterSequenceT &masterSequence,
    EventT &editEvent,
    const cmn::ROCSEvent *originalEvent = NULL)
{
    typedef typename MasterSequenceT::value_type MasterEventT;
    
    assert(std::is_sorted(masterSequence.begin(), masterSequence.end()));

    // Find the first MasterEventT that ends after editEvent.start()
    auto masterEventIt = std::upper_bound(
        masterSequence.begin(),
        masterSequence.end(),
        editEvent.start(),
        [] (const UInt32 &tickIndex, const MasterEventT& click)
        {
            return tickIndex < click.end();
        });

    if (masterEventIt == masterSequence.end())
    {
        // Either there are no other MasterEventT events, or they all end before
        // this editEvent starts.
        // This editEvent automatically passes.
        return;
    }

    // Idiot check
    assert(masterEventIt->end() > editEvent.start());
    
    UInt32 masterStart, masterEnd;
    for (; masterEventIt != masterSequence.end(); masterEventIt++)
    {
        if (&(*masterEventIt) == originalEvent)
        {
            continue;
        }

        masterStart = masterEventIt->start();
        masterEnd = masterEventIt->end();
        if (masterEventIt->start() >= editEvent.end())
        {
            // There is no overlap.
            // Automatically passes, and there are no other events to consider.
            return;
        }
        
        if ((masterEventIt->start() < editEvent.start())
            && (masterEventIt->end() > editEvent.start())
            && (masterEventIt->end() < editEvent.end()))
        {
            // editEvent overlaps the tail end of masterEvent
            editEvent.start(masterEventIt->end());
        } else if ((masterEventIt->start() > editEvent.start())
            && (masterEventIt->start() < editEvent.end())
            && (masterEventIt->end() > editEvent.end()))
        {
            // editEvent overlaps the head of masterEvent
            editEvent.end(masterEventIt->start());
        } else if ((masterEventIt->start() == editEvent.start())
            && (masterEventIt->end() == editEvent.end()))
        {
            throw EditorEditNotAllowed(
                "Edit must not match start and end positions");
        }
    }
}

} // end namespace CL
