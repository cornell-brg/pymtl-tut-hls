#==============================================================================
# IteratorTranslationUnitHLS_test.py
#==============================================================================

import pytest
import struct

from pymtl             import *
from pclib.test        import TestSource, TestSink, mk_test_case_table
from IteratorMsg       import IteratorMsg

from dstu.XcelMsg      import XcelMsg
from dstu.MemMsgFuture import MemMsg

from dstu.TestMemoryOpaque  import TestMemory

#from IteratorTranslationUnitHLS import IteratorTranslationUnitHLS_Wrapper as ITU
from IteratorTranslationUnitHLSAlt import IteratorTranslationUnitHLSAlt as ITU
#from PolyDsu import PolyDsu as ITU

from dstu.TypeDescriptor import TypeDescriptor

#------------------------------------------------------------------------------
# TestHarness
#------------------------------------------------------------------------------
class TestHarness( Model ):

  def __init__( s, model,
                src_msgs,   sink_msgs,
                src_delay,  sink_delay,
                stall_prob, latency,
                dump_vcd=False,
                test_verilog=False
              ):

    # Interfaces
    cfg_ifc = XcelMsg()
    itu_ifc = IteratorMsg(32)
    mem_ifc = MemMsg(8,32,32)

    # Go bit
    s.go = Wire( 1 )

    # Instantiate Models
    s.src  = TestSource ( itu_ifc.req, src_msgs, src_delay )

    # Translate the model into verilog
    if test_verilog:
      s.itu = TranslationTool( model() )
    else:
      s.itu  = model()

    s.sink = TestSink ( itu_ifc.resp, sink_msgs, sink_delay )

    s.mem  = TestMemory ( mem_ifc, 1, stall_prob, latency )

    # Connect
    s.connect( s.src.out.msg, s.itu.xcelreq.msg )

    s.connect( s.itu.xcelresp, s.sink.in_     )
    s.connect( s.itu.memreq,   s.mem.reqs[0]  )
    s.connect( s.itu.memresp,  s.mem.resps[0] )

    #s.connect( s.itu.debug.rdy, 1 )

    @s.combinational
    def logic():
      s.itu.xcelreq.val.value = s.src.out.val & s.go
      s.src.out.rdy.value     = s.itu.xcelreq.rdy & s.go

  def done( s ):
    return s.src.done and s.sink.done

  def line_trace( s ):
    return s.itu.line_trace()
    #return "{} | {} | {}".format( s.src.out,
    #                              s.mem.line_trace(),
    #                              s.sink.in_ )

#------------------------------------------------------------------------------
# run_itu_test
#------------------------------------------------------------------------------
def run_itu_test( model, mem_array, ds_type, ds_offset, dump_vcd = None,
                  test_verilog = None ):

  model.vcd_file = dump_vcd

  # Elaborate the model
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
  model.itu.cfgreq.msg.id.next    = 3       # DSTU_ID
  model.itu.cfgresp.rdy.next      = 1

  while model.itu.cfgreq.rdy.value == 0:
    sim.print_line_trace()
    sim.cycle()

  #print "Sent ALLOC"

  model.itu.cfgreq.val.next       = 0

  while model.itu.cfgresp.val.value == 0:
    sim.print_line_trace()
    sim.cycle()

  # ds-id allocated by the itu
  alloc_ds_id = model.itu.cfgresp.msg.data

  #print "Received ALLOC response:", alloc_ds_id

  # Init data structure - ds_desc
  model.itu.cfgreq.val.next       = 1
  model.itu.cfgreq.msg.data.next  = mem_array[0]+ds_offset # base addr
  model.itu.cfgreq.msg.raddr.next = 1                      # init
  model.itu.cfgreq.msg.id.next    = alloc_ds_id            # id of the ds

  while model.itu.cfgreq.rdy.value == 0:
    sim.print_line_trace()
    sim.cycle()

  #print "Sent DS Descriptor"

  model.itu.cfgreq.val.next       = 0

  while model.itu.cfgresp.val.value == 0:
    sim.print_line_trace()
    sim.cycle()

  #print "Received DS Descriptor response", model.itu.cfgresp.msg

  # Init data structure - dt_desc
  model.itu.cfgreq.val.next       = 1
  model.itu.cfgreq.msg.data.next  = mem_array[0] # dt_desc_ptr
  model.itu.cfgreq.msg.raddr.next = 2            # init
  model.itu.cfgreq.msg.id.next    = alloc_ds_id  # id of the ds

  while model.itu.cfgreq.rdy.value == 0:
    sim.print_line_trace()
    sim.cycle()

  #print "Sent DT Descriptor"

  model.itu.cfgreq.val.next       = 0

  # End itu configuration
  while model.itu.cfgresp.val.value == 0:
    sim.print_line_trace()
    sim.cycle()

  #print "Received DT Descriptor response", model.itu.cfgresp.msg

  # Allow source to inject messages
  model.go.next = 1

  while not model.done() and sim.ncycles < 1000:
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

def req_rd( ds_id, iter, addr, data ):
  return req( 0, ds_id, 0, iter, addr, data )

def req_wr( ds_id, iter, addr, data ):
  return req( 0, ds_id, 1, iter, addr, data )

def req_inc( ds_id, iter, addr, data ):
  return req( 0, ds_id, 2, iter, addr, data )

def req_dec( ds_id, iter, addr, data ):
  return req( 0, ds_id, 3, iter, addr, data )

def resp_rd( ds_id, addr, data ):
  return resp( 0, ds_id, 0, addr, data )

def resp_wr( ds_id, addr, data ):
  return resp( 0, ds_id, 1, addr, data )

def resp_inc( ds_id, addr, data ):
  return resp( 0, ds_id, 2, addr, data )

def resp_dec( ds_id, addr, data ):
  return resp( 0, ds_id, 3, addr, data )

#------------------------------------------------------------------------------
# Memory array and messages to test vector of integers
#------------------------------------------------------------------------------

# preload the memory to known values
vector_int_mem = mem_array_int( 8, [0x00040000,1,2,3,4] )

# messages that assume memory is preloaded and test for the case using the
# data structure with an id value to be 0
vector_int_msgs = [
  req_rd ( 0, 0, 12, 0 ), resp_rd ( 0, 12, 1 ),
  req_rd ( 0, 1, 16, 0 ), resp_rd ( 0, 16, 2 ),
  req_rd ( 0, 2, 20, 0 ), resp_rd ( 0, 20, 3 ),
  req_rd ( 0, 3, 24, 0 ), resp_rd ( 0, 24, 4 ),
  req_wr ( 0, 3, 24, 5 ), resp_wr ( 0, 24, 0 ),
  req_wr ( 0, 2, 20, 6 ), resp_wr ( 0, 20, 0 ),
  req_rd ( 0, 2, 20, 0 ), resp_rd ( 0, 20, 6 ),
  req_rd ( 0, 3, 24, 0 ), resp_rd ( 0, 24, 5 ),
  req_inc( 0, 2, 20, 0 ), resp_inc( 0, 24, 0 ),
  req_dec( 0, 2, 20, 0 ), resp_dec( 0, 16, 0 ),
]

#------------------------------------------------------------------------------
# Memory array and messages to test vector of unsigned chars
#------------------------------------------------------------------------------

# preload the memory to known values
# NOTE: The first four bytes describe the dt_descriptor written out in
# little-endian format
vector_uchar_mem = mem_array_uchar( 8, [0,0,1,0,1,2,3,4] )

vector_uchar_msgs = [
  req_rd ( 0, 0, 12, 0 ), resp_rd ( 0, 12, 1 ),
  req_rd ( 0, 1, 13, 0 ), resp_rd ( 0, 13, 2 ),
  req_rd ( 0, 2, 14, 0 ), resp_rd ( 0, 14, 3 ),
  req_rd ( 0, 3, 15, 0 ), resp_rd ( 0, 15, 4 ),
  req_wr ( 0, 3, 15, 5 ), resp_wr ( 0, 15, 0 ),
  req_wr ( 0, 2, 14, 6 ), resp_wr ( 0, 14, 0 ),
  req_rd ( 0, 2, 14, 0 ), resp_rd ( 0, 14, 6 ),
  req_rd ( 0, 3, 15, 0 ), resp_rd ( 0, 15, 5 ),
  req_inc( 0, 2, 13, 0 ), resp_inc( 0, 14, 0 ),
  req_dec( 0, 2, 12, 0 ), resp_dec( 0, 11, 0 ),
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
  req_rd ( 0, 0, 4,  0 ), resp_rd ( 0, 4,  1 ),
  req_rd ( 0, 0, 16, 0 ), resp_rd ( 0, 16, 2 ),
  req_rd ( 0, 0, 28, 0 ), resp_rd ( 0, 28, 3 ),
  req_rd ( 0, 0, 40, 0 ), resp_rd ( 0, 40, 4 ),

  req_inc( 0, 0, 4,  0 ), resp_inc( 0, 16, 0 ),
  req_inc( 0, 0, 16, 0 ), resp_inc( 0, 28, 0 ),
  req_inc( 0, 0, 28, 0 ), resp_inc( 0, 40, 0 ),
  req_inc( 0, 0, 40, 0 ), resp_inc( 0,  0, 0 ),

  req_dec( 0, 0, 4,  0 ), resp_dec( 0,  4, 0 ),
  req_dec( 0, 0, 16, 0 ), resp_dec( 0,  4, 0 ),
  req_dec( 0, 0, 28, 0 ), resp_dec( 0, 16, 0 ),
  req_dec( 0, 0, 40, 0 ), resp_dec( 0, 28, 0 ),

  req_wr ( 0, 0, 4,  5 ), resp_wr ( 0, 4,  0 ),
  req_wr ( 0, 0, 16, 6 ), resp_wr ( 0, 16, 0 ),
  req_wr ( 0, 0, 28, 7 ), resp_wr ( 0, 28, 0 ),
  req_wr ( 0, 0, 40, 8 ), resp_wr ( 0, 40, 0 ),

  req_rd ( 0, 0, 4,  0 ), resp_rd ( 0, 4,  5 ),
  req_rd ( 0, 0, 16, 0 ), resp_rd ( 0, 16, 6 ),
  req_rd ( 0, 0, 28, 0 ), resp_rd ( 0, 28, 7 ),
  req_rd ( 0, 0, 40, 0 ), resp_rd ( 0, 40, 8 ),
]

#-------------------------------------------------------------------------
# Test Case Table
#-------------------------------------------------------------------------

test_case_table = mk_test_case_table([
  (                           "msgs              src sink stall lat  mem               ds  ds_offset" ),
  [ "list_int_0x0_0.0_0",     list_int_msgs,     0,  0,   0.0,  0,   list_int_mem,     1,  4],
  [ "list_int_5x0_0.5_0",     list_int_msgs,     5,  0,   0.5,  0,   list_int_mem,     1,  4],
  [ "list_int_0x5_0.0_4",     list_int_msgs,     0,  5,   0.0,  4,   list_int_mem,     1,  4],
  [ "list_int_3x9_0.5_3",     list_int_msgs,     3,  9,   0.5,  3,   list_int_mem,     1,  4],
  [ "vector_int_0x0_0.0_0",   vector_int_msgs,   0,  0,   0.0,  0,   vector_int_mem,   0,  4],
  [ "vector_int_5x0_0.5_0",   vector_int_msgs,   5,  0,   0.5,  0,   vector_int_mem,   0,  4],
  [ "vector_int_0x5_0.0_4",   vector_int_msgs,   0,  5,   0.0,  4,   vector_int_mem,   0,  4],
  [ "vector_int_3x9_0.5_3",   vector_int_msgs,   3,  9,   0.5,  3,   vector_int_mem,   0,  4],
  [ "vector_uchar_0x0_0.0_0", vector_uchar_msgs, 0,  0,   0.0,  0,   vector_uchar_mem, 0,  4],
  [ "vector_uchar_5x0_0.5_0", vector_uchar_msgs, 5,  0,   0.5,  0,   vector_uchar_mem, 0,  4],
  [ "vector_uchar_0x5_0.0_4", vector_uchar_msgs, 0,  5,   0.0,  4,   vector_uchar_mem, 0,  4],
  [ "vector_uchar_3x9_0.5_3", vector_uchar_msgs, 3,  9,   0.5,  3,   vector_uchar_mem, 0,  4],
])

#-------------------------------------------------------------------------
# Test cases
#-------------------------------------------------------------------------

@pytest.mark.parametrize( **test_case_table )
def test( test_params, dump_vcd, test_verilog ):

  ITU.vcd_file = dump_vcd
  run_itu_test( TestHarness(  ITU,
                              test_params.msgs[::2],
                              test_params.msgs[1::2],
                              test_params.src,
                              test_params.sink,
                              test_params.stall,
                              test_params.lat,
                              dump_vcd,
                              test_verilog ),
                test_params.mem,
                test_params.ds,
                test_params.ds_offset,
                dump_vcd,
                test_verilog )
