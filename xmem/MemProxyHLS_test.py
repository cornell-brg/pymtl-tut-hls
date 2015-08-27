#=========================================================================
# MemProxyHLS_test
#=========================================================================
# Replicate tests from MemProxyHLS.t.cc

import pytest
import struct

from pymtl        import *
from pclib.test   import run_sim

from pclib.test   import TestSource, TestSink

from TestMemoryOpaque import TestMemory
from MemMsgFuture     import MemMsg

from xcel.XcelMsg     import XcelReqMsg, XcelRespMsg

from MemProxyHLS import MemProxyHLS

#-------------------------------------------------------------------------
# TestHarness
#-------------------------------------------------------------------------

class TestHarness (Model):

  def __init__( s, xcel, src_msgs, sink_msgs,
                dump_vcd=False, test_verilog=False ):

    # Instantiate models

    s.src  = TestSource ( XcelReqMsg(), src_msgs )
    s.xcel = xcel
    s.mem  = TestMemory ( MemMsg(8,32,32), 1 )
    s.sink = TestSink   ( XcelRespMsg(), sink_msgs )

    # Dump VCD

    if dump_vcd:
      s.xcel.vcd_file = dump_vcd

    # Translation

    if test_verilog:
      s.xcel = TranslationTool( s.xcel )

    # Connect

    s.connect( s.src.out,       s.xcel.xcelreq )
    s.connect( s.xcel.memreq,   s.mem.reqs[0]  )
    s.connect( s.xcel.memresp,  s.mem.resps[0] )
    s.connect( s.xcel.xcelresp, s.sink.in_     )

  def done( s ):
    return s.src.done and s.sink.done

  def line_trace( s ):
    return s.src.line_trace()  + " > " + \
           s.xcel.line_trace() + " | " + \
           s.mem.line_trace()  + " > " + \
           s.sink.line_trace()

#-------------------------------------------------------------------------
# run_test
#-------------------------------------------------------------------------

def run_test( test_num, data_in_addr, data_in, data_out_addr, data_out ):

  # Convert test data into byte array

  data_in_bytes = struct.pack("<{}I".format(len(data_in)),*data_in)

  # Protocol messages

  msg = XcelReqMsg()
  msg.type_  = XcelReqMsg.TYPE_WRITE
  msg.raddr  = 0
  msg.data   = test_num
  msg.opaque = 0x88
  xreqs = [ msg ]

  msg = XcelRespMsg()
  msg.type_  = XcelRespMsg.TYPE_WRITE
  msg.data   = 0
  msg.opaque = 0x88
  xresps = [ msg ]

  # Create test harness with protocol messagse

  th = TestHarness( MemProxyHLS(), xreqs, xresps )

  # Load the data into the test memory

  th.mem.write_mem( data_in_addr, data_in_bytes )

  # Run the test

  run_sim( th, dump_vcd=False, max_cycles=20000 )

  # Retrieve data from test memory

  result_bytes = th.mem.read_mem( data_out_addr, len(data_out)*4 )

  # Convert result bytes into list of ints

  result = list( struct.unpack( "<{}I".format(len(data_out)),
                                buffer(result_bytes) ))

  # Compare result to sorted reference

  assert result == data_out

#-------------------------------------------------------------------------
# test_basic_write
#-------------------------------------------------------------------------

def test_basic_write():
  data_in  = [    ]
  data_out = [ 42 ]
  run_test( 0, 0x1000, data_in, 0x1000, data_out )

#-------------------------------------------------------------------------
# test_copy
#-------------------------------------------------------------------------

def test_copy():
  data_in  = [ 13 ]
  data_out = [ 13 ]
  run_test( 1, 0x1000, data_in, 0x2000, data_out )

#-------------------------------------------------------------------------
# test_operators
#-------------------------------------------------------------------------

def test_operators():
  data_in  = [ ]
  data_out = [ 42, 47, 0x00010100 ]
  run_test( 2, 0x1000, data_in, 0x2000, data_out )

#-------------------------------------------------------------------------
# test_memoize
#-------------------------------------------------------------------------

def test_memoize():
  data_in  = [ 0x0eadbeef ]
  data_out = [ 0x0eadbeef, 0x0eadbeef, 0x0eadbeef ]
  run_test( 3, 0x1000, data_in, 0x2000, data_out )

