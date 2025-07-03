## V3.4.1.2_Patch_0002

### Features
* N/A.

### Bug Fixes
* Fix an issue: ATT TX Buffer could overflow when the exchanged effective MTU size is greater than default MTU size 23.

### BREAKING CHANGES
* N/A.


### Features
* N/A.

### Bug Fixes
* 修复: 当有效MTU值大于默认MTU值23时，ATT TX Buffer会溢出。

### BREAKING CHANGES
* N/A.


## V3.4.1.2_Patch_0001

### Features
* SDK version：telink_b80_ble_single_connection_sdk_v3.4.1.2_patch_0001.
* Add a new API "blc_contr_setBluetoothVersion" for changing the default Bluetooth version to meet some customers' special requirements.
* Add a new API "blc_att_holdAttributeResponsePayloadDuringPairingPhase" for setting whether to hold the ATT Response PDU during the pairing phase 3 to meet some customer's special requirement.

### Bug Fixes
* Fix an issue: For 4 kinds of ATT client commands below, the ATT server responds with data split by default MTU size 23 rather than the exchanged effective MTU size:
   - "ATT_OP_READ_BY_GROUP_TYPE_REQ"
   - "ATT_OP_FIND_BY_TYPE_VALUE_REQ"
   - "ATT_OP_READ_BY_TYPE_REQ"
   - "ATT_OP_FIND_INFO_REQ"
* Fix an issue: Some special Central devices send the "LL_PAUSE_ENC_REQ" request at encryption procedure, and may trigger peripheral devices working at error flow with very low probability, leading to disconnection.
* Fix an issue: Add 24M RC oscillator calibration with periodic time to enhance the oscillation starting ability of 24M crystal waking up from sleep, improving the situation that some poorly performing crystals may encounter insufficient oscillation with low probability.

### BREAKING CHANGES
* N/A.


### Features
* SDK 版本: telink_b80_ble_single_connection_sdk_v3.4.1.2_patch_0001.
* 增加API blc_contr_setBluetoothVersion，用于更改默认蓝牙版本，以满足一些客户的特殊要求。
* 增加API blc_att_holdAttributeResponsePayloadDuringPairingPhase，用于设置是否在配对阶段暂停ATT的交互，以满足一些客户的特殊要求。

### Bug Fixes
* 修复: 对于以下4种ATT客户端命令，ATT服务器响应数据按默认MTU大小23来分割，而不是按交换的有效MTU大小分割：
   - “ATT_OP_READ_BY_GROUP_TYPE_REQ”
   - “ATT_OP_FIND_BY_TYPE_VALUE_REQ”
   - “ATT_OP_READ_BY_TYPE_REQ”
   - “ATT_OP_FIND_INFO_REQ”
* 修复: 一些特殊的Central设备在加密过程中发送“LL_PAUSE_ENC_REQ”请求，极低的概率可能会触发Peripheral设备工作错误，导致断开连接。
* 修复: 增加24M RC定时校准，提高睡眠唤醒后的晶体起振能力，改善一些较差性能晶体可能存在的低概率起振不充分问题。

### BREAKING CHANGES
* N/A.
