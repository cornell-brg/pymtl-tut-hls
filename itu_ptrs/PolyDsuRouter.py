#=========================================================================
# PolyDsuRouter.py
#=========================================================================

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle

#-------------------------------------------------------------------------
# PolyDsuRouter
#-------------------------------------------------------------------------

class PolyDsuRouter( Model ):

  def __init__( s, nports, MsgType ):

    #---------------------------------------------------------------------
    # Interface
    #---------------------------------------------------------------------

    s.id         = InPort                    (       4 )
    s.dtdesc_in  = InPort                    (      32 )
    s.in_        = InValRdyBundle            ( MsgType )
    s.out        = OutValRdyBundle [ nports ]( MsgType )
    s.dtdesc_out = OutPort         [ nports ](      32 )

    #---------------------------------------------------------------------
    # Assign outputs
    #---------------------------------------------------------------------

    @s.combinational
    def output_logic():
      s.in_.rdy.value = 0

      for i in xrange( nports ):

        s.dtdesc_out[i].value = s.dtdesc_in

        s.out[i].val.value = 0
        s.out[i].msg.value = 0

        if s.in_.val and s.id == i:
          s.out[i].val.value = s.in_.val
          s.out[i].msg.value = s.in_.msg
        if s.id == i:
          s.in_.rdy.value = s.out[i].rdy

  #-----------------------------------------------------------------------
  # line_trace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    out_str = '{' + '|'.join(map(str,s.out)) + '}'
    return "{} () {}".format( s.in_ , out_str )
