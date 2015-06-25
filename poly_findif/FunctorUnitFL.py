#=========================================================================
# FindIfUnitFL
#=========================================================================
# Functional-level model of a find-if algorithm-specific-unit that works
# computes primitive types internally and sends Point types to an
# external Processing Element
#
# predicate value   func
# ---------------   ----
#               0   greater than zero
#               1   less    than zero
#               2   equal   to   zero
#               3   has an odd integer
#               4   has an even integer
#

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from pclib.cl   import InValRdyQueue, OutValRdyQueue

from dstu.BytesMemPortProxyFuture import BytesMemPortProxy
from dstu.TypeDescriptor import TypeDescriptor
from dstu.UserTypes import TypeEnum

#-------------------------------------------------------------------------
# FindIfUnitFL
#-------------------------------------------------------------------------
class FindIfUnitFL( Model ):

  def __init__( s, cfg_ifc_types, itu_ifc_types, mem_ifc_types, pe_ifc_types ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.cfgreq  = InValRdyBundle  ( cfg_ifc_types.req  )
    s.cfgresp = OutValRdyBundle ( cfg_ifc_types.resp )

    s.itureq  = OutValRdyBundle ( itu_ifc_types.req  )
    s.ituresp = InValRdyBundle  ( itu_ifc_types.resp )

    s.memreq   = OutValRdyBundle ( mem_ifc_types.req   )
    s.memresp  = InValRdyBundle  ( mem_ifc_types.resp  )

    s.pereq   = OutValRdyBundle ( pe_ifc_types.req   )
    s.peresp  = InValRdyBundle  ( pe_ifc_types.resp  )

    #---------------------------------------------------------------------
    # Adapters
    #---------------------------------------------------------------------

    # Adapter for the CFG request val-rdy bundle
    s.cfgreq_q  = InValRdyQueue  ( cfg_ifc_types.req  )
    s.connect( s.cfgreq_q.in_, s.cfgreq )

    # Adapter for the CFG response val-rdy bundle
    s.cfgresp_q = OutValRdyQueue ( cfg_ifc_types.resp )
    s.connect( s.cfgresp_q.out, s.cfgresp )

    # Adapter for the ITU request val-rdy bundle
    s.itureq_q  = OutValRdyQueue  ( itu_ifc_types.req  )
    s.connect( s.itureq_q.out, s.itureq )

    # Adapter for the response val-rdy bundle
    s.ituresp_q = InValRdyQueue ( itu_ifc_types.resp )
    s.connect( s.ituresp_q.in_, s.ituresp )
    
    # Adapter for the PE request val-rdy bundle
    s.pereq_q  = OutValRdyQueue  ( pe_ifc_types.req  )
    s.connect( s.utreq_q.out, s.pereq )

    # Adapter for the response val-rdy bundle
    s.peresp_q = InValRdyQueue ( pe_ifc_types.resp )
    s.connect( s.peresp_q.in_, s.peresp )

    # Memory port adapter
    s.mem = BytesMemPortProxy ( mem_ifc_types, s.memreq, s.memresp )

    #---------------------------------------------------------------------
    # Internal State
    #---------------------------------------------------------------------

    s.go               = False  # reg 0
    s.iter_first_ds_id = 0      # reg 1
    s.iter_first_iter  = 0      # reg 2
    s.iter_last_ds_id  = 0      # reg 3

