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
from dstu.TypeDescriptor import TypeDescriptor
from dstu.IteratorMsg               import IteratorMsg
from dstu.XcelMsg                   import XcelMsg
from dstu.UserTypes                 import Point

from FindIfUnitFL_test             import *
from dstu.IteratorTranslationUnitHLS import IteratorTranslationUnitHLS as ITU
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
    s.itu = TranslationUnit( )
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
    s.connect( s.itu.memresp.msg,  s.mem.resps[1].msg )
    s.connect( s.itu.memresp.val,  s.mem.resps[1].val )
    s.connect( 1,  s.mem.resps[1].rdy )

    @s.combinational
    def logic():
      s.asu.cfgreq.val.value = s.src.out.val & s.go
      s.src.out.rdy.value    = s.asu.cfgreq.rdy & s.go

  def done( s ):
    return s.src.done and s.sink.done

  def line_trace( s ):
    return  s.asu.line_trace() + " > " + \
            s.mem.line_trace() + " > " + \
            s.sink.line_trace() + " > "

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
  model.itu.cfgreq.msg.id.next    = 3       # DSTU_ID
  model.itu.cfgresp.rdy.next      = 1

  while model.itu.cfgreq.rdy.value == 0:
    sim.cycle()

  print "Sent ALLOC"

  model.itu.cfgreq.val.next       = 0

  while model.itu.cfgresp.val.value == 0:
    sim.cycle()

  # ds-id allocated by the itu
  alloc_ds_id = model.itu.cfgresp.msg.data

  print "Received ALLOC response:", alloc_ds_id

  # Init data structure - ds_desc
  model.itu.cfgreq.val.next       = 1
  model.itu.cfgreq.msg.data.next  = mem_array[0]+mem_array[1] # base addr
  model.itu.cfgreq.msg.raddr.next = 1              # init
  model.itu.cfgreq.msg.id.next    = alloc_ds_id    # id of the ds

  while model.itu.cfgreq.rdy.value == 0:
    sim.cycle()

  print "Sent DS Descriptor"

  model.itu.cfgreq.val.next       = 0

  while model.itu.cfgresp.val.value == 0:
    sim.cycle()

  print "Received DS Descriptor response", model.itu.cfgresp.msg

  # Init data structure - dt_desc
  model.itu.cfgreq.val.next       = 1
  model.itu.cfgreq.msg.data.next  = mem_array[0] # dt_desc_ptr
  model.itu.cfgreq.msg.raddr.next = 2            # init
  model.itu.cfgreq.msg.id.next    = alloc_ds_id  # id of the ds

  while model.itu.cfgreq.rdy.value == 0:
    sim.cycle()

  print "Sent DT Descriptor"

  model.itu.cfgreq.val.next       = 0

  # End itu configuration
  while model.itu.cfgresp.val.value == 0:
    sim.cycle()

  print "Received DT Descriptor response", model.itu.cfgresp.msg

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
