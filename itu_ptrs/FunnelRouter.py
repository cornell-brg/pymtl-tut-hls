#=========================================================================
# FunnelRouter.py
#=========================================================================
# The FunnelRouter model combines a val-rdy funnel and a val-rdy router
# model. The model is parameterized based on the number of the ports, the
# message type handled by the funnel, and the message type handled by the
# router

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from Funnel     import Funnel
from Router     import Router

#-------------------------------------------------------------------------
# FunnelRouter
#-------------------------------------------------------------------------

class FunnelRouter( Model ):

  def __init__( s, nports, FunnelMsgType, RouterMsgType ):

    #---------------------------------------------------------------------
    # Interface
    #---------------------------------------------------------------------

    s.funnel_in  = InValRdyBundle  [ nports ]( FunnelMsgType )
    s.funnel_out = OutValRdyBundle           ( FunnelMsgType )

    s.router_in  = InValRdyBundle            ( RouterMsgType )
    s.router_out = OutValRdyBundle [ nports ]( RouterMsgType )

    #---------------------------------------------------------------------
    # Funnel
    #---------------------------------------------------------------------

    s.funnel = Funnel( nports, FunnelMsgType )
    for i in xrange( nports ):
      s.connect( s.funnel.in_[i], s.funnel_in[i]  )
    s.connect( s.funnel.out, s.funnel_out )

    #---------------------------------------------------------------------
    # Router
    #---------------------------------------------------------------------

    s.router = Router( nports, RouterMsgType )
    s.connect( s.router.in_, s.router_in  )
    for i in xrange( nports ):
      s.connect( s.router.out[i], s.router_out[i]  )

  #-----------------------------------------------------------------------
  # line_trace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    return "{} {}".format( s.funnel.line_trace(), s.router.line_trace() )
