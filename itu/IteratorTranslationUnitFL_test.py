#==============================================================================
# IteratorTranslationUnitFL_test
#==============================================================================

import pytest
import struct

from pymtl                       import *
from pclib.test                  import TestSource, TestSink, mk_test_case_table
from pclib.ifcs                  import MemMsg
from pclib.test.TestMemoryFuture import TestMemory

from IteratorTranslationUnitFL   import IteratorTranslationUnitFL as ITU
from IteratorMsg                 import IteratorMsg
from CoprocessorMsg              import CoprocessorMsg

#------------------------------------------------------------------------------
# TestHarness
#------------------------------------------------------------------------------
class TestHarness( Model ):

  def __init__( s, TranslationUnit,
                src_msgs,  sink_msgs,
                src_delay, sink_delay,
                mem_delay ):

    # Interfaces
    cfg_ifc          = CoprocessorMsg(  5, 32, 32 )
    accel_ifc        = IteratorMsg   (         32 )
    mem_ifc          = MemMsg        (     32, 32 )

    # Go bit
    s.go             = Wire( 1 )

    # Instantiate Models
    s.src            = TestSource     ( accel_ifc.req, src_msgs, src_delay    )
    s.itu            = TranslationUnit( cfg_ifc, accel_ifc, mem_ifc           )
    s.sink           = TestSink       ( accel_ifc.resp, sink_msgs, sink_delay )
    s.mem            = TestMemory     ( mem_ifc, 1, 0, mem_delay              )

    # Connect
    s.connect( s.src.out.msg,            s.itu.accel_ifc.req_msg )

    s.connect( s.itu.accel_ifc.resp_msg, s.sink.in_.msg          )
    s.connect( s.itu.accel_ifc.resp_val, s.sink.in_.val          )
    s.connect( s.itu.accel_ifc.resp_rdy, s.sink.in_.rdy          )

    s.connect( s.itu.mem_ifc.req_msg,    s.mem.reqs[0].msg       )
    s.connect( s.itu.mem_ifc.req_val,    s.mem.reqs[0].val       )
    s.connect( s.itu.mem_ifc.req_rdy,    s.mem.reqs[0].rdy       )

    s.connect( s.mem.resps[0].msg,       s.itu.mem_ifc.resp_msg  )
    s.connect( s.mem.resps[0].val,       s.itu.mem_ifc.resp_val  )
    s.connect( s.mem.resps[0].rdy,       s.itu.mem_ifc.resp_rdy  )

    @s.combinational
    def logic():
      s.itu.accel_ifc.req_val.value = s.src.out.val & s.go
      s.src.out.rdy.value           = s.itu.accel_ifc.req_rdy & s.go

  def done( s ):
    return s.src.done and s.sink.done

  def line_trace( s ):
    return  s.src.line_trace() + " > " + \
            s.itu.line_trace() + " > " + \
            s.sink.line_trace()

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
  model.itu.cfg_ifc.req_val.next       = 1       # set the request
  model.itu.cfg_ifc.req_msg.type_.next = 1       # write request
  model.itu.cfg_ifc.req_msg.data.next  = ds_type # request for vector of ints
  model.itu.cfg_ifc.req_msg.addr.next  = 1       # alloc
  model.itu.cfg_ifc.req_msg.id.next    = 0       # dont care for now
  model.itu.cfg_ifc.resp_rdy.next      = 1

  sim.cycle()

  # ds-id allocated by the itu
  alloc_ds_id = model.itu.cfg_ifc.resp_msg.data

  sim.cycle()
  sim.cycle()

  # Init data structure
  model.itu.cfg_ifc.req_msg.data.next = mem_array[0] # base addr
  model.itu.cfg_ifc.req_msg.addr.next = 2            # init
  model.itu.cfg_ifc.req_msg.id.next   = alloc_ds_id  # id of the ds

  sim.cycle()
  sim.cycle()

  # End itu configuration
  model.itu.cfg_ifc.req_val.next      = 0

  sim.cycle()
  sim.cycle()

  # Allow source to inject messages
  model.itu.cfg_ifc.resp_rdy.next  = 0
  model.go.next = 1

  while not model.done() and sim.ncycles < 200:
    sim.print_line_trace()
    sim.cycle()
  sim.print_line_trace()
  assert model.done()

  # Add a couple extra ticks so that the VCD dump is nicer
  sim.cycle()
  sim.cycle()
  sim.cycle()

#------------------------------------------------------------------------------
# mem_array_32bit
#------------------------------------------------------------------------------
# Utility function for creating arrays formatted for memory loading.
def mem_array_32bit( base_addr, data ):
  bytes = struct.pack( "<{}i".format( len(data) ), *data )
  return [base_addr, bytes]

#------------------------------------------------------------------------------
# Helper functions for Test src/sink messages
#------------------------------------------------------------------------------

req  = IteratorMsg( 32 ).req.mk_req
resp = IteratorMsg( 32 ).resp.mk_resp

def req_wr( ds_id, index, data ):
  return req( 1, ds_id, index, data )

def req_rd( ds_id, index, data ):
  return req( 0, ds_id, index, data )

def resp_wr( data ):
  return resp( 1, data )

def resp_rd(  data ):
  return resp( 0, data )

#------------------------------------------------------------------------------
# Memory array and messages to test vector of integers
#------------------------------------------------------------------------------

# preload the memory to known values
vec_mem = mem_array_32bit( 8, [1,2,3,4] )

# messages that assume memory is preloaded and test for the case using the
# data structure with an id value to be 0
vector_int_msgs = [
  req_rd( 0, 0, 0 ), resp_rd( 0x00000001 ),
  req_rd( 0, 1, 0 ), resp_rd( 0x00000002 ),
  req_rd( 0, 2, 0 ), resp_rd( 0x00000003 ),
  req_rd( 0, 3, 0 ), resp_rd( 0x00000004 ),
  req_wr( 0, 0, 7 ), resp_wr( 0x00000000 ),
  req_rd( 0, 0, 0 ), resp_rd( 0x00000007 ),
]

#------------------------------------------------------------------------------
# Memory array and messages to test list of integers
#------------------------------------------------------------------------------

# preload the memory to known values
list_mem = mem_array_32bit( 0,    # value prev next
                                [
                                    1,    0,   12,
                                    2,    0,   24,
                                    3,    12,  36,
                                    4,    24,  0,
                                ]
                          )

# messages that assume memory is preloaded and test for the case using the
# data structure with an id value to be 0
list_int_msgs = [
  req_rd( 0, 0, 0 ), resp_rd( 0x00000001 ),
  req_rd( 0, 1, 0 ), resp_rd( 0x00000002 ),
  req_rd( 0, 2, 0 ), resp_rd( 0x00000003 ),
  req_rd( 0, 3, 0 ), resp_rd( 0x00000004 ),
  req_wr( 0, 0, 5 ), resp_wr( 0x00000000 ),
  req_wr( 0, 1, 6 ), resp_wr( 0x00000000 ),
  req_wr( 0, 2, 7 ), resp_wr( 0x00000000 ),
  req_rd( 0, 1, 0 ), resp_rd( 0x00000006 ),
  req_rd( 0, 0, 0 ), resp_rd( 0x00000005 ),
  req_rd( 0, 2, 0 ), resp_rd( 0x00000007 ),
]

#-------------------------------------------------------------------------
# Test Case Table
#-------------------------------------------------------------------------

test_case_table = mk_test_case_table([
  (         "msgs            src_delay sink_delay mem_array  ds_type "),
  [ "0x0",  vector_int_msgs, 0,        0,         vec_mem,   1 ],
  [ "5x0",  vector_int_msgs, 5,        0,         vec_mem,   1 ],
  [ "0x5",  vector_int_msgs, 0,        5,         vec_mem,   1 ],
  [ "3x9",  vector_int_msgs, 3,        9,         vec_mem,   1 ],
  [ "0x0",  list_int_msgs,   0,        0,         list_mem,  5 ],
  [ "5x0",  list_int_msgs,   5,        0,         list_mem,  5 ],
  [ "0x5",  list_int_msgs,   0,        5,         list_mem,  5 ],
  [ "3x9",  list_int_msgs,   3,        9,         list_mem,  5 ],
])

#-------------------------------------------------------------------------
# Test cases
#-------------------------------------------------------------------------

@pytest.mark.parametrize( **test_case_table )
def test( test_params, dump_vcd ):

  run_itu_test( TestHarness(  ITU,
                              test_params.msgs[::2],
                              test_params.msgs[1::2],
                              test_params.src_delay,
                              test_params.sink_delay, 0 ),
                test_params.mem_array,
                test_params.ds_type,
                dump_vcd )
