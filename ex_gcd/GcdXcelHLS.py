#=========================================================================
# GcdXcelHLS
#=========================================================================
# Wrapper module for HLS generated hardware

from pymtl        import *
from pclib.ifcs   import InValRdyBundle, OutValRdyBundle
from xcel.XcelMsg import XcelReqMsg, XcelRespMsg

class GcdXcelHLS( VerilogModel ):

  def __init__( s ):

    s.xcelreq  = InValRdyBundle ( XcelReqMsg()  )
    s.xcelresp = OutValRdyBundle( XcelRespMsg() )

    s.set_ports({
      'ap_clk'             : s.clk,
      'ap_rst'             : s.reset,
      'xcelreq_V'          : s.xcelreq.msg,
      'xcelreq_V_ap_vld'   : s.xcelreq.val,
      'xcelreq_V_ap_ack'   : s.xcelreq.rdy,
      'xcelresp_V'         : s.xcelresp.msg,
      'xcelresp_V_ap_vld'  : s.xcelresp.val,
      'xcelresp_V_ap_ack'  : s.xcelresp.rdy
    })
