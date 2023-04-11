#include "core/changelog/read_marker.h"

using namespace std;
using namespace cmn;

namespace CL
{

/*
@b, @c, @m, @vs, @ve, @rs, @re, @f, @j

Revision 12/2013:
@c: applies to ShowReady, StageTracks, and FillHarmonic
@c1: applies to ShowReady
@c2: applies to StageTracks/FillHarmonic
@in: Applies to ShowReady and FillHarmonic
@in1: applies to ShowReady
@in2: applies to FillHarmonic
@out applies to ShowReady and FillHarmonic
@out1 applies to ShowReady.
@out2 applies to FillHarmonic
*/
set<string> CreateValidTokens()
{
    set<string> validTokens;
    validTokens.insert("@b");
    validTokens.insert("@c");
    validTokens.insert("@m");
    validTokens.insert("@re");
    validTokens.insert("@rs");
    validTokens.insert("@ve");
    validTokens.insert("@vs");
    validTokens.insert("@f");
    validTokens.insert("@j");
    
    // Added 12/2013
    validTokens.insert("@c1");
    validTokens.insert("@c2");
    validTokens.insert("@in");
    validTokens.insert("@in1");
    validTokens.insert("@in2");
    validTokens.insert("@out");
    validTokens.insert("@out1");
    validTokens.insert("@out2");
    
    // The following tokens are for proper handling of the first marker of every song,
    // which may contain these tokens.
    validTokens.insert("@a");
    validTokens.insert("@x");
    validTokens.insert("@s");
    validTokens.insert("@n");
    return validTokens;
}

TokensByEventCodeT CreateTokensByEventCode()
{
    TokensByEventCodeT map;
    set<string> str_set;

    str_set.insert("@b");
    map[codes::bar] = str_set;
    str_set.clear();

    str_set.insert({"@c", "@c1", "@c2"});
    map[codes::caesura] = str_set;
    str_set.clear();
    
    str_set.insert({"@in", "@in1", "@in2"});
    map[codes::inPoint] = str_set;
    str_set.clear();

    str_set.insert({"@out", "@out1", "@out2"});
    map[codes::outPoint] = str_set;
    str_set.clear();

    str_set.insert("@m");
    map[codes::marker]= str_set;
    str_set.clear();

    str_set.insert("@re");
    str_set.insert("@rs");
    map[codes::repeat] = str_set;
    str_set.clear();

    str_set.insert("@ve");
    str_set.insert("@vs");
    map[codes::vamp] = str_set;
    str_set.clear();

    str_set.insert("@j");
    str_set.insert("@f");
    map[codes::fermata] = str_set;

    return map;
}

set<string> validTokens(CreateValidTokens());
TokensByEventCodeT tokensByEventCode(CreateTokensByEventCode());

void ThrowOnArg(const string& arg, const std::string& token)
{
    if (arg.size())
    {
        throw ArgError(ex::format(
            "Unexpected argument found after %s",
            token.c_str()));
    }
}

void ThrowOnNoArg(const string& arg, const std::string& token)
{
    if (!arg.size())
    {
        throw ArgError(ex::format(
            "Expected argument found after %s. Found none.",
            token.c_str()));
    }
}

ROCSEvtPtrT gen_b(UInt32 abs_time, const string& s)
{
    ThrowOnNoArg(s, "@b");
    return ROCSEvtPtrT(new CustomBar(abs_time, s));
}

ROCSEvtPtrT gen_c(UInt32 abs_time, const string& s)
{
    ThrowOnArg(s, "@c"); 
    return ROCSEvtPtrT(new Caesura(abs_time, CAESURA_TARGETS_ALL));
}

ROCSEvtPtrT gen_c1(UInt32 absTime, const string& s)
{
    ThrowOnArg(s, "@c1");
    return ROCSEvtPtrT(new Caesura(absTime, CAESURA_TARGETS_SHOWREADY));
}

ROCSEvtPtrT gen_c2(UInt32 absTime, const string& s)
{
    ThrowOnArg(s, "@c2");
    return ROCSEvtPtrT(new Caesura(absTime, CAESURA_TARGETS_STAGETRACKS_FILLHARMONIC));
}

ROCSEvtPtrT gen_in(UInt32 absTime, const string& s)
{
    ThrowOnArg(s, "@in");
    return ROCSEvtPtrT(new InPoint(absTime, INOUT_TARGETS_ALL_MIDI));
}

ROCSEvtPtrT gen_in1(UInt32 absTime, const string& s)
{
    ThrowOnArg(s, "@in1");
    return ROCSEvtPtrT(new InPoint(absTime, INOUT_TARGETS_SHOWREADY));
}

ROCSEvtPtrT gen_in2(UInt32 absTime, const string& s)
{
    ThrowOnArg(s, "@in2");
    return ROCSEvtPtrT(new InPoint(absTime, INOUT_TARGETS_FILLHARMONIC));
}

ROCSEvtPtrT gen_out(UInt32 absTime, const string& s)
{
    ThrowOnArg(s, "@out");
    return ROCSEvtPtrT(new OutPoint(absTime, INOUT_TARGETS_ALL_MIDI));
}

ROCSEvtPtrT gen_out1(UInt32 absTime, const string& s)
{
    ThrowOnArg(s, "@out1");
    return ROCSEvtPtrT(new OutPoint(absTime, INOUT_TARGETS_SHOWREADY));
}

ROCSEvtPtrT gen_out2(UInt32 absTime, const string& s)
{
    ThrowOnArg(s, "@out2");
    return ROCSEvtPtrT(new OutPoint(absTime, INOUT_TARGETS_FILLHARMONIC));
}

ROCSEvtPtrT gen_m(UInt32 abs_time, const string& s)
{
    ThrowOnNoArg(s, "@m");
    return ROCSEvtPtrT(new Marker(abs_time, s));
}

ROCSEvtPtrT gen_vs(UInt32 abs_time, const string& s)
{
    SInt32 val = 0;
    if (s.size())
    {
        if (!ex::str_to_num(s, val))
        {
            throw ArgError("Expected integer argument or no argument for @vs");
        }
    }

    return ROCSEvtPtrT(new Vamp(abs_time, -1, val));
}

ROCSEvtPtrT gen_ve(UInt32 abs_time, const string& s)
{
    ThrowOnArg(s, "@ve");
    return ROCSEvtPtrT(new Vamp(-1, abs_time));
}

ROCSEvtPtrT gen_rs(UInt32 abs_time, const string& s)
{
    SInt32 val = 2;
    if (s.size())
    {
        if (!ex::str_to_num(s, val))
        {
            throw ArgError("Expected integer argument or no argument for @rs");
        }
    }

    return ROCSEvtPtrT(new Repeat(abs_time, -1, val));
}

ROCSEvtPtrT gen_re(UInt32 abs_time, const string& s)
{
    ThrowOnArg(s, "@re");
    return ROCSEvtPtrT(new Repeat(-1, abs_time));
}

ROCSEvtPtrT gen_f(UInt32 abs_time, const string& s)
{
    ThrowOnArg(s, "@f");
    return ROCSEvtPtrT(new Fermata(abs_time, -1));
}

ROCSEvtPtrT gen_j(UInt32 abs_time, const string& s)
{
    ThrowOnArg(s, "@j");
    return ROCSEvtPtrT(new Fermata(-1, abs_time));
}


GenerateEventFuncByTokenT create_gen_funcs()
{
    GenerateEventFuncByTokenT generateEventFuncByToken;
    generateEventFuncByToken["@b"] = &gen_b;
    generateEventFuncByToken["@c"] = &gen_c;
    generateEventFuncByToken["@c1"] = &gen_c1;
    generateEventFuncByToken["@c2"] = &gen_c2;
    generateEventFuncByToken["@in"] = &gen_in;
    generateEventFuncByToken["@in1"] = &gen_in1;
    generateEventFuncByToken["@in2"] = &gen_in2;
    generateEventFuncByToken["@out"] = &gen_out;
    generateEventFuncByToken["@out1"] = &gen_out1;
    generateEventFuncByToken["@out2"] = &gen_out2;
    generateEventFuncByToken["@m"] = &gen_m;
    generateEventFuncByToken["@vs"] = &gen_vs;
    generateEventFuncByToken["@ve"] = &gen_ve;
    generateEventFuncByToken["@rs"] = &gen_rs;
    generateEventFuncByToken["@re"] = &gen_re;
    generateEventFuncByToken["@f"] = &gen_f;
    generateEventFuncByToken["@j"] = &gen_j;
    return generateEventFuncByToken;
}

ROCS_CORE_API GenerateEventFuncByTokenT generateEventFuncByToken(create_gen_funcs());

ROCS_CORE_API 
list<cmn::ROCSEvtPtrT> read_marker(UInt32 abs_time, const std_midi::Marker& marker) 
{
    list<cmn::ROCSEvtPtrT> evts;
   
    if (!marker.message().size())
    {
        return evts;
    }

    // Separate marker message on spaces.
    auto spaceSeparatedBits = ex::split_string(marker.message());
    deque<string> spaceSeparatedDeque(
        spaceSeparatedBits.begin(),
        spaceSeparatedBits.end());
  
    map<string, string> argsByToken;
     
    if (!validTokens.count(spaceSeparatedDeque.front()))
    {
        throw ArgError(
            "Marker must start with symbol: " + marker.message());
    }
    
    string currentToken;

    // Assemble the tokens and arguments in a map 
    for (auto it: spaceSeparatedDeque)
    {
        if (validTokens.count(it))
        {
            currentToken = it;
            argsByToken[currentToken] = "";
        } else
        {
            argsByToken[currentToken].append(it + " "); 
        }
    }
    
    cout << "argsByToken:" << endl; 
    for (auto it: argsByToken)
    {
        cout << it.first << ": " << it.second << endl; 
    }
    
    for (auto& it: argsByToken)
    {
        it.second.assign(ex::strip(it.second)); 
    }

    if (argsByToken.count("@s"))
    {
        // Do not parse the first marker of the song. This could break
        // intended behavior, if someone tries to include valid marker data
        // in a marker string that begins with @s.
        return evts;
    }

    for (auto it: argsByToken)
    {
        if (generateEventFuncByToken.count(it.first))
        {
            try
            {
                evts.push_back(generateEventFuncByToken[it.first](
                    abs_time,
                    it.second));
            } catch(ArgError& e)
            {
                throw ArgError(ex::format("%s at %u", e.what(), abs_time));
            }
        } else
        {
            warnings << "Unknown token " << it.first << " at " << abs_time << ". "
                << "Full marker string: " << marker.message() << endl;
        }
    }

    // Sort evts so that 'End' events come before 'Start' events
    evts.sort([](const ROCSEvtPtrT &evt1, const ROCSEvtPtrT &evt2)->bool
        {
            if (evt1->code() == evt2->code())
            {
                if (codes::paired_event_codes.count(evt1->code()))
                {
                    // 
                    auto pairedEvent1 = dynamic_pointer_cast<CL::PairedEvent>(evt1);
                    return pairedEvent1->is_end(); 
                } else
                {
                    // Does not matter whether this returns true or false.
                    return false;
                }
            } else
            {
                return evt1->code() < evt2->code();
            }
        });
    
    return evts;
}

} // End namespace CL
