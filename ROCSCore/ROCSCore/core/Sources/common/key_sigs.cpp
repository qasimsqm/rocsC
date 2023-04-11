#include "core/common/key_sigs.h"

using namespace std;

inline KeySigs::KeySigs() {
    // Major Keys
    TKeySigMap k_maj;
    TKeySigMap k_min;

    k_maj[-7] = KeySigValue("Cb Major", 11);
    k_maj[-6] = KeySigValue("Gb Major", 6);
    k_maj[-5] = KeySigValue("Db Major", 1);
    k_maj[-4] = KeySigValue("Ab Major", 8);
    k_maj[-3] = KeySigValue("Eb Major", 3);
    k_maj[-2] = KeySigValue("Bb Major", 10);
    k_maj[-1] = KeySigValue("F Major", 5);
    k_maj[ 0] = KeySigValue("C Major", 0);
    k_maj[ 1] = KeySigValue("G Major", 7);
    k_maj[ 2] = KeySigValue("D Major", 2);
    k_maj[ 3] = KeySigValue("A Major", 9);
    k_maj[ 4] = KeySigValue("E Major", 4);
    k_maj[ 5] = KeySigValue("B Major", 11);
    k_maj[ 6] = KeySigValue("F# Major", 6);
    k_maj[ 7] = KeySigValue("C# Major", 1);

    // Minor Keys
    k_min[-7] = KeySigValue("Ab Minor", 8);
    k_min[-6] = KeySigValue("Eb Minor", 3);
    k_min[-5] = KeySigValue("Bb Minor", 10);
    k_min[-4] = KeySigValue("F Minor", 5);
    k_min[-3] = KeySigValue("C Minor", 0);
    k_min[-2] = KeySigValue("G Minor", 7);
    k_min[-1] = KeySigValue("D Minor", 2);
    k_min[ 0] = KeySigValue("A Minor", 9);
    k_min[ 1] = KeySigValue("E Minor", 4);
    k_min[ 2] = KeySigValue("B Minor", 11);
    k_min[ 3] = KeySigValue("F# Minor", 6);
    k_min[ 4] = KeySigValue("C# Minor", 1);
    k_min[ 5] = KeySigValue("G# Minor", 8);
    k_min[ 6] = KeySigValue("D# Minor", 3);
    k_min[ 7] = KeySigValue("A# Minor", 10);

    key_sigs.push_back(k_maj);
    key_sigs.push_back(k_min);
}


inline KeyOrds::KeyOrds() {
    // Major Keys
    TKeyOrdMap k_maj;
    TKeyOrdMap k_min;

    k_maj[0] =  0;
    k_maj[1] = -5;
    k_maj[2] =  2;
    k_maj[3] = -3;
    k_maj[4] =  4;
    k_maj[5] = -1;
    k_maj[6] = -6;
    k_maj[7] =  1;
    k_maj[8] = -4;
    k_maj[9] =  3;
    k_maj[10] = -2;
    k_maj[11] =  5;

    // Minor Keys
    k_min[0] = -3;
    k_min[1] =  4;
    k_min[2] = -1;
    k_min[3] =  6;
    k_min[4] =  1;
    k_min[5] = -4;
    k_min[6] =  3;
    k_min[7] = -2;
    k_min[8] =  5;
    k_min[9] =  0;
    k_min[10] = -5;
    k_min[11] =  2;

    key_ords.push_back(k_maj);
    key_ords.push_back(k_min);
}


#pragma mark -
#pragma mark Keys Implementation


string Keys::keyNameFromSig(const SInt16 mode, const SInt16 sharps)
{
    return ks.key_sigs[mode][sharps].key_name;
}

SInt16 Keys::orderFromSig(const SInt16 mode, const SInt16 sharps)
{
    return ks.key_sigs[mode][sharps].order;
}

string Keys::keyNameFromOrder(const SInt16 mode, const SInt16 order)
{
    SInt16 sharps = ko.key_ords[mode][order];
    return keyNameFromSig(mode, sharps);
}

string Keys::getTransposedName(const SInt16 mode, const SInt16 sharps, const SInt16 transpose)
{
    if (transpose % 12 == 0) return keyNameFromSig(mode, sharps);
    SInt16 order = orderFromSig(mode, sharps);
    order += transpose;
    order %= 12;
    if (order < 0) order += 12;
    return keyNameFromOrder(mode, order);
}


#pragma mark -
#pragma mark Initialize Static Variables


KeySigs Keys::ks;
KeyOrds Keys::ko;

