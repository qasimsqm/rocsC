#include "core/common/parse_filename.h"

using namespace std;

namespace cmn {

ROCS_CORE_API void parse_filename(const std::string& filename,
                    std::string& song_number, std::string& song_name, UInt8& attacca_state)
{
    string fname(ex::path::basename_no_ext(filename));
    vector<string> tokens = {"@s", "@n", "@a", "@x"};
    auto parts = ex::tokenize_string(tokens.begin(), tokens.end(), fname);
    if (parts.size() == 4) {
        if ((parts[0] == "@s") && (parts[2] == "@n")) {
            song_number = parts[1];
            song_name = parts[3];
            attacca_state = 0;
            return;
        }
    } else if(parts.size() == 6) {
        if ((parts[0] == "@s") && (parts[2] == "@n") && (parts[4] == "@a")) {
            song_number = parts[1];
            song_name = parts[3];
            attacca_state = 1;
            return;
        } else if ((parts[0] == "@s") && (parts[2] == "@n") && (parts[4] == "@x")) {
            song_number = parts[1];
            song_name = parts[3];
            attacca_state = 2;
            return;
        }
    }
    // Execution reaches this point if a valid filename has not been detected.
    throw FilenameError(fname + " is not formatted correctly");

}



} // namespace cmn
