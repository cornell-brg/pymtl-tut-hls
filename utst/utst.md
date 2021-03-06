
Unit Testing Framework User Guide
==========================================================================
* Author : Christopher Batten
* Date   : September 24, 2008

The unit test subproject provides a simple way to write and manage unit
tests. Although we could write unit tests with ad-hoc test code, there
are several advantages to using some kind of unit testing framework. A
common unit test framework simplifies writing tests, makes it easier for
developers to understand each other's tests, provides a consistent way
to run and log tests, and reduces false errors due to incorrect testing
code. It is important to make unit testing consistent and simple since a
good developer will spend almost as much (if not more) time writing unit
tests as they do writing the actual code being tested.

A test includes a set of checks which are grouped into test cases which
are then further grouped into test suites. Running the tests is
controlled by a test driver. Much of the details of the framework are
abstracted away behind preprocessor macros and helper functions, making
it quite simple to write basic unit tests.

This user guide is divided into two parts: the first part is a tutorial,
and the second part contains general user reference. We will assume that
you are using the unit testing framework as part of the Modular C++
Build System so you might want to read the documentation for the build
system first `mcppbs-uguide.md`.

#### Table of Contents

 - Part 1 : Tutorial
    + 1.1. Basic Unit Test for the STL Vector Class
    + 1.2. Additional Test Cases for the STL Vector Class
    + 1.3. Using Parameterized Test Cases
    + 1.4. Using the Extra Test Suites
    + 1.5. Controlling Which Test Cases Are Run

 - Part 2 : Reference
    + 2.1. The Basic `UTST_CHECK` Macros
    + 2.2. Grouping Checks into Test Cases
    + 2.3. Collecting Test Cases into Test Suites
    + 2.4. Managing Test Suites with Test Drivers
    + 2.5. Using the Auto Registration Features
    + 2.6. Integrating Unit Tests into the Build System

1. Tutorial
--------------------------------------------------------------------------

This tutorial walks through using the unit testing framework to test the
standard STL `vector` class. You can follow along through the tutorial
yourself by typing in the commands marked with a `%` symbol at the shell
prompt. Tasks to try on your own are denoted with (*). To cut and paste
commands from this tutorial into your bash shell (and make sure bash
ignores the `%` character) just use an alias to "undefine" the `%`
character like this:

```
 % alias %=""
```

Before working through the tutorial we need to instantiate the Modular
C++ Build System which contains the utst subproject. We do this by first
creating a temporary directory to work in, setting an environment
variable to simplify the rest of the tutorial, and downloading the
`mcppbs` project from the public git repository.

```
 % TGZ=<location-of-template>
 % wget -O- $TGZ | tar xzf -
 % mv cbatten-sw-mcppbs mcppbs-tut
 % cd mcppbs-tut
 % PROJROOT=$PWD
```

Now that we have the build system let's go ahead and build the project,
although the only subproject in the default project is the unit test
framework itself.

```
 % cd $PROJROOT
 % mkdir build
 % cd build
 % ../configure
 % make
```

To run all of the unit tests use use the makefile's `check` target.

```
 % cd $PROJROOT/build
 % make check
```

As explained in the user guide for the build system, the way we add code
to our project is through subprojects. We assume you are familiar with
how the build system works, so we will just quickly create a subproject
for this tutorial called `utst-tut`.

```
 % cd $PROJROOT
 % mkdir utst-tut
 % echo "MCPPBS_SUBPROJECT([utst-tut])" > utst-tut/utst-tut.ac
 % cat > utst-tut/utst-tut.mk.in \
<<'END'

 utst_tut_intdeps   = @utst_tut_intdeps@
 utst_tut_cppflags  = @utst_tut_cppflags@
 utst_tut_ldflags   = @utst_tut_ldflags@
 utst_tut_libs      = @utst_tut_libs@

END

 % cd $PROJROOT
 % sed -i.bak '/MCPPBS_INCLUDE_INTERNAL(\[utst\])/ a \
     MCPPBS_INCLUDE_INTERNAL([utst-tut])
   ' configure.ac
 % autoconf && autoheader
```

Normally, we each physical "component" (i.e. a C++ header and source
pair) has an associated unit test with a `.t.cc` suffix, but for this
tutorial we will only write the unit tests.

Usually, every physical component (ie. a header/source file pair) should
have an additional unit test file with the special `.t.cc` extension.
For example, let's assume I am working on a new class named `Bar` in the
`foo` subproject. The declaration of `Bar` will be in `foo/Bar.h` and
the definition will be in `foo/Bar.cc`. My unit tests should be placed
in `foo/Bar.t.cc`. As set by the general build policies, these three
files will be in the `foo` subdirectory. To tell the build system about
these new files, the developer needs to add them to specific make
variables in `foo/foo.mk.in`: `foo/Bar.h` is added to `foo_hdrs`,
`foo/Bar.cc` is added to `foo_srcs`, and `foo/Bar.t.cc` is added to
`foo_test_srcs`. The build system will automatically include rules to
build a `foo/Bar-utst` executable from `foo/Bar.t.cc`. However, for this
tutorial we will simplify things and only write the unit tests (not the
header/source files for the actual things we might be testing).

1.1. Basic Test Case for the STL Vector Class
--------------------------------------------------------------------------

The most straightforward way to define a test case is to use the
`UTST_AUTO_TEST_CASE` macro. We will learn later in the reference
sections exactly what this macro does, but for now all we need to know
is that we give the macro the name of the test and use braces to define
the body of the test. The test name should be camel case and often we
begin the name with `Test`. The following test case uses this macro to
perform some basic tests of the STL `vector` class.

```
 % cd $PROJROOT
 % cat > utst-tut/Test1.t.cc \
<<'END'

 #include "utst/utst.h"
 #include <vector>

 UTST_AUTO_TEST_CASE( TestBasic )
 {
   std::vector<int> vec;

   UTST_CHECK( vec.empty() );
   UTST_CHECK_EQ( vec.size(), 0u );

   vec.resize(3);
   vec.at(0) = 0;
   vec.at(1) = 1;
   vec.at(2) = 2;

   UTST_CHECK( !vec.empty() );
   UTST_CHECK_EQ( vec.size(), 3u );
   UTST_CHECK_EQ( vec.at(0), 0 );
   UTST_CHECK_EQ( vec.at(1), 1 );
   UTST_CHECK_EQ( vec.at(2), 2 );
 }

 int main( int argc, char* argv[] )
 {
   utst::auto_command_line_driver( argc, argv, "utst-tut" );
 }

END
```

Notice that we need to include the `utst.h` header and we use the
`UTST_CHECK` and `UTST_CHECK_EQ` macros to perform various tests. The
rest of the `UTST_CHECK` macros are described in the reference sections.
Our `main` function is relatively simple - we just call the built-in
command line driver which will parse the command line and execute the
tests we have listed in this file. As a quick aside, we are using the
automatic registration features of the framework in this unit test. The
framework automatically registers the `TestBasic` test case with a
default global test suite, and the command line driver automatically
references this global test suite. We will learn more in later sections
about how to write test cases which need to be explicitly registered
with a test suite, and also about the various global test suites.

Now that we have a unit test we need to add it to the list of unit tests
in our subproject's make fragment.

```
 % cd $PROJROOT
 % echo "utst_tut_test_srcs += utst-tut/Test1.t.cc" >> utst-tut/utst-tut.mk.in
```

We are finally ready to build and run our first test case using the
build system.

```
 % cd $PROJROOT/build
 % make utst-tut/Test1-utst
 % ./utst-tut/Test1-utst
```

The output from running the unit test should be:

```
 Unit Tests : utst-tut/Test1
 Running test suite : default
  + Running test case : TestBasic
```

Essentially the build system knows how to compile all the `.t.cc` test
source files into executables - one binary per unit test. Each binary is
named the same as the basename of the `.t.cc` source with an additional
`-utst` suffix. The output from running the unit test shows that we have
run the default test suite for the `utst-tut/Test1` unit test. Use
`--help` to see the various command line options provided by the unit
test framework. For example, to see more output use the `--log-level`
option.

```
 % ./utst-tut/Test1-utst --log-level verbose
```

The output from running the unit test will now be more verbose:

```
 Unit Tests : utst-tut/Test1
 Running test suite : default

  + Running test case : TestBasic
     [ passed ] Line 9 : vec.empty()
     [ passed ] Line 10 : vec.size() == 0u [ 0 == 0 ]
     [ passed ] Line 17 : !vec.empty()
     [ passed ] Line 18 : vec.size() == 3u [ 3 == 3 ]
     [ passed ] Line 19 : vec.at(0) == 0 [ 0 == 0 ]
     [ passed ] Line 20 : vec.at(1) == 1 [ 1 == 1 ]
     [ passed ] Line 21 : vec.at(2) == 2 [ 2 == 2 ]
```

The extra information shows the result of each `UTST_CHECK` regardless
of whether or not it passed or failed. We are often repeatedly building
and running a unit test in our edit-compile-debug loop. You can combine
the building and execution of a unit test into a single command line
with the `&&` shell operator. The `&&` operator means that we will only
try and run the unit test if in fact we were able to successfully build
the unit test.

```
 % make utst-tut/Test1-utst && ./utst-tut/Test1-utst
```

(*) Try purposely changing the `TestBasic` test cases so that it will
fail. For example, change the test for the size of the vector from 3 to
2. Then rerun the tests and notice how the failure is reported. (*) Try
all three log levels and see how the reporting for passing and failing
test cases changes.

1.2. Additional Test Cases for the STL Vector Class
--------------------------------------------------------------------------

In this section we will experiment with some additional test cases.
Normally we might just append these to the end the previous unit test,
for the purposes of this tutorial we will just make a completely new
unit test source file.

```
 % cd $PROJROOT
 % cat > utst-tut/Test2.t.cc \
<<'END'

 #include "utst/utst.h"
 #include <vector>
 #include <list>
 #include <stdexcept>

 UTST_AUTO_TEST_CASE( TestRangeException )
 {
   std::vector<int> vec(3);
   vec.at(0) = 0;
   vec.at(1) = 1;
   vec.at(2) = 2;

   UTST_CHECK_THROW( std::out_of_range, vec.at(3) );
 }

 UTST_AUTO_TEST_CASE( TestPushBack )
 {
   std::list<int> lst;
   lst.push_back(0);
   lst.push_back(1);
   lst.push_back(2);

   std::vector<int> vec;
   vec.push_back(0);
   vec.push_back(1);
   vec.push_back(2);

   UTST_CHECK_CONT_EQ( lst, vec );
 }

 int main( int argc, char* argv[] )
 {
   utst::auto_command_line_driver( argc, argv, "utst-tut" );
 }

END
```

The `UTST_CHECK_THROW` macro checks if an expression throws a specific
exception. In the above example, we use this macro to verify that the
`vector` class throws a `std::out_of_range` exception when we access an
element which is beyond the size of the vector. The `UTST_CHECK_CONT_EQ`
macro verifies that two containers are the same. It requires that the
containers have `begin()`, `end()`, and `size()` methods. Let's add the
new unit test to the make fragment and compile and built the new unit
test.

```
 % cd $PROJROOT
 % echo "utst_tut_test_srcs += utst-tut/Test2.t.cc" >> utst-tut/utst-tut.mk.in

 % cd $PROJROOT/build
 % make utst-tut/Test2-utst && ./utst-tut/Test2-utst
```

(*) Try adding a new test case to the utst-tut/Test2 unit test which
tests the `insert` and `erase` methods.

1.3. Using Parameterized Test Cases
--------------------------------------------------------------------------

Although most of the time we will use the `UTST_AUTO_TEST_CASE` macro
sometimes we may want to use the classes in the unit test framework
explicitly. For example, we might want a test case which takes a few
constructor arguments that control what we are checking. Such
parameterized test cases allow us to run many different tests just by
calling the same test case with different arguments. To implement such
test cases we explicitly subclass the `ITestCase_BasicImpl` base class
which is described in more detail in the reference sections.

The following example tests the `push_back` method but it is
parameterized by how many elements to actually push into the vector. To
test push back we first assume that the STL `list` container has already
been unit tested. So we randomly pick a number, push it into both the
vector and the list, and then check at the end that both containers have
the same values.

```
 % cd $PROJROOT
 % cat > utst-tut/Test3.t.cc \
<<'END'

 #include "utst/utst.h"
 #include <vector>
 #include <list>

 class TestPushBackParam
   : public utst::ITestCase_BasicImpl<TestPushBackParam>
 {

  public:
   TestPushBackParam( int size ) : m_size(size)
   {
     std::ostringstream ost;
     ost << "TestPushBack_size" << size;
     set_name( ost.str() );
   }

   void the_test()
   {
     std::list<int>   lst;
     std::vector<int> vec;

     for ( int i = 0; i < m_size; i++ ) {
       int random_int = rand() % 100;
       lst.push_back( random_int );
       vec.push_back( random_int );
     }

     UTST_CHECK_CONT_EQ( lst, vec );
   }

  private:
   int m_size;

 };

 using namespace utst;

 AutoRegister ar_test1   ( &g_default_suite(), TestPushBackParam(1)   );
 AutoRegister ar_test2   ( &g_default_suite(), TestPushBackParam(2)   );
 AutoRegister ar_test100 ( &g_default_suite(), TestPushBackParam(100) );

 int main( int argc, char* argv[] )
 {
   utst::auto_command_line_driver( argc, argv, "utst-tut" );
 }

END
```

Although we are explicitly instantiating a test case we can still
leverage the auto registration features through the `AutoRegister`
class. This class essentially registers the given test case in the given
test suite when it is instantiated. Again, these auto registration
features prevent us from having to explicitly register tests cases in
the main function. The reference section explains how to do the
registration manually although it is rarely necessary. Essentially the
`UTST_AUTO_TEST_CASE` macro takes care of defining a new test case class
which inherits from `ITestCase_BasicImpl` and then instantiating an
appropriate `AutoRegister` object.

We instantiate three test cases which will test pushing one, two, and
100 elements. We now add the new unit test to the make fragment, build,
and run the new test cases.

```
 % cd $PROJROOT
 % echo "utst_tut_test_srcs += utst-tut/Test3.t.cc" >> utst-tut/utst-tut.mk.in

 % cd $PROJROOT/build
 % make utst-tut/Test3-utst && ./utst-tut/Test3-utst
```

1.4. Using the Extra Test Suites
--------------------------------------------------------------------------

There are a total of three global test suites:

 - `g_default_suite()`  : For default functional unit tests
 - `g_longrun_suite()`  : For long running functional unit tests
 - `g_perf_suite()`     : For performance regression tests

We have already seen that the `UTST_AUTO_TEST_CASE` defines new test
cases and automatically adds them to the default test suite. For the
other two test suites we can use the `UTST_AUTO_EXTRA_TEST_CASE` macro
as follows:

```
 UTST_AUTO_EXTRA_TEST_CASE( suite_name, TestCase )
```

For the suite name we should use either `longrun` or `perf`. For
example, the following test cases measure how long it takes to
initialize a thousand element vector using the assign statement versus
the `push_back` method.

```
 % cd $PROJROOT
 % cat > utst-tut/Test4.t.cc \
<<'END'

 #include "utst/utst.h"
 #include <vector>

 UTST_AUTO_EXTRA_TEST_CASE( perf, PerfAssign )
 {
   using namespace std;

   int num_trials      = 1e6;
   int num_elm_per_vec = 1e3;

   clock_t start = clock();
   for ( int i = 0; i < num_trials; i++ ) {
     std::vector<int> vec(num_elm_per_vec);
     for ( int j = 0; j < num_elm_per_vec; j++ )
       vec[j] = j;
   }
   clock_t end = clock();

   utst::TestLog& log = utst::TestLog::instance();
   ostream& os = log.get_log_ostream( utst::TestLog::LogLevel::minimal );
   os << "     - assign_num_trials = " << num_trials << endl;
   os << "     - assign_num_elm    = " << num_elm_per_vec << endl;
   os << "     - assign_total_time = " << (end - start) << endl;
 }

 UTST_AUTO_EXTRA_TEST_CASE( perf, PerfPushBack )
 {
   using namespace std;

   int num_trials      = 5e5;
   int num_elm_per_vec = 1e3;

   clock_t start = clock();
   for ( int i = 0; i < num_trials; i++ ) {
     std::vector<int> vec;
     for ( int j = 0; j < num_elm_per_vec; j++ )
       vec.push_back(j);;
   }
   clock_t end = clock();

   utst::TestLog& log = utst::TestLog::instance();
   ostream& os = log.get_log_ostream( utst::TestLog::LogLevel::minimal );
   os << "     - push_num_trials   = " << num_trials << endl;
   os << "     - push_num_elm      = " << num_elm_per_vec << endl;
   os << "     - push_total_time   = " << (end - start) << endl;
 }

 int main( int argc, char* argv[] )
 {
   utst::auto_command_line_driver( argc, argv, "utst-tut" );
 }

END
```

Notice how these test cases use the test log. By passing
`LogLevel::minimal` to the `get_log_ostream` method, the test case is
telling the test log that whatever is inserted into the log stream
should be displayed whenever the log level is equal to or more verbose
than `minimal` (ie. always). So the performance results will always be
displayed regardless of log level. The following test case measures how
long it takes to initialize a thousand element vector using the
`push_back` method.

```
 % cd $PROJROOT
 % echo "utst_tut_test_srcs += utst-tut/Test4.t.cc" >> utst-tut/utst-tut.mk.in

 % cd $PROJROOT/build
 % make utst-tut/Test4-utst && ./utst-tut/Test4-utst
```

The output from these performance tests should look something like:

```
  Unit Tests : utst-tut-Test4
  Running test suite : perf
   + Running test case : PerfAssign
      - assign_num_trials = 1000000
      - assign_num_elm    = 1000
      - assign_total_time = 197
   + Running test case : PerfPushBack
      - push_num_trials   = 500000
      - push_num_elm      = 1000
      - push_total_time   = 374
```

These results show that using the assignment statement (which allocates
all of the memory first) is faster than using repeated calls to the
`push_back` (which logarithmically increases the allocated space). (*)
Try adding a new test case to the `longrun` test suite.

1.5. Controlling Which Tests Are Run
--------------------------------------------------------------------------

As one adds more and more test cases to their unit test they may want to
run manage which test cases are run in a more fine-grain way. You can
use the `--list-tests` command line option to see which test cases are
available in which test suites. To run a specific test case or test
suite simply specify its name on the command line. To just run the
`TestRangeException` test case or to just run the `perf` test suite from
some of the earlier examples we would use the following:

```
 % ./utst-tut/Test2-utst TestRangeException
 % ./utst-tut/Test4-utst perf
```

We can run all the unit tests for this subproject with the
`check-utst-tut` make target and as mentioned earlier we can run all the
unit tests for all test cases with the `check` make target.

```
 % make check
```

What the `check` target does is build and run each unit test in the
default test suite and save the output from each unit test in a `.out`
file. Then it greps through these `.out` files for errors to present a
very compact summary of the unit test results.

2. Reference
--------------------------------------------------------------------------

In this part, we walk through how to use the unit test framework
starting with the simplest possible way to write unit tests and then
gradually discussing more features of the framework.

2.1. The Basic UTST_CHECK Macros
--------------------------------------------------------------------------

Let's assume we want to add some unit tests for some functions or a
class we have written. The most basic way to do this is to create a
separate toplevel source file and write some tests for our class in the
toplevel source file's `main` function. We can use the `UTST_CHECK`
macros located in `utst/Checks.h` to check for various conditions which
we know should be true. The basic `UTST_CHECK` macros are listed below.

 - `UTST_CHECK( e1 )`              : Does expression e1 eval true?
 - `UTST_CHECK_EQ( e1, e2 )`       : Are two expressions equal?
 - `UTST_CHECK_NEQ( e1, e2 )`      : Are two expressions not equal?
 - `UTST_CHECK_CONT_EQ( c1, c2 )`  : Are two containers equal?
 - `UTST_CHECK_THROW( excep, e1 )` : Expression throws given exception?
 - `UTST_CHECK_FAILED( msg )`      : Always fails with given message

Unlike `UTST_CHECK( e1 == e2 )`, using the `UTST_CHECK_EQ( e1, e2 )`
macro will display the values of both expressions which is useful for
debugging. Similarly the `UTST_CHECK_NEQ` and `UTST_CHECK_CONT_EQ`
macros can be more useful that just using `UTST_CHECK` directly. The
`UTST_CHECK_THROW` macro will see if the given expression throws the
given exception and then continue executing. The `UTST_CHECK_FAILED`
macro is useful when the condition for failure is too complicated for
the basic macros (eg. we want to check if a data structure contains some
value and if not the check should fail with a meaningful message).

Each macro does the appropriate check and then logs its result to the
global `TestLog`. The `TestLog` manages the output for the unit test
framework; a developer can control how much output is displayed by
setting the log level to one of three values:

 - `minimal`  : Output failing checks only
 - `moderate` : Output failing checks and other log output
 - `verbose`  : Output passing and failing checks and other log output

For example, a developer can set the log level to moderate as follows:

```
 utst::TestLog& log = utst::TestLog::instance();
 log.set_log_level( utst::TestLog::LogLevel::moderate );
```

Notice that there is only one global test log and it is accessed through
the static singleton method `TestLog::instance()`. Also notice that the
log level names are static constants in the `TestLog::LogLevel`
namespace. A developer can write general information to the test log by
inserting into the output stream provided through the
`TestLog::get_log_ostream` method. Here is an example:

```
 std::ostream& os = utst::TestLog::instance().get_log_ostream();
 os << "Starting new set of checks ..." << std::endl;
```

The `get_log_ostream` method takes an optional parameter which sets the
the minimum log level at which whatever is inserted into the stream
should be displayed. For example, if we pass `LogLevel::verbose` into
`get_log_ostream`, then whatever is inserted into that stream will only
be displayed if the log level is set to verbose. The default is
`moderate` meaning the output is only displayed if the log level is set
to `moderate` or `verbose`.

In the following code we use the `UTST_CHECK` macros to write a simple
unit test for the standard STL `vector` class.

```
 int main( int argc, char* argv[] )
 {
   utst::TestLog& log = utst::TestLog::instance();
   log.set_log_level( utst::TestLog::LogLevel::verbose );

   std::vector<int> vec(2);
   vec.at(0) = 0; vec.at(1) = 1;

   UTST_CHECK_EQ( vec.size(), 2u );
   UTST_CHECK_EQ( vec.at(0),  0  );
   UTST_CHECK_EQ( vec.at(1),  1  );
 }
```

Obviously we can add even more checks, possibly using the
`UTST_CHECK_THROW` macro to make sure the `vector` class throws a range
exception for out-of-bounds accesses or using the `UTST_CHECK_CONT_EQ`
macro to compare different instances of the `vector` class.

2.2. Grouping Checks into Test Cases
--------------------------------------------------------------------------

We can use test cases to better structure the checks used in our unit
tests. A test case is simply a named group of checks which test a
specific functionality of interest.

Test cases are implemented as classes which derive from the common
abstract base class `ITestCase`. Instead of deriving from `ITestCase`
directly (and thus having to implement all of the virtual functions from
scratch), developers can derive from the implementation helper class
`ITestCase_BasicImpl`. `ITestCase_BasicImpl` is a subclass of
`ITestCase` which implements all of the virtual functions except for a
new protected member called `the_test`. To create a new test case,
developers just derive from `ITestCase_BasicImpl` and overload
`the_test` with the appropriate checks. The reason developers overload
`ITestCase_BasicImpl::the_test` instead of `ITestCase::run` directly, is
because the implementation helper class adds some code before and after
it executes `the_test`. It writes some log output indicating that it is
starting a test case and it also catches any exceptions thrown from
inside `the_test`. If code within `the_test` throws an exception which
is not caught then the test case fails but the program keeps executing.
As an example of using `ITestCase_BasicImpl`, the following test case
encapsulates the basic tests from the previous section.

```
 class TestBasic : public utst::ITestCase_BasicImpl<TestBasic> {
  public:
   TestBasic() { set_name( "TestBasic" ); }

   void the_test()
   {
     std::vector<int> vec(2);
     vec.at(0) = 0; vec.at(1) = 1;

     UTST_CHECK_EQ( vec.size(), 2u );
     UTST_CHECK_EQ( vec.at(0),  0  );
     UTST_CHECK_EQ( vec.at(1),  1  );
   }

 };
```

`ITestCase_BasicImpl` is templated by the type of the derived class.
This is so the base class can implement the `clone` method. Notice that
in addition to implementing `the_test`, a developer also needs to add a
constructor which sets the name of the test case. The policy is that the
name of the test case should be the same as the name of the class which
implements that test case.

The framework provides a macro called `UTST_TEST_CASE` which makes it
much easier to write these type of basic test cases. The following
example shows how to use this macro to write the same basic test case.

```
 UTST_TEST_CASE( TestBasic )
 {
   std::vector<int> vec(2);
   vec.at(0) = 0; vec.at(1) = 1;

   UTST_CHECK_EQ( vec.size(), 2u );
   UTST_CHECK_EQ( vec.at(0),  0  );
   UTST_CHECK_EQ( vec.at(1),  1  );
 }
```

This macro will create a new class called `TestBasic` and it adds the
code in the associated `{ }` block to `the_test`. Even though this macro
makes it very easy to write test cases, there are still times where we
want to explicitly subclass `ITestCase_BasicImpl`. For example, we might
want a test case which takes a few constructor arguments that control
what we are checking. These parameterized test cases allow us to run
many different tests just by calling the same test case with different
arguments.

The following example is similar to before, except that it is
parameterized by the size of the vector. For this test we first assume
that the STL `list` container has already been unit tested. So we
randomly pick a number, add it into both the `vector` and the `list`,
and then check at the end that both containers have the same values.

```
 class TestBasic : public utst::ITestCase_BasicImpl<TestBasic> {
  public:
   TestBasic( int size ) : m_size(size)
   {
     std::ostringstream ost;
     ost << "TestBasic_size" << size;
     set_name( ost.str() );
   }

   void the_test()
   {
     std::list<int>   lst;
     std::vector<int> vec;

     for ( int i = 0; i < m_size; i++ ) {
       int random_int = rand() % 100;
       lst.push_back( random_int );
       vec.push_back( random_int );
     }

     UTST_CHECK_CONT_EQ( lst, vec );
   }

  private:
   int m_size;

 };
```

Notice that we create the test case name from the arguments so that the
various instantiations of this test case have different and descriptive
names.

So let's assume we have created several different test cases which each
check different aspects of the standard STL `vector` class. We can then
instantiate these test cases and run them in `main`. Since test cases
are just normal objects, they can be stored in a container just like any
other object with a common base class. Then we can iterate over the
container and call the `run` virtual function. Notice how we use the
parameterized test case to test the push back with one and 100 elements.

```
 int main( int argc, char* argv[] )
 {
   utst::TestLog& log = utst::TestLog::instance();
   log.set_log_level( utst::TestLog::LogLevel::verbose );

   // Instantiate the test cases and add them to a container
   std::list<utst::ITestCase*> tests;
   tests.push_back( new TestBasic()          );
   tests.push_back( new TestRangeException() );
   tests.push_back( new TestPushBack(1)      );
   tests.push_back( new TestPushBack(100)    );

   // Iterate over test cases and run them
   typedef std::list<utst::ITestCase*>::iterator ITR;
   for ( ITR itr = tests.begin(); itr != tests.end(); ++itr )
     (*itr)->run();

   // Delete the test cases since they were dynamically allocated
   for ( ITR itr = tests.begin(); itr != tests.end(); ++itr )
     delete *itr;

 }
```

2.3. Collecting Test Cases into Test Suites
--------------------------------------------------------------------------

To simplify managing collections of test cases we can group them into
test suites. Organizing unit tests into test cases and test suites
allows us to easily just run a single test case or run all the tests in
a suite. Developers might create multiple test suites for different
types of testing. For example, we might have a policy that in addition
to the default test suite, developers can create extra suites with
long-running test cases and/or performance tests. In the following code,
we add the test cases from the previous section to a new test suite and
then run all the tests.

```
 int main( int argc, char* argv[] )
 {
   utst::TestLog& log = utst::TestLog::instance();
   log.set_log_level( utst::TestLog::LogLevel::verbose );

   // Instantiate the test cases and add them to a test suite
   utst::TestSuite suite("std::vector");
   suite.add_test( TestBasic()          );
   suite.add_test( TestRangeException() );
   suite.add_test( TestPushBack(1)      );
   suite.add_test( TestPushBack(100)    );

   // Run all the test cases
   suite.run_all();
 }
```

2.4. Managing Test Suites with Test Drivers
--------------------------------------------------------------------------

Test drivers manage setting up the test log and running the desired
tests. Currently there is only one type of test driver
(`CommandLineTestDriver`) which uses the command line to control the
unit testing. One can imagine additional test drivers which might use an
XML file or a GUI to control the unit testing. New test drivers might
also demand different forms of test output, and thus we might need to
improve how the `utst::TestLog` is handled. For now, we focus on
providing a clean way to manage unit tests from the command line.

Test drivers enable a developer to work with several test suites at the
same time. In addition to a basic default test suite for functional unit
testing, we might want to add a test suite for performance regression
tests. The following code illustrates how to use the command line test
driver.

```
 int main( int argc, char* argv[] )
 {
   // Instantiate functional test cases and add them to default suite
   utst::TestSuite default_suite("default");
   default_suite.add_test( TestBasic()          );
   default_suite.add_test( TestRangeException() );
   default_suite.add_test( TestPushBack(1)      );
   default_suite.add_test( TestPushBack(100)    );

   // Instantiate performance test cases and add them to perf suite
   utst::TestSuite perf_suite("perf");
   perf_suite.add_test( PerfAssign()   );
   perf_suite.add_test( PerfPushBack() );

   // Use command line to drive the tests
   utst::CommandLineTestDriver driver;
   driver.add_suite( &default_suite );
   driver.add_suite( &perf_suite );
   driver.run( argc, argv );
 }
```

The test driver accepts the following command line usage:

```
 > utst-exe [options] [suite ...] [case ...]
```

With the following command line options:

 - `--log-level <level>` : Set level of detail for output
 - `--list-tests`        : List all test suites and cases
 - `--help`              : Show usage information

Use the `--list-tests` flag to see a list of all the possible test
suites and test cases. By default all test suites are run, but often we
only want to run a subset of the tests. For example, we might want to
run a single test case to debug it quickly, or we might want to disable
some long running performance tests. A user can select which tests to
run by simply listing the names of the desired test suites and test
cases on the command line. Possible values for the log levels are:
`minimal`, `moderate`, and `verbose`.

Note that the test driver will have trouble if you use the same name for
multiple test cases or test suites. Try to use unique lower case names
for test suites and unique camel case names for the test cases.

2.5. Using the Auto Registration Features
--------------------------------------------------------------------------

A common error when using test cases and test suites is to define a new
test case but forget to add it to the appropriate test suite. The auto
registration features can help avoid these kind of errors. The
`utst/AutoUtils.h` header declares global test suites as well as helper
classes and macros for automatically adding test cases to these global
test suites. The three global test suites are as follows:

 - `g_default_suite()`  : For default functional unit tests
 - `g_longrun_suite()`  : For long running functional unit tests
 - `g_perf_suite()`     : For performance regression tests

To avoid issues with static initialization order, these global test
suites are not implemented with global variables. Instead they are
implemented as free functions which return a reference to a local static
variable. Here is an example of defining a new test case and then using
the `AutoRegister` class to register the test case right away.

```
 class TestBasic : public utst::ITestCase_BasicImpl<TestBasic> {
  public:
   TestBasic() { set_name( "TestBasic" ); }

   void the_test()
   {
     std::vector<int> vec(2);
     vec.at(0) = 0; vec.at(1) = 1;

     UTST_CHECK_EQ( vec.size(), 2u );
     UTST_CHECK_EQ( vec.at(0),  0  );
     UTST_CHECK_EQ( vec.at(1),  1  );
   }

 };

 utst::AutoRegister
   ar_TestBasic( &utst::g_default_suite(), TestBasic() );
```

The key idea is that we want to register this test case with the global
default test suite right now and not wait until the `main` function. We
can't really execute any code right away, though, since we are just at
the general file scope. We can, however, execute some code in a
constructor for a class which we instantiate. The `AutoRegister` class
is just a simple helper whose constructor takes two arguments: a test
suite and a test case. The constructor just adds the test case to the
test suite. If we instantiate a new `AutoRegister` object right away
then we can uses its constructor to execute the desired code.

Of course this isn't really easier than remembering to add the test case
to the test suite in `main`. The `UTST_AUTO_TEST_CASE` macro does all
this for us and makes it trivial to define new test cases which are
automatically added to the global default test suite.

```
 UTST_AUTO_TEST_CASE( TestBasic )
 {
   std::vector<int> vec(2);
   vec.at(0) = 0; vec.at(1) = 1;

   UTST_CHECK_EQ( vec.size(), 2u );
   UTST_CHECK_EQ( vec.at(0),  0  );
   UTST_CHECK_EQ( vec.at(1),  1  );
 }
```

To add test cases to one of the other (non-default) test suites, we use
the `UTST_AUTO_EXTRA_TEST_CASE` macro which takes an additional argument
specifying the desired global test suite.

```
 UTST_AUTO_EXTRA_TEST_CASE( longrun, TestBasic )
 {
   std::vector<int> vec(2);
   vec.at(0) = 0; vec.at(1) = 1;

   UTST_CHECK_EQ( vec.size(), 2u );
   UTST_CHECK_EQ( vec.at(0),  0  );
   UTST_CHECK_EQ( vec.at(1),  1  );
 }
```

Finally, in the unit test's `main` function we can use the
`auto_command_line_driver` free function to automatically instantiate a
`CommandLineTestDriver`, add the three global test suites, and run the
unit tests. Then our `main` function simply looks like this:

```
 void main( int argc, char* argv[] )
 {
   utst::auto_command_line_driver( argc, argv );
 }
```

The auto registration features make writing unit tests very simple. Most
of the time we write our test cases using `UTST_AUTO_TEST_CASE` and
sometimes we use `UTST_AUTO_EXTRA_TEST_CASE` when we want to add a test
case to either the `longrun` or the `perf` test suites. In the unit
test's `main` function we just call the `auto_command_line_driver` free
function.

Although the auto registration features makes unit testing simple, we
can always explicitly use the framework as described in the earlier
sections when we need to do something non-standard (eg. parameterized
test cases, additional test suites, etc).

