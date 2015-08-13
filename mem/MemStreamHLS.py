#=========================================================================
# MemStreamHLS
#=========================================================================
# Wrapper module for HLS generated hardware

import os

from pymtl        import *
from pclib.ifcs   import InValRdyBundle, OutValRdyBundle
from pclib.rtl    import SingleElementBypassQueue
from pclib.rtl    import SingleElementPipelinedQueue

from xcel.XcelMsg     import XcelReqMsg, XcelRespMsg

from MemMsgFuture import MemReqMsg, MemRespMsg

#-------------------------------------------------------------------------
# MemStreamHLS
#-------------------------------------------------------------------------

class MemStreamHLS( VerilogModel ):

  def __init__( s ):

    s.xcelreq  = InValRdyBundle ( XcelReqMsg()  )
    s.xcelresp = OutValRdyBundle( XcelRespMsg() )

    s.memreq   = OutValRdyBundle( MemReqMsg (8,32,32) )
    s.memresp  = InValRdyBundle ( MemRespMsg(8,32)    )

    s.set_ports({
      'ap_clk'            : s.clk,
      'ap_rst'            : s.reset,
      'xcelreq_V'         : s.xcelreq.msg,
      'xcelreq_V_ap_vld'  : s.xcelreq.val,
      'xcelreq_V_ap_ack'  : s.xcelreq.rdy,
      'xcelresp_V'        : s.xcelresp.msg,
      'xcelresp_V_ap_vld' : s.xcelresp.val,
      'xcelresp_V_ap_ack' : s.xcelresp.rdy,
      'memreq_V'          : s.memreq.msg,
      'memreq_V_ap_vld'   : s.memreq.val,
      'memreq_V_ap_ack'   : s.memreq.rdy,
      'memresp_V'         : s.memresp.msg,
      'memresp_V_ap_vld'  : s.memresp.val,
      'memresp_V_ap_ack'  : s.memresp.rdy
    })

