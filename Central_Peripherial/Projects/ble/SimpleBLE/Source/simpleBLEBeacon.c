#include "simpleBLEBeacon.h"
#include "simpleBLECentral.h"

#if (PRESET_ROLE == BLE_PRE_ROLE_BEACON)
uint8 advertData_iBeacon[] =
{
  0x02, // length of this data, 0
  GAP_ADTYPE_FLAGS, // 1
  GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, // 2
  0x1A, // length of this data 26byte, 3
  GAP_ADTYPE_MANUFACTURER_SPECIFIC, // 4
  /*Apple Pre-Amble*/
  0x4C, // 5
  0x00, // 6
  0x02, // 7
  0x15, // 8
  /*Device UUID (16 Bytes)*/
  0x53, 0x4D, 0x54, // SMT 3 Bytes.
  0x00, // 12 reserved
  0x00, // 13 reserved.
  MAJOR_HW_VERSION, MAJOR_SW_VERSION, MINOR_SW_VERSION, // 14, 15, 16, HW/SW version
  BLE_BEACON, // 17 Device Type 3 bytes.

  BLE_CMD_POWER_ON, //18

  SCAN_ADV_TRANS_MIN_PERIOD, //19 How often the beacon will scan for the station in power on mode.
  DEFAULT_WAKE_TIME_MINS, //20 The period which the device keeps poweron even without scanning any data.
  SBP_PERIODIC_OFF_SCAN_PERIOD_SEC_1, //21 How often the beacon will scan for the station in power off mode.
  SBP_PERIODIC_OFF_SCAN_PERIOD_SEC_2, //22 The Data will be ((SEC_1 << 8) + SEC_2)

  /*Specific Data*/
  0x00, // 23
  0x00, // 24, Station Index
  /*Major Value (2 Bytes)*/
  0x00, // 25 for min left
  0x00, // 26 for index 
  /*Minor Value (2 Bytes)*/
  0x00, // 27 FlagByte. bit7 rapid bit6 low_bat
  0x00, // 28 Battery Value

  0xCD //29  /*Measured Power*/
};



extern SYS_CONFIG sys_config;
extern uint8 simpleBLETaskId;

void scan_adv_event_callback(uint8 role)
{
    if (role == BLE_ROLE_CENTRAL)
    {
        if (getCurrentBLEStatus() == BLE_STATUS_ON_SCAN)
        {
            sys_config.status = BLE_STATUS_ON_ADV;
            simpleBLE_SaveAndReset();
        }
    }
    else if (role == BLE_ROLE_PERIPHERAL)
    {
        sys_config.status = BLE_STATUS_ON_SCAN;
        sys_config.stationIndex = (advertData_iBeacon[ADV_STATION_INDEX_1] << 8) + advertData_iBeacon[ADV_STATION_INDEX_2];
        sys_config.minLeft = advertData_iBeacon[ADV_MIN_LEFT_BYTE];
        simpleBLE_SaveAndReset();
    }
}

void scan_device_info_callback(gapCentralRoleEvent_t *pEvent)
{
    if (simpleBLEFilterSelfBeacon(pEvent->deviceInfo.pEvtData, pEvent->deviceInfo.dataLen) == TRUE)
    {
        if (simpleBLEFilterIsSmart(pEvent->deviceInfo.pEvtData, pEvent->deviceInfo.dataLen) == TRUE)
        {
            DEBUG_PRINT("Found Smart Device\r\n");
            // Our adv content. do further process.
            if (getCurrentBLEStatus() == BLE_STATUS_ON_SCAN)
            {
                if (simpleBLEFilterDeviceType(pEvent->deviceInfo.pEvtData, pEvent->deviceInfo.dataLen) == BLE_STATION_ADV)
                {
                    DEBUG_PRINT("Station ADV\r\n");
                    uint16 advStationIndex = (pEvent->deviceInfo.pEvtData[ADV_STATION_INDEX_1] << 8) + pEvent->deviceInfo.pEvtData[ADV_STATION_INDEX_2];
                    uint8 cmd = pEvent->deviceInfo.pEvtData[ADV_STATION_CMD_INDEX];
                    if (cmd == BLE_CMD_POWER_OFF)
                    {
                        DEBUG_PRINT("POWER_OFF\r\n");
                        sys_config.stationIndex = 0;
                        sys_config.powerOnPeriod = pEvent->deviceInfo.pEvtData[ADV_STATION_POWER_ON_PERIOD_INDEX];
                        sys_config.powerOnScanInterval = pEvent->deviceInfo.pEvtData[ADV_STATION_ON_SCAN_INTERVAL_INDEX];
                        sys_config.powerOffScanInterval = (pEvent->deviceInfo.pEvtData[ADV_STATION_OFF_SCAN_INTERVAL_INDEX_1] << 8) + pEvent->deviceInfo.pEvtData[ADV_STATION_OFF_SCAN_INTERVAL_INDEX_2];

                        sys_config.status = BLE_STATUS_OFF;
                        simpleBLE_SaveAndReset();
                    }
                    else if (cmd == BLE_CMD_POWER_ON)
                    {
                        DEBUG_VALUE("current stationIndex: ", sys_config.stationIndex, 10);
                        sys_config.stationIndex = advStationIndex;
                        sys_config.powerOnPeriod = pEvent->deviceInfo.pEvtData[ADV_STATION_POWER_ON_PERIOD_INDEX];
                        sys_config.powerOnScanInterval = pEvent->deviceInfo.pEvtData[ADV_STATION_ON_SCAN_INTERVAL_INDEX];
                        sys_config.powerOffScanInterval = (pEvent->deviceInfo.pEvtData[ADV_STATION_OFF_SCAN_INTERVAL_INDEX_1] << 8) + pEvent->deviceInfo.pEvtData[ADV_STATION_OFF_SCAN_INTERVAL_INDEX_2];
#ifdef DEBUG_BOARD
                        DEBUG_VALUE("CMD: ", pEvent->deviceInfo.pEvtData[ADV_STATION_CMD_INDEX], 10);
                        DEBUG_VALUE("StationIndexUpdate to ", advStationIndex, 10);
                        DEBUG_VALUE("MinLeft : ", sys_config.minLeft, 10);
                        DEBUG_VALUE("PowerOnPeriod", sys_config.powerOnPeriod, 10);
                        DEBUG_VALUE("powerOnScanInterval", sys_config.powerOnScanInterval, 10);
                        DEBUG_VALUE("powerOffScanInterval", sys_config.powerOffScanInterval, 10);
#endif
                        sys_config.minLeft = pEvent->deviceInfo.pEvtData[ADV_STATION_POWER_ON_PERIOD_INDEX];
                    }
                }
                else if (simpleBLEFilterDeviceType(pEvent->deviceInfo.pEvtData, pEvent->deviceInfo.dataLen) == BLE_BEACON)
                {
                    DEBUG_PRINT("Beacon\r\n");
                    uint16 advStationIndex = (pEvent->deviceInfo.pEvtData[ADV_STATION_INDEX_1] << 8) + pEvent->deviceInfo.pEvtData[ADV_STATION_INDEX_2];
                    if (sys_config.stationIndex < advStationIndex)
                    {
                        DEBUG_PRINT("StationIndex Update\r\n");
                        sys_config.stationIndex = advStationIndex;
                        // Copy the minleft from the beacon.
                        sys_config.minLeft = pEvent->deviceInfo.pEvtData[ADV_MIN_LEFT_BYTE];
                    }
                }
            }
            else if (getCurrentBLEStatus() == BLE_STATUS_OFF)
            {
                if (simpleBLEFilterDeviceType(pEvent->deviceInfo.pEvtData, pEvent->deviceInfo.dataLen) == BLE_STATION_ADV)
                {
                    DEBUG_PRINT("OFF: Station ADV\r\n");
                    BLE_STATION_CMD cmd = pEvent->deviceInfo.pEvtData[ADV_STATION_CMD_INDEX];
                    if (cmd == BLE_CMD_POWER_ON)
                    {
                        uint16 advStationIndex = (pEvent->deviceInfo.pEvtData[ADV_STATION_INDEX_1] << 8) + pEvent->deviceInfo.pEvtData[ADV_STATION_INDEX_2];
                        sys_config.stationIndex = advStationIndex;
                        sys_config.minLeft = pEvent->deviceInfo.pEvtData[ADV_STATION_POWER_ON_PERIOD_INDEX];
                        sys_config.powerOnPeriod = pEvent->deviceInfo.pEvtData[ADV_STATION_POWER_ON_PERIOD_INDEX];
                        sys_config.powerOnScanInterval = pEvent->deviceInfo.pEvtData[ADV_STATION_ON_SCAN_INTERVAL_INDEX];
                        sys_config.powerOffScanInterval = (pEvent->deviceInfo.pEvtData[ADV_STATION_OFF_SCAN_INTERVAL_INDEX_1] << 8) + pEvent->deviceInfo.pEvtData[ADV_STATION_OFF_SCAN_INTERVAL_INDEX_2];

                        sys_config.status = BLE_STATUS_ON_ADV;

#ifdef DEBUG_BOARD
                        DEBUG_VALUE("powerOffScanInterval", sys_config.powerOffScanInterval, 10);
                        DEBUG_VALUE("PowerOnPeriod", sys_config.powerOnPeriod, 10);
                        DEBUG_VALUE("powerOnScanInterval", sys_config.powerOnScanInterval, 10);
                        DEBUG_VALUE("CMD: ", cmd, 10);
                        DEBUG_VALUE("StationIndex = ", advStationIndex, 10);
                        DEBUG_VALUE("MinLeft : ", sys_config.minLeft, 10);
#endif
                        simpleBLE_SaveAndReset();
                    }
                    else if (cmd == BLE_CMD_POWER_OFF)
                    {
                        sys_config.stationIndex = 0;
                        sys_config.powerOnPeriod = pEvent->deviceInfo.pEvtData[ADV_STATION_POWER_ON_PERIOD_INDEX];
                        sys_config.powerOnScanInterval = pEvent->deviceInfo.pEvtData[ADV_STATION_ON_SCAN_INTERVAL_INDEX];
                        sys_config.powerOffScanInterval = (pEvent->deviceInfo.pEvtData[ADV_STATION_OFF_SCAN_INTERVAL_INDEX_1] << 8) + pEvent->deviceInfo.pEvtData[ADV_STATION_OFF_SCAN_INTERVAL_INDEX_2];
                        sys_config.status = BLE_STATUS_OFF;
                        simpleBLE_WriteAllDataToFlash();
                    }
                    else if (cmd == BLE_CMD_LED_BLINK)
                    {
                        sys_config.stationIndex = 0;
                        sys_config.powerOnPeriod = pEvent->deviceInfo.pEvtData[ADV_STATION_POWER_ON_PERIOD_INDEX];
                        sys_config.powerOnScanInterval = pEvent->deviceInfo.pEvtData[ADV_STATION_ON_SCAN_INTERVAL_INDEX];
                        sys_config.powerOffScanInterval = (pEvent->deviceInfo.pEvtData[ADV_STATION_OFF_SCAN_INTERVAL_INDEX_1] << 8) + pEvent->deviceInfo.pEvtData[ADV_STATION_OFF_SCAN_INTERVAL_INDEX_2];
                        sys_config.status = BLE_STATUS_OFF;
                        led_toggle_set_param(PERIPHERAL_START_LED_TOGGLE_PERIOD_ON, PERIPHERAL_START_LED_TOGGLE_PERIOD_OFF, BUTTON_LED_TOGGLE_COUNT, 0);
                    }
                }
            }
        }
    }
}

bool scan_discovery_callback(void)
{
    if (getCurrentBLEStatus() == BLE_STATUS_ON_SCAN && getScanTimeLeft() == 0)
    {
      DEBUG_PRINT("ON_SCAN, TimeLeft 0\r\n");
      osal_set_event(simpleBLETaskId, SBP_SCAN_ADV_TRANS_EVT);
    }
    else if (getCurrentBLEStatus() == BLE_STATUS_OFF && getScanTimeLeft() == 0)
    {
      DEBUG_PRINT("OFF, TimeLeft 0\r\n");
      resetScanTimeLeft();
      osal_start_timerEx(simpleBLETaskId, SBP_WAKE_EVT, (sys_config.powerOffScanInterval * 1000));
    }
    else
    {
      simpleBLEStartScan();
    }
    return TRUE;
}

void key_press_callback(uint8 key_cnt_number)
{
    if (getCurrentBLEStatus() == BLE_STATUS_OFF)
    {
        DEBUG_PRINT("STATUS_OFF, ignore key\r\n");
        return;
    }

    if (key_cnt_number == 0)
    {
        return;
    }

    DEBUG_PRINT("ON_SCAN, KEY CALLBACK\r\n");
    if (key_cnt_number >= 2)
    {
        DEBUG_PRINT("Timer is reset\r\n");
        /*
      wake_up_hours_remain = DEFAULT_WAKE_TIME_HOURS;
      // reset wake_up_left
      advertData_iBeacon[ADV_HOUR_LEFT_BYTE] = wake_up_hours_remain;
      */
        // LED blink twice
        led_toggle_set_param(PERIPHERAL_START_LED_TOGGLE_PERIOD_ON, PERIPHERAL_START_LED_TOGGLE_PERIOD_OFF, PERIPHERAL_WAKEUP_LED_TOGGLE_CNT, BUTTON_LED_DELAY);
    }
    else if (key_cnt_number == 1)
    {
        // Blink once
        led_toggle_set_param(PERIPHERAL_START_LED_TOGGLE_PERIOD_ON, PERIPHERAL_START_LED_TOGGLE_PERIOD_OFF, BUTTON_LED_TOGGLE_COUNT, BUTTON_LED_DELAY);
        /*
#if (POWER_OFF_SUPPORT == TRUE)
      key_cnt_number = 0;
      osal_set_event(simpleBLETaskId, SBP_KEY_LONG_PRESSED_EVT);
#endif
      */
    }
    osal_set_event(simpleBLETaskId, SBP_SCAN_ADV_TRANS_EVT);
    return;
}
#endif