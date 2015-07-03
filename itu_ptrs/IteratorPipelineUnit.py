#=========================================================================
# IteratorPipelineUnit
#=========================================================================
# This model mocks up a send stage and a receive stage that interact with the
# iterator translation unit. The send stage processes an incoming transaction
# and "sends" either a configuration message request or a iterator message to
# the ITU. The receive stage receives the response message from the ITU and
# writes the result to a sink. The model assumes that the send stage is
# connected to two sources at the start of the pipeline and two sinks at the
# end of the pipeline

from pymtl        import *
from pclib.ifcs   import InValRdyBundle, OutValRdyBundle
from pclib.rtl    import SingleElementPipelinedQueue, RegEn
from IteratorMsg  import IteratorMsg
from dstu.XcelMsg import XcelMsg

from IteratorTranslationUnitHLS import IteratorTranslationUnitHLS_Wrapper

#-------------------------------------------------------------------------
# IteratorPipelineUnit
#-------------------------------------------------------------------------

class IteratorPipelineUnit( Model ):

  def __init__( s ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.cfgreq   = InValRdyBundle  ( XcelMsg().req   )
    s.cfgresp  = OutValRdyBundle ( XcelMsg().resp  )

    s.xcelreq  = InValRdyBundle  ( IteratorMsg(32).req  )
    s.xcelresp = OutValRdyBundle ( IteratorMsg(32).resp )

    #---------------------------------------------------------------------
    # ITU
    #---------------------------------------------------------------------

    s.itu = IteratorTranslationUnitHLS_Wrapper()

    #---------------------------------------------------------------------
    # Misc
    #---------------------------------------------------------------------

    s.SR_cfg_stall  = Wire( 1 )
    s.SR_xcel_stall = Wire( 1 )

    s.R_cfg_valid   = RegEn( 1 )
    s.R_cfg_msg     = RegEn( XcelMsg().req )

    s.R_xcel_valid  = RegEn( 1 )
    s.R_xcel_msg    = RegEn( IteratorMsg( 32 ).req )

    #---------------------------------------------------------------------
    # Send stage
    #---------------------------------------------------------------------
    # send the configuration message or the iterator message

    s.connect( s.cfgreq.msg,  s.itu.cfgreq.msg  )
    s.connect( s.xcelreq.msg, s.itu.xcelreq.msg )

    s.connect( s.cfgreq.msg,  s.R_cfg_msg.in_   )
    s.connect( s.xcelreq.msg, s.R_xcel_msg.in_  )

    @s.combinational
    def send_request():
      s.itu.cfgreq.val.value  = ~s.SR_cfg_stall   & s.cfgreq.val
      s.itu.xcelreq.val.value = ~s.SR_xcel_stall  & s.xcelreq.val
      s.cfgreq.rdy.value      = s.itu.cfgreq.rdy  & ~s.SR_cfg_stall
      s.xcelreq.rdy.value     = s.itu.xcelreq.rdy & ~s.SR_xcel_stall

    @s.combinational
    def next_stage_valid():
      s.R_cfg_valid.in_.value  = s.cfgreq.val  & s.cfgreq.rdy
      s.R_xcel_valid.in_.value = s.xcelreq.val & s.xcelreq.rdy

    #---------------------------------------------------------------------
    # Receive stage
    #---------------------------------------------------------------------
    # receive response messages

    s.R_cfg_en  = Wire( 1 )
    s.R_xcel_en = Wire( 1 )

    s.connect( s.itu.cfgresp.msg,   s.cfgresp.msg  )
    s.connect( s.itu.xcelresp.msg,  s.xcelresp.msg )

    s.connect( s.R_cfg_msg.en,    s.R_cfg_en     )
    s.connect( s.R_xcel_msg.en,   s.R_xcel_en    )

    s.connect( s.R_cfg_valid.en,  s.R_cfg_en     )
    s.connect( s.R_xcel_valid.en, s.R_xcel_en    )

    @s.combinational
    def receive_response():
      s.itu.cfgresp.rdy.value  = s.R_cfg_valid.out  & s.cfgresp.rdy
      s.itu.xcelresp.rdy.value = s.R_xcel_valid.out & s.xcelresp.rdy

      s.cfgresp.val.value  = s.R_cfg_valid.out  & s.itu.cfgresp.val
      s.xcelresp.val.value = s.R_xcel_valid.out & s.itu.xcelresp.val

    @s.combinational
    def enable_stage():
      s.R_cfg_en.value  = ~s.SR_cfg_stall
      s.R_xcel_en.value = ~s.SR_xcel_stall

    @s.combinational
    def SR_stall_signal():
      s.SR_cfg_stall.value  = s.R_cfg_valid.out  & ( ~s.itu.cfgresp.val  | ~s.cfgresp.rdy  )
      s.SR_xcel_stall.value = s.R_xcel_valid.out & ( ~s.itu.xcelresp.val | ~s.xcelresp.rdy )

  #-----------------------------------------------------------------------
  # linetrace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    return "{} {} | {} {} | {} {}".format(
      s.cfgreq,
      s.xcelreq,
      s.cfgresp,
      s.xcelresp,
      s.itu.memreq,
      s.itu.memresp
    )
