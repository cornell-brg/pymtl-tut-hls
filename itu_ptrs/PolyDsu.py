#=========================================================================
# PolyDsu
#=========================================================================
# Top-level model that composes a PolyDsu that handles a number of
# different data-structure types.

from pymtl      import *

from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from pclib.ifcs import valrdy_to_str

from pclib.rtl  import SingleElementPipelinedQueue
from pclib.rtl  import SingleElementBypassQueue
from pclib.rtl  import SingleElementNormalQueue
from pclib.rtl  import RegisterFile

from dstu.XcelMsg      import XcelMsg
from dstu.MemMsgFuture import MemMsg

from XcelFunnel        import XcelFunnel
from FunnelRouter      import FunnelRouter
from IteratorMsg       import IteratorMsg

from PolyDsuConfigHLS  import PolyDsuConfigHLS
from PolyDsuRouter     import PolyDsuRouter
from PolyDsuListRTL    import PolyDsuListRTL
from PolyDsuVectorRTL  import PolyDsuVectorRTL

#-------------------------------------------------------------------------
# PolyDsu
#-------------------------------------------------------------------------

class PolyDsu( Model ):

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

    # config logic (HLS generated)
    s.config_hls   = PolyDsuConfigHLS()

    # vector data-structure
    s.vector_rtl      = PolyDsuVectorRTL()

    # list data-structure
    s.list_rtl     = PolyDsuListRTL()

    # polydsu router
    s.dsu_router   = PolyDsuRouter( 2, IteratorMsg(32).req )

    # funnel for polydsu responses
    s.dsu_funnel   = XcelFunnel( 2, IteratorMsg(32).resp )

    # funnel Router for the memory ports
    s.mem_fr       = FunnelRouter( 3, MemMsg(8,32,32).req, MemMsg(8,32,32).resp )

    # state
    s.dstype_rf    = RegisterFile( dtype=4  )
    s.dsdesc_rf    = RegisterFile( dtype=32 )
    s.dtdesc_rf    = RegisterFile( dtype=32 )

    # NOTE: We need a queue to cut the combinational path from the cfgreq
    # port (valid signal) to the cfgresp port (ready signal). The queue is
    # added to the output channel and not the input channel to optimize the
    # state transitions in the generated HLS hardware. XXX: We do not need
    # a queue on the output channel for xcelresp as no such combination
    # path between the xcelreq and the xcelresp ports exists.
    s.cfgresp_q    = SingleElementPipelinedQueue ( XcelMsg().resp )

    # NOTE: We need a bypass queue here to cut combinational paths that
    # could cause deadlocks. This is because an xcelreq port could be
    # connected to the output port of a funnel model. Since, the xcelreq
    # port then feeds the polydsu-Router model and the incoming message
    # from the message determines the polydsu-router ready, we can cause a
    # combinational loop as the Funnel model uses the ready signals to
    # determine the output for the funnel model but the ready signals
    # depend on the message that comes from the funnel model.
    s.byp_q        = SingleElementBypassQueue ( IteratorMsg(32).req )

    #---------------------------------------------------------------------
    # Connections
    #---------------------------------------------------------------------

    s.ds_id = Wire ( 5 )

    @s.combinational
    def comb():
      s.ds_id.value = s.byp_q.deq.msg.ds_id & 0x1f

    # config_hls <-> dstype_rf connections
    s.connect( s.dstype_rf.wr_en,      s.config_hls.dstype_we   )
    s.connect( s.dstype_rf.wr_addr,    s.config_hls.dstype_addr )
    s.connect( s.dstype_rf.wr_data,    s.config_hls.dstype_data )
    s.connect( s.dstype_rf.rd_addr[0], s.ds_id                  )
    s.connect( s.dstype_rf.rd_data[0], s.dsu_router.id          )

    # config_hls <-> dsdesc_rf connections
    s.connect( s.dsdesc_rf.wr_en,   s.config_hls.dsdesc_we   )
    s.connect( s.dsdesc_rf.wr_addr, s.config_hls.dsdesc_addr )
    s.connect( s.dsdesc_rf.wr_data, s.config_hls.dsdesc_data )

    # config_hls <-> dtdesc_rf connections
    s.connect( s.dtdesc_rf.wr_en,      s.config_hls.dtdesc_we   )
    s.connect( s.dtdesc_rf.wr_addr,    s.config_hls.dtdesc_addr )
    s.connect( s.dtdesc_rf.wr_data,    s.config_hls.dtdesc_data )
    s.connect( s.dtdesc_rf.rd_addr[0], s.ds_id                  )
    s.connect( s.dtdesc_rf.rd_data[0], s.dsu_router.dtdesc_in   )

    # config request/response ports
    s.connect( s.cfgreq,        s.config_hls.cfgreq  )
    s.connect( s.cfgresp_q.enq, s.config_hls.cfgresp )
    s.connect( s.cfgresp,       s.cfgresp_q.deq      )

    # dsu router connections
    s.connect( s.xcelreq,                  s.byp_q.enq          )
    s.connect( s.byp_q.deq,                s.dsu_router.in_     )
    s.connect( s.dsu_router.out[0],        s.vector_rtl.xcelreq )
    s.connect( s.dsu_router.dtdesc_out[0], s.vector_rtl.dtdesc  )
    s.connect( s.dsu_router.out[1],        s.list_rtl.xcelreq   )
    s.connect( s.dsu_router.dtdesc_out[1], s.list_rtl.dtdesc    )

    # dsu funnel connections
    s.connect( s.xcelresp,            s.dsu_funnel.out    )
    s.connect( s.vector_rtl.xcelresp, s.dsu_funnel.in_[0] )
    s.connect( s.list_rtl.xcelresp,   s.dsu_funnel.in_[1] )

    # mem funnel connections
    s.connect( s.mem_fr.funnel_in[0], s.config_hls.memreq )
    s.connect( s.mem_fr.funnel_in[1], s.vector_rtl.memreq )
    s.connect( s.mem_fr.funnel_in[2], s.list_rtl.memreq   )
    s.connect( s.memreq,              s.mem_fr.funnel_out )

    # mem router connections
    s.connect( s.memresp,              s.mem_fr.router_in   )
    s.connect( s.mem_fr.router_out[0], s.config_hls.memresp )
    s.connect( s.mem_fr.router_out[1], s.vector_rtl.memresp )
    s.connect( s.mem_fr.router_out[2], s.list_rtl.memresp   )

  #-----------------------------------------------------------------------
  # line_trace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    return "{} {}|{} {}|{} {}".format(
      s.cfgreq,
      s.cfgresp,
      s.xcelreq,
      s.xcelresp,
      s.memreq,
      s.memresp
  )
