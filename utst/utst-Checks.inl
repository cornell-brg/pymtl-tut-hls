//========================================================================
// utst-Checks.inl
//========================================================================
// Refactoring the check macros into functions as much as possible
// should improve compile times as opposed to having each macro expand
// to lots of code. A single test case might use tens of check macros so
// we need to be careful how much code this expands to.

#include "utst-TestLog.h"
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <functional>

namespace utst {
namespace details {

  //----------------------------------------------------------------------
  // ComparisonFunctor
  //----------------------------------------------------------------------

  template < typename T >
  struct ComparisonFunctor
  {
    bool operator()( const T& value0, const T& value1 ) {
      return ( value0 == value1 );
    }
  };

  //----------------------------------------------------------------------
  // ComparisonFunctor floating point specializations
  //----------------------------------------------------------------------

  template <>
  struct ComparisonFunctor<float>
  {
    bool operator()( const float& value0, const float& value1 ) {
      return ( std::abs(value0 - value1) <= 0.000001f * std::abs(value0) );
    }
  };

  template <>
  struct ComparisonFunctor<double>
  {
    bool operator()( const double& value0, const double& value1 ) {
      return ( std::abs(value0 - value1) <= 0.000001 * std::abs(value0) );
    }
  };

  //----------------------------------------------------------------------
  // check_eq
  //----------------------------------------------------------------------

  template < typename Expr1, typename Expr2 >
  void check_eq( const std::string& file_name, int line_num,
                 const Expr1& expression0, const Expr2& expression1,
                 const char* expression0_str,
                 const char* expression1_str )
  {
    utst::TestLog& log = utst::TestLog::instance();

    ComparisonFunctor<Expr1> comp_func;
    bool result = comp_func( expression0, expression1 );

    std::ostringstream descr;
    descr << expression0_str << " == " << expression1_str;

    std::ostringstream msg;
    msg << expression0 << (result ? " == " : " != ") << expression1;

    log.log_test( file_name, line_num, descr.str(), result, msg.str() );
  }

  //----------------------------------------------------------------------
  // check_neq
  //----------------------------------------------------------------------

  template < typename Expr1, typename Expr2 >
  void check_neq( const std::string& file_name, int line_num,
                  const Expr1& expression0, const Expr2& expression1,
                  const char* expression0_str,
                  const char* expression1_str )
  {
    utst::TestLog& log = utst::TestLog::instance();

    ComparisonFunctor<Expr1> comp_func;
    bool result = !comp_func( expression0, expression1 );

    std::ostringstream descr;
    descr << expression0_str << " != " << expression1_str;

    std::ostringstream msg;
    msg << expression0 << (result ? " != " : " == ") << expression1;

    log.log_test( file_name, line_num, descr.str(), result, msg.str() );
  }

  //----------------------------------------------------------------------
  // check_cont_eq
  //----------------------------------------------------------------------

  template < typename Cont1, typename Cont2 >
  void check_cont_eq( const std::string& file_name, int line_num,
                      const Cont1& container1, const Cont2& container2,
                      const char* container1_str,
                      const char* container2_str )
  {
    utst::TestLog& log = utst::TestLog::instance();

    // Check containers are of the same size

    if ( container1.size() != container2.size() ) {

      std::ostringstream descr;
      descr << container1_str << " and "
            << container2_str << " same size?";

      std::ostringstream msg;
      msg << container1.size() << " != " << container2.size();

      log.log_test( file_name, line_num, descr.str(), false, msg.str() );
      return;
    }

    // Check each element in each container

    typename Cont1::const_iterator itr1 = container1.begin();
    typename Cont2::const_iterator itr2 = container2.begin();

    bool equal = true;
    int  index = 0;
    while ((itr1 != container1.end()) && (itr2 != container2.end())) {

      ComparisonFunctor<typename Cont1::value_type> comp_func;
      if ( !( comp_func( *itr1, *itr2 ) ) ) {
        equal = false;

        std::ostringstream descr;
        descr << container1_str << "[" << index << "]" << " == "
              << container2_str << "[" << index << "]";

        std::ostringstream msg;
        msg << *itr1 << " != " << *itr2;

        log.log_test( file_name, line_num, descr.str(), false, msg.str() );
      }

      itr1++; itr2++; index++;
    }

    // Are they equal?

    if ( equal ) {
      std::ostringstream descr;
      descr << container1_str << " == " << container2_str;
      log.log_test( file_name, line_num, descr.str(), true );
    }

  }

  //----------------------------------------------------------------------
  // ECheckFailed
  //----------------------------------------------------------------------

  struct EFatalFailure : std::exception {
    ~EFatalFailure() throw() { };
  };

}}

//------------------------------------------------------------------------
// UTST_CHECK
//------------------------------------------------------------------------

#define UTST_CHECK_( expression_ )                                      \
  utst::TestLog::instance().log_test( __FILE__, __LINE__,               \
    #expression_, expression_ );

//------------------------------------------------------------------------
// UTST_CHECK_EQ
//------------------------------------------------------------------------

#define UTST_CHECK_EQ_( expression0_, expression1_ )                    \
  utst::details::check_eq( __FILE__, __LINE__,                          \
    expression0_, expression1_, #expression0_, #expression1_ );

//------------------------------------------------------------------------
// UTST_CHECK_NEQ
//------------------------------------------------------------------------

#define UTST_CHECK_NEQ_( expression0_, expression1_ )                   \
  utst::details::check_neq( __FILE__, __LINE__,                         \
    expression0_, expression1_, #expression0_, #expression1_ );

//------------------------------------------------------------------------
// UTST_CHECK_THROW
//------------------------------------------------------------------------

#define UTST_CHECK_THROW_( exception_, expression_ )                    \
{                                                                       \
  bool caught_ = false;                                                 \
  try { expression_; }                                                  \
  catch ( exception_& e_ ) { caught_ = true; }                          \
  std::ostringstream ost_;                                              \
  ost_ << "( " << #expression_ << " ) throw " << #exception_ << "?";    \
  utst::TestLog::instance().log_test( __FILE__, __LINE__,               \
                                      ost_.str(), caught_ );            \
}

//------------------------------------------------------------------------
// UTST_CHECK_CONT_EQ
//------------------------------------------------------------------------

#define UTST_CHECK_CONT_EQ_( container0_, container1_ )                 \
  utst::details::check_cont_eq( __FILE__, __LINE__,                     \
    container0_, container1_, #container0_, #container1_ );

//------------------------------------------------------------------------
// UTST_CHECK_FAILED
//------------------------------------------------------------------------

#define UTST_CHECK_FAILED_( message_ )                                  \
{                                                                       \
  std::ostringstream ost;                                               \
  ost << message_;                                                      \
  utst::TestLog::instance().log_test( __FILE__, __LINE__,               \
                                      ost.str(), false );               \
  throw utst::details::EFatalFailure();                                 \
}

//------------------------------------------------------------------------
// UTST_LOG_MSG
//------------------------------------------------------------------------

#define UTST_LOG_MSG_( message_ )                                       \
{                                                                       \
  std::ostringstream ost;                                               \
  ost << message_;                                                      \
  utst::TestLog::instance().log_note( __FILE__, __LINE__, ost.str() );  \
}

//------------------------------------------------------------------------
// UTST_LOG_VAR
//------------------------------------------------------------------------

#define UTST_LOG_VAR_( variable_ )                                      \
{                                                                       \
  std::ostringstream ost;                                               \
  ost << #variable_ << " = " << variable_;                              \
  utst::TestLog::instance().log_note( __FILE__, __LINE__, ost.str() );  \
}

