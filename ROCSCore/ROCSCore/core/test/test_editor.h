#pragma once
#include "core/changelog/editor.h"

namespace CL
{

class TestEditor
    :
    public Editor
{
public:
    TestEditor(ChangeLog &changeLog, const rocs_midi::ShowData &showData)
        :
        Editor(changeLog, showData)
    {};
    
public:
    // Public Access to protected methods for testing
    
    cmn::ROCSEvent &TestAddEvent_(const cmn::ROCSEvent &newEvent)
    {
        return this->AddEvent_(newEvent);
    }
    
    void TestAlignEventWithBeat_(cmn::ROCSEvent &event)
    {
        this->AlignEventWithBeat_(event);
    }

    void TestCreateBeatTickIndices_()
    {
        this->CreateBeatTickIndices_();
    }

    void TestDeleteEvent_(const cmn::ROCSEvent &eventToDelete)
    {
        this->DeleteEvent_(eventToDelete); 
    }

    cmn::ROCSEvent *TestFindEvent_(const cmn::ROCSEvent &eventToFind)
    {
        return this->FindEvent_(eventToFind);
    }
    
    void TestFinishEdit_(const cmn::ROCSEvent &evt)
    {
        this->FinishEdit_(evt);
    }

    EditorHistory &TestGetActiveHistory()
    {
        return this->GetActiveHistory();
    }

    const TL::Timeline &TestGetActiveTimeline() const
    {
        return this->GetActiveTimeline();
    }

    UInt32 TestGetBeatAlignedTickIndex_(UInt32 tickIndex)
    {
        return this->GetBeatAlignedTickIndex_(tickIndex);
    }

    void TestPunchHoleForCut_(const CL::Cut &cutEvent)
    {
        return this->PunchHoleForCut_(cutEvent);
    }
       
    const cmn::ROCSEvent & TestSortEditedVector_(const cmn::ROCSEvent &editedEvent)
    {
        return this->SortEditedVector_(editedEvent);
    }

    // A dispatcher to all validation work.
    void TestValidateEdit_(cmn::ROCSEvent &newEvent)
    {
        this->ValidateEdit_(newEvent);
    }

    void TestValidateCut_(cmn::ROCSEvent &newEvent)
    {
        this->ValidateCut_(newEvent);
    }

    void TestValidatePairedExceptCut_(cmn::ROCSEvent &newEvent)
    {
        this->ValidatePairedExceptCut_(newEvent);
    }

    void TestValidateVampOrRepeat_(cmn::ROCSEvent &newEvent)
    {
        this->ValidateVampOrRepeat_(newEvent);
    }

    void TestValidateClick_(cmn::ROCSEvent &newEvent)
    {
        this->ValidateClick_(newEvent);
    }

    void TestValidateNonPairedEvent_(cmn::ROCSEvent &newEvent)
    {
        this->ValidateNonPairedEvent_(newEvent);
    }

    CL::ChangeLog &GetChangeLog()
    {
        return this->changeLog_;
    }
};

} // end namespace CL
