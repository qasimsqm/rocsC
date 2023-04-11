#include <UnitTest++/UnitTest++.h>
#include <sstream>
#include <cmath>
#include "changelog/cl_sequences.h"
#include "timeline/tl_sequences.h"
#include "objects_compare.h"

using namespace std;
using namespace TL;
using namespace CL;
using namespace rocs_testing;

SUITE(SequenceSerializeTests)
{
    TEST(TempoSeqTest)
    {
        TempoSeqT seq;
        for (int i = 400000, t = 0; i < 600000; i += 10000, t += 3840) {
            seq.push_back(Tempo(t, i));
        }
        stringstream ss;
        seq.WriteBinary(ss);
        TempoSeqT seq_(ss);
        CHECK(seq == seq_);
    }

    TEST(BarsBeatsSeqTest)
    {
        BarsBeatsSeqT seq;
        for (int bar_number = 0, t = 0; bar_number < 100; bar_number++, t += 7680) {
            for (int beat = 1; beat < 5; beat++) {
                seq.push_back(BarsBeats(t, bar_number, beat));
            }
        }

        stringstream ss;
        seq.WriteBinary(ss);
        BarsBeatsSeqT seq_(ss);
        CHECK(seq == seq_);
    }

    TEST(KeySeqTest)
    {
        KeySeqT seq;
        for (int sharps = -7, t = 0; sharps < 8; sharps++, t += 15360) {
            seq.push_back(Key(t, sharps, 0));
            seq.push_back(Key(t, sharps, 1));
        }
        stringstream ss;
        seq.WriteBinary(ss);
        KeySeqT seq_(ss);
        CHECK(seq == seq_);
    }

    TEST(PageNumSeqTest)
    {
        PageNumSeqT seq;
        for (int pagenum = 1, t = 0; pagenum < 301; pagenum++, t += 57600) {
            seq.push_back(PageNum(t, pagenum));
        }
        stringstream ss;
        seq.WriteBinary(ss);
        PageNumSeqT seq_(ss);
        CHECK(seq == seq_);
    }

    TEST(MeterSeqTest)
    {
        MeterSeqT seq;
        for (int t = 0, z = 0; z < 20; t += 1920, z++) {
            seq.push_back(Meter(t, 4 + pow(-1.0, z), 8, 24, 8));
        }
        stringstream ss;
        seq.WriteBinary(ss);
        MeterSeqT seq_(ss);
        CHECK(seq == seq_);
    }

    TEST(CaesuraSeqTest)
    {
        CaesuraSeqT seq;
        for (int t = 0; t < 100000; t += 7680) {
            seq.push_back(Caesura(t));
        }
        stringstream ss;
        seq.WriteBinary(ss);
        CaesuraSeqT seq_(ss);
        CHECK(seq == seq_);
    }

    TEST(MarkerSeqTest)
    {
        MarkerSeqT seq;
        stringstream marker_ss;
        marker_ss << "Marker ";
        for (int t = 0, int i = 1; i < 10; i++, t += 7680) {
            marker_ss << i << " ";
            seq.push_back(Marker(t, marker_ss.str()));
        }

        stringstream ss;
        cout << "writing marker sequence" << endl;
        seq.WriteBinary(ss);
        cout << "reading marker_sequence" << endl;
        MarkerSeqT seq_(ss);
        cout << "Checking marker sequences" << endl;
        CHECK(seq == seq_);
    }

    TEST(CutSeqTest)
    {
        CutSeqT seq;
        for (int t = 0; t < 100000; t += 7680) {
            seq.push_back(Cut(t, t + 3840));
        }

        stringstream ss;
        seq.WriteBinary(ss);
        CutSeqT seq_(ss);
        CHECK(seq == seq_);
    }

    TEST(VampSeqTest)
    {
        VampSeqT seq;
        for (int t = 0, i = 0; t < 100000; i++, t += 7680) {
            seq.push_back(Vamp(t, t + 3840, i));
        }

        stringstream ss;
        seq.WriteBinary(ss);
        VampSeqT seq_(ss);
        CHECK(seq == seq_);
    }

    TEST(RepeatSeqTest)
    {
        RepeatSeqT seq;
        for (int t = 0, i = 0; t < 100000; i++, t += 7680) {
            seq.push_back(Repeat(t, t + 3840, i));
        }

        stringstream ss;
        seq.WriteBinary(ss);
        RepeatSeqT seq_(ss);
        CHECK(seq == seq_);
    }

    TEST(TransposeSeqTest)
    {
        TransposeSeqT seq;
        for (int t = 0, i = -6; i < 7; i++, t += 15360) {
            seq.push_back(Transpose(t, t + 7680, i));
        }

        stringstream ss;
        seq.WriteBinary(ss);
        TransposeSeqT seq_(ss);
        CHECK(seq == seq_);
    }

    TEST(ClickSeqTest)
    {
        ClickSeqT seq;
        for (int t = 0, i = 1; i < 33 ; i++, t += 15360) {
            seq.push_back(Click(t, t + 7680, i));
        }

        stringstream ss;
        seq.WriteBinary(ss);
        ClickSeqT seq_(ss);
        CHECK(seq == seq_);
    }

    TEST(TempoScaleSeqTest)
    {
        TempoScaleSeqT seq;
        for (double t = 0.0, i = .50; i < 1.50 ; i += .01, t += 15360) {
            seq.push_back(TempoScale(t, t + 7680, i));
        }

        stringstream ss;
        seq.WriteBinary(ss);
        TempoScaleSeqT seq_(ss);
        CHECK(seq == seq_);
    }
}
