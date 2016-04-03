#=========================================================================
# PopCount_test.py
#=========================================================================
# This file contains unit tests for the PopCount model

import pytest

from pymtl      import *
from pclib.test import TestVectorSimulator, TestSource, TestSink

from PopCount import PopCount

#=========================================================================
# Test Vector Tests - PopCount
#=========================================================================

#-------------------------------------------------------------------------
# test_1entry_normal_queue_tv
#-------------------------------------------------------------------------
def test_PopCount( dump_vcd, test_verilog ):
  #PopCount Test Vector Tests

  mini = [ 0x21, 0x14, 0x1133 ]
  mini_results = [ 2, 2, 6 ]

  test_vectors = [

    # x_V     num   num_ap_vld
    [ 0x21,   '?',  0     ],
    [ 0x41,   2,    1     ],
    [ 0x1133, 2,    0     ],
    [ 0x0,    6,    1     ],
    [ 0x41,   6,    0     ],
    [ 0x0,    2,    1     ],

  ]

  # Instantiate and elaborate the model

  model = PopCount()
  model.vcd_file = dump_vcd
  if test_verilog:
    model = TranslationTool( model )
  model.elaborate()

  # Define functions mapping the test vector to ports in model

  def tv_in( model, test_vector ):

    model.x_V.value = test_vector[0]

  def tv_out( model, test_vector ):

    if not test_vector[1] == '?':
      assert model.num.value == test_vector[1]
    if not test_vector[2] == '?':
      assert model.num_ap_vld.value == test_vector[2]

  # Run the test

  sim = TestVectorSimulator( model, test_vectors, tv_in, tv_out )
  sim.run_test()
