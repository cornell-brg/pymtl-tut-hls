#=========================================================================
# IteratorTranslationUnitHLSAlt
#=========================================================================
# Wrapper module for HLS generated hardware

import os

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from pclib.ifcs import valrdy_to_str

from pclib.rtl  import SingleElementPipelinedQueue
from pclib.rtl  import SingleElementBypassQueue
from pclib.rtl  import RegisterFile

from FunnelRouter import FunnelRouter
from IteratorMsg  import IteratorMsg

from dstu.XcelMsg      import XcelMsg
from dstu.MemMsgFuture import MemMsg

from PolyDsuListRTL    import PolyDsuListRTL

#-------------------------------------------------------------------------
# PolyDsuConfigHLS
#-------------------------------------------------------------------------

class PolyDsuConfigHLS( VerilogModel ):

  def __init__( s ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.cfgreq     = InValRdyBundle  ( XcelMsg().req   )
    s.cfgresp    = OutValRdyBundle ( XcelMsg().resp  )

    s.memreq     = OutValRdyBundle ( MemMsg(8,32,32).req   )
    s.memresp    = InValRdyBundle  ( MemMsg(8,32,32).resp  )

    s.dstype_ce   = OutPort( 1 )
    s.dstype_we   = OutPort( 1 )
    s.dstype_data = OutPort( 4 )
    s.dstype_addr = OutPort( 5 )

    s.dsdesc_ce   = OutPort( 1 )
    s.dsdesc_we   = OutPort( 1 )
    s.dsdesc_data = OutPort( 32 )
    s.dsdesc_addr = OutPort( 5 )

    s.dtdesc_ce   = OutPort( 1 )
    s.dtdesc_we   = OutPort( 1 )
    s.dtdesc_data = OutPort( 32 )
    s.dtdesc_addr = OutPort( 5 )

    s.set_ports({
      'ap_clk'              : s.clk,
      'ap_rst'              : s.reset,
      'cfgreq_V'            : s.cfgreq.msg,
      'cfgreq_V_ap_vld'     : s.cfgreq.val,
      'cfgreq_V_ap_ack'     : s.cfgreq.rdy,
      'cfgresp_V'           : s.cfgresp.msg,
      'cfgresp_V_ap_vld'    : s.cfgresp.val,
      'cfgresp_V_ap_ack'    : s.cfgresp.rdy,
      'memreq_V'            : s.memreq.msg,
      'memreq_V_ap_vld'     : s.memreq.val,
      'memreq_V_ap_ack'     : s.memreq.rdy,
      'memresp_V'           : s.memresp.msg,
      'memresp_V_ap_vld'    : s.memresp.val,
      'memresp_V_ap_ack'    : s.memresp.rdy,
      'dstype_V_address0'   : s.dstype_addr,
      'dstype_V_ce0'        : s.dstype_ce,
      'dstype_V_we0'        : s.dstype_we,
      'dstype_V_d0'         : s.dstype_data,
      'dsdesc_V_address0'   : s.dsdesc_addr,
      'dsdesc_V_ce0'        : s.dsdesc_ce,
      'dsdesc_V_we0'        : s.dsdesc_we,
      'dsdesc_V_d0'         : s.dsdesc_data,
      'dtdesc_V_address0'   : s.dtdesc_addr,
      'dtdesc_V_ce0'        : s.dtdesc_ce,
      'dtdesc_V_we0'        : s.dtdesc_we,
      'dtdesc_V_d0'         : s.dtdesc_data,
  })

#-------------------------------------------------------------------------
# PolyDsuListHLS
#-------------------------------------------------------------------------

class PolyDsuListHLS( VerilogModel ):

  def __init__( s ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.xcelreq    = InValRdyBundle  ( IteratorMsg(32).req  )
    s.xcelresp   = OutValRdyBundle ( IteratorMsg(32).resp )

    s.memreq     = OutValRdyBundle ( MemMsg(8,32,32).req   )
    s.memresp    = InValRdyBundle  ( MemMsg(8,32,32).resp  )

    s.dtdesc     = InPort ( 32 )

    s.set_ports({
      'ap_clk'              : s.clk,
      'ap_rst'              : s.reset,
      'xcelreq_V'           : s.xcelreq.msg,
      'xcelreq_V_ap_vld'    : s.xcelreq.val,
      'xcelreq_V_ap_ack'    : s.xcelreq.rdy,
      'xcelresp_V'          : s.xcelresp.msg,
      'xcelresp_V_ap_vld'   : s.xcelresp.val,
      'xcelresp_V_ap_ack'   : s.xcelresp.rdy,
      'memreq_V'            : s.memreq.msg,
      'memreq_V_ap_vld'     : s.memreq.val,
      'memreq_V_ap_ack'     : s.memreq.rdy,
      'memresp_V'           : s.memresp.msg,
      'memresp_V_ap_vld'    : s.memresp.val,
      'memresp_V_ap_ack'    : s.memresp.rdy,
      'dtdesc_V'            : s.dtdesc
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

    s.hls_dispatch = PolyDsuConfigHLS()

    s.hls_list     = PolyDsuListHLS()

    s.pipe_q       = SingleElementPipelinedQueue( IteratorMsg(32).req )
    s.byp_q        = SingleElementBypassQueue( IteratorMsg(32).resp )

    s.fr           = FunnelRouter( 2, MemMsg(8,32,32).req, MemMsg(8,32,32).resp )

    s.dstype_rf    = RegisterFile( dtype=4  )
    s.dsdesc_rf    = RegisterFile( dtype=32 )
    s.dtdesc_rf    = RegisterFile( dtype=32 )

    #---------------------------------------------------------------------
    # Connections
    #---------------------------------------------------------------------

    s.connect( s.cfgreq,        s.hls_dispatch.cfgreq  )
    s.connect( s.cfgresp_q.enq, s.hls_dispatch.cfgresp )
    s.connect( s.cfgresp,       s.cfgresp_q.deq        )

    #s.connect( s.xcelreq,       s.hls_list.xcelreq     )
    #s.connect( s.xcelresp,      s.hls_list.xcelresp    )
    s.connect( s.xcelreq,       s.pipe_q.enq )
    s.connect( s.hls_list.xcelreq, s.pipe_q.deq )
    s.connect( s.hls_list.xcelresp, s.byp_q.enq        )
    s.connect( s.xcelresp, s.byp_q.deq )

    s.connect( s.fr.funnel_in[0], s.hls_dispatch.memreq )
    s.connect( s.fr.funnel_in[1], s.hls_list.memreq     )
    s.connect( s.memreq,          s.fr.funnel_out       )

    s.connect( s.memresp,          s.fr.router_in         )
    s.connect( s.fr.router_out[0], s.hls_dispatch.memresp )
    s.connect( s.fr.router_out[1], s.hls_list.memresp     )

    s.connect( s.dstype_rf.wr_en,   s.hls_dispatch.dstype_we   )
    s.connect( s.dstype_rf.wr_addr, s.hls_dispatch.dstype_addr )
    s.connect( s.dstype_rf.wr_data, s.hls_dispatch.dstype_data )

    s.connect( s.dsdesc_rf.wr_en,   s.hls_dispatch.dsdesc_we   )
    s.connect( s.dsdesc_rf.wr_addr, s.hls_dispatch.dsdesc_addr )
    s.connect( s.dsdesc_rf.wr_data, s.hls_dispatch.dsdesc_data )

    s.connect( s.dtdesc_rf.wr_en,   s.hls_dispatch.dtdesc_we   )
    s.connect( s.dtdesc_rf.wr_addr, s.hls_dispatch.dtdesc_addr )
    s.connect( s.dtdesc_rf.wr_data, s.hls_dispatch.dtdesc_data )

    s.ds_id = Wire( 5 )
    s.combinational
    def comb():
      s.ds_id.value = s.hls_list.xcelreq.msg.ds_id & 0x1f

    s.connect( s.dtdesc_rf.rd_addr[0], s.ds_id   )
    s.connect( s.dtdesc_rf.rd_data[0], s.hls_list.dtdesc )

  def line_trace( s ):
    return "{} {}|{} {}|{} {}".format(
      s.cfgreq,
      s.cfgresp,
      s.xcelreq,
      s.xcelresp,
      s.memreq,
      s.memresp )
