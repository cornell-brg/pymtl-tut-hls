#=========================================================================
# PopCount_test
#=========================================================================

import pytest
import random

from pymtl       import *
from pclib.test  import mk_test_case_table, run_sim
from pclib.test  import TestSource, TestSink

from PopCount  import PopCount_wrapper

from pclib.ifcs.XcelMsg       import *

#-------------------------------------------------------------------------
# TestHarness
#-------------------------------------------------------------------------

class TestHarness (Model):

  def __init__( s, xcel, src_msgs, sink_msgs,
                src_delay, sink_delay,
                dump_vcd=False, test_verilog=False ):

    # Instantiate models

    s.src  = TestSource ( Bits(64),  src_msgs,  src_delay  )
    s.xcel = xcel
    s.sink = TestSink   ( Bits(32), sink_msgs, sink_delay )

    # Dump VCD

    if dump_vcd:
      s.xcel.vcd_file = dump_vcd

    # Translation

    if test_verilog:
      s.xcel = TranslationTool( s.xcel )

    # Connect

    s.connect( s.src.out,       s.xcel.xcelreq )
    s.connect( s.xcel.xcelresp, s.sink.in_     )

  def done( s ):
    return s.src.done and s.sink.done

  def line_trace( s ):
    return s.src.line_trace()  + " > " + \
           s.xcel.line_trace() + " | " + \
           s.sink.line_trace()

#-------------------------------------------------------------------------
# Test Cases
#-------------------------------------------------------------------------

mini = [ 0x21, 0x14, 0x1133 ]
mini_results = [ 2, 2, 6 ]

#-------------------------------------------------------------------------
# Test Case Table
#-------------------------------------------------------------------------

test_case_table = mk_test_case_table([
  (                      "data    result            src sink "),
  [ "mini",               mini,   mini_results,     0,  0,   ],
  [ "mini_0_2",           mini,   mini_results,     0,  2,   ],
  [ "mini_2_1",           mini,   mini_results,     2,  1,   ],
])

#-------------------------------------------------------------------------
# run_test
#-------------------------------------------------------------------------

def run_test( xcel, test_params, dump_vcd, test_verilog=False ):

  # Convert test data into byte array

  data = test_params.data
  result = test_params.result

  # Protocol messages

  xreqs  = data
  xresps = result

  # Create test harness with protocol messagse

  th = TestHarness( xcel, xreqs, xresps,
                    test_params.src, test_params.sink,
                    dump_vcd, test_verilog )

  # Run the test

  run_sim( th, dump_vcd, max_cycles=20000 )

#-------------------------------------------------------------------------
# Test cases
#-------------------------------------------------------------------------

@pytest.mark.parametrize( **test_case_table )
def test( test_params, dump_vcd ):
  run_test( PopCount_wrapper(), test_params, dump_vcd )

