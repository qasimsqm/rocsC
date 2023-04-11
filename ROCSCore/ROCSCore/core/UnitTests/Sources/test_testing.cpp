/** A test of operator== overloads in rocs_events_compare.h **/

#include <UnitTest++/UnitTest++.h>
#include "events_compare.h"
#include "objects_compare.h"
#include "timeline/tl_sequences.h"
#include "changelog/cl_sequences.h"
#include <cmath>

using namespace rocs_midi;
using namespace TL;
using namespace CL;
using namespace rocs_testing;

SUITE(CompareTest)
{
    TEST(VoiceEvent_Equals)
    {
        VoiceEvent x(1920, 0x90, 64, 127);
        VoiceEvent y(1920, 0x90, 64, 127);
        CHECK(x == y);
    }

    TEST(VoiceEvent_NotEquals)
    {
        VoiceEvent x(1920, 0x90, 64, 127);
        VoiceEvent y(1920, 0x90, 63, 127);
        CHECK(!(x == y));
    }

    TEST(Tempo_Equals)
    {
        Tempo x(45600, 499001);
        Tempo y(45600, 499001);
        CHECK(x == y);
    }

    TEST(Tempo_NotEquals)
    {
        Tempo x(45600, 499001);
        Tempo y(45600, 499000);
        CHECK(!(x == y));
    }

    TEST(BarsBeats_Equals)
    {
        BarsBeats x(1920, "57c", 3);
        BarsBeats y(1920, "57c", 3);
        CHECK(x == y);
    }

    TEST(BarsBeats_NotEquals)
    {
        BarsBeats x(1920, "57c", 3);
        BarsBeats y(1921, "57c", 3);
        CHECK(!(x == y));
    }

    TEST(Key_Equals)
    {
        Key x(5621, 4, 1);
        Key y(5621, 4, 1);
        CHECK(x == y);
    }

    TEST(Key_NotEquals)
    {
        Key x(5621, 4, 1);
        Key y(5621, 4, 0);
        CHECK(!(x == y));
    }

    TEST(PageNum_Equals)
    {
        PageNum x(10000, 32);
        PageNum y(10000, 32);
        CHECK(x == y);
    }

    TEST(PageNum_NotEquals)
    {
        PageNum x(10000, 31);
        PageNum y(10000, 32);
        CHECK(!(x == y));
    }

    TEST(Meter_Equals)
    {
        Meter x(778889, 3, 8, 12, 6);
        Meter y(778889, 3, 8, 12, 6);
        CHECK(x == y);
    }

    TEST(Meter_NotEquals)
    {
        Meter x(778888, 3, 8, 12, 6);
        Meter y(778889, 3, 8, 12, 6);
        CHECK(!(x == y));
    }

    TEST(CustomBar_Equals)
    {
        CustomBar x(6799, "f984");
        CustomBar y(6799, "f984");
        CHECK(x == y);
    }

    TEST(CustomBar_NotEquals)
    {
        CustomBar x(6799, "g984");
        CustomBar y(6799, "f984");
        CHECK(!(x == y));
    }

    TEST(Marker_Equals)
    {
        Marker x(2, "Fusistance is Retile, Your Ass Will Be Laminated!");
        Marker y(2, "Fusistance is Retile, Your Ass Will Be Laminated!");
        CHECK(x == y);
    }

    TEST(Marker_NotEquals)
    {
        Marker x(2, "Fusistance is Retile, Your Ass Will Be Laminated!");
        Marker y(2, "Fusistance is Retile, Your Ass Will Be laminated!");
        CHECK(!(x == y));
    }

    TEST(Caesura_Equals)
    {
        Caesura x(45);
        Caesura y(45);
        CHECK(x == y);
    }

    TEST(Caesura_NotEquals)
    {
        Caesura x(45);
        Caesura y(46);
        CHECK(!(x == y));
    }

    TEST(Vamp_Equals)
    {
        Vamp x(65445, 78889, 3);
        Vamp y(65445, 78889, 3);
        CHECK(x == y);
    }

    TEST(Vamp_NotEquals)
    {
        Vamp x(65445, 78888, 3);
        Vamp y(65445, 78889, 3);
        CHECK(!(x == y));
    }

    TEST(Repeat_Equals)
    {
        Repeat x(44, 58, 2);
        Repeat y(44, 58, 2);
        CHECK(x == y);
    }

    TEST(Repeat_NotEquals)
    {
        Repeat x(44, 58, 2);
        Repeat y(44, 58, 3);
        CHECK(!(x == y));
    }

    TEST(Transpose_Equals)
    {
        Transpose x(99, 101, -4);
        Transpose y(99, 101, -4);
        CHECK(x == y);
    }

    TEST(Transpose_NotEquals)
    {
        Transpose x(99, 101, -3);
        Transpose y(99, 101, -4);
        CHECK(!(x == y));
    }

    TEST(TempoScale_Equals)
    {
        TempoScale x(1000, 1101, 1.34);
        TempoScale y(1000, 1101, 1.34);
        CHECK(x == y);
    }

    TEST(TempoScale_NotEquals)
    {
        TempoScale x(1000, 1101, 1.344);
        TempoScale y(1000, 1101, 1.34);
        CHECK(!(x == y));
    }
}

SUITE(SequenceCompareTests)
{
    TEST(VoiceTrack_Equals)
    {
        VoiceTrack trk;
        trk.group_id(3);
        trk.range_low(27);
        trk.range_high(88);
        trk.track_id(46);
        for (auto t = 0, n = 27; n < 88; t += 1920, n++) {
            trk.events().push_back(VoiceEvent(t, 0x90, n, 100));
        }
        CHECK(trk == trk);
    }

    TEST(TempoSeqT_Equals)
    {
        TempoSeqT seq1, seq2;
        for (auto i = 250000, t = 0; i < 750000; i += 10000, t += 1920) {
            seq1.push_back(Tempo(t, i));
            seq2.push_back(Tempo(t, i));
        }
        CHECK(seq1 == seq2);
    }

    TEST(MeterSeqT_Equals)
    {
        MeterSeqT seq1, seq2;
        for (auto t = 0, z = 0; z < 20; t += 1920, z++) {
            seq1.push_back(Meter(t, 4 + pow(-1, z), 8, 24, 8));
            seq2.push_back(Meter(t, 4 + pow(-1, z), 8, 24, 8));
        }
        CHECK(seq1 == seq2);
    }

    TEST(VampSeqT_Equals)
    {
        VampSeqT seq1, seq2;
        for (auto t = 0, z = 0; z < 17; t += 7680, z++) {
            seq1.push_back(Vamp(t, t + 3840, z));
            seq2.push_back(Vamp(t, t + 3840, z));
        }
        CHECK(seq1 == seq2);
    }

    TEST(TempoScaleSeqT_Equals)
    {
        TempoScaleSeqT seq1, seq2;
        for (auto t = 0.0, z = .75; z < 1.25; t += 15360, z += .05) {
            seq1.push_back(TempoScale(t, t + 7680, z));
            seq2.push_back(TempoScale(t, t + 7680, z));
        }
        CHECK(seq1 == seq2);
    }

    TEST(TempoSeqT_NotEquals)
    {
        TempoSeqT seq1, seq2;
        for (auto i = 250000, t = 0; i < 750000; i += 10000, t += 1920) {
            seq1.push_back(Tempo(t, i));
            seq2.push_back(Tempo(t, i));
        }
        seq1[14].microseconds(499004);
        CHECK(seq1 != seq2);
    }

    TEST(MeterSeqT_NotEquals)
    {
        MeterSeqT seq1, seq2;
        for (auto t = 0, z = 0; z < 20; t += 1920, z++) {
            seq1.push_back(Meter(t, 4 + pow(-1, z), 8, 24, 8));
            seq2.push_back(Meter(t, 4 + pow(-1, z), 8, 24, 8));
        }
        seq2[8].denominator(4);
        CHECK(seq1 != seq2);
    }

    TEST(VampSeqT_NotEquals)
    {
        VampSeqT seq1, seq2;
        for (auto t = 0, z = 0; z < 17; t += 7680, z++) {
            seq1.push_back(Vamp(t, t + 3840, z));
            seq2.push_back(Vamp(t, t + 3840, z));
        }
        seq2[11].end(900543654);
        CHECK(seq1 != seq2);
    }

    TEST(TempoScaleSeqT_NotEquals)
    {
        TempoScaleSeqT seq1, seq2;
        for (auto t = 0.0, z = .75; z < 1.25; t += 15360, z += .05) {
            seq1.push_back(TempoScale(t, t + 7680, z));
            seq2.push_back(TempoScale(t, t + 7680, z));
        }
        seq1[4].value(2.01);
        CHECK(seq1 != seq2);
    }
}

