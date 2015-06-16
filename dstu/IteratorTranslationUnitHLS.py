#=========================================================================
# IteratorTranslationUnitHLS
#=========================================================================
# Wrapper module for HLS generated hardware

import os

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle

from XcelMsg      import XcelMsg
from IteratorMsg  import IteratorMsg
from MemMsgFuture import MemMsg

class IteratorTranslationUnitHLS( VerilogModel ):

  def __init__( s ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.cfgreq   = InValRdyBundle  ( XcelMsg().req   )
    s.cfgresp  = OutValRdyBundle ( XcelMsg().resp  )

    s.xcelreq  = InValRdyBundle  ( IteratorMsg(32).req  )
    s.xcelresp = OutValRdyBundle ( IteratorMsg(32).resp )

    #s.debug    = OutValRdyBundle ( Bits(32)  )

    s.memreq   = OutValRdyBundle ( MemMsg(8,32,32).req   )
    s.memresp  = InValRdyBundle  ( MemMsg(8,32,32).resp  )

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
      'memresp_V_ap_ack'  : s.memresp.rdy,
      'cfgreq_V'          : s.cfgreq.msg,
      'cfgreq_V_ap_vld'   : s.cfgreq.val,
      'cfgreq_V_ap_ack'   : s.cfgreq.rdy,
      'cfgresp_V'         : s.cfgresp.msg,
      'cfgresp_V_ap_vld'  : s.cfgresp.val,
      'cfgresp_V_ap_ack'  : s.cfgresp.rdy
      #'debug_V_V'         : s.debug.msg,
      #'debug_V_V_ap_vld'  : s.debug.val,
      #'debug_V_V_ap_ack'  : s.debug.rdy
    })

