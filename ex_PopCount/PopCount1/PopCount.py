#=========================================================================
# PopCount
#=========================================================================
# Wrapper module for HLS generated hardware

import os

from pymtl        import *
from pclib.ifcs   import InValRdyBundle, OutValRdyBundle
from pclib.ifcs   import valrdy_to_str
from pclib.ifcs.XcelMsg   import *

#-------------------------------------------------------------------------
# PopCount
#-------------------------------------------------------------------------

class PopCount( VerilogModel ):

  def __init__( s ):

    s.x_V = InPort ( 64 )
    s.num = OutPort( 32 )
    s.num_ap_vld = OutPort(1)

    s.set_ports({
      'ap_clk'            : s.clk,
      'ap_rst'            : s.reset,
      'x_V'               : s.x_V,
      'num'               : s.num,
      'num_ap_vld'        : s.num_ap_vld
    })

  def line_trace(s):
    return "{}|{}|{}".format(
      s.x_V,
      s.num,
      s.num_ap_vld
    )

