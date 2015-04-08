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
    # From the Xilinx HLS Guide
    # ap_start should be asserted high and lowered when ap_ready is seen
    # ap_idle will be asserted low over duration of operation
    # ap_ready will be asserted high for 1 cycle when inputs have been read
    # ap_done will be asserted high for 1 cycle when return data is ready
    #s.ap_start    = InPort ( 1 );
    #s.ap_done     = OutPort ( 1 );
    #s.ap_idle     = OutPort ( 1 );
    #s.ap_ready    = OutPort ( 1 );
    s.cfg_in      = InValRdyBundle  ( 70 );
    s.cfg_out     = OutValRdyBundle ( 33 );
    s.itu_in      = InValRdyBundle  ( 33 );
    s.itu_out     = OutValRdyBundle ( 97 );

    s.set_ports ({
      'clk'   :   s.clk,
      'reset' :   s.reset,
      #'ap_start' :    s.ap_start,
      #'ap_done'  :    s.ap_done,
      #'ap_idle'  :    s.ap_idle,
      #'ap_ready' :    s.ap_ready,
      'cfg_req_V'         :   s.cfg_in.msg,
      'cfg_req_V_ap_vld'  :   s.cfg_in.val,
      'cfg_req_V_ap_ack'  :   s.cfg_in.rdy,
      'cfg_resp_V'        :   s.cfg_out.msg,
      'cfg_resp_V_ap_vld' :   s.cfg_out.val,
      'cfg_resp_V_ap_ack' :   s.cfg_out.rdy,
      'g_itu_iface_req_V'         :   s.itu_out.msg,
      'g_itu_iface_req_V_ap_vld'  :   s.itu_out.val,
      'g_itu_iface_req_V_ap_ack'  :   s.itu_out.rdy,
      'g_itu_iface_resp_V'        :   s.itu_in.msg,
      'g_itu_iface_resp_V_ap_vld' :   s.itu_in.val,
      'g_itu_iface_resp_V_ap_ack' :   s.itu_in.rdy,
    });

  def line_trace( s ):
    return "{}(){}{}(){}".format(
      valrdy_to_str( s.cfg_in.msg,
                     s.cfg_in.val,
                     s.cfg_in.rdy ),
      
      valrdy_to_str( s.itu_out.msg,
                     s.itu_out.val,
                     s.itu_out.rdy ),

      valrdy_to_str( s.itu_in.msg,
                     s.itu_in.val,
                     s.itu_in.rdy ),
      
      valrdy_to_str( s.cfg_out.msg,
                     s.cfg_out.val,
                     s.cfg_out.rdy )

    )
    
