#====================================================================
# FindIfUnitHLS.py
#====================================================================
# PyMtl wrapper for HLS-synthesized Verilog module of find-if unit
#

from pymtl       import *
from pclib.ifcs  import InValRdyBundle, OutValRdyBundle
from pclib.ifcs  import valrdy_to_str
from pclib.rtl   import SingleElementPipelinedQueue

#--------------------------------------------------------------------
# FindIfUnitHLS
#--------------------------------------------------------------------
class FindIfUnitHLS ( VerilogModel ):
  def __init__ ( s ):
    s.cfgreq      = InValRdyBundle  ( 57 )
    s.cfgresp     = OutValRdyBundle ( 52 )
    s.itureq      = OutValRdyBundle ( 106 )
    s.ituresp     = InValRdyBundle  ( 52 )
    s.memreq      = OutValRdyBundle ( 77 )
    s.memresp     = InValRdyBundle  ( 45 )
    s.pereq       = OutValRdyBundle ( 36 )
    s.peresp      = InValRdyBundle  ( 36 )

    s.set_ports ({
      'ap_clk'   :   s.clk,
      'ap_rst'   :   s.reset,
      'ac_req_V'         :   s.cfgreq.msg,
      'ac_req_V_ap_vld'  :   s.cfgreq.val,
      'ac_req_V_ap_ack'  :   s.cfgreq.rdy,
      'ac_resp_V'        :   s.cfgresp.msg,
      'ac_resp_V_ap_vld' :   s.cfgresp.val,
      'ac_resp_V_ap_ack' :   s.cfgresp.rdy,
      'mem_req_V'         :   s.memreq.msg,
      'mem_req_V_ap_vld'  :   s.memreq.val,
      'mem_req_V_ap_ack'  :   s.memreq.rdy,
      'mem_resp_V'         :  s.memresp.msg,
      'mem_resp_V_ap_vld'  :  s.memresp.val,
      'mem_resp_V_ap_ack'  :  s.memresp.rdy,
      'g_dstu_iface_req_V'         :   s.itureq.msg,
      'g_dstu_iface_req_V_ap_vld'  :   s.itureq.val,
      'g_dstu_iface_req_V_ap_ack'  :   s.itureq.rdy,
      'g_dstu_iface_resp_V'        :   s.ituresp.msg,
      'g_dstu_iface_resp_V_ap_vld' :   s.ituresp.val,
      'g_dstu_iface_resp_V_ap_ack' :   s.ituresp.rdy,
      'g_pe_iface_req_V'         :   s.pereq.msg,
      'g_pe_iface_req_V_ap_vld'  :   s.pereq.val,
      'g_pe_iface_req_V_ap_ack'  :   s.pereq.rdy,
      'g_pe_iface_resp_V'        :   s.peresp.msg,
      'g_pe_iface_resp_V_ap_vld' :   s.peresp.val,
      'g_pe_iface_resp_V_ap_ack' :   s.peresp.rdy,
    });

#--------------------------------------------------------------------
# FindIfUnitHLS_Wrapper
#--------------------------------------------------------------------
class FindIfUnitHLS_Wrapper( Model ):
  def __init__( s ):
    s.cfgreq      = InValRdyBundle  ( 57 )
    s.cfgresp     = OutValRdyBundle ( 52 )
    s.itureq      = OutValRdyBundle ( 106 )
    s.ituresp     = InValRdyBundle  ( 52 )
    s.memreq      = OutValRdyBundle ( 77 )
    s.memresp     = InValRdyBundle  ( 45 )
    s.pereq       = OutValRdyBundle ( 36 )
    s.peresp      = InValRdyBundle  ( 36 )

    s.cfgresp_q   = SingleElementPipelinedQueue ( 52 )
    s.itureq_q    = SingleElementPipelinedQueue ( 106 )
    s.memreq_q    = SingleElementPipelinedQueue ( 77 )
    s.pereq_q     = SingleElementPipelinedQueue ( 36 )

    s.hls = FindIfUnitHLS()

    # cfg resp queue
    s.connect( s.cfgreq,        s.hls.cfgreq    )
    s.connect( s.cfgresp_q.enq, s.hls.cfgresp   )
    s.connect( s.cfgresp,       s.cfgresp_q.deq )

    # itu req queue
    s.connect( s.ituresp,      s.hls.ituresp  )
    s.connect( s.itureq_q.enq, s.hls.itureq   )
    s.connect( s.itureq,       s.itureq_q.deq )

    # mem req queue
    s.connect( s.memresp,      s.hls.memresp  )
    s.connect( s.memreq_q.enq, s.hls.memreq   )
    s.connect( s.memreq,       s.memreq_q.deq )

    # pe req queue
    s.connect( s.peresp,      s.hls.peresp  )
    s.connect( s.pereq_q.enq, s.hls.pereq   )
    s.connect( s.pereq,       s.pereq_q.deq )

  def line_trace( s ):
    return "{}{}|{}{}|{}{}".format(
      s.memreq,
      s.memresp,
      s.itureq,
      s.ituresp,
      s.pereq,
      s.peresp,
    )

