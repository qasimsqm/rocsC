#pragma once

#include "core/win32/declspec.h"

#include <map>
#include <vector>
#include <string>

#include "exlib/xplatform_types.h"

class ROCS_CORE_API KeySigValue {
public:
    KeySigValue(): order(0) {}
    KeySigValue(const std::string key_name, SInt16 order)
        :
        key_name(key_name),
        order(order)
    {}
   
    MSC_DISABLE_WARNING(4251);
    std::string key_name;
    MSC_RESTORE_WARNING(4251);
    SInt16 order;
};

typedef std::map<SInt16, KeySigValue> TKeySigMap;
typedef std::map<SInt16, SInt16> TKeyOrdMap;

ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<std::map<SInt16, KeySigValue>>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<std::map<SInt16, SInt16>>);

struct ROCS_CORE_API KeySigs
{
    KeySigs();
    std::vector<TKeySigMap> key_sigs;
};


struct ROCS_CORE_API KeyOrds
{
    KeyOrds();
    std::vector<TKeyOrdMap> key_ords;
};


class ROCS_CORE_API Keys
{
public:
    static std::string keyNameFromSig(const SInt16 mode, const SInt16 sharps);
    static SInt16 orderFromSig(const SInt16 mode, const SInt16 sharps);
    static std::string keyNameFromOrder(const SInt16 mode, const SInt16 order);
    static std::string getTransposedName(const SInt16 mode, const SInt16 sharps, const SInt16 transpose);

    static KeySigs ks;
    static KeyOrds ko;
};
