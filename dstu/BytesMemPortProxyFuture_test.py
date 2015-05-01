#=========================================================================
# BytesMemPortProxyFuture_test.py
#=========================================================================

from __future__ import print_function

import pytest

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from pclib.test import mk_test_case_table

from pclib.fl.Bytes          import Bytes
from BytesMemPortProxyFuture import BytesMemPortProxy
from MemMsgFuture            import MemMsg
from TestMemoryOpaque        import TestMemory

#-------------------------------------------------------------------------
# Function Implementation
#-------------------------------------------------------------------------
# This is a plain function implementation which copies n bytes in memory.

def mem_copy( mem, src_ptr, dest_ptr, nbytes ):

  for i in xrange(nbytes):
    mem[dest_ptr+i] = mem[src_ptr+i]

#-------------------------------------------------------------------------
# Test for underlying mem_copy
#-------------------------------------------------------------------------

def test_mem_copy():

  data = [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 ]
  data_bytes = Bytes(18)
  for i in xrange(18):
    data_bytes[i] = data[i]

  mem_copy( data_bytes, 4, 12, 4 )

  data_ref = [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 4, 5, 6, 7, 16, 17 ]
  data_ref_bytes = Bytes(18)
  for i in xrange(18):
    data_ref_bytes[i] = data_ref[i]

  assert data_bytes == data_ref_bytes

#-------------------------------------------------------------------------
# MemCopy
#-------------------------------------------------------------------------
# An example model that simply copies n bytes from a source pointer to a
# destination pointer in a memory.

class MemCopy (Model):

  #-----------------------------------------------------------------------
  # Constructor
  #-----------------------------------------------------------------------

  def __init__( s, mem_ifc_types,
                src_ptr, dest_ptr, nbytes ):

    s.src_ptr  = src_ptr
    s.dest_ptr = dest_ptr
    s.nbytes   = nbytes
    s.done     = False

    # Memory request/response ports

    s.memreq  = InValRdyBundle  ( mem_ifc_types.req  )
    s.memresp = OutValRdyBundle ( mem_ifc_types.resp )

    # BytesMemPortProxy object

    s.mem = BytesMemPortProxy( mem_ifc_types, s.memreq, s.memresp )

  #-----------------------------------------------------------------------
  # elaborate_logic
  #-----------------------------------------------------------------------

  def elaborate_logic( s ):

    # This looks like a regular tick block, but because it is a
    # pausable_tick there is something more sophisticated is going on.
    # The first time we call the tick, the mem_copy function will try to
    # ready from memory. This will get proxied to the BytesMemPortProxy
    # which will create a memory request and send it out the memreq port.
    # Since we do not have the data yet, we cannot return the data to the
    # mem_copy function so instead we use greenlets to switch back to the
    # pausable tick function. The next time we call tick we jump back
    # into the BytesMemPortProxy object to see if the response has
    # returned. When the response eventually returns, the
    # BytesMemPortProxy object will return the data and the underlying
    # mem_copy function will move onto writing the memory.

    @s.tick_fl
    def logic():
      mem_copy( s.mem, s.src_ptr, s.dest_ptr, s.nbytes )
      s.done = True

  #-----------------------------------------------------------------------
  # done
  #-----------------------------------------------------------------------

  def done( s ):
    return s.done

  #-----------------------------------------------------------------------
  # line_trace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    return "(" + s.mem.line_trace() + ")"

#-------------------------------------------------------------------------
# TestHarness
#-------------------------------------------------------------------------
class TestHarness( Model ):

  def __init__( s, stall_prob, latency, src_ptr, dest_ptr, nbytes ):

    # Create parameters

    mem_ifc_types = MemMsg(8,32,32)

    # Instantiate models

    s.mcopy = MemCopy( mem_ifc_types,
                       src_ptr, dest_ptr, nbytes )

    s.mem = TestMemory( mem_ifc_types, 1, stall_prob, latency )

  def elaborate_logic( s ):

    s.connect( s.mcopy.memreq,  s.mem.reqs[0]  )
    s.connect( s.mcopy.memresp, s.mem.resps[0] )

  def done( s ):
    return s.mcopy.done

  def line_trace( s ):
    #return s.mcopy.line_trace() + " " + s.mem.line_trace()
    return s.mem.line_trace()

#-------------------------------------------------------------------------
# test
#-------------------------------------------------------------------------
test_case_table = mk_test_case_table([
  (                    "stall lat"),
  [ "stall_0.0_lat_0",  0.0,  0   ],
  [ "stall_0.0_lat_4",  0.0,  4   ],
  [ "stall_0.5_lat_0",  0.5,  0   ],
  [ "stall_0.5_lat_4",  0.5,  4   ],
])


@pytest.mark.parametrize( **test_case_table )
def test( test_params, dump_vcd ):

  stall   = test_params.stall
  latency = test_params.lat

  # Instantiate and elaborate the model

  model = TestHarness( stall, latency,  0x0004, 0x000c, 4 )
  model.vcd_file = dump_vcd
  model.elaborate()

  # Write test data into the test memory

  for i in xrange(18):
    model.mem.mem[ 0x0000 + i ] = i

  # Create a simulator using the simulation tool

  sim = SimulationTool( model )

  # Run the simulation

  print()

  sim.reset()
  while not model.done():
    sim.print_line_trace()
    sim.cycle()

  # Add a couple extra ticks so that the VCD dump is nicer

  sim.cycle()
  sim.cycle()
  sim.cycle()

  # Verify the output

  data_ref = [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 4, 5, 6, 7, 16, 17 ]
  for i in xrange(18):
    assert model.mem.mem[ 0x0000 + i ] == data_ref[i]

