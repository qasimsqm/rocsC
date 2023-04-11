#include "core/common/codes.h"

using namespace std;

namespace codes
{

CodeMapT create_names()
{
    CodeMapT code_map;

    // ChangeLog events
    code_map[vamp] = "Vamp";
    code_map[repeat] = "Repeat";
    code_map[cut] = "Cut";
    code_map[transpose] = "Transpose";
    code_map[tempo_scale] = "TempoScale";
    code_map[marker] = "Marker";
    code_map[bar] = "CustomBar";
    code_map[click] = "Click";
    code_map[caesura] = "Caesura";
    code_map[fermata] = "Fermata";

    // Timeline events
    code_map[tempo] = "Tempo";
    code_map[bars_beats] = "BarsBeats";
    code_map[key] = "Key";
    code_map[page_num] = "PageNum";
    code_map[meter] = "Meter";

    code_map[inPoint] = "inPoint";
    code_map[outPoint] = "outPoint";

    return code_map;
}

std::set<Byte> create_paired_event_codes()
{
    std::set<Byte> result(
        {
            vamp,
            repeat,
            cut,
            transpose,
            tempo_scale,
            click,
            fermata
        });
    return result;
}

ROCS_CORE_API CodeMapT code_names = create_names();
ROCS_CORE_API std::set<Byte> paired_event_codes(
    {
        vamp,
        repeat,
        cut,
        transpose,
        tempo_scale,
        click,
        fermata
    });
}
