//========================================================================
// Unit Tests for the Unit Test Framework
//========================================================================

#include "utst.h"
#include <vector>

//------------------------------------------------------------------------
// Test check basics
//------------------------------------------------------------------------

UTST_TEST_CASE( TestCheckBasics )
{
  UTST_CHECK( 1 + 1 == 2 );
  UTST_CHECK_EQ  ( 1 + 1, 2 );
  UTST_CHECK_NEQ ( 1 + 2, 2 );
}

//------------------------------------------------------------------------
// Test check floating-point
//------------------------------------------------------------------------

UTST_TEST_CASE( TestCheckFloatingPoint )
{
  UTST_CHECK_EQ  ( 0.5 + 0.5, 1.0  );
  UTST_CHECK_EQ  ( 0.1 + 0.1, 0.2  );
  UTST_CHECK_EQ  ( 0.3 + 0.3, 0.6  );
  UTST_CHECK_NEQ ( 0.5 + 0.5, 1.10 );
  UTST_CHECK_NEQ ( 0.1 + 0.1, 0.25 );
  UTST_CHECK_NEQ ( 0.3 + 0.3, 0.66 );

  UTST_CHECK_EQ  ( 0.5f + 0.5f, 1.0f  );
  UTST_CHECK_EQ  ( 0.1f + 0.1f, 0.2f  );
  UTST_CHECK_EQ  ( 0.3f + 0.3f, 0.6f  );
  UTST_CHECK_NEQ ( 0.5f + 0.5f, 1.10f );
  UTST_CHECK_NEQ ( 0.1f + 0.1f, 0.25f );
  UTST_CHECK_NEQ ( 0.3f + 0.3f, 0.66f );
}

//------------------------------------------------------------------------
// Test check throw
//------------------------------------------------------------------------

UTST_TEST_CASE( TestCheckThrow )
{
  int foo = 0;
  UTST_CHECK_THROW( int, throw foo );
}

//------------------------------------------------------------------------
// Test check containers equal
//------------------------------------------------------------------------

std::vector<int> mk_vec( int elm0, int elm1, int elm2 )
{
  std::vector<int> vec;
  vec.push_back(elm0);
  vec.push_back(elm1);
  vec.push_back(elm2);
  return vec;
}

UTST_TEST_CASE( TestCheckContEq )
{
  std::vector<int> testVec1;
  std::vector<int> testVec2;

  testVec1.push_back(1);
  testVec1.push_back(2);
  testVec1.push_back(3);

  testVec2.push_back(1);
  testVec2.push_back(2);
  testVec2.push_back(3);

  UTST_CHECK_CONT_EQ( testVec1, testVec2 );
  UTST_CHECK_CONT_EQ( testVec1, mk_vec(1,2,3) );

  testVec1.push_back(4);
  testVec2.push_back(4);

  UTST_CHECK_CONT_EQ( testVec1, testVec2 );
}

//------------------------------------------------------------------------
// Test log macros
//------------------------------------------------------------------------

UTST_TEST_CASE( TestLogMacros )
{
  int foo = 32;
  UTST_LOG_VAR( foo );
  UTST_LOG_MSG( "Done testing log macros" );
}

//------------------------------------------------------------------------
// Test parameterized test case class
//------------------------------------------------------------------------

class TestParameterizedTestCase
  : public utst::ITestCase_BasicImpl<TestParameterizedTestCase>
{

 public:

  TestParameterizedTestCase( int op1, int op2, int result )
    : m_op1(op1), m_op2(op2), m_result(result)
  {
    std::ostringstream ost;
    ost << "TestParameterizedTestCase_" << m_op1 << "_"
        << m_op2 << "_" << m_result;
    set_name( ost.str() );
  }

  void the_test()
  {
    UTST_CHECK( m_op1 + m_op2 == m_result );
    UTST_CHECK_EQ( m_op1 + m_op2, m_result );
  }

 private:
  int m_op1, m_op2, m_result;

};

//------------------------------------------------------------------------
// Test auto registered test case explicitly
//------------------------------------------------------------------------

class TestAutoReg1 : public utst::ITestCase_BasicImpl<TestAutoReg1> {

 public:

  TestAutoReg1()
  {
    set_name( "TestAutoReg1" );
  }

  void the_test()
  {
    UTST_CHECK( 1 + 1 == 2 );
    UTST_CHECK_EQ(  1 + 1, 2 );
    UTST_CHECK_NEQ( 1 + 2, 2 );
  }

};

utst::AutoRegister ar( &utst::g_default_suite(), TestAutoReg1() );

//------------------------------------------------------------------------
// Test auto registered test case with macro
//------------------------------------------------------------------------

UTST_AUTO_TEST_CASE( TestAutoReg2 )
{
  UTST_CHECK( 1 + 1 == 2 );
  UTST_CHECK_EQ(  1 + 1, 2 );
  UTST_CHECK_NEQ( 1 + 2, 2 );
}

//------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  utst::g_default_suite().add_test( TestCheckBasics() );
  utst::g_default_suite().add_test( TestCheckFloatingPoint() );
  utst::g_default_suite().add_test( TestCheckThrow() );
  utst::g_default_suite().add_test( TestCheckContEq() );
  utst::g_default_suite().add_test( TestLogMacros() );
  utst::g_default_suite().add_test( TestParameterizedTestCase(1,1,2) );
  utst::g_default_suite().add_test( TestParameterizedTestCase(2,2,4) );

  utst::auto_command_line_driver( argc, argv );
}

