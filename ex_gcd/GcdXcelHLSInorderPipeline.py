#=========================================================================
# GcdXcelHLSInorderPipeline
#=========================================================================
# Author : Shreesha Srinath
# Date   : March 17th, 2016
#
# This model mocks up a send stage and a receive stage as in an inorder
# processor pipeline. The send stage processes an incoming transaction and
# "sends" a request to the Xcel under test. The receive stage receives the
# response message from the Xcel and writes the result to a sink.

from pymtl               import *
from pclib.ifcs          import InValRdyBundle, OutValRdyBundle
from pclib.ifcs          import valrdy_to_str
from pclib.rtl           import RegEn, SingleElementPipelinedQueue

from xcel.XcelMsg        import XcelReqMsg, XcelRespMsg
from misc.PipeCtrlFuture import PipeCtrlFuture

from GcdXcelHLS          import GcdXcelHLS

#-------------------------------------------------------------------------
# GcdXcelHLSInorderPipeline
#-------------------------------------------------------------------------

class GcdXcelHLSInorderPipeline( Model ):

  def __init__( s ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.xcelreq  = InValRdyBundle ( XcelReqMsg()  )
    s.xcelresp = OutValRdyBundle( XcelRespMsg() )

    #---------------------------------------------------------------------
    # Gcd XcelHLS
    #---------------------------------------------------------------------

    s.gcd_xcel  = GcdXcelHLS()

    s.connect( s.gcd_xcel.xcelreq.msg, s.xcelreq.msg )

    #---------------------------------------------------------------------
    # Send stage
    #---------------------------------------------------------------------
    # send the configuration message or the iterator message

    s.do_send = Wire( 1 )

    @s.combinational
    def send_request():

      s.gcd_xcel.xcelreq.val.value = ~s.pipe_stg.prev_stall & s.xcelreq.val

      s.xcelreq.rdy.value = s.gcd_xcel.xcelreq.rdy & ~s.pipe_stg.prev_stall

      s.do_send.value = s.xcelreq.val & s.xcelreq.rdy

    #---------------------------------------------------------------------
    # Receive stage
    #---------------------------------------------------------------------
    # receive response messages

    s.pipe_stg = PipeCtrlFuture()

    s.connect( s.pipe_stg.next_squash, 0 )
    s.connect( s.pipe_stg.next_stall,  0 )
    s.connect( s.pipe_stg.curr_squash, 0 )

    s.connect( s.pipe_stg.prev_val, s.do_send )

    s.xcelresp_q = SingleElementPipelinedQueue( XcelRespMsg() )

    s.connect( s.gcd_xcel.xcelresp,  s.xcelresp_q.enq )
    s.connect( s.xcelresp_q.deq.msg, s.xcelresp.msg   )

    @s.combinational
    def receive_response():

      s.xcelresp_q.deq.rdy.value = s.pipe_stg.curr_val & s.xcelresp.rdy

      s.xcelresp.val.value = s.pipe_stg.curr_val & s.xcelresp_q.deq.val

      s.pipe_stg.curr_stall.value = \
        s.pipe_stg.curr_val & ( ~s.xcelresp.rdy | ~s.xcelresp_q.deq.val )

  #-----------------------------------------------------------------------
  # linetrace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    return "{} | {}".format(
      valrdy_to_str(
                    s.gcd_xcel.xcelreq.msg,
                    s.gcd_xcel.xcelreq.val,
                    s.gcd_xcel.xcelreq.rdy
                   ),
      valrdy_to_str(
                    s.gcd_xcel.xcelresp.msg,
                    s.gcd_xcel.xcelresp.val,
                    s.gcd_xcel.xcelresp.rdy
                   )
    )
