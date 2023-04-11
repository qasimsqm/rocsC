#include "core/changelog/copy_rocs_event.h"
#include "exlib/format.h"

namespace CL
{

ROCS_CORE_API
cmn::ROCSEvtPtrT CopyRocsEvent(const cmn::ROCSEvent &evt)
{
    switch (evt.code())
    {
        case codes::vamp:
            return cmn::ROCSEvtPtrT(new Vamp(evt));
            break;
        case codes::repeat:
            return cmn::ROCSEvtPtrT(new Repeat(evt));
            break;
        case codes::cut:
            return cmn::ROCSEvtPtrT(new Cut(evt));
            break;
        case codes::transpose:
            return cmn::ROCSEvtPtrT(new Transpose(evt));
            break;
        case codes::tempo_scale:
            return cmn::ROCSEvtPtrT(new TempoScale(evt));
            break;
        case codes::marker:
            return cmn::ROCSEvtPtrT(new Marker(evt));
            break;
        case codes::click:
            return cmn::ROCSEvtPtrT(new Click(evt));
            break;
        case codes::caesura:
            return cmn::ROCSEvtPtrT(new Caesura(evt));
            break;
        case codes::fermata:
            return cmn::ROCSEvtPtrT(new Fermata(evt));
            break;
        default:
            throw std::logic_error(ex::format("Unsupported event code: %i", evt.code()));
    }
}

} // end namespace CL
