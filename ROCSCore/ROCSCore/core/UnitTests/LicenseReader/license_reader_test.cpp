#include "LicenseReader/license_reader.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cassert>

using namespace std;

int main(int argc, char **argv)
{
    assert(argc == 2);
    rocs_sec::LicenseData data;
    try {
        data = rocs_sec::read_license(argv[1]);
    } catch(exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    cout << "Key: ";
    // cout.setf(ios_base::showbase|ios_base::hex);
    cout << showbase << hex;
    ostream_iterator<unsigned short> out_it(cout, " ");
    copy(data.key.begin(), data.key.end(), out_it);
    cout << endl << "data_iv: ";
    copy(data.data_iv.begin(), data.data_iv.end(), out_it);
    cout << endl << "log_iv: ";
    copy(data.log_iv.begin(), data.log_iv.end(), out_it);
    cout << endl;
    return 0;
}
