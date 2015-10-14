# BikeNavigation BLE profile specifications

## Description
BikeNavigation BLE profile enables GATT client to send turn-by-turn navigation instructions to BikeNavigation device. Typical use case would be in navigation mobile applications where it would enable sending of turn instructions and use of BikeNavigation as extended/companion navigation display.

## Specifications
BikeNavigation profile consists of single BikeNavigation service with single write characteristic.

### BikeNavigation service specifications:
|  | |
|:-|:-|
|**Name**|BikeNavigation
|**Type**|Custom
|**UUID**|FED0
|**Description**|This service implements data consumer to be used by a navigation companion or extended display.
|**GATT Requirements**|Write Characteristic Value ```Mandatory```
|**Error Codes**|This service does not define any application error codes that are used in Attribute Protocol.
|**Service Characteristics**|BikeNavigation Icon

### BikeNavigation Icon characteristic specifications:
|  | |
|:-|:-|
|**Name**|BikeNavigation Icon
|**Type**|Custom
|**UUID**|FED1
|**Requirement**|Mandatory
|**Format**|[8bit](https://developer.bluetooth.org/gatt/Pages/FormatTypes.aspx)
|**Properties**|WriteWithoutResponse ```Mandatory```
|**Security**|None
|**Descriptors**|None
|**Value Fields**|icon ```Mandatory```

### icon value field specifications:
|  | |
|:-|:-|
|**Requirement**|Mandatory
|**Format**|[8bit](https://developer.bluetooth.org/gatt/Pages/FormatTypes.aspx)
|**Minimum Value**|0x00
|**Maximum Value**|0xFF

**Value mapping**:

| value | icon  
| :---- |:----|
|0|undefined
|1|straight
|2|right u-turn
|3|left u-turn
|4|keep right
|5|light right
|6|right
|7|heavy right
|8|keep middle
|9|keep left
|10|light left
|11|left
|12|heavy left
|13|navigation start
|14|navigation end
|20|light right soon
|21|right soon
|22|heavy right soon
|23|light left soon
|24|left soon
|25|heavy left soon
|26|light right now
|27|right now
|28|heavy right now
|29|light left now
|30|left now
|31|heavy left now
|32|keep right soon
|33|keep right now
|34|keep left soon
|35|keep right now
|40|start heading 0
|41|start heading 45
|42|start heading 90
|43|start heading 135
|44|start heading 180
|45|start heading 225
|46|start heading 270
|47|start heading 315
|60|gps lost
|61|rerouting
|70|ble connected
|71|ble disconnected
