#=========================================================================
# IteratorTranslationUnitHLSAlt
#=========================================================================
# Wrapper module for HLS generated hardware

import os

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from pclib.ifcs import valrdy_to_str

from pclib.rtl  import SingleElementPipelinedQueue

from FunnelRouter import FunnelRouter
from IteratorMsg  import IteratorMsg

from dstu.XcelMsg      import XcelMsg
from dstu.MemMsgFuture import MemMsg

#-------------------------------------------------------------------------
# PolyDsuDispatch
#-------------------------------------------------------------------------

class PolyDsuDispatch( VerilogModel ):

  def __init__( s ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.cfgreq     = InValRdyBundle  ( XcelMsg().req   )
    s.cfgresp    = OutValRdyBundle ( XcelMsg().resp  )

    s.xcelreq    = InValRdyBundle  ( IteratorMsg(32).req  )
    s.polydsureq = OutValRdyBundle ( 145 )

    s.memreq     = OutValRdyBundle ( MemMsg(8,32,32).req   )
    s.memresp    = InValRdyBundle  ( MemMsg(8,32,32).resp  )

    s.set_ports({
      'ap_clk'              : s.clk,
      'ap_rst'              : s.reset,
      'cfgreq_V'            : s.cfgreq.msg,
      'cfgreq_V_ap_vld'     : s.cfgreq.val,
      'cfgreq_V_ap_ack'     : s.cfgreq.rdy,
      'cfgresp_V'           : s.cfgresp.msg,
      'cfgresp_V_ap_vld'    : s.cfgresp.val,
      'cfgresp_V_ap_ack'    : s.cfgresp.rdy,
      'xcelreq_V'           : s.xcelreq.msg,
      'xcelreq_V_ap_vld'    : s.xcelreq.val,
      'xcelreq_V_ap_ack'    : s.xcelreq.rdy,
      'polydsureq_V'        : s.polydsureq.msg,
      'polydsureq_V_ap_vld' : s.polydsureq.val,
      'polydsureq_V_ap_ack' : s.polydsureq.rdy,
      'memreq_V'            : s.memreq.msg,
      'memreq_V_ap_vld'     : s.memreq.val,
      'memreq_V_ap_ack'     : s.memreq.rdy,
      'memresp_V'           : s.memresp.msg,
      'memresp_V_ap_vld'    : s.memresp.val,
      'memresp_V_ap_ack'    : s.memresp.rdy
  })

#-------------------------------------------------------------------------
# PolyDsuList
#-------------------------------------------------------------------------

class PolyDsuList( VerilogModel ):

  def __init__( s ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.polydsureq = InValRdyBundle ( 145 )
    s.xcelresp   = OutValRdyBundle ( IteratorMsg(32).resp )

    s.memreq     = OutValRdyBundle ( MemMsg(8,32,32).req   )
    s.memresp    = InValRdyBundle  ( MemMsg(8,32,32).resp  )

    s.set_ports({
      'ap_clk'              : s.clk,
      'ap_rst'              : s.reset,
      'polydsureq_V'        : s.polydsureq.msg,
      'polydsureq_V_ap_vld' : s.polydsureq.val,
      'polydsureq_V_ap_ack' : s.polydsureq.rdy,
      'xcelresp_V'          : s.xcelresp.msg,
      'xcelresp_V_ap_vld'   : s.xcelresp.val,
      'xcelresp_V_ap_ack'   : s.xcelresp.rdy,
      'memreq_V'            : s.memreq.msg,
      'memreq_V_ap_vld'     : s.memreq.val,
      'memreq_V_ap_ack'     : s.memreq.rdy,
      'memresp_V'           : s.memresp.msg,
      'memresp_V_ap_vld'    : s.memresp.val,
      'memresp_V_ap_ack'    : s.memresp.rdy
  })

#-------------------------------------------------------------------------
# IteratorTranslationUnitHLSAlt
#-------------------------------------------------------------------------

class IteratorTranslationUnitHLSAlt( Model ):

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

    #---------------------------------------------------------------------
    # Model instantiations
    #---------------------------------------------------------------------

    # NOTE: We need a queue to cut the combinational path from the cfgreq
    # port (valid signal) to the cfgresp port (ready signal). The queue is
    # added to the output channel and not the input channel to optimize the
    # state transitions in the generated HLS hardware. XXX: We do not need
    # a queue on the output channel for xcelresp as no such combination
    # path between the xcelreq and the xcelresp ports exists.
    s.cfgresp_q    = SingleElementPipelinedQueue ( XcelMsg().resp       )

    s.hls_dispatch = PolyDsuDispatch()

    s.hls_list     = PolyDsuList()

    s.fr           = FunnelRouter( 2, MemMsg(8,32,32).req, MemMsg(8,32,32).resp )

    #---------------------------------------------------------------------
    # Connections
    #---------------------------------------------------------------------

    s.connect( s.cfgreq,        s.hls_dispatch.cfgreq  )
    s.connect( s.cfgresp_q.enq, s.hls_dispatch.cfgresp )
    s.connect( s.cfgresp,       s.cfgresp_q.deq        )

    s.connect( s.xcelreq,       s.hls_dispatch.xcelreq )
    s.connect( s.xcelresp,      s.hls_list.xcelresp    )

    s.connect( s.hls_dispatch.polydsureq, s.hls_list.polydsureq )

    s.connect( s.fr.funnel_in[0], s.hls_dispatch.memreq )
    s.connect( s.fr.funnel_in[1], s.hls_list.memreq     )
    s.connect( s.memreq,          s.fr.funnel_out       )

    s.connect( s.memresp,          s.fr.router_in         )
    s.connect( s.fr.router_out[0], s.hls_dispatch.memresp )
    s.connect( s.fr.router_out[1], s.hls_list.memresp     )

  def line_trace( s ):
    return "{} {}|{} {}|{} {}".format(
      s.cfgreq,
      s.cfgresp,
      s.xcelreq,
      s.xcelresp,
      s.memreq,
      s.memresp )
