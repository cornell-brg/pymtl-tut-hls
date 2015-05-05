#==============================================================================
# IteratorTranslationUnitFL_test.py
#==============================================================================

import pytest
import struct

from pymtl            import *
from pclib.test       import TestSource, TestSink, mk_test_case_table
from XcelMsg          import XcelMsg
from IteratorMsg      import IteratorMsg
from MemMsgFuture     import MemMsg
from TestMemoryOpaque import TestMemory

from IteratorTranslationUnitFL import IteratorTranslationUnitFL as ITU
from IteratorTranslationUnitFL import TypeDescriptor

#------------------------------------------------------------------------------
# TestHarness
#------------------------------------------------------------------------------
class TestHarness( Model ):

  def __init__( s, model,
                src_msgs,   sink_msgs,
                src_delay,  sink_delay,
                stall_prob, latency,
                dump_vcd=False ):

    # Interfaces
    cfg_ifc = XcelMsg()
    itu_ifc = IteratorMsg(32)
    mem_ifc = MemMsg(8,32,32)

    # Go bit
    s.go = Wire( 1 )

    # Instantiate Models
    s.src  = TestSource ( itu_ifc.req, src_msgs, src_delay )

    s.itu  = model ( cfg_ifc, itu_ifc, mem_ifc )

    s.sink = TestSink ( itu_ifc.resp, sink_msgs, sink_delay )

    s.mem  = TestMemory ( mem_ifc, 1, stall_prob, latency )

    # Connect
    s.connect( s.src.out.msg, s.itu.xcelreq.msg )

    s.connect( s.itu.xcelresp, s.sink.in_     )
    s.connect( s.itu.memreq,   s.mem.reqs[0]  )
    s.connect( s.itu.memresp,  s.mem.resps[0] )

    @s.combinational
    def logic():
      s.itu.xcelreq.val.value = s.src.out.val & s.go
      s.src.out.rdy.value     = s.itu.xcelreq.rdy & s.go

  def done( s ):
    return s.src.done and s.sink.done

  def line_trace( s ):
    return s.itu.line_trace()

#------------------------------------------------------------------------------
# run_itu_test
#------------------------------------------------------------------------------
def run_itu_test( model, mem_array, ds_type, dump_vcd = None ):

  # Elaborate
  model.vcd_file = dump_vcd
  model.elaborate()

  # Create a simulator
  sim = SimulationTool( model )

  # Load the memory
  if mem_array:
    model.mem.write_mem( mem_array[0], mem_array[1] )

  # Run simulation
  sim.reset()
  print

  # Start itu configuration
  sim.cycle()
  sim.cycle()

  # Alloc data structure
  model.itu.cfgreq.val.next       = 1       # set the request
  model.itu.cfgreq.msg.type_.next = 1       # write request
  model.itu.cfgreq.msg.data.next  = ds_type # request for ds
  model.itu.cfgreq.msg.raddr.next = 1       # alloc
  model.itu.cfgreq.msg.id.next    = 0       # dont care for now
  model.itu.cfgresp.rdy.next      = 1

  sim.cycle()

  # ds-id allocated by the itu
  alloc_ds_id = model.itu.cfgresp.msg.data

  sim.cycle()
  sim.cycle()

  # Init data structure - ds_desc
  model.itu.cfgreq.msg.data.next  = mem_array[0]+4 # base addr
  model.itu.cfgreq.msg.raddr.next = 2              # init
  model.itu.cfgreq.msg.id.next    = alloc_ds_id    # id of the ds

  sim.cycle()
  sim.cycle()

  # Init data structure - dt_desc
  model.itu.cfgreq.msg.data.next  = mem_array[0] # dt_desc_ptr
  model.itu.cfgreq.msg.raddr.next = 3            # init
  model.itu.cfgreq.msg.id.next    = alloc_ds_id  # id of the ds

  sim.cycle()
  sim.cycle()

  # End itu configuration
  model.itu.cfgreq.val.next = 0

  sim.cycle()
  sim.cycle()

  # Allow source to inject messages
  model.itu.cfgresp.rdy.next  = 0
  model.go.next = 1

  while not model.done() and sim.ncycles < 250:
    sim.print_line_trace()
    sim.cycle()
  sim.print_line_trace()
  assert model.done()

  # Add a couple extra ticks so that the VCD dump is nicer
  sim.cycle()
  sim.cycle()
  sim.cycle()

#------------------------------------------------------------------------------
# Utility functions for creating arrays formatted for memory loading.
#------------------------------------------------------------------------------
# mem_array_int
def mem_array_int( base_addr, data ):
  bytes = struct.pack( "<{}i".format( len(data) ), *data )
  return [base_addr, bytes]

# mem_array_uchar
def mem_array_uchar( base_addr, data ):
  bytes = struct.pack( "<{}B".format( len(data) ), *data )
  return [base_addr, bytes]

#------------------------------------------------------------------------------
# Helper functions for Test src/sink messages
#------------------------------------------------------------------------------

req  = IteratorMsg( 32 ).req.mk_msg
resp = IteratorMsg( 32 ).resp.mk_msg

def req_wr( ds_id, iter, field, data ):
  return req( 0, 1, ds_id, iter, field, data )

def req_rd( ds_id, iter, field, data ):
  return req( 0, 0, ds_id, iter, field, data )

def resp_wr( data ):
  return resp( 0, 1, data )

def resp_rd(  data ):
  return resp( 0, 0, data )

#------------------------------------------------------------------------------
# Memory array and messages to test vector of integers
#------------------------------------------------------------------------------

# preload the memory to known values
vec_int_mem = mem_array_int( 8, [0x00040000,1,2,3,4] )

# messages that assume memory is preloaded and test for the case using the
# data structure with an id value to be 0
vector_int_msgs = [
  req_rd( 0, 0, 0, 0 ), resp_rd( 0x00000001 ),
  req_rd( 0, 1, 0, 0 ), resp_rd( 0x00000002 ),
  req_rd( 0, 2, 0, 0 ), resp_rd( 0x00000003 ),
  req_rd( 0, 3, 0, 0 ), resp_rd( 0x00000004 ),
  req_wr( 0, 0, 0, 7 ), resp_wr( 0x00000000 ),
  req_rd( 0, 0, 0, 0 ), resp_rd( 0x00000007 ),
]

#------------------------------------------------------------------------------
# Memory array and messages to test vector of unsigned chars
#------------------------------------------------------------------------------

# preload the memory to known values
# NOTE: The first four bytes describe the dt_descriptor written out in
# little-endian format
vec_uchar_mem = mem_array_uchar( 8, [0,0,1,0,1,2,3,4] )

# messages that assume memory is preloaded and test for the case using the
# data structure with an id value to be 0
vector_uchar_msgs = [
  req_rd( 0, 0, 0, 0 ), resp_rd( 0x00000001 ),
  req_rd( 0, 1, 0, 0 ), resp_rd( 0x00000002 ),
  req_rd( 0, 2, 0, 0 ), resp_rd( 0x00000003 ),
  req_rd( 0, 3, 0, 0 ), resp_rd( 0x00000004 ),
  req_wr( 0, 0, 0, 7 ), resp_wr( 0x00000000 ),
  req_rd( 0, 0, 0, 0 ), resp_rd( 0x00000007 ),
]

#------------------------------------------------------------------------------
# Memory array and messages to test list of integers
#------------------------------------------------------------------------------

# preload the memory to known values
list_int_mem = mem_array_int( 0,
                                [   #metadata
                                    0x00040000,
                                    # value prev next
                                    1,      4, 16,
                                    2,      4, 28,
                                    3,     16, 40,
                                    4,     28, 0,
                                ]
                             )

# messages that assume memory is preloaded and test for the case using the
# data structure with an id value to be 0
list_int_msgs = [
  req_rd( 0, 0, 0, 0 ), resp_rd( 0x00000001 ),
  req_rd( 0, 1, 0, 0 ), resp_rd( 0x00000002 ),
  req_rd( 0, 2, 0, 0 ), resp_rd( 0x00000003 ),
  req_rd( 0, 3, 0, 0 ), resp_rd( 0x00000004 ),
  req_wr( 0, 0, 0, 5 ), resp_wr( 0x00000000 ),
  req_wr( 0, 1, 0, 6 ), resp_wr( 0x00000000 ),
  req_wr( 0, 2, 0, 7 ), resp_wr( 0x00000000 ),
  req_rd( 0, 1, 0, 0 ), resp_rd( 0x00000006 ),
  req_rd( 0, 0, 0, 0 ), resp_rd( 0x00000005 ),
  req_rd( 0, 2, 0, 0 ), resp_rd( 0x00000007 ),
]

#-------------------------------------------------------------------------
# Test Case Table
#-------------------------------------------------------------------------

test_case_table = mk_test_case_table([
  (                         "msgs              src sink stall lat  mem            ds" ),
  [ "vec_int_0x0_0.0_0",    vector_int_msgs,   0,  0,   0.0,  0,   vec_int_mem,   ITU.VECTOR ],
  [ "vec_int_5x0_0.5_0",    vector_int_msgs,   5,  0,   0.5,  0,   vec_int_mem,   ITU.VECTOR ],
  [ "vec_int_0x5_0.0_4",    vector_int_msgs,   0,  5,   0.0,  4,   vec_int_mem,   ITU.VECTOR ],
  [ "vec_int_3x9_0.5_3",    vector_int_msgs,   3,  9,   0.5,  3,   vec_int_mem,   ITU.VECTOR ],
  [ "vec_uchar_0x0_0.0_0",  vector_uchar_msgs, 0,  0,   0.0,  0,   vec_uchar_mem, ITU.VECTOR ],
  [ "vec_uchar_5x0_0.5_0",  vector_uchar_msgs, 5,  0,   0.5,  0,   vec_uchar_mem, ITU.VECTOR ],
  [ "vec_uchar_0x5_0.0_4",  vector_uchar_msgs, 0,  5,   0.0,  4,   vec_uchar_mem, ITU.VECTOR ],
  [ "vec_uchar_3x9_0.5_3",  vector_uchar_msgs, 3,  9,   0.5,  3,   vec_uchar_mem, ITU.VECTOR ],
  [ "list_int_0x0_0.0_0",   list_int_msgs,     0,  0,   0.0,  0,   list_int_mem,  ITU.LIST   ],
  [ "list_int_5x0_0.5_0",   list_int_msgs,     5,  0,   0.5,  0,   list_int_mem,  ITU.LIST   ],
  [ "list_int_0x5_0.0_4",   list_int_msgs,     0,  5,   0.0,  4,   list_int_mem,  ITU.LIST   ],
  [ "list_int_3x9_0.5_3",   list_int_msgs,     3,  9,   0.5,  3,   list_int_mem,  ITU.LIST   ],
])

#-------------------------------------------------------------------------
# Test cases
#-------------------------------------------------------------------------

@pytest.mark.parametrize( **test_case_table )
def test( test_params, dump_vcd ):

  run_itu_test( TestHarness(  ITU,
                              test_params.msgs[::2],
                              test_params.msgs[1::2],
                              test_params.src,
                              test_params.sink,
                              test_params.stall,
                              test_params.lat ),
                test_params.mem,
                test_params.ds,
                dump_vcd )
