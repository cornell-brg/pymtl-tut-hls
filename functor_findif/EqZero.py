#====================================================================
# EqZero.py
#====================================================================
# PyMtl wrapper for HLS-synthesized Verilog module of EqZero PE
#

from pymtl       import *
from pclib.ifcs  import InValRdyBundle, OutValRdyBundle
from pclib.ifcs  import valrdy_to_str
from pclib.rtl   import SingleElementPipelinedQueue
from pclib.rtl   import SingleElementBypassQueue

#-------------------------------------------------------------------
# EqZero
#-------------------------------------------------------------------
class EqZero ( VerilogModel ):
  def __init__( s ):
    s.xcelreq     = InValRdyBundle  ( 36 )
    s.xcelresp    = OutValRdyBundle ( 36 )

    s.set_ports ({
      'ap_clk'   :   s.clk,
      'ap_rst'   :   s.reset,
      'pe_req_V'         :   s.xcelreq.msg,
      'pe_req_V_ap_vld'  :   s.xcelreq.val,
      'pe_req_V_ap_ack'  :   s.xcelreq.rdy,
      'pe_resp_V'        :   s.xcelresp.msg,
      'pe_resp_V_ap_vld' :   s.xcelresp.val,
      'pe_resp_V_ap_ack' :   s.xcelresp.rdy,
    });

#-------------------------------------------------------------------
# EqZero_Wrapper
#-------------------------------------------------------------------
class EqZero_Wrapper( Model ):
  def __init__( s ):
    s.xcelreq     = InValRdyBundle  ( 36 )
    s.xcelresp    = OutValRdyBundle ( 36 )

    s.xcelresp_q  = SingleElementPipelinedQueue ( 36 )

    s.hls = EqZero()

    s.connect( s.xcelreq,        s.hls.xcelreq    )
    s.connect( s.xcelresp_q.enq, s.hls.xcelresp   )
    s.connect( s.xcelresp,       s.xcelresp_q.deq )

  def line_trace( s ):
    return "{}{}".format(
      s.xcelreq.
      s.xcelresp,
    )



