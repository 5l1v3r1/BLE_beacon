/**
 * @file nfc-types.h
 * @brief Define NFC types
 */

#ifndef __NFC_TYPES_H__
#define __NFC_TYPES_H__

#ifndef NFC_BUFSIZE_CONNSTRING
#define NFC_BUFSIZE_CONNSTRING 1024
#endif

#include "../simpleble.h"

/**
 * NFC context
 */
typedef struct nfc_context nfc_context;

/**
 * NFC device
 */
typedef struct nfc_device nfc_device;

/**
 * NFC device driver
 */
typedef struct nfc_driver nfc_driver;

/**
 * Connection string
 */
typedef char nfc_connstring[NFC_BUFSIZE_CONNSTRING];

/**
 * Properties
 */
typedef enum {
  /**
   * Default command processing timeout
   * Property value's (duration) unit is ms and 0 means no timeout (infinite).
   * Default value is set by driver layer
   */
  NP_TIMEOUT_COMMAND,
  /**
   * Timeout between ATR_REQ and ATR_RES
   * When the device is in initiator mode, a target is considered as mute if no
   * valid ATR_RES is received within this timeout value.
   * Default value for this property is 103 ms on PN53x based devices.
   */
  NP_TIMEOUT_ATR,
  /**
   * Timeout value to give up reception from the target in case of no answer.
   * Default value for this property is 52 ms).
   */
  NP_TIMEOUT_COM,
  /** Let the PN53X chip handle the CRC bytes. This means that the chip appends
  * the CRC bytes to the frames that are transmitted. It will parse the last
  * bytes from received frames as incoming CRC bytes. They will be verified
  * against the used modulation and protocol. If an frame is expected with
  * incorrect CRC bytes this option should be disabled. Example frames where
  * this is useful are the ATQA and UID+BCC that are transmitted without CRC
  * bytes during the anti-collision phase of the ISO14443-A protocol. */
  NP_HANDLE_CRC,
  /** Parity bits in the network layer of ISO14443-A are by default generated and
   * validated in the PN53X chip. This is a very convenient feature. On certain
   * times though it is useful to get full control of the transmitted data. The
   * proprietary MIFARE Classic protocol uses for example custom (encrypted)
   * parity bits. For interoperability it is required to be completely
   * compatible, including the arbitrary parity bits. When this option is
   * disabled, the functions to communicating bits should be used. */
  NP_HANDLE_PARITY,
  /** This option can be used to enable or disable the electronic field of the
   * NFC device. */
  NP_ACTIVATE_FIELD,
  /** The internal CRYPTO1 co-processor can be used to transmit messages
   * encrypted. This option is automatically activated after a successful MIFARE
   * Classic authentication. */
  NP_ACTIVATE_CRYPTO1,
  /** The default configuration defines that the PN53X chip will try indefinitely
   * to invite a tag in the field to respond. This could be desired when it is
   * certain a tag will enter the field. On the other hand, when this is
   * uncertain, it will block the application. This option could best be compared
   * to the (NON)BLOCKING option used by (socket)network programming. */
  NP_INFINITE_SELECT,
  /** If this option is enabled, frames that carry less than 4 bits are allowed.
   * According to the standards these frames should normally be handles as
   * invalid frames. */
  NP_ACCEPT_INVALID_FRAMES,
  /** If the NFC device should only listen to frames, it could be useful to let
   * it gather multiple frames in a sequence. They will be stored in the internal
   * FIFO of the PN53X chip. This could be retrieved by using the receive data
   * functions. Note that if the chip runs out of bytes (FIFO = 64 bytes long),
   * it will overwrite the first received frames, so quick retrieving of the
   * received data is desirable. */
  NP_ACCEPT_MULTIPLE_FRAMES,
  /** This option can be used to enable or disable the auto-switching mode to
   * ISO14443-4 is device is compliant.
   * In initiator mode, it means that NFC chip will send RATS automatically when
   * select and it will automatically poll for ISO14443-4 card when ISO14443A is
   * requested.
   * In target mode, with a NFC chip compliant (ie. PN532), the chip will
   * emulate a 14443-4 PICC using hardware capability */
  NP_AUTO_ISO14443_4,
  /** Use automatic frames encapsulation and chaining. */
  NP_EASY_FRAMING,
  /** Force the chip to switch in ISO14443-A */
  NP_FORCE_ISO14443_A,
  /** Force the chip to switch in ISO14443-B */
  NP_FORCE_ISO14443_B,
  /** Force the chip to run at 106 kbps */
  NP_FORCE_SPEED_106,
} nfc_property;

// Compiler directive, set struct alignment to 1 uint8 for compatibility
#  pragma pack(1)

/**
 * @enum nfc_dep_mode
 * @brief NFC D.E.P. (Data Exchange Protocol) active/passive mode
 */
typedef enum {
  NDM_UNDEFINED = 0,
  NDM_PASSIVE,
  NDM_ACTIVE,
} nfc_dep_mode;

/**
 * @struct nfc_dep_info
 * @brief NFC target information in D.E.P. (Data Exchange Protocol) see ISO/IEC 18092 (NFCIP-1)
 */
typedef struct {
  /** NFCID3 */
  uint8  abtNFCID3[10];
  /** DID */
  uint8  btDID;
  /** Supported send-bit rate */
  uint8  btBS;
  /** Supported receive-bit rate */
  uint8  btBR;
  /** Timeout value */
  uint8  btTO;
  /** PP Parameters */
  uint8  btPP;
  /** General Bytes */
  uint8  abtGB[48];
  size_t  szGB;
  /** DEP mode */
  nfc_dep_mode ndm;
} nfc_dep_info;

/**
 * @struct nfc_iso14443a_info
 * @brief NFC ISO14443A tag (MIFARE) information
 */
typedef struct {
  uint8  abtAtqa[2];
  uint8  btSak;
  size_t  szUidLen;
  uint8  abtUid[10];
  size_t  szAtsLen;
  uint8  abtAts[254]; // Maximal theoretical ATS is FSD-2, FSD=256 for FSDI=8 in RATS
} nfc_iso14443a_info;

/**
 * @struct nfc_felica_info
 * @brief NFC FeLiCa tag information
 */
typedef struct {
  size_t  szLen;
  uint8  btResCode;
  uint8  abtId[8];
  uint8  abtPad[8];
  uint8  abtSysCode[2];
} nfc_felica_info;

/**
 * @struct nfc_iso14443b_info
 * @brief NFC ISO14443B tag information
 */
typedef struct {
  /** abtPupi store PUPI contained in ATQB (Answer To reQuest of type B) (see ISO14443-3) */
  uint8 abtPupi[4];
  /** abtApplicationData store Application Data contained in ATQB (see ISO14443-3) */
  uint8 abtApplicationData[4];
  /** abtProtocolInfo store Protocol Info contained in ATQB (see ISO14443-3) */
  uint8 abtProtocolInfo[3];
  /** ui8CardIdentifier store CID (Card Identifier) attributted by PCD to the PICC */
  uint8 ui8CardIdentifier;
} nfc_iso14443b_info;

/**
 * @struct nfc_iso14443bi_info
 * @brief NFC ISO14443B' tag information
 */
typedef struct {
  /** DIV: 4 LSBytes of tag serial number */
  uint8 abtDIV[4];
  /** Software version & type of REPGEN */
  uint8 btVerLog;
  /** Config Byte, present if long REPGEN */
  uint8 btConfig;
  /** ATR, if any */
  size_t szAtrLen;
  uint8  abtAtr[33];
} nfc_iso14443bi_info;

/**
 * @struct nfc_iso14443b2sr_info
 * @brief NFC ISO14443-2B ST SRx tag information
 */
typedef struct {
  uint8 abtUID[8];
} nfc_iso14443b2sr_info;

/**
 * @struct nfc_iso14443b2ct_info
 * @brief NFC ISO14443-2B ASK CTx tag information
 */
typedef struct {
  uint8 abtUID[4];
  uint8 btProdCode;
  uint8 btFabCode;
} nfc_iso14443b2ct_info;

/**
 * @struct nfc_jewel_info
 * @brief NFC Jewel tag information
 */
typedef struct {
  uint8  btSensRes[2];
  uint8  btId[4];
} nfc_jewel_info;

/**
 * @union nfc_target_info
 * @brief Union between all kind of tags information structures.
 */
typedef union {
  nfc_iso14443a_info nai;
  nfc_felica_info nfi;
  nfc_iso14443b_info nbi;
  nfc_iso14443bi_info nii;
  nfc_iso14443b2sr_info nsi;
  nfc_iso14443b2ct_info nci;
  nfc_jewel_info nji;
  nfc_dep_info ndi;
} nfc_target_info;

/**
 * @enum nfc_baud_rate
 * @brief NFC baud rate enumeration
 */
typedef enum {
  NBR_UNDEFINED = 0,
  NBR_106,
  NBR_212,
  NBR_424,
  NBR_847,
} nfc_baud_rate;

/**
 * @enum nfc_modulation_type
 * @brief NFC modulation type enumeration
 */
typedef enum {
  NMT_ISO14443A = 1,
  NMT_JEWEL,
  NMT_ISO14443B,
  NMT_ISO14443BI, // pre-ISO14443B aka ISO/IEC 14443 B' or Type B'
  NMT_ISO14443B2SR, // ISO14443-2B ST SRx
  NMT_ISO14443B2CT, // ISO14443-2B ASK CTx
  NMT_FELICA,
  NMT_DEP,
} nfc_modulation_type;

/**
 * @enum nfc_mode
 * @brief NFC mode type enumeration
 */
typedef enum {
  N_TARGET,
  N_INITIATOR,
} nfc_mode;

/**
 * @struct nfc_modulation
 * @brief NFC modulation structure
 */
typedef struct {
  nfc_modulation_type nmt;
  nfc_baud_rate nbr;
} nfc_modulation;

/**
 * @struct nfc_target
 * @brief NFC target structure
 */
typedef struct {
  nfc_target_info nti;
  nfc_modulation nm;
} nfc_target;

// Reset struct alignment to default
#  pragma pack()

#endif // _LIBNFC_TYPES_H_
