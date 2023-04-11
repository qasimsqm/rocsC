#include <UnitTest++/UnitTest++.h>
#include "common/warnings.h"
#include "changelog/read_marker.h"
#include "changelog/cl_events.h"
#include "std_midi/meta_messages.h"
#include "events_compare.h"
#include <string>
#include <vector>
#include <iostream>
#include <exception>

using namespace std;
using namespace cmn;
using namespace rocs_testing;

struct TestData
{
    TestData(UInt32 abs_time): abs_time(abs_time) {}
    UInt32 abs_time;
    vector<ROCSEvtPtrT>   manual_evts;
    vector<ROCSEvtPtrT>   create_evts;
    std_midi::Marker      marker;
    bool test() {
        try {
            create_evts = CL::read_marker(abs_time, marker);
        } catch(CL::ReadMarkerException& e) {
            cout << "ReadMarkerException& : " << e.what() << endl;
            throw;
        } catch (...) {
            cout << "Unknown Exception!" << endl;
            return false;
        }
        // cout << "Escaped from try block without catching an exception" << endl;
        vector<ROCSEvtPtrT>::iterator it1, it2;
        for (   it1=create_evts.begin(), it2=manual_evts.begin();
                it2 < manual_evts.end();
                it1++, it2++                                        )
        {
            if (!(*it1 == *it2)) {
                (*it1)->WriteString(cout);
                cout << " does not compare equal to ";
                (*it2)->WriteString(cout);
                cout << endl;
                return false;
            }
        }
        return true;
    }
};


SUITE(ReadMarkerTest)
{
    TEST(b)
    {
        TestData td(54360);
        td.marker.message("@b 45a");
        ROCSEvtPtrT evt(new CL::CustomBar(td.abs_time, "45a"));
        td.manual_evts.push_back(evt);
        CHECK(td.test());
    }

    TEST(b_with_error)
    {
        TestData td(54360);
        td.marker.message("@b");
        ROCSEvtPtrT evt(new CL::CustomBar(td.abs_time, ""));
        td.manual_evts.push_back(evt);
        CHECK_THROW(td.test(), CL::ArgError);
    }

    TEST(c)
    {
        TestData td(1920);
        td.marker.message("@c");
        ROCSEvtPtrT evt(new CL::Caesura(td.abs_time));
        td.manual_evts.push_back(evt);
        CHECK(td.test());
    }

    TEST(c_with_error)
    {
        TestData td(1920);
        td.marker.message("@c 6");
        ROCSEvtPtrT evt(new CL::Caesura(td.abs_time));
        td.manual_evts.push_back(evt);
        CHECK_THROW(td.test(), CL::ArgError);
    }

    TEST(m)
    {
        TestData td(987652);
        td.marker.message("@m Your Ass Will Be Laminated");
        ROCSEvtPtrT evt(new CL::Marker(td.abs_time, "Your Ass Will Be Laminated"));
        td.manual_evts.push_back(evt);
        CHECK(td.test());
    }

    TEST(m_with_error)
    {
        TestData td(987652);
        td.marker.message("@m  ");
        ROCSEvtPtrT evt(new CL::Marker(td.abs_time, "Your Ass Will Be Laminated"));
        td.manual_evts.push_back(evt);
        CHECK_THROW(td.test(), CL::ArgError);
    }

    TEST(vs_without_value)
    {
        TestData td(1920);
        td.marker.message("@vs");
        ROCSEvtPtrT evt(new CL::Vamp(td.abs_time, -1));
        td.manual_evts.push_back(evt);
        CHECK(td.test());
    }

    TEST(vs_with_value)
    {
        TestData td(1920);
        td.marker.message("@vs 6");
        ROCSEvtPtrT evt(new CL::Vamp(td.abs_time, -1, 6));
        td.manual_evts.push_back(evt);
        CHECK(td.test());
    }

    TEST(vs_with_error)
    {
        TestData td(1920);
        td.marker.message("@vs p");
        ROCSEvtPtrT evt(new CL::Vamp(td.abs_time, -1, 6));
        td.manual_evts.push_back(evt);
        CHECK_THROW(td.test(), CL::ArgError);
    }

    TEST(ve)
    {
        TestData td(4440);
        td.marker.message("@ve");
        ROCSEvtPtrT evt(new CL::Vamp(-1, td.abs_time));
        td.manual_evts.push_back(evt);
        CHECK(td.test());
    }

    TEST(ve_with_error)
    {
        TestData td(4440);
        td.marker.message("@ve 5");
        ROCSEvtPtrT evt(new CL::Vamp(-1, td.abs_time));
        td.manual_evts.push_back(evt);
        CHECK_THROW(td.test(), CL::ArgError);
    }

    TEST(rs_without_value)
    {
        TestData td(1920);
        td.marker.message("@rs");
        ROCSEvtPtrT evt(new CL::Repeat(td.abs_time, -1));
        td.manual_evts.push_back(evt);
        CHECK(td.test());

    }

    TEST(rs_with_value)
    {
        TestData td(1920);
        td.marker.message("@rs 3");
        ROCSEvtPtrT evt(new CL::Repeat(td.abs_time, -1, 3));
        td.manual_evts.push_back(evt);
        CHECK(td.test());
    }

    TEST(rs_with_error)
    {
        TestData td(1920);
        td.marker.message("@rs m");
        ROCSEvtPtrT evt(new CL::Repeat(td.abs_time, -1, 3));
        td.manual_evts.push_back(evt);
        CHECK_THROW(td.test(), CL::ArgError);
    }

    TEST(re)
    {
        TestData td(71040);
        td.marker.message("@re");
        ROCSEvtPtrT evt(new CL::Repeat(-1, td.abs_time));
        td.manual_evts.push_back(evt);
        CHECK(td.test());
    }

    TEST(re_with_error)
    {
        TestData td(71040);
        td.marker.message("@re 4");
        ROCSEvtPtrT evt(new CL::Repeat(-1, td.abs_time));
        td.manual_evts.push_back(evt);
        CHECK_THROW(td.test(), CL::ArgError);
    }

    TEST(Group1)
    {
        TestData td(1920);
        td.marker.message("@b 6c @vs 3 @rs @m Fusistance is retile!");
        ROCSEvtPtrT evt1(new CL::CustomBar(td.abs_time, "6c"));
        td.manual_evts.push_back(evt1);
        ROCSEvtPtrT evt2(new CL::Vamp(td.abs_time, -1, 3));
        td.manual_evts.push_back(evt2);

        ROCSEvtPtrT evt3(new CL::Repeat(td.abs_time, -1));
        td.manual_evts.push_back(evt3);

        ROCSEvtPtrT evt4(new CL::Marker(td.abs_time, "Fusistance is retile!"));
        td.manual_evts.push_back(evt4);
        CHECK(td.test());
    }

    TEST(Group1_with_error)
    {
        TestData td(1920);
        td.marker.message("@z 6c @vs 3 @rs @m Fusistance is retile!");

        ROCSEvtPtrT evt2(new CL::Vamp(td.abs_time, -1, 3));
        td.manual_evts.push_back(evt2);

        ROCSEvtPtrT evt3(new CL::Repeat(td.abs_time, -1));
        td.manual_evts.push_back(evt3);

        ROCSEvtPtrT evt4(new CL::Marker(td.abs_time, "Fusistance is retile!"));
        td.manual_evts.push_back(evt4);

        CHECK(td.test());
        cout << warnings.str() << endl;
    }

    TEST(Group2)
    {
        TestData td(460004);
        td.marker.message("@re @ve @c @vs");

        ROCSEvtPtrT evt1(new CL::Repeat(-1, td.abs_time));
        td.manual_evts.push_back(evt1);

        ROCSEvtPtrT evt2(new CL::Vamp(-1, td.abs_time));
        td.manual_evts.push_back(evt2);

        ROCSEvtPtrT evt3(new CL::Caesura(td.abs_time));
        td.manual_evts.push_back(evt3);

        ROCSEvtPtrT evt4(new CL::Vamp(td.abs_time, -1));
        td.manual_evts.push_back(evt4);

        CHECK(td.test());
    }

    TEST(Group2_with_error)
    {
        TestData td(460004);
        td.marker.message("@re @ve @c 16 @vs");
        ROCSEvtPtrT evt1(new CL::Repeat(-1, td.abs_time));
        td.manual_evts.push_back(evt1);

        ROCSEvtPtrT evt2(new CL::Vamp(-1, td.abs_time));
        td.manual_evts.push_back(evt2);

        ROCSEvtPtrT evt3(new CL::Caesura(td.abs_time));
        td.manual_evts.push_back(evt3);

        ROCSEvtPtrT evt4(new CL::Vamp(td.abs_time, -1));
        td.manual_evts.push_back(evt4);

        CHECK_THROW(td.test(), CL::ArgError);
    }

    TEST(Group3)
    {
        TestData td(1919);
        td.marker.message("@m Fools Rush In...  @b 38aa @re @vs 16 @c");

        ROCSEvtPtrT evt1(new CL::Marker(td.abs_time, "Fools Rush In..."));
        td.manual_evts.push_back(evt1);

        ROCSEvtPtrT evt2(new CL::CustomBar(td.abs_time, "38aa"));
        td.manual_evts.push_back(evt2);

        ROCSEvtPtrT evt3(new CL::Repeat(-1, td.abs_time));
        td.manual_evts.push_back(evt3);

        ROCSEvtPtrT evt4(new CL::Vamp(td.abs_time, -1, 16));
        td.manual_evts.push_back(evt4);

        ROCSEvtPtrT evt5(new CL::Caesura(td.abs_time));
        td.manual_evts.push_back(evt5);

        CHECK(td.test());
    }

    TEST(Group3_with_error)
    {
        TestData td(1919);
        td.marker.message("@m Fools Rush In...  @b 38aa @re vs 16 @c");
        ROCSEvtPtrT evt1(new CL::Marker(td.abs_time, "Fools Rush In..."));
        td.manual_evts.push_back(evt1);

        ROCSEvtPtrT evt2(new CL::CustomBar(td.abs_time, "38aa"));
        td.manual_evts.push_back(evt2);

        ROCSEvtPtrT evt3(new CL::Repeat(-1, td.abs_time));
        td.manual_evts.push_back(evt3);

        ROCSEvtPtrT evt4(new CL::Vamp(td.abs_time, -1, 16));
        td.manual_evts.push_back(evt4);

        ROCSEvtPtrT evt5(new CL::Caesura(td.abs_time));
        td.manual_evts.push_back(evt5);

        CHECK_THROW(td.test(), CL::ArgError);
    }
}
