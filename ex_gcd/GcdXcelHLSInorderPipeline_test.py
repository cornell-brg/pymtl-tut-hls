#=========================================================================
# GcdXcelHLSInorderPipeline_test
#=========================================================================

import pytest

from pymtl       import *
from pclib.test  import run_sim

from GcdXcelHLSInorderPipeline import GcdXcelHLSInorderPipeline

#-------------------------------------------------------------------------
# Reuse tests from FL model
#-------------------------------------------------------------------------

from GcdXcelFL_test import TestHarness, test_case_table, run_test

@pytest.mark.parametrize( **test_case_table )
def test( test_params, dump_vcd, test_verilog ):
  run_test( GcdXcelHLSInorderPipeline(), test_params, dump_vcd, test_verilog )

