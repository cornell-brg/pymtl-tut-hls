#==============================================================================
# FindIfUnitFL_test
#==============================================================================

import pytest

from pymtl      import *
from pclib.test import TestSource, TestSink, TestMemory, mk_test_case_table
from pclib.ifcs import mem_msgs, MemMsg

from itu.IteratorTranslationUnitFL import IteratorTranslationUnitFL as ITU
from itu.IteratorMsg               import IteratorMsg
from itu.CoprocessorMsg            import CoprocessorMsg

from FindIfUnitFL import FindIfUnitFL

#------------------------------------------------------------------------------
# TestHarness
#------------------------------------------------------------------------------
class TestHarness( Model ):

  def __init__( s, FindIfUnit, TranslationUnit,
                src_msgs,  sink_msgs,
                src_delay, sink_delay,
                mem_delay ):

    # Local Parameters
    s.memreq_params  = mem_msgs.MemReqParams ( 32, 32 )
    s.memresp_params = mem_msgs.MemRespParams(     32 )

    # Interfaces
    asu_cfg_ifc      = CoprocessorMsg(  5, 32, 32 )
    asu_itu_ifc      = IteratorMsg   (         32 )
    itu_cfg_ifc      = CoprocessorMsg(  5, 32, 32 )
    itu_mem_ifc      = MemMsg        (     32, 32 )

    # Go bit
    s.go             = Wire( 1 )

    # Instantiate Models
    s.src            = TestSource     ( asu_cfg_ifc.req.nbits, src_msgs, src_delay      )
    s.asu            = FindIfUnit     ( asu_cfg_ifc, asu_itu_ifc                        )
    s.itu            = TranslationUnit( itu_cfg_ifc, asu_itu_ifc, itu_mem_ifc           )
    s.sink           = TestSink       ( asu_cfg_ifc.resp.nbits, sink_msgs, sink_delay   )
    s.mem            = TestMemory     ( s.memreq_params, s.memresp_params, 1, mem_delay )

    # Connect

    # src -> asu request bundle
    s.connect( s.src.out.msg,            s.asu.cfg_ifc.req_msg   )

    # src -> asu request bundle
    s.connect( s.asu.cfg_ifc.resp_msg,   s.sink.in_.msg          )
    s.connect( s.asu.cfg_ifc.resp_val,   s.sink.in_.val          )
    s.connect( s.asu.cfg_ifc.resp_rdy,   s.sink.in_.rdy          )

    # asu -> itu request bundle
    s.connect( s.asu.itu_ifc.req_msg,    s.itu.accel_ifc.req_msg )
    s.connect( s.asu.itu_ifc.req_val,    s.itu.accel_ifc.req_val )
    s.connect( s.asu.itu_ifc.req_rdy,    s.itu.accel_ifc.req_rdy )

    # itu -> asu response bundle
    s.connect( s.itu.accel_ifc.resp_msg, s.asu.itu_ifc.resp_msg  )
    s.connect( s.itu.accel_ifc.resp_val, s.asu.itu_ifc.resp_val  )
    s.connect( s.itu.accel_ifc.resp_rdy, s.asu.itu_ifc.resp_rdy )

    # itu -> mem request bundle
    s.connect( s.itu.mem_ifc.req_msg,    s.mem.reqs[0].msg       )
    s.connect( s.itu.mem_ifc.req_val,    s.mem.reqs[0].val       )
    s.connect( s.itu.mem_ifc.req_rdy,    s.mem.reqs[0].rdy       )

    # mem -> response bundle
    s.connect( s.mem.resps[0].msg,       s.itu.mem_ifc.resp_msg  )
    s.connect( s.mem.resps[0].val,       s.itu.mem_ifc.resp_val  )
    s.connect( s.mem.resps[0].rdy,       s.itu.mem_ifc.resp_rdy  )

    @s.combinational
    def logic():
      s.asu.cfg_ifc.req_val.value = s.src.out.val         & s.go
      s.src.out.rdy.value         = s.asu.cfg_ifc.req_rdy & s.go

  def done( s ):
    return s.src.done and s.sink.done

  def line_trace( s ):
    return  s.src.line_trace() + " > " + \
            s.asu.line_trace() + " > " + \
            s.itu.line_trace() + " > " + \
            s.sink.line_trace()

#------------------------------------------------------------------------------
# run_asu_test
#------------------------------------------------------------------------------
def run_asu_test( model, vec_base_addr, mem_array=None, dump_vcd = None ):

  # Elaborate
  model.vcd_file = dump_vcd
  model.elaborate()

  # Create a simulator
  sim = SimulationTool( model )

  # Load the memory
  if mem_array:
    model.mem.load_memory( mem_array )

  # Run simulation
  sim.reset()
  print

  sim.cycle()
  sim.cycle()

  # Start the ITU configuration
  model.itu.cfg_ifc.req_val.next      = 1
  model.itu.cfg_ifc.req_msg.data.next = vec_base_addr
  model.itu.cfg_ifc.req_msg.addr.next = 2
  model.itu.cfg_ifc.req_msg.id.next   = 1

  sim.cycle()
  sim.cycle()

  # End the ITU configuration
  model.itu.cfg_ifc.req_val.next      = 0

  sim.cycle()
  sim.cycle()

  # Allow source to inject messages
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
# mem_array_32bit
#------------------------------------------------------------------------------
# Utility function for creating arrays formatted for memory loading.
from itertools import chain
def mem_array_32bit( base_addr, data ):
  return [base_addr,
          list( chain.from_iterable([ [x,0,0,0] for x in data ] ))
         ]

# preload the memory to known values
preload_mem_array = mem_array_32bit( 8, [1,1,3,3,5,5,6] )

#------------------------------------------------------------------------------
# Test src/sink messages
#------------------------------------------------------------------------------

req  = CoprocessorMsg( 5, 32, 32).req.mk_req
resp = CoprocessorMsg( 5, 32, 32).resp.mk_resp

# configure the asu state and expect a response for a given predicate
basic_msgs = [
              req  ( 1, 1, 1, 0 ), resp( 1, 0 ),# first ds-id
              req  ( 1, 2, 0, 0 ), resp( 1, 0 ),# first index
              req  ( 1, 3, 1, 0 ), resp( 1, 0 ),# last ds-id
              req  ( 1, 4, 7, 0 ), resp( 1, 0 ),# last index
              req  ( 1, 5, 4, 0 ), resp( 1, 0 ),# predicate val = IsEven
              req  ( 1, 0, 0, 0 ), resp( 1, 0 ),# go
              req  ( 0, 0, 0, 0 ), resp( 0, 6 ) # check done
             ]

#-------------------------------------------------------------------------
# Test Case Table
#-------------------------------------------------------------------------

test_case_table = mk_test_case_table([
  (               "msgs       src_delay sink_delay vec_base"),
  [ "basic_0x0",  basic_msgs, 0,        0,         8 ],
  [ "basic_5x0",  basic_msgs, 5,        0,         8 ],
  [ "basic_0x5",  basic_msgs, 0,        5,         8 ],
  [ "basic_3x9",  basic_msgs, 3,        9,         8 ],
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
                              test_params.src_delay,
                              test_params.sink_delay, 0 ),
                test_params.vec_base,
                preload_mem_array,
                dump_vcd )
