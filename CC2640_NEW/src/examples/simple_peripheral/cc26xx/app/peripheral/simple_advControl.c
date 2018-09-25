#include "simple_advControl.h"
#include "peripheral_observer.h"
#include "simple_stateControl.h"
#include "simple_touchRecord.h"
#include "simple_gatt_profile.h"
#include "gatt_profile_uuid.h"
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include "gap.h"
#include "util.h"
#include "hci.h"

#define DEFAULT_DISCOVERABLE_MODE GAP_ADTYPE_FLAGS_GENERAL

static uint8_t advEnable = false;

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8_t advertData[] =
{
    // Flags; this sets the device to use limited discoverable
    // mode (advertises for 30 seconds at a time) instead of general
    // discoverable mode (advertises indefinitely)
    0x02, // length of this data
    GAP_ADTYPE_FLAGS,
    DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    0x1A,
    GAP_ADTYPE_MANUFACTURER_SPECIFIC,
    /*Apple Pre-Amble*/
    0x4C, // 5
    0x00, // 6
    0x02, // 7
    0x15, // 8
    /*Device UUID (16 Bytes)*/
    0x53, 0x4D, 0x54,       // SMT 3 Bytes.
    0xFF,                   // 12 MAC CRC,
    BLE_DEV_BEACON, 0xFF,   // 13 Device Type, 14 Version (higher 4 bits for HW, lower 4 bit for SW)
    0xFF, 0xFF, 0xFF, 0xFF, // 15~18 for advertise MAC.

    0xFF, //19 How often the beacon will scan for the station in power on mode.
    0xFF, //20 The period which the device keeps poweron even without scanning any data.
    0xFF, //21 How often the beacon will scan for the station in power off mode.
    0xFF, //22 The Data will be ((SEC_1 << 8) + SEC_2)
    0xFF, // 23
    0xFF, // 24, Station Index

    /*Major Value (2 Bytes)*/
    0x00, // 25 for min left
    0x00, // 26 for index
    /*Minor Value (2 Bytes)*/
    0x00, // 27 FlagByte. bit7 rapid, bit6 low_bat, bit5 Comms.
    0x00, // 28 Battery Value
    0xCD  //29  /*Measured Power*/
};

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
    // complete name
    0x06, // length of this data
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'B',
    'A',
    'D',
    'G',
    'E',

    0x11, // 0
    GAP_ADTYPE_SERVICE_DATA_128BIT, //1 16 bytes. 
    0x53, 0x4D, 0x54, // 2~4, SMT 3 Bytes
    0xFF, // 5 MAC CRC - 12
    BLE_DEV_BEACON, 0xFF,   // 6~7 Device Type, Version (higher 4 bits for HW, lower 4 bit for SW)
    0x05, 0x06, 0x07, 0x08, // 8~11 Advertise MAC.
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, // 12~17 real self mac.

    0x03,                               // 9
    GAP_ADTYPE_16BIT_COMPLETE,          //10
    LO_UINT16(SIMPLEPROFILE_SERV_UUID), // 100ms
    HI_UINT16(SIMPLEPROFILE_SERV_UUID),
};

void updateAdvMac(uint8_t *macAddr, uint8_t crcByte)
{
    advertData[MAC_CRC_ADV_BYTE] = crcByte;
    scanRspData[MAC_CRC_RES_BYTE] = crcByte;
    for (uint8_t i = 0; i < B_ADDR_LEN; i++)
    {
        advertData[MAC_XOR_ADV_BYTE + i] = (~macAddr[i]) & 0xFF;
        scanRspData[MAC_XOR_RES_BYTE + i] = (~macAddr[i]) & 0xFF;
    }
}

void applyAdvData()
{
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);
}

void applyResData()
{
    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);
}

uint8_t *getAdvData()
{
    return advertData;
}

uint8_t *getResData()
{
    return scanRspData;
}

void updateRapidBit(uint8_t enable)
{
    if (enable)
    {
        advertData[ADV_FLAG_BYTE] |= (0x1 << ADV_FLAG_RAPID_BIT);
    }
    else
    {
        advertData[ADV_FLAG_BYTE] &= ~(0x1 << ADV_FLAG_RAPID_BIT);
    }
}

void updateComBit(uint8_t enable)
{
    if (enable)
    {
        advertData[ADV_FLAG_BYTE] |= (0x01 << ADV_FLAG_COMMS_BIT);
    }
    else
    {
        advertData[ADV_FLAG_BYTE] &= ~(0x01 << ADV_FLAG_COMMS_BIT);
    }
}

void updateAdvInterval(uint16_t advInterval)
{
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, advInterval);
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, advInterval);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, advInterval);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, advInterval);
}

void updateBeaconIndex()
{
    advertData[ADV_INDEX_BYTE] += 1;
    applyAdvData();
    bleSysConfig.beaconIndex = advertData[ADV_INDEX_BYTE];
}

void updateBeaconTouchData(uint8_t *macData)
{
    advertData[TOUCH_BEACON_MAC] = (macData[3]);
    advertData[TOUCH_BEACON_MAC + 1] = macData[2];
    advertData[TOUCH_BEACON_MAC + 2] = macData[1];
    advertData[TOUCH_BEACON_MAC + 3] = macData[0];
    applyAdvData();
}

void updateBeaconTouchMac(uint8_t *macAddr)
{
    advertData[TOUCH_BEACON_MAC] = (macAddr[5] + macAddr[4] + macAddr[3]) & 0xFF;
    advertData[TOUCH_BEACON_MAC + 1] = macAddr[2];
    advertData[TOUCH_BEACON_MAC + 2] = macAddr[1];
    advertData[TOUCH_BEACON_MAC + 3] = macAddr[0];
    applyAdvData();
}

void resetBeaconTouchInfo()
{
    advertData[TOUCH_BEACON_MAC] = 0xFF;
    advertData[TOUCH_BEACON_MAC + 1] = 0xFF;
    advertData[TOUCH_BEACON_MAC + 2] = 0xFF;
    advertData[TOUCH_BEACON_MAC + 3] = 0xFF;
    applyAdvData();
}

void bleAdvControl(uint8_t enable, bool connectable)
{
    uint8_t disableOther = false;
    advEnable = enable;
    //GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &advEnable);

    // Set the GAP Role Parameters
    if (connectable)
    {
        GAPRole_SetParameter(GAPROLE_ADV_NONCONN_ENABLED, sizeof(uint8_t), &disableOther);
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &advEnable);
    }
    else
    {
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &disableOther);
        GAPRole_SetParameter(GAPROLE_ADV_NONCONN_ENABLED, sizeof(uint8_t), &advEnable);
    }
}

void bleSetTxPower(uint8_t level)
{
    HCI_EXT_SetTxPowerCmd(level);
}