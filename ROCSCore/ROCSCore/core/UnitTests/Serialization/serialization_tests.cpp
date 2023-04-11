// #include <UnitTest++/UnitTest++.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <exception>

#include "exlib/cxx11.h"
#ifdef CXX11
    #include <memory>
#else
    #include <tr1/memory>
#endif

#include "exlib/listdir.h"
#include "exlib/path.h"
#include "objects_compare.h"


#include "create_show.h" // from ROCS/packager

using namespace std;
using namespace rocs_testing;


int main()
{
    string outdir("/Users/Jonathan/Desktop/TestShows");
    string path("/Users/Jonathan/Desktop/HappyDaysMidi");
    vector<string> files;
    try {
        files = ex::listdir(path);
    } catch (exception& e) {
        cout << "ex::listdir threw exception: " << e.what() << endl;
    }

    vector<string> midi_files;
    remove_copy_if(files.begin(), files.end(), back_inserter(midi_files),
                    [] (const string& filename) -> bool
                    {
                        return ex::path::extension(filename) != ".mid";
                    });

    vector<string>::iterator z;
    for (z = midi_files.begin(); z < midi_files.end(); z++) {
        *z = ex::path::join(path, *z);
    }

    ShowCreator *sc;
    try {
        sc = new ShowCreator("Grease", outdir, midi_files);
    } catch (cmn::ROCSException& e) {
        cout << " Caught ROCSException: " << e.what() << endl;
        cout.flush();
        sleep(1);
        throw;
    } catch (logic_error& e) {
        cout << " Caught logic_error: " << e.what() << endl;
        cout.flush();
        sleep(1);
        throw;
    } catch (runtime_error& e) {
        cout << " Caught runtime_error: " << e.what() << endl;
        cout.flush();
        sleep(1);
        throw;
    } catch (exception& e) {
        cout << " Caught exception: " << e.what() << endl;
        cout.flush();
        sleep(1);
        throw;
    } catch (...) {
        cout << " Caught unknown exception: " << endl;
        cout.flush();
        sleep(1);
        throw;
    }

    cout << "Ready to save files..." << endl;

    string sr_name(sc->save_show_ready());
    cout << "saved " << sr_name << endl;
    string fh_name(sc->save_fill_harmonic());
    cout << "saved " << fh_name << endl;
    string cl_name(sc->save_change_log());
    cout << "saved " << cl_name << endl;

    ifstream sr_ifs(sr_name.c_str(), fstream::in | fstream::binary);
    ifstream fh_ifs(fh_name.c_str(), fstream::in | fstream::binary);
    ifstream cl_ifs(cl_name.c_str(), fstream::in | fstream::binary);

    rocs_midi::ShowDataPtrT sr_from_stream;
    rocs_midi::ShowDataPtrT fh_from_stream;
    CL::ChangeLogPtrT cl_from_stream;

    try {
        sr_from_stream = rocs_midi::ShowDataPtrT(new rocs_midi::ShowData(sr_ifs));
        cout << "loaded from sr_ifs..." << endl;
        fh_from_stream = rocs_midi::ShowDataPtrT(new rocs_midi::ShowData (fh_ifs));
        cout << "loaded from fh_ifs..." << endl;
        cl_from_stream = CL::ChangeLogPtrT(new CL::ChangeLog(cl_ifs));
        cout << "loaded from cl_ifs..." << endl;
    } catch(exception& e) {
        cout << "Caught exception: " << e.what() << endl;
        throw;
    } catch(...) {
        cout << "Caught unknown exception type." << endl;
        throw;
    }


    cout << "Checking the equals checks..." << endl;
    if (sc->sd() == sc->sd()) {
        cout << "ShowData operator== is working." << endl;
    } else {
        cout << "ShowData operator== is broken." << endl;
    }

    if (sc->cl() == sc->cl()) {
        cout << "ChangeLog operator== is working." << endl;
    } else {
        cout << "ChangeLog operator== is broken." << endl;
    }

    if (!(sc->sd() == *sr_from_stream)) {
        cout << "sr_from_stream failed to pass equals check" << endl;
        return -1;
    }
    if (!(sc->sd() == *fh_from_stream)) {
        cout << "fh_from_stream failed to pass equals check" << endl;
        return -1;
    }
    if (!(sc->cl() == *cl_from_stream)) {
        cout << "cl_from_stream failed to pass equals check" << endl;
        return -1;
    }

    cout << "3 tests passed" << endl;
    return 0;
}
