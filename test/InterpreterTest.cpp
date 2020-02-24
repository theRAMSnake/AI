#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "seg/interpreter.hpp"
#include "seg/functions.hpp"

class InterpreterTest
{
public:
   InterpreterTest()
   : mMem(16)
   , mPrimitives(seg::createPrimitivesLibrary())
   , mExtensions(seg::createExtensionLibrary())
   , mInt(mMem, mInput, mPrimitives, mExtensions, seg::FunctionLibrary())
   {
        
   }

protected:
    double mInput[5];
    seg::Memory mMem;
    seg::FunctionLibrary mPrimitives;
    seg::FunctionLibrary mExtensions;
    seg::FunctionLibrary mDomain;
    seg::Interpreter mInt;
};

BOOST_FIXTURE_TEST_CASE( InterpreterTest_Values, InterpreterTest ) 
{  
    seg::VAL v1 = 0.2;
    BOOST_CHECK_EQUAL(0.2, mInt.resolve(v1));

    seg::VAL v2 = seg::MemAddress{1};
    mMem.put(1, 0.4);
    BOOST_CHECK_EQUAL(0.4, mInt.resolve(v2));

    seg::VAL v5 = seg::InputAddress{1};
    mInput[1] = 0.4;
    BOOST_CHECK_EQUAL(0.4, mInt.resolve(v5));

    seg::VAL v3 = seg::AlwaysZero();
    BOOST_CHECK_EQUAL(0, mInt.resolve(v3));

    seg::VAL v4 = seg::RandomNumber();
    BOOST_CHECK(0 != mInt.resolve(v4));
}

BOOST_FIXTURE_TEST_CASE( InterpreterTest_Primitives, InterpreterTest ) 
{  
    seg::FUNC f{seg::FuncCategory::Primitive, 0, {seg::VAL{0.6}, seg::MemAddress{1}}};

    mMem.put(1, 0.4);

    BOOST_CHECK_EQUAL(1.0, mInt.resolve(f));
}