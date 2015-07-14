#=========================================================================
# PolyDsuConfigHLS
#=========================================================================
# Wrapper model for the HLS generated hardware

from pymtl      import *

from pclib.ifcs import InValRdyBundle, OutValRdyBundle

from dstu.XcelMsg      import XcelMsg
from dstu.MemMsgFuture import MemMsg

#-------------------------------------------------------------------------
# PolyDsuConfigHLS
#-------------------------------------------------------------------------

class PolyDsuConfigHLS( VerilogModel ):

  def __init__( s ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.cfgreq     = InValRdyBundle  ( XcelMsg().req   )
    s.cfgresp    = OutValRdyBundle ( XcelMsg().resp  )

    s.memreq     = OutValRdyBundle ( MemMsg(8,32,32).req   )
    s.memresp    = InValRdyBundle  ( MemMsg(8,32,32).resp  )

    s.dstype_ce   = OutPort( 1 )
    s.dstype_we   = OutPort( 1 )
    s.dstype_data = OutPort( 4 )
    s.dstype_addr = OutPort( 5 )

    s.dsdesc_ce   = OutPort( 1 )
    s.dsdesc_we   = OutPort( 1 )
    s.dsdesc_data = OutPort( 32 )
    s.dsdesc_addr = OutPort( 5 )

    s.dtdesc_ce   = OutPort( 1 )
    s.dtdesc_we   = OutPort( 1 )
    s.dtdesc_data = OutPort( 32 )
    s.dtdesc_addr = OutPort( 5 )

    s.set_ports({
      'ap_clk'              : s.clk,
      'ap_rst'              : s.reset,
      'cfgreq_V'            : s.cfgreq.msg,
      'cfgreq_V_ap_vld'     : s.cfgreq.val,
      'cfgreq_V_ap_ack'     : s.cfgreq.rdy,
      'cfgresp_V'           : s.cfgresp.msg,
      'cfgresp_V_ap_vld'    : s.cfgresp.val,
      'cfgresp_V_ap_ack'    : s.cfgresp.rdy,
      'memreq_V'            : s.memreq.msg,
      'memreq_V_ap_vld'     : s.memreq.val,
      'memreq_V_ap_ack'     : s.memreq.rdy,
      'memresp_V'           : s.memresp.msg,
      'memresp_V_ap_vld'    : s.memresp.val,
      'memresp_V_ap_ack'    : s.memresp.rdy,
      'dstype_V_address0'   : s.dstype_addr,
      'dstype_V_ce0'        : s.dstype_ce,
      'dstype_V_we0'        : s.dstype_we,
      'dstype_V_d0'         : s.dstype_data,
      'dsdesc_V_address0'   : s.dsdesc_addr,
      'dsdesc_V_ce0'        : s.dsdesc_ce,
      'dsdesc_V_we0'        : s.dsdesc_we,
      'dsdesc_V_d0'         : s.dsdesc_data,
      'dtdesc_V_address0'   : s.dtdesc_addr,
      'dtdesc_V_ce0'        : s.dtdesc_ce,
      'dtdesc_V_we0'        : s.dtdesc_we,
      'dtdesc_V_d0'         : s.dtdesc_data,
  })
