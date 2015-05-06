#====================================================================
# FindIfUnitVRTL
#====================================================================
# PyMtl wrapper for HLS-synthesized Verilog module of find-if unit
#

from pymtl       import *
from pclib.ifcs  import InValRdyBundle, OutValRdyBundle
from pclib.ifcs  import valrdy_to_str

class FindIfUnitVRTL ( VerilogModel ):
  def __init__ ( s ):
    s.cfgreq      = InValRdyBundle  ( 56 );
    s.cfgresp     = OutValRdyBundle ( 51 );
    s.itureq      = OutValRdyBundle ( 97 );
    s.ituresp     = InValRdyBundle  ( 33 );
    s.memreq      = OutValRdyBundle ( 77 )
    s.memresp     = InValRdyBundle  ( 45 )

    s.set_ports ({
      'clk'   :   s.clk,
      'reset' :   s.reset,
      'cfg_req_V'         :   s.cfgreq.msg,
      'cfg_req_V_ap_vld'  :   s.cfgreq.val,
      'cfg_req_V_ap_ack'  :   s.cfgreq.rdy,
      'cfg_resp_V'        :   s.cfgresp.msg,
      'cfg_resp_V_ap_vld' :   s.cfgresp.val,
      'cfg_resp_V_ap_ack' :   s.cfgresp.rdy,
      'mem_req_V'         :   s.memreq.msg,
      'mem_req_V_ap_vld'  :   s.memreq.val,
      'mem_req_V_ap_ack'  :   s.memreq.rdy,
      'mem_resp_V'         :  s.memresp.msg,
      'mem_resp_V_ap_vld'  :  s.memresp.val,
      'mem_resp_V_ap_ack'  :  s.memresp.rdy,
      'g_dtu_iface_req_V'         :   s.itureq.msg,
      'g_dtu_iface_req_V_ap_vld'  :   s.itureq.val,
      'g_dtu_iface_req_V_ap_ack'  :   s.itureq.rdy,
      'g_dtu_iface_resp_V'        :   s.ituresp.msg,
      'g_dtu_iface_resp_V_ap_vld' :   s.ituresp.val,
      'g_dtu_iface_resp_V_ap_ack' :   s.ituresp.rdy,
    });

  def line_trace( s ):
    return "{}(){}{}(){}".format(
      valrdy_to_str( s.cfgreq.msg,
                     s.cfgreq.val,
                     s.cfgreq.rdy ),
      
      valrdy_to_str( s.itureq.msg,
                     s.itureq.val,
                     s.itureq.rdy ),

      valrdy_to_str( s.ituresp.msg,
                     s.ituresp.val,
                     s.ituresp.rdy ),
      
      valrdy_to_str( s.cfgresp.msg,
                     s.cfgresp.val,
                     s.cfgresp.rdy )

    )
    
