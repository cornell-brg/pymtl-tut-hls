#=========================================================================
# VecIncrXcelHLS_test
#=========================================================================

import pytest
import random
import struct

random.seed(0xdeadbeef)

from pymtl         import *
from pclib.test    import mk_test_case_table, run_sim
from pclib.test    import TestSource, TestSink

from dstu.MemMsgFuture import MemMsg
from dstu.XcelMsg      import XcelReqMsg, XcelRespMsg

from VecincrXcelHLS import VecincrXcelHLS

from pclib.test.TestMemoryFuture import TestMemory

#-------------------------------------------------------------------------
# TestHarness
#-------------------------------------------------------------------------

class TestHarness (Model):

  def __init__( s, xcel, src_msgs, sink_msgs,
                stall_prob, latency, src_delay, sink_delay,
                dump_vcd=False, test_verilog=False ):

    # Instantiate models

    s.src  = TestSource ( XcelReqMsg(),  src_msgs,  src_delay     )
    s.xcel = xcel
    s.mem  = TestMemory ( MemMsg(8,32,32), 1, stall_prob, latency )
    s.sink = TestSink   ( XcelRespMsg(), sink_msgs, sink_delay    )

    # Translation

    if test_verilog:
      s.xcel = TranslationTool( s.xcel )

    # Connect

    s.connect( s.src.out,       s.xcel.xcelreq )

    s.connect( s.xcel.xcelresp, s.sink.in_     )

    s.connect( s.xcel.memreq,  s.mem.reqs[0]  )

    s.connect( s.xcel.memresp.msg,  s.mem.resps[0].msg )
    s.connect( s.xcel.memresp.val,  s.mem.resps[0].val )

    s.connect( s.mem.resps[0].rdy, 1 )

  def done( s ):
    return s.src.done and s.sink.done

  def line_trace( s ):
    return s.src.line_trace()  + " > " + \
           s.mem.line_trace()  + " > " + \
           s.sink.line_trace()

#-------------------------------------------------------------------------
# make messages
#-------------------------------------------------------------------------

def req( opaque, type_, raddr, data, id ):
  msg = XcelReqMsg()

  if   type_ == 'rd': msg.type_ = XcelReqMsg.TYPE_READ
  elif type_ == 'wr': msg.type_ = XcelReqMsg.TYPE_WRITE

  msg.opaque = opaque
  msg.raddr  = raddr
  msg.data   = data
  msg.id     = id
  return msg

def resp( opaque, type_, data, id ):
  msg = XcelRespMsg()

  if   type_ == 'rd': msg.type_ = XcelRespMsg.TYPE_READ
  elif type_ == 'wr': msg.type_ = XcelRespMsg.TYPE_WRITE

  msg.opaque = opaque
  msg.data   = data
  msg.id     = id
  return msg

#-------------------------------------------------------------------------
# Xcel Protocol
#-------------------------------------------------------------------------
# These are the source sink messages we need to configure the accelerator
# and wait for it to finish. We use the same messages in all of our
# tests. The difference between the tests is the data to be sorted in the
# test memory.

def gen_xcel_protocol_msgs( size, incr ):
  return [
    req( 124, 'wr', 1, 0x1000, 1 ), resp( 124, 'wr', 0, 1 ),
    req(   0, 'wr', 2, size,   1 ), resp(   0, 'wr', 0, 1 ),
    req(   4, 'wr', 3, incr,   1 ), resp(   4, 'wr', 0, 1 ),
    req( 255, 'wr', 0, 0,      1 ), resp( 255, 'wr', 0, 1 ),
    req(  24, 'rd', 0, 0,      1 ), resp(  24, 'rd', 1, 1 ),
  ]

#-------------------------------------------------------------------------
# Test Cases
#-------------------------------------------------------------------------

mini          = [ 0x21, 0x14, 0x42, 0x03 ]
small_data    = [ random.randint(0,0xffff)     for i in xrange(32) ]
large_data    = [ random.randint(0,0x7fffffff) for i in xrange(32) ]

#-------------------------------------------------------------------------
# Test Case Table
#-------------------------------------------------------------------------

test_case_table = mk_test_case_table([
                         #                  delays   test mem
                         #                  -------- ---------
  (                      "data        incr  src sink stall lat"),
  [ "mini",               mini,       1,    0,  0,   0,    0   ],
  [ "mini_delay_x4",      mini,       1,    3, 14,   0.5,  2   ],
  [ "small_data",         small_data, 2,    0,  0,   0,    0   ],
  [ "large_data",         large_data, 5,    0,  0,   0,    0   ],
  [ "small_data_3x14x0",  small_data, 4,    3, 14,   0,    0   ],
  [ "small_data_0x0x4",   small_data, 4,    0,  0,   0.5,  4   ],
  [ "small_data_3x14x4",  small_data, 4,    3, 14,   0.5,  4   ],
])

#-------------------------------------------------------------------------
# increment
#-------------------------------------------------------------------------
# helper function to verify results
def increment( data, incr ):
  for i in xrange( len( data ) ):
    data[i] = data[i] + incr
  return data

#-------------------------------------------------------------------------
# run_test
#-------------------------------------------------------------------------

def run_test( xcel, test_params, dump_vcd, test_verilog=False ):

  # Convert test data into byte array

  data       = test_params.data
  data_bytes = struct.pack("<{}I".format(len(data)),*data)

  # Protocol messages

  xcel_protocol_msgs = gen_xcel_protocol_msgs( len(data), test_params.incr )
  xreqs              = xcel_protocol_msgs[::2]
  xresps             = xcel_protocol_msgs[1::2]

  # Create test harness with protocol messagse

  th = TestHarness( xcel, xreqs, xresps,
                    test_params.stall, test_params.lat,
                    test_params.src, test_params.sink,
                    dump_vcd, test_verilog )

  # Load the data into the test memory

  th.mem.write_mem( 0x1000, data_bytes )

  # Run the test

  run_sim( th, dump_vcd, max_cycles=20000 )

  # Retrieve data from test memory

  result_bytes = th.mem.read_mem( 0x1000, len(data_bytes) )

  # Convert result bytes into list of ints

  result = list(struct.unpack("<{}I".format(len(data)),buffer(result_bytes)))

  # Compare result to sorted reference

  assert result == increment( data, test_params.incr )

#-------------------------------------------------------------------------
# Test cases
#-------------------------------------------------------------------------

@pytest.mark.parametrize( **test_case_table )
def test( test_params, dump_vcd ):
  VecincrXcelHLS.vcd_file = dump_vcd
  run_test( VecincrXcelHLS(), test_params, dump_vcd )

