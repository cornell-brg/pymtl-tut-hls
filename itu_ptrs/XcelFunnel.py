#=========================================================================
# XcelFunnel.py
#=========================================================================
# Funnel used by accelerators where the opaque field is not modified. I
# added a new model here as I was not sure if the CPU sets the opaque
# field and if so, it should not modified

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from pclib.rtl  import RoundRobinArbiter

#-------------------------------------------------------------------------
# XcelFunnel
#-------------------------------------------------------------------------

class XcelFunnel( Model ):

  def __init__( s, nports, MsgType ):

    #---------------------------------------------------------------------
    # Interface
    #---------------------------------------------------------------------

    s.in_ = InValRdyBundle  [ nports ]( MsgType )
    s.out = OutValRdyBundle           ( MsgType )

    #---------------------------------------------------------------------
    # Setup round robin arbiter
    #---------------------------------------------------------------------
    # Notice that we AND the output ready with each request signal, so
    # if the output port is not ready we do not make any requests to the
    # arbiter. This will prevent the arbiter priority from changing.

    s.vals    = Wire( nports )
    s.arbiter = RoundRobinArbiter( nports )

    @s.combinational
    def arbiter_logic():
      for i in xrange( nports ):
        s.vals[i].value         = s.in_[i].val
        s.arbiter.reqs[i].value = s.in_[i].val & s.out.rdy
        s.in_[i].rdy.value      = s.arbiter.grants[i]

    #---------------------------------------------------------------------
    # Assign outputs
    #---------------------------------------------------------------------

    @s.combinational
    def output_logic():
      s.out.val.value = reduce_or( s.vals )
      s.out.msg.value = 0
      for i in xrange( nports ):
        if s.arbiter.grants[i]:
          s.out.msg.value        = s.in_[i].msg

  #-----------------------------------------------------------------------
  # line_trace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    in_str = '{' + '|'.join(map(str,s.in_)) + '}'
    return "{} ({}) {}".format( in_str, s.arbiter.line_trace(), s.out )
