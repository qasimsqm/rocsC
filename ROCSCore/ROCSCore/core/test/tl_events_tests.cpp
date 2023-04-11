#include "gtest/gtest.h"
#include "core/timeline/tl_events.h"

using namespace TL;

#define OPERATOR_EQ_TESTER(eventType) \
TEST(OperatorEQ, eventType) \
{ \
    UInt32 testTickIndex = 20000; \
    TL::eventType event(testTickIndex); \
    EXPECT_TRUE(event == testTickIndex); \
    EXPECT_FALSE(event == testTickIndex + 1); \
}

#define OPERATOR_NE_TESTER(eventType) \
TEST(OperatorNE, eventType) \
{ \
    UInt32 testTickIndex = 20000; \
    TL::eventType event(testTickIndex); \
    EXPECT_FALSE(event != testTickIndex); \
    EXPECT_TRUE(event != testTickIndex + 1); \
}

#define OPERATOR_LT_TESTER(eventType) \
TEST(OperatorLT, eventType) \
{ \
    UInt32 testTickIndex = 20000; \
    TL::eventType event(testTickIndex); \
    EXPECT_TRUE(event < testTickIndex + 1); \
    EXPECT_TRUE(testTickIndex - 1 < event); \
    EXPECT_FALSE(event < testTickIndex); \
    EXPECT_FALSE(testTickIndex < event); \
}


#define OPERATOR_GT_TESTER(eventType) \
TEST(OperatorGT, eventType) \
{ \
    UInt32 testTickIndex = 20000; \
    TL::eventType event(testTickIndex); \
    EXPECT_TRUE(event > testTickIndex - 1); \
    EXPECT_FALSE(event > testTickIndex); \
}

#define OPERATOR_LE_TESTER(eventType) \
TEST(OperatorLE, eventType) \
{ \
    UInt32 testTickIndex = 20000; \
    TL::eventType event(testTickIndex); \
    EXPECT_TRUE(event <= testTickIndex); \
    EXPECT_TRUE(event <= testTickIndex + 1); \
    EXPECT_FALSE(event <= testTickIndex - 1); \
}

#define OPERATOR_GE_TESTER(eventType) \
TEST(OperatorGE, eventType) \
{ \
    UInt32 testTickIndex = 20000; \
    TL::eventType event(testTickIndex); \
    EXPECT_TRUE(event >= testTickIndex); \
    EXPECT_TRUE(event >= testTickIndex - 1); \
    EXPECT_FALSE(event >= testTickIndex + 1); \
}

OPERATOR_EQ_TESTER(Tempo);
OPERATOR_EQ_TESTER(BarsBeats);
OPERATOR_EQ_TESTER(Key);
OPERATOR_EQ_TESTER(PageNum);
OPERATOR_EQ_TESTER(Meter);

OPERATOR_NE_TESTER(Tempo);
OPERATOR_NE_TESTER(BarsBeats);
OPERATOR_NE_TESTER(Key);
OPERATOR_NE_TESTER(PageNum);
OPERATOR_NE_TESTER(Meter);

OPERATOR_LT_TESTER(Tempo);
OPERATOR_LT_TESTER(BarsBeats);
OPERATOR_LT_TESTER(Key);
OPERATOR_LT_TESTER(PageNum);
OPERATOR_LT_TESTER(Meter);

OPERATOR_GT_TESTER(Tempo);
OPERATOR_GT_TESTER(BarsBeats);
OPERATOR_GT_TESTER(Key);
OPERATOR_GT_TESTER(PageNum);
OPERATOR_GT_TESTER(Meter);

OPERATOR_LE_TESTER(Tempo);
OPERATOR_LE_TESTER(BarsBeats);
OPERATOR_LE_TESTER(Key);
OPERATOR_LE_TESTER(PageNum);
OPERATOR_LE_TESTER(Meter);

OPERATOR_GE_TESTER(Tempo);
OPERATOR_GE_TESTER(BarsBeats);
OPERATOR_GE_TESTER(Key);
OPERATOR_GE_TESTER(PageNum);
OPERATOR_GE_TESTER(Meter);
