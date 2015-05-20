#=============================================================================
# FindIfUnitFL_test.py
#=============================================================================
# XXX: Configure dt_desc_ptr in the asu
# XXX: Check accel-id returned in responses
# XXX: Tests for user-defined types

import pytest
import struct

from pymtl      import *
from pclib.test import TestSource, TestSink, mk_test_case_table

from dstu.MemMsgFuture              import MemMsg
from dstu.TestMemoryOpaque          import TestMemory
from dstu.IteratorTranslationUnitFL import IteratorTranslationUnitFL as ITU
from dstu.IteratorTranslationUnitFL import TypeDescriptor
from dstu.IteratorMsg               import IteratorMsg
from dstu.XcelMsg                   import XcelMsg
from dstu.UserTypes                 import Point

from FindIfUnitFL import FindIfUnitFL


#------------------------------------------------------------------------------
# TestHarness
#------------------------------------------------------------------------------
class TestHarness( Model ):

  def __init__( s, FindIfUnit, TranslationUnit,
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
    s.asu = FindIfUnit( asu_cfg_ifc, asu_itu_ifc , mem_ifc )
    s.itu = TranslationUnit( itu_cfg_ifc, asu_itu_ifc, mem_ifc )
    s.sink = TestSink( asu_cfg_ifc.resp, sink_msgs, sink_delay )
    s.mem = TestMemory( mem_ifc, 2, stall_prob, latency )

    # Connect

    # src <-> asu
    s.connect( s.src.out.msg, s.asu.cfgreq.msg )
    s.connect( s.asu.cfgresp, s.sink.in_ )

    # asu <-> mem
    s.connect( s.asu.memreq,   s.mem.reqs[0]  )
    s.connect( s.asu.memresp,  s.mem.resps[0] )

    # asu <-> itu
    s.connect( s.asu.itureq,    s.itu.xcelreq )
    s.connect( s.asu.ituresp, s.itu.xcelresp  )

    # itu <-> mem
    s.connect( s.itu.memreq,   s.mem.reqs[1]  )
    s.connect( s.itu.memresp,  s.mem.resps[1] )

    @s.combinational
    def logic():
      s.asu.cfgreq.val.value = s.src.out.val & s.go
      s.src.out.rdy.value    = s.asu.cfgreq.rdy & s.go

  def done( s ):
    return s.src.done and s.sink.done

  def line_trace( s ):
    return  s.asu.line_trace() + " > " + \
            s.itu.line_trace()

#------------------------------------------------------------------------------
# run_asu_test
#------------------------------------------------------------------------------
def run_asu_test( model, mem_array, ds_type, dump_vcd = None ):

  # Elaborate
  model.vcd_file = dump_vcd
  model.elaborate()

  # Create a simulator
  sim = SimulationTool( model )

  # Load the memory
  if mem_array:
    model.mem.write_mem( mem_array[0], mem_array[2] )

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
  model.itu.cfgreq.msg.data.next  = mem_array[0]+mem_array[1] # base addr
  model.itu.cfgreq.msg.raddr.next = 1              # init
  model.itu.cfgreq.msg.id.next    = alloc_ds_id    # id of the ds

  sim.cycle()
  sim.cycle()

  # Init data structure - dt_desc
  model.itu.cfgreq.msg.data.next  = mem_array[0] # dt_desc_ptr
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

  while not model.done() and sim.ncycles < 500:
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
  return [base_addr, 4, bytes]

# mem_array_uchar
def mem_array_uchar( base_addr, data ):
  bytes = struct.pack( "<{}B".format( len(data) ), *data )
  return [base_addr, 4, bytes]

# mem_array_point
# data is a list of Point
def mem_array_point( base_addr, metadata, data):
  mbytes = struct.pack( "<{}i".format( len(metadata) ), *metadata)
  dbytes = ''.join( [d.pack() for d in data] )
  return [base_addr, 4*len(metadata), mbytes+dbytes]

#------------------------------------------------------------------------------
# Helper functions for Test src/sink messages
#------------------------------------------------------------------------------

req  = XcelMsg().req.mk_msg
resp = XcelMsg().resp.mk_msg

#------------------------------------------------------------------------------
# Memory array and messages to test vector of integers
#------------------------------------------------------------------------------

# preload the memory to known values
vec_int_mem = mem_array_int( 8, [0x00040500,1,-1,3,-3,5,-7,0] )

# configure the asu state and expect a response for a given predicate
vector_int_msgs = [
                   req( 0, 1, 1, 0, 0 ), resp( 0, 1, 0, 0 ), # first ds-id
                   req( 0, 1, 2, 0, 0 ), resp( 0, 1, 0, 0 ), # first iter
                   req( 0, 1, 3, 0, 0 ), resp( 0, 1, 0, 0 ), # last ds-id
                   req( 0, 1, 4, 7, 0 ), resp( 0, 1, 0, 0 ), # last iter
                   req( 0, 1, 5, 2, 0 ), resp( 0, 1, 0, 0 ), # predicate val = EqZero
                   req( 0, 1, 6, 8, 0 ), resp( 0, 1, 0, 0 ), # dt_desc_ptr
                   req( 0, 1, 0, 0, 0 ), resp( 0, 1, 0, 0 ), # go
                   req( 0, 0, 0, 0, 0 ), resp( 0, 0, 6, 0 )  # check done
                  ]

#------------------------------------------------------------------------------
# Memory array and messages to test vector of unsigned chars
#------------------------------------------------------------------------------
# preload the memory to known values
# NOTE: The first four bytes describe the dt_descriptor written out in
# little-endian format
# offset, size, type, fields
vec_uchar_mem = mem_array_uchar( 8, [0,1,1,0 ,1,1,3,7,5,9,0] )

# configure the asu state and expect a response for a given predicate
vector_uchar_msgs = [
                      req( 0, 1, 1, 0, 0 ), resp( 0, 1, 0, 0 ), # first ds-id
                      req( 0, 1, 2, 0, 0 ), resp( 0, 1, 0, 0 ), # first iter
                      req( 0, 1, 3, 0, 0 ), resp( 0, 1, 0, 0 ), # last ds-id
                      req( 0, 1, 4, 7, 0 ), resp( 0, 1, 0, 0 ), # last iter
                      req( 0, 1, 5, 2, 0 ), resp( 0, 1, 0, 0 ), # predicate val = EqZero
                      req( 0, 1, 6, 8, 0 ), resp( 0, 1, 0, 0 ), # dt_desc_ptr
                      req( 0, 1, 0, 0, 0 ), resp( 0, 1, 0, 0 ), # go
                      req( 0, 0, 0, 0, 0 ), resp( 0, 0, 6, 0 )  # check done
                    ]

#------------------------------------------------------------------------------
# Memory array and messages to test list of integers
#------------------------------------------------------------------------------

# preload the memory to known values
list_int_mem = mem_array_int( 0,
                                [   # metadata
                                    0x00040500,
                                    # value prev next
                                    1,      4, 16,
                                    1,      4, 28,
                                    3,     16, 40,
                                    3,     28, 52,
                                    5,     40, 64,
                                    5,     52, 76,
                                    0,     64, 0,
                                ]
                             )

# configure the asu state and expect a response for a given predicate
list_int_msgs = [
                 req( 0, 1, 1, 0, 0 ), resp( 0, 1, 0, 0 ), # first ds-id
                 req( 0, 1, 2, 0, 0 ), resp( 0, 1, 0, 0 ), # first iter
                 req( 0, 1, 3, 0, 0 ), resp( 0, 1, 0, 0 ), # last ds-id
                 req( 0, 1, 4, 7, 0 ), resp( 0, 1, 0, 0 ), # last iter
                 req( 0, 1, 5, 2, 0 ), resp( 0, 1, 0, 0 ), # predicate val = EqZero
                 req( 0, 1, 6, 0, 0 ), resp( 0, 1, 0, 0 ), # dt_desc_ptr
                 req( 0, 1, 0, 0, 0 ), resp( 0, 1, 0, 0 ), # go
                 req( 0, 0, 0, 0, 0 ), resp( 0, 0, 6, 0 )  # check done
                ]

#------------------------------------------------------------------------------
# Memory array and messages to test array of Points
#------------------------------------------------------------------------------
# offset, size, type, fields

vec_point_mem = mem_array_point( 8,
                                    [ # metadata
                                      0x000C0B03,
                                      0x00040300,
                                      0x04040500,
                                      0x08040500,
                                    ],
                                    [ # values
                                      Point( 0, 2, 3 ),
                                      Point( 1, 4, 5 ),
                                      Point( 2, 9, 1 ),
                                      Point( 3, 2, 7 ),
                                      Point( 4, 0, 1 ),
                                      Point( 5, 1, 0 ),
                                      Point( 6, 0, 0 ),
                                    ]
                                  )

# configure the asu state and expect a response for a given predicate
vector_point_msgs = [
                      req( 0, 1, 1, 0, 0 ), resp( 0, 1, 0, 0 ), # first ds-id
                      req( 0, 1, 2, 0, 0 ), resp( 0, 1, 0, 0 ), # first iter
                      req( 0, 1, 3, 0, 0 ), resp( 0, 1, 0, 0 ), # last ds-id
                      req( 0, 1, 4, 7, 0 ), resp( 0, 1, 0, 0 ), # last iter
                      req( 0, 1, 5, 2, 0 ), resp( 0, 1, 0, 0 ), # predicate val = EqZero
                      req( 0, 1, 6, 8, 0 ), resp( 0, 1, 0, 0 ), # dt_desc_ptr
                      req( 0, 1, 0, 0, 0 ), resp( 0, 1, 0, 0 ), # go
                      req( 0, 0, 0, 0, 0 ), resp( 0, 0, 6, 0 )  # check done
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
  [ "vec_point_0x0_0.0_0",  vector_point_msgs, 0,  0,   0.0,  0,   vec_point_mem, ITU.VECTOR ],
  [ "vec_point_5x0_0.5_0",  vector_point_msgs, 5,  0,   0.5,  0,   vec_point_mem, ITU.VECTOR ],
  [ "vec_point_0x5_0.0_4",  vector_point_msgs, 0,  5,   0.0,  4,   vec_point_mem, ITU.VECTOR ],
  [ "vec_point_3x9_0.5_3",  vector_point_msgs, 3,  9,   0.5,  3,   vec_point_mem, ITU.VECTOR ],
])

#-------------------------------------------------------------------------
# Test cases
#-------------------------------------------------------------------------

@pytest.mark.parametrize( **test_case_table )
def test( test_params, dump_vcd ):

  run_asu_test( TestHarness(  FindIfUnitFL,
                              ITU,
                              test_params.msgs[::2],
                              test_params.msgs[1::2],
                              test_params.src,
                              test_params.sink,
                              test_params.stall,
                              test_params.lat ),
                test_params.mem,
                test_params.ds,
                dump_vcd )
