#=========================================================================
# IteratorMsg_test.py
#=========================================================================
# Test suite for iterator request and response messages

from pymtl       import *
from IteratorMsg import IteratorReqMsg, IteratorRespMsg

#-------------------------------------------------------------------------
# test_req_msg
#-------------------------------------------------------------------------

def test_req_msg():

  # Create msg

  msg = IteratorReqMsg(32)
  msg.type_ = IteratorReqMsg.TYPE_READ
  msg.ds_id = 3
  msg.iter  = 12
  msg.field = 10
  msg.data  =  0

  # Verify msg

  assert msg.type_ == 0
  assert msg.ds_id == 3
  assert msg.iter  == 12
  assert msg.field == 10
  assert msg.data  == 0

  # Verify string

  assert str(msg) == "rd:03:00000c:0000000a:        "

  # Create msg

  msg = IteratorReqMsg(32)
  msg.type_ = IteratorReqMsg.TYPE_WRITE
  msg.ds_id = 6
  msg.iter  = 8
  msg.field = 4
  msg.data  = 0xdeadbeef

  # Verify msg

  assert msg.type_ == 1
  assert msg.ds_id == 6
  assert msg.iter  == 8
  assert msg.field == 4
  assert msg.data  == 0xdeadbeef

  # Verify string

  assert str(msg) == "wr:06:000008:00000004:deadbeef"

#-------------------------------------------------------------------------
# test_resp_msg
#-------------------------------------------------------------------------

def test_resp_msg():

  # Create msg

  msg = IteratorRespMsg(32)
  msg.type_ = IteratorRespMsg.TYPE_READ
  msg.data  = 0xdeadbeef

  # Verify msg

  assert msg.type_ == 0
  assert msg.data  == 0xdeadbeef

  # Verify string

  assert str(msg) == "rd:deadbeef"

  # Create msg

  msg = IteratorRespMsg(32)
  msg.type_ = IteratorRespMsg.TYPE_WRITE
  msg.data  = 0

  # Verify msg

  assert msg.type_ == 1
  assert msg.data  == 0

  # Verify string

  assert str(msg) == "wr:        "


