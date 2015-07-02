#=========================================================================
# IteratorTranslationUnitHLS
#=========================================================================
# Wrapper module for HLS generated hardware

import os

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from pclib.ifcs import valrdy_to_str

from pclib.rtl  import SingleElementPipelinedQueue

from IteratorMsg  import IteratorMsg

from dstu.XcelMsg      import XcelMsg
from dstu.MemMsgFuture import MemMsg

#-------------------------------------------------------------------------
# IteratorTranslationUnitHLS
#-------------------------------------------------------------------------

class IteratorTranslationUnitHLS( VerilogModel ):

  def __init__( s ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.cfgreq   = InValRdyBundle  ( XcelMsg().req   )
    s.cfgresp  = OutValRdyBundle ( XcelMsg().resp  )

    s.xcelreq  = InValRdyBundle  ( IteratorMsg(32).req  )
    s.xcelresp = OutValRdyBundle ( IteratorMsg(32).resp )

    s.memreq   = OutValRdyBundle ( MemMsg(8,32,32).req   )
    s.memresp  = InValRdyBundle  ( MemMsg(8,32,32).resp  )

    #s.debug    = OutValRdyBundle ( Bits(32)  )

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

#-------------------------------------------------------------------------
# IteratorTranslationUnitHLS_Wrapper
#-------------------------------------------------------------------------

class IteratorTranslationUnitHLS_Wrapper( Model ):

  def __init__( s ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.cfgreq   = InValRdyBundle  ( XcelMsg().req   )
    s.cfgresp  = OutValRdyBundle ( XcelMsg().resp  )

    s.xcelreq  = InValRdyBundle  ( IteratorMsg(32).req  )
    s.xcelresp = OutValRdyBundle ( IteratorMsg(32).resp )

    s.memreq   = OutValRdyBundle ( MemMsg(8,32,32).req   )
    s.memresp  = InValRdyBundle  ( MemMsg(8,32,32).resp  )

    s.cfgresp_q  = SingleElementPipelinedQueue ( XcelMsg().resp       )
    s.xcelresp_q = SingleElementPipelinedQueue ( IteratorMsg(32).resp )

    s.hls = IteratorTranslationUnitHLS()

    s.connect( s.cfgreq,        s.hls.cfgreq     )
    s.connect( s.cfgresp_q.enq, s.hls.cfgresp    )
    s.connect( s.cfgresp,       s.cfgresp_q.deq  )

    s.connect( s.xcelreq,       s.hls.xcelreq    )
    s.connect( s.xcelresp_q.enq,s.hls.xcelresp   )
    s.connect( s.xcelresp,      s.xcelresp_q.deq )

    s.connect( s.memreq,        s.hls.memreq     )
    s.connect( s.memresp,       s.hls.memresp    )

  def line_trace( s ):
    return "{} {}|{} {}|{} {}".format(
      s.cfgreq,
      s.cfgresp,
      s.xcelreq,
      s.xcelresp,
      s.memreq,
      s.memresp )
