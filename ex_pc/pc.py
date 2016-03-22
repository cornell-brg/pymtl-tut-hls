#=========================================================================
# pc
#=========================================================================
# Wrapper module for HLS generated hardware

import os

from pymtl        import *
from pclib.ifcs   import InValRdyBundle, OutValRdyBundle
from pclib.ifcs   import valrdy_to_str
from pclib.ifcs.XcelMsg   import *

#-------------------------------------------------------------------------
# pc
#-------------------------------------------------------------------------

class pc( VerilogModel ):

  def __init__( s ):

    s.xcelreq  = InValRdyBundle ( Bits(64) )
    s.xcelresp = OutValRdyBundle( Bits(32) )

    s.set_ports({
      'ap_clk'            : s.clk,
      'ap_rst'            : s.reset,
      'x_V'         : s.xcelreq.msg,
      'x_V_ap_vld'  : s.xcelreq.val,
      'x_V_ap_ack'  : s.xcelreq.rdy,
      'num'        : s.xcelresp.msg,
      'num_ap_vld' : s.xcelresp.val,
      'num_ap_ack' : s.xcelresp.rdy,
    })

class pc_wrapper( Model ):

  def __init__( s ):

    s.xcelreq  = InValRdyBundle ( Bits(64)  )
    s.xcelresp = OutValRdyBundle( Bits(32) )

    s.xcel = pc()

    s.connect( s.xcelreq,   s.xcel.xcelreq )
    s.connect( s.xcelresp,  s.xcel.xcelresp )

  def line_trace(s):
    return "{}|{}".format(
      valrdy_to_str( s.xcelreq.msg, s.xcelreq.val, s.xcelreq.rdy ),
      valrdy_to_str( s.xcelresp.msg, s.xcelresp.val, s.xcelresp.rdy )
    )

