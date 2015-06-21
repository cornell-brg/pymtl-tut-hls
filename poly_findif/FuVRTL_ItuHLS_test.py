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

from FindIfUnitFL_test              import *
from FuFL_ItuHLS_test               import run_asu_test
from dstu.IteratorTranslationUnitHLS import IteratorTranslationUnitHLS as ITU
from FindIfUnitVRTL import FindIfUnitVRTL

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
    s.asu = FindIfUnit( )
    s.itu = TranslationUnit( )
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
    #s.connect( s.asu.memresp,  s.mem.resps[0] )
    s.connect( s.asu.memresp.msg,   s.mem.resps[0].msg )
    s.connect( s.asu.memresp.val,   s.mem.resps[0].val )
    s.connect( 1,   s.mem.resps[0].rdy )

    # asu <-> itu
    s.connect( s.asu.itureq,    s.itu.xcelreq )
    s.connect( s.asu.ituresp, s.itu.xcelresp  )

    # itu <-> mem
    s.connect( s.itu.memreq,   s.mem.reqs[1]  )
    s.connect( s.itu.memresp.msg,  s.mem.resps[1].msg )
    s.connect( s.itu.memresp.val,  s.mem.resps[1].val )
    s.connect( 1,  s.mem.resps[1].rdy )

    # asu -> itu convert Bits to BitStruct
    @s.combinational
    def convert():
      s.mem.reqs[0].msg.value = mem_ifc.req.unpck( s.asu.memreq.msg )

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

#-------------------------------------------------------------------------
# Test cases
#-------------------------------------------------------------------------

@pytest.mark.parametrize( **test_case_table )
def test( test_params, dump_vcd ):

  run_asu_test( TestHarness(  FindIfUnitVRTL,
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
