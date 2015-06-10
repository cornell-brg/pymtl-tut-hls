#=========================================================================
# GcdXcelHLS
#=========================================================================
# Wrapper module for HLS generated hardware

from pymtl        import *
from pclib.ifcs   import InValRdyBundle, OutValRdyBundle
from dstu.XcelMsg import XcelReqMsg, XcelRespMsg

class GcdXcelHLS( VerilogModel ):

  def __init__( s ):

    s.xcelreq  = InValRdyBundle ( XcelReqMsg()  )
    s.xcelresp = OutValRdyBundle( XcelRespMsg() )

    s.set_ports({
      'ap_clk'            : s.clk,
      'ap_rst'            : s.reset,
      'xcelIn_V'          : s.xcelreq.msg,
      'xcelIn_V_ap_vld'   : s.xcelreq.val,
      'xcelIn_V_ap_ack'   : s.xcelreq.rdy,
      'xcelOut_V'         : s.xcelresp.msg,
      'xcelOut_V_ap_vld'  : s.xcelresp.val,
      'xcelOut_V_ap_ack'  : s.xcelresp.rdy
    })
