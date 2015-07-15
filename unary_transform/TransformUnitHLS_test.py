#=============================================================================
# TransformUnitHLS_test.py
#=============================================================================
# XXX: Check accel-id returned in responses

import pytest
import struct

from pymtl      import *
from pclib.test import TestSource, TestSink, mk_test_case_table

from dstu.MemMsgFuture              import MemMsg
from dstu.TestMemoryOpaque          import TestMemory
from dstu.IteratorTranslationUnitFL import IteratorTranslationUnitFL as ITU
from dstu.IteratorMsg               import IteratorMsg
from dstu.XcelMsg                   import XcelMsg

from TransformUnitHLS               import TransformUnitHLS_Wrapper as ASU
from Square                         import Square_Wrapper as PE

#------------------------------------------------------------------------------
# TestHarness
#------------------------------------------------------------------------------
class TestHarness( Model ):

  def __init__( s, TransformUnit, TranslationUnit, ProcessingElement,
                src_msgs,  sink_msgs,
                src_delay, sink_delay,
                stall_prob, latency,
                dump_vcd=False ):

    # Interfaces
    asu_cfg_ifc = XcelMsg()
    asu_itu_ifc = IteratorMsg (32)
    itu_cfg_ifc = XcelMsg()
    mem_ifc     = MemMsg (8,32,32)

    # Go bit
    s.go = Wire( 1 )

    # Instantiate Models
    s.src = TestSource( asu_cfg_ifc.req, src_msgs, src_delay )
    s.asu = TransformUnit( )
    s.itu = TranslationUnit( itu_cfg_ifc, asu_itu_ifc, mem_ifc )
    s.pe  = ProcessingElement( )
    s.sink = TestSink( asu_cfg_ifc.resp, sink_msgs, sink_delay )
    s.mem = TestMemory( mem_ifc, 2, stall_prob, latency )

    # Connect

    # src <-> asu
    s.connect( s.src.out.msg, s.asu.cfgreq.msg )
    s.connect( s.asu.cfgresp, s.sink.in_ )

    # asu <-> mem
    #s.connect( s.asu.memreq,   s.mem.reqs[0] )
    s.connect( s.asu.memreq.val,   s.mem.reqs[0].val )
    s.connect( s.asu.memreq.rdy,   s.mem.reqs[0].rdy )
    s.connect( s.asu.memresp,   s.mem.resps[0] )

    # asu <-> itu
    #s.connect( s.asu.itureq,   s.itu.xcelreq ) 
    s.connect( s.asu.ituresp,   s.itu.xcelresp  )
    s.connect( s.asu.itureq.val, s.itu.xcelreq.val)
    s.connect( s.asu.itureq.rdy, s.itu.xcelreq.rdy)

    # asu <-> pe
    s.connect( s.asu.pereq,  s.pe.xcelreq )
    s.connect( s.asu.peresp, s.pe.xcelresp )

    # itu <-> mem
    s.connect( s.itu.memreq,   s.mem.reqs[1]  )
    s.connect( s.itu.memresp,  s.mem.resps[1] )

    # asu -> itu convert Bits to BitStruct
    @s.combinational
    def convert():
      s.itu.xcelreq.msg.value = asu_itu_ifc.req.unpck( s.asu.itureq.msg )
      s.mem.reqs[0].msg.value = mem_ifc.req.unpck( s.asu.memreq.msg )

    # testbench -> asu request bundle val/rdy signals
    @s.combinational
    def logic():
      s.asu.cfgreq.val.value = s.src.out.val & s.go
      s.src.out.rdy.value    = s.asu.cfgreq.rdy & s.go

  def done( s ):
    return s.src.done and s.sink.done

  def line_trace( s ):
            #s.itu.line_trace() + " > " + \
    return  s.src.line_trace() + " >asu " + \
            s.asu.line_trace() + " >mem " + \
            s.mem.line_trace() + " >sink " + \
            s.sink.line_trace()

#------------------------------------------------------------------------------
# run_asu_test
#  input_data  - input memory array, list of bytes
#  outputt_data - master output memory array, list of bytes
#------------------------------------------------------------------------------
def run_asu_test( model, input_data, output_data, ds_type, dump_vcd = None ):

  # Elaborate
  model.vcd_file = dump_vcd
  model.elaborate()

  # Create a simulator
  sim = SimulationTool( model )

  # Create the memory array
  base_addr = 0
  bytes = struct.pack( "<{}B".format( len(input_data) ), *input_data )
  model.mem.write_mem( base_addr, bytes )

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
  model.itu.cfgreq.msg.id.next    = model.itu.DSTU_ID
  model.itu.cfgresp.rdy.next      = 1

  sim.cycle()

  # ds-id allocated by the itu
  alloc_ds_id = model.itu.cfgresp.msg.data

  sim.cycle()
  sim.cycle()

  # Init data structure
  model.itu.cfgreq.msg.data.next  = base_addr + 4  # base addr for data array
  model.itu.cfgreq.msg.raddr.next = 1              # init
  model.itu.cfgreq.msg.id.next    = alloc_ds_id    # id of the ds

  sim.cycle()
  sim.cycle()

  # Init data structure - dt_desc
  model.itu.cfgreq.msg.data.next  = base_addr    # dt_desc_ptr
  model.itu.cfgreq.msg.raddr.next = 2            # init
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

  while not model.done() and sim.ncycles < 1000:
    sim.print_line_trace()
    sim.cycle()
  sim.print_line_trace()
  assert model.done()

  # Add a couple extra ticks so that the VCD dump is nicer
  sim.cycle()
  sim.cycle()
  sim.cycle()

  # Check the results
  # Read the memory
  result_bytes = model.mem.read_mem( base_addr, len(input_data) )
  result = struct.unpack("<{}B".format( len(input_data )), result_bytes)

  print "input: ", input_data
  print "result: ", result
  print "expect: ", output_data

#------------------------------------------------------------------------------
# Helper functions for Test src/sink messages
#------------------------------------------------------------------------------

req  = XcelMsg().req.mk_msg
resp = XcelMsg().resp.mk_msg

#-------------------------------------------------------------------------
# Test Vectors
#-------------------------------------------------------------------------

vec_rgb_mem1 = [0,12,4,0, 0,0,0,0,   255,0,0,0,   0,255,0,0,   0,0,255,0 ]
vec_rgb_res1 = [0,12,4,0, 0,0,0,255, 0,255,255,0, 255,0,255,0, 255,255,0,0]

vector_rgb_msgs = [
                req( 0, 1, 1, 0, 0 ), resp( 0, 1, 0, 0 ), # first ds-id
                req( 0, 1, 2, 0, 0 ), resp( 0, 1, 0, 0 ), # first iter
                req( 0, 1, 3, 0, 0 ), resp( 0, 1, 0, 0 ), # last ds-id
                req( 0, 1, 4, 4, 0 ), resp( 0, 1, 0, 0 ), # last iter
                req( 0, 1, 5, 0, 0 ), resp( 0, 1, 0, 0 ), # first2 ds-id
                req( 0, 1, 6, 0, 0 ), resp( 0, 1, 0, 0 ), # first2 iter
                req( 0, 1, 7, 1, 0 ), resp( 0, 1, 0, 0 ), # predicate val
                req( 0, 1, 8, 0, 0 ), resp( 0, 1, 0, 0 ), # dt_desc_ptr
                req( 0, 1, 0, 0, 0 ), resp( 0, 1, 0, 0 ), # go
                req( 0, 0, 0, 0, 0 ), resp( 0, 0, 4, 0 ), # check done
               ]

#-------------------------------------------------------------------------
# Test Case Table
#-------------------------------------------------------------------------
test_case_table = mk_test_case_table([
  (                      "msgs             src sink stall lat  mem           res           ds" ),
  [ "vec_rgb_0x0_0.0_0",  vector_rgb_msgs, 0,  0,   0.0,  0,   vec_rgb_mem1, vec_rgb_res1, ITU.VECTOR ],
  [ "vec_rgb_5x0_0.5_0",  vector_rgb_msgs, 5,  0,   0.5,  0,   vec_rgb_mem1, vec_rgb_res1, ITU.VECTOR ],
  [ "vec_rgb_0x5_0.0_4",  vector_rgb_msgs, 0,  5,   0.0,  4,   vec_rgb_mem1, vec_rgb_res1, ITU.VECTOR ],
  [ "vec_rgb_3x9_0.5_3",  vector_rgb_msgs, 3,  9,   0.5,  3,   vec_rgb_mem1, vec_rgb_res1, ITU.VECTOR ],
])

@pytest.mark.parametrize( **test_case_table )
def test( test_params, dump_vcd ):

  run_asu_test( TestHarness(  ASU,
                              ITU,
                              PE,
                              test_params.msgs[::2],
                              test_params.msgs[1::2],
                              test_params.src,
                              test_params.sink,
                              test_params.stall,
                              test_params.lat ),
                test_params.mem,
                test_params.res,
                test_params.ds,
                dump_vcd )
