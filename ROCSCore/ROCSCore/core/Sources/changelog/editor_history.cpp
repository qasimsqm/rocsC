#include "core/changelog/editor_history.h"
#include "core/changelog/copy_rocs_event.h"

namespace CL
{

void WriteRocsEvtPtrAsString(
    cmn::ROCSEvtPtrT ptr,
    std::ostream &outputStream,
    int indentSpaces)
{
    std::string indent(indentSpaces, ' ');
    outputStream << indent << "cmn::ROCSEvtPtrT<" << codes::code_names[ptr->code()]
        << ">:" << std::endl;
    outputStream << indent << "  code = " << ptr->code() << std::endl;
    outputStream << indent << "  start = " << ptr->start() << std::endl;
    outputStream << indent << "  end = " << ptr->end() << std::endl;
    outputStream << indent << "  value = " << ptr->value();
}

void WriteRocsEvtPtrListAsString(
    const std::list<cmn::ROCSEvtPtrT> &rocsEvtPtrList,
    std::ostream &outputStream,
    int indentSpaces)
{   
    int listIndex = 0;
    std::string indent(indentSpaces, ' ');

    outputStream << indent << "{" << std::endl;
    for (auto it: rocsEvtPtrList)
    {
        outputStream << indent << "  [" << listIndex << "] = " << std::endl;
        WriteRocsEvtPtrAsString(it, outputStream, indentSpaces + 4);
        outputStream << "," << std::endl;
        listIndex++;
    }
    outputStream << indent << "}";
}

void EditorHistory::AddNewEventToCurrentAction(const cmn::ROCSEvent &newEvent)
{
    // There is no originalEvent, just the new editedEvent
    this->currentAction_.push_back(HistoryItem(
        cmn::ROCSEvtPtrT(nullptr),
        CopyRocsEvent(newEvent)));
}

void EditorHistory::AddEditedEventToCurrentAction(
    const cmn::ROCSEvent &originalEvent, 
    const cmn::ROCSEvent &editedEvent)
{
    this->currentAction_.push_back(HistoryItem(
        CopyRocsEvent(originalEvent),
        CopyRocsEvent(editedEvent)));
}

void EditorHistory::AddDeletedEventToCurrentAction(const cmn::ROCSEvent &deletedEvent)
{   
    // There is no editedEvent, just the originalEvent that was deleted.
    this->currentAction_.push_back(HistoryItem(
        CopyRocsEvent(deletedEvent),
        cmn::ROCSEvtPtrT(nullptr)));
}

EditorHistory::HistoryActionT EditorHistory::PopUndoAction()
{
    if (!this->CanUndo())
    {
        throw EditorHistoryNoActionError("There are no actions to undo.");
    }

    HistoryActionT result(this->undoHistory_.back());
    this->redoHistory_.push_back(result);
    this->undoHistory_.pop_back();
    return result;
}

EditorHistory::HistoryActionT EditorHistory::PeekUndoAction()
{
    if (!this->CanUndo())
    {
        throw EditorHistoryNoActionError("There are no actions to undo.");
    }
    
    return this->undoHistory_.back();
}

EditorHistory::HistoryActionT EditorHistory::PopRedoAction()
{
    if (!this->CanRedo())
    {
        throw EditorHistoryNoActionError("There are no actions to redo.");
    }
    
    HistoryActionT result(this->redoHistory_.back());
    this->undoHistory_.push_back(result);
    this->redoHistory_.pop_back();
    return result;
}

EditorHistory::HistoryActionT EditorHistory::PeekRedoAction()
{
    if (!this->CanRedo())
    {
        throw EditorHistoryNoActionError("There are no actions to redo.");
    }
    
    return this->redoHistory_.back();
}

} // end namespace CL
