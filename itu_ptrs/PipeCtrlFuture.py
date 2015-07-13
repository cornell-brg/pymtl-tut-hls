#=======================================================================
# PipeCtrlFuture.py
#=======================================================================
# NOTE: This model improves the PipeCtrl logic present in the PyMTL pclib
# to include the improvements from the verilog implementation that include
# bubble squeezing etc. I am not adding to the PyMTL master yet as it
# may/may not break stuff there

from pymtl     import *
from pclib.rtl import RegEnRst

#-----------------------------------------------------------------------
# PipeCtrlFuture
#-----------------------------------------------------------------------
class PipeCtrlFuture( Model ):
  '''Manages squash and stall ctrl signals for a single pipeline stage.

   Inputs
   ------

   prev_val    : Valid bit coming from the previous stage
   next_stall  : Stall signal originating from the next stage
   next_squash : Squash signal originating from the next stage
   curr_stall  : Stall signal originating from the current pipeline stage
   curr_squash : Squash signal originating from the current pipeline stage

   Outputs
   -------

   next_val     : Valid bit calculation for the next stage
   prev_stall   : Aggregate stall signal for the previous stage
   prev_squash  : Aggregate squash signal for the previous stage
   curr_reg_en  : Enable Signal for all the pipeline registers at the
                  begining of current pipeline stage
   curr_val     : Combinational valid bit value of the current stage
   '''

  def __init__( s ):

    #-------------------------------------------------------------------
    # Input  Ports
    #-------------------------------------------------------------------

    s.prev_val    = InPort ( 1 )
    s.next_stall  = InPort ( 1 )
    s.next_squash = InPort ( 1 )
    s.curr_stall  = InPort ( 1 )
    s.curr_squash = InPort ( 1 )

    #-------------------------------------------------------------------
    # Output Ports
    #-------------------------------------------------------------------

    s.next_val     = OutPort ( 1 )
    s.prev_stall   = OutPort ( 1 )
    s.prev_squash  = OutPort ( 1 )
    s.curr_reg_en  = OutPort ( 1 )
    s.curr_val     = OutPort ( 1 )

    #-------------------------------------------------------------------
    # Static Elaboration
    #-------------------------------------------------------------------

    # current pipeline stage valid bit register

    s.val_reg = RegEnRst( 1, reset_value = 0 )

    s.connect( s.val_reg.in_, s.prev_val )

    # combinationally read out the valid bit of the current state and
    # assign it to curr_val

    s.connect( s.val_reg.out, s.curr_val )

    #---------------------------------------------------------------------
    # Combinational Logic
    #---------------------------------------------------------------------

    @s.combinational
    def comb():

      # Accumulate stall signals
      s.prev_stall.value = ~s.next_squash & s.curr_val & ( s.curr_stall | s.next_stall )

      # Accumulate squash signals
      s.prev_squash.value = s.next_squash | ( s.curr_squash & ~s.curr_stall & ~s.next_stall )

      # Enable the pipeline registers when the current stage is squashed
      # due to next_squash or when the current stage is not stalling due to
      # the curr_stall or next_stall. Otherwise do not set the enable signal
      # XXX: We factor in the previous valid signal here to not flop in
      # values that otherwise might mess up the datapath.
      s.curr_reg_en.value = s.next_squash | \
        ( ~s.prev_stall & s.prev_val )

      # Enable logic for the current pipeline stage valid bit register
      s.val_reg.en.value = ~s.prev_stall | s.next_squash

      # Insert microarchitectural 'nop' value when the current stage is
      # squashed due to next_squash or when the current stage is stalled due
      # to curr_stall or when the current stage is stalled due to next_stall.
      # Otherwise pipeline the valid bit. NOTE: This signal can also be
      # used as the "go" signal for executing a transaction in the current
      # pipeline state.
      s.next_val.value = s.curr_val & ~s.next_squash & ~s.next_stall & ~s.curr_stall
