#=========================================================================
# IteratorPipelineUnit_test
#=========================================================================

import pytest
import struct

from pymtl             import *
from pclib.test        import TestSource, TestSink, mk_test_case_table
from IteratorMsg       import IteratorMsg
from dstu.XcelMsg      import XcelMsg
from dstu.MemMsgFuture import MemMsg

from IteratorPipelineUnit import IteratorPipelineUnit

from dstu.TestMemoryOpaque  import TestMemory

#-------------------------------------------------------------------------
# TestHarness
#-------------------------------------------------------------------------
class TestHarness( Model ):

  def __init__( s, model,
                src_cfg_msgs,  sink_cfg_msgs,
                src_xcel_msgs, sink_xcel_msgs,
                src_delay,     sink_delay,
                stall_prob,    latency,
                dump_vcd=False ):

    # Interfaces
    cfg_ifc  = XcelMsg()
    xcel_ifc = IteratorMsg(32)
    mem_ifc  = MemMsg(8,32,32)

    # Instantiate Models
    s.cfg_src   = TestSource ( cfg_ifc.req,  src_cfg_msgs,  src_delay )
    s.xcel_src  = TestSource ( xcel_ifc.req, src_xcel_msgs, src_delay )
    s.pipe      = model()
    s.cfg_sink  = TestSink ( cfg_ifc.resp,  sink_cfg_msgs,  sink_delay )
    s.xcel_sink = TestSink ( xcel_ifc.resp, sink_xcel_msgs, sink_delay )
    s.mem       = TestMemory ( mem_ifc, 1, stall_prob, latency )


    # Connect
    s.connect( s.pipe.cfgreq,  s.cfg_src.out  )
    s.connect( s.pipe.cfgresp, s.cfg_sink.in_ )

    # NOTE: We pause the xcel messages till the itu is configured
    s.connect( s.pipe.xcelreq.msg, s.xcel_src.out.msg )

    @s.combinational
    def pause():
      s.pipe.xcelreq.val.value = s.cfg_src.done & s.cfg_sink.done & s.xcel_src.out.val
      s.xcel_src.out.rdy.value = s.cfg_src.done & s.cfg_sink.done & s.pipe.xcelreq.rdy

    s.connect( s.pipe.xcelresp, s.xcel_sink.in_ )

    s.connect( s.pipe.itu.memreq,  s.mem.reqs[0]  )
    s.connect( s.pipe.itu.memresp, s.mem.resps[0] )

  def done( s ):
    return s.cfg_src.done & s.cfg_sink.done & s.xcel_src.done & s.xcel_sink.done

  def line_trace( s ):
    return s.pipe.line_trace()

#-------------------------------------------------------------------------
# run_pipe_test
#-------------------------------------------------------------------------
def run_pipe_test( model, mem_array, dump_vcd = None ):

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

  while not model.done() and sim.ncycles < 1000:
    sim.print_line_trace()
    sim.cycle()
  sim.print_line_trace()
  assert model.done()

  # Add a couple extra ticks so that the VCD dump is nicer
  sim.cycle()
  sim.cycle()
  sim.cycle()

#-------------------------------------------------------------------------
# Utility functions for creating arrays formatted for memory loading.
#-------------------------------------------------------------------------
# mem_array_int
def mem_array_int( base_addr, data ):
  bytes = struct.pack( "<{}i".format( len(data) ), *data )
  return [base_addr, bytes]

#-------------------------------------------------------------------------
# Helper functions for Test xcel_src/xcel_sink messages
#-------------------------------------------------------------------------

xcel_req  = IteratorMsg( 32 ).req.mk_msg
xcel_resp = IteratorMsg( 32 ).resp.mk_msg

def xcel_req_rd( ds_id, iter, addr, data ):
  return xcel_req( 0, ds_id, 0, iter, addr, data )

def xcel_req_wr( ds_id, iter, addr, data ):
  return xcel_req( 0, ds_id, 1, iter, addr, data )

def xcel_req_inc( ds_id, iter, addr, data ):
  return xcel_req( 0, ds_id, 2, iter, addr, data )

def xcel_req_dec( ds_id, iter, addr, data ):
  return xcel_req( 0, ds_id, 3, iter, addr, data )

def xcel_resp_rd( ds_id, addr, data ):
  return xcel_resp( 0, ds_id, 0, addr, data )

def xcel_resp_wr( ds_id, addr, data ):
  return xcel_resp( 0, ds_id, 1, addr, data )

def xcel_resp_inc( ds_id, addr, data ):
  return xcel_resp( 0, ds_id, 2, addr, data )

def xcel_resp_dec( ds_id, addr, data ):
  return xcel_resp( 0, ds_id, 3, addr, data )

#-------------------------------------------------------------------------
# Helper functions for Test cfg_src/cfg_sink messages
#-------------------------------------------------------------------------

cfg_req  = XcelMsg().req.mk_msg
cfg_resp = XcelMsg().resp.mk_msg

def cfg_req_rd( opq, raddr, data, id ):
  return cfg_req( opq, 0, raddr, data, id )

def cfg_req_wr( opq, raddr, data, id ):
  return cfg_req( opq, 1, raddr, data, id )

def cfg_resp_rd( opq, data, id ):
  return cfg_resp( opq, 0, data, id )

def cfg_resp_wr( opq, data, id ):
  return cfg_resp( opq, 1, data, id )

#-------------------------------------------------------------------------
# Memory array and messages to test list of integers
#-------------------------------------------------------------------------

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
  xcel_req_rd ( 0, 0, 4,  0 ), xcel_resp_rd ( 0, 4,  1 ),
  xcel_req_rd ( 0, 0, 16, 0 ), xcel_resp_rd ( 0, 16, 2 ),
  xcel_req_rd ( 0, 0, 28, 0 ), xcel_resp_rd ( 0, 28, 3 ),
  xcel_req_rd ( 0, 0, 40, 0 ), xcel_resp_rd ( 0, 40, 4 ),

  xcel_req_inc( 0, 0, 4,  0 ), xcel_resp_inc( 0, 16, 0 ),
  xcel_req_inc( 0, 0, 16, 0 ), xcel_resp_inc( 0, 28, 0 ),
  xcel_req_inc( 0, 0, 28, 0 ), xcel_resp_inc( 0, 40, 0 ),
  xcel_req_inc( 0, 0, 40, 0 ), xcel_resp_inc( 0,  0, 0 ),

  xcel_req_dec( 0, 0, 4,  0 ), xcel_resp_dec( 0,  4, 0 ),
  xcel_req_dec( 0, 0, 16, 0 ), xcel_resp_dec( 0,  4, 0 ),
  xcel_req_dec( 0, 0, 28, 0 ), xcel_resp_dec( 0, 16, 0 ),
  xcel_req_dec( 0, 0, 40, 0 ), xcel_resp_dec( 0, 28, 0 ),

  xcel_req_wr ( 0, 0, 4,  5 ), xcel_resp_wr ( 0, 4,  0 ),
  xcel_req_wr ( 0, 0, 16, 6 ), xcel_resp_wr ( 0, 16, 0 ),
  xcel_req_wr ( 0, 0, 28, 7 ), xcel_resp_wr ( 0, 28, 0 ),
  xcel_req_wr ( 0, 0, 40, 8 ), xcel_resp_wr ( 0, 40, 0 ),

  xcel_req_rd ( 0, 0, 4,  0 ), xcel_resp_rd ( 0, 4,  5 ),
  xcel_req_rd ( 0, 0, 16, 0 ), xcel_resp_rd ( 0, 16, 6 ),
  xcel_req_rd ( 0, 0, 28, 0 ), xcel_resp_rd ( 0, 28, 7 ),
  xcel_req_rd ( 0, 0, 40, 0 ), xcel_resp_rd ( 0, 40, 8 ),
]

#-------------------------------------------------------------------------
# List config messages
#-------------------------------------------------------------------------

list_cfg_msgs = [
  cfg_req_wr( 0, 1, 1, 3     ), cfg_resp_wr( 0, 0x400, 3     ), # alloc
  cfg_req_wr( 0, 1, 4, 0x400 ), cfg_resp_wr( 0, 0,     0x400 ), # ds_init
  cfg_req_wr( 0, 2, 0, 0x400 ), cfg_resp_wr( 0, 0,     0x400 ), # dt_init
]

#-------------------------------------------------------------------------
# Test Case Table
#-------------------------------------------------------------------------

test_case_table = mk_test_case_table([
  (                       "xcel_msgs     cfg_msgs       src sink stall lat mem         " ),
  [ "list_int_0x0_0.0_0", list_int_msgs, list_cfg_msgs, 0,  0,   0.0,  0,  list_int_mem  ],
  [ "list_int_5x0_0.5_0", list_int_msgs, list_cfg_msgs, 5,  0,   0.5,  0,  list_int_mem  ],
  [ "list_int_0x5_0.0_4", list_int_msgs, list_cfg_msgs, 0,  5,   0.0,  4,  list_int_mem  ],
  [ "list_int_3x9_0.5_3", list_int_msgs, list_cfg_msgs, 3,  9,   0.5,  3,  list_int_mem  ],
])

#-------------------------------------------------------------------------
# Test cases
#-------------------------------------------------------------------------

@pytest.mark.parametrize( **test_case_table )
def test( test_params, dump_vcd ):
  IteratorPipelineUnit.vcd_file = dump_vcd
  run_pipe_test( TestHarness( IteratorPipelineUnit,
                              test_params.cfg_msgs[::2],
                              test_params.cfg_msgs[1::2],
                              test_params.xcel_msgs[::2],
                              test_params.xcel_msgs[1::2],
                              test_params.src,
                              test_params.sink,
                              test_params.stall,
                              test_params.lat ),
                 test_params.mem,
                 dump_vcd )
