#pragma once

#include "core/win32/declspec.h"
#include "core/changelog/change_log.h"
#include "core/rocs_midi/show_data.h"

namespace CL
{

// Some useful methods for debugging.
void WriteRocsEvtPtrAsString(
    cmn::ROCSEvtPtrT ptr,
    std::ostream &outputStream,
    int indentSpaces=0);

void WriteRocsEvtPtrListAsString(
    const std::list<cmn::ROCSEvtPtrT> &rocsEvtPtrList,
    std::ostream &outputStream,
    int indentSpaces=0);

class HistoryItem
{
public:
    HistoryItem(
        cmn::ROCSEvtPtrT originalEvent, // can be nullptr for AddEvent actions
        cmn::ROCSEvtPtrT editedEvent)   // The new/edited event, can be nullptr for
                                        // for deleted events
        :
        originalEvent_(originalEvent),
        editedEvent_(editedEvent)
    {}
    
    /* OriginalEvent() returns nullptr if this HistoryItem represents an Add */
    cmn::ROCSEvtPtrT OriginalEvent() { return this->originalEvent_; }
    
    /* EditedEvent returns nullptr if this HistoryItem represents a Delete */
    cmn::ROCSEvtPtrT EditedEvent() { return this->editedEvent_; }
    
    void WriteString(std::ostream &outputStream, int indentSpaces=0) const
    {
        std::string indent(indentSpaces, ' ');
        outputStream << indent << "HistoryItem ";
        if (this->originalEvent_ == nullptr)
        {
            // This item represents a creation.
            outputStream << "created:" << std::endl;
            WriteRocsEvtPtrAsString(
                this->editedEvent_,
                outputStream,
                indentSpaces + 2);

        } else if (this->editedEvent_ == nullptr)
        {
            // Deletion
            outputStream << "deleted:" << std::endl;
            WriteRocsEvtPtrAsString(
                this->originalEvent_,
                outputStream,
                indentSpaces + 2);
        } else
        {
            // Modification
            outputStream << "modified:" << std::endl;
            outputStream  << indent << "  Original:" << std::endl;
            WriteRocsEvtPtrAsString(
                this->originalEvent_,
                outputStream,
                indentSpaces + 4);

            outputStream << '\n' << indent << "  Edited:" << std::endl;
            WriteRocsEvtPtrAsString(
                this->editedEvent_,
                outputStream,
                indentSpaces + 4);
        }
    }

private:
    cmn::ROCSEvtPtrT originalEvent_;
    cmn::ROCSEvtPtrT editedEvent_;
};



class EditorHistoryNoActionError : public std::out_of_range
{
public:
    EditorHistoryNoActionError(const std::string &what): std::out_of_range(what) {}
};

/* This class is not for direct use by clients */
class EditorHistory
{
public:
    typedef std::list<HistoryItem> HistoryActionT;
    typedef std::deque<HistoryActionT> HistoryDequeT;

public:
    EditorHistory() {}
   
    void BeginAction()
    {
        this->currentAction_.clear();
    }

    void EndAction()
    {
        this->undoHistory_.push_back(this->currentAction_);

        // We have stored a new Action, which invalidates the current redo
        // actions.
        this->redoHistory_.clear();
    }
    
    void AddNewEventToCurrentAction(const cmn::ROCSEvent &newEvent); 

    void AddEditedEventToCurrentAction(
        const cmn::ROCSEvent &originalEvent, 
        const cmn::ROCSEvent &editedEvent);
   
    void AddDeletedEventToCurrentAction(const cmn::ROCSEvent &deletedEvent);

    bool CanUndo() const { return (this->undoHistory_.size() != 0); }
   
    bool CanRedo() const { return (this->redoHistory_.size() != 0); }

    /* Returns the most recent Undo action, removes it from the undo deque, and
     * adds it to the redo deque. */
    HistoryActionT PopUndoAction();
    
    /* Returns the most recent Undo action, but leaves the deques unmodified. */ 
    HistoryActionT PeekUndoAction();

    /* Returns the most recent Redo action, removes it from the redo deque, and
     * adds it to the undo deque. */
    HistoryActionT PopRedoAction();

    /* Returns the most recent Redo action, but leaves the deques unmodified. */ 
    HistoryActionT PeekRedoAction();

private:
    HistoryActionT currentAction_;
    HistoryDequeT undoHistory_;
    HistoryDequeT redoHistory_;
};

} // end namespace CL
