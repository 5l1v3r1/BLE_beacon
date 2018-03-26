/** \mainpage Driver Overview
  *
  * \section section_drv_info Driver Information
  * This Sensor Controller Interface driver has been generated by the Texas Instruments Sensor Controller
  * Studio tool:
  * - <b>Project name</b>:     uartEmulate
  * - <b>Project file</b>:     C:/Users/Darren Sun/Documents/Texas Instruments/Sensor Controller Studio/projects/uartEmulate.scp
  * - <b>Code prefix</b>:      -
  * - <b>Operating system</b>: TI-RTOS
  * - <b>Tool version</b>:     1.5.0.188
  * - <b>Tool patches</b>:     None
  * - <b>Target chip</b>:      CC2640, revision -, package QFN32 4x4 RSM
  * - <b>Created</b>:          2018-03-18 11:16:04.123
  * - <b>Computer</b>:         DESKTOP-4FB9335
  * - <b>User</b>:             Darren Sun
  *
  * No user-provided resource definitions were used to generate this driver.
  *
  * No user-provided procedure definitions were used to generate this driver.
  *
  * Do not edit the generated source code files other than temporarily for debug purposes. Any
  * modifications will be overwritten by the Sensor Controller Studio when generating new output.
  *
  * \section section_drv_modules Driver Modules
  * The driver is divided into three modules:
  * - \ref module_scif_generic_interface, providing the API for:
  *     - Initializing and uninitializing the driver
  *     - Task control (for starting, stopping and executing Sensor Controller tasks)
  *     - Task data exchange (for producing input data to and consume output data from Sensor Controller
  *       tasks)
  * - \ref module_scif_driver_setup, containing:
  *     - The AUX RAM image (Sensor Controller code and data)
  *     - I/O mapping information
  *     - Task data structure information
  *     - Driver setup data, to be used in the driver initialization
  *     - Project-specific functionality
  * - \ref module_scif_osal, for flexible OS support:
  *     - Interfaces with the selected operating system
  *
  * It is possible to use output from multiple Sensor Controller Studio projects in one application. Only
  * one driver setup may be active at a time, but it is possible to switch between these setups. When
  * using this option, there is one instance of the \ref module_scif_generic_interface and
  * \ref module_scif_osal modules, and multiple instances of the \ref module_scif_driver_setup module.
  * This requires that:
  * - The outputs must be generated using the same version of Sensor Controller Studio
  * - The outputs must use the same operating system
  * - The outputs must use different source code prefixes (inserted into all globals of the
  *   \ref module_scif_driver_setup)
  *
  *
  * \section section_project_info Project Description
  * uartEmulate for beacon project.
  *
  *
  * \subsection section_io_mapping I/O Mapping
  * Task I/O functions are mapped to the following pins:
  * - uartEmulator:
  *     - <b>UART RX</b>: DIO9
  *     - <b>UART TX</b>: DIO8
  *
  *
  * \section section_task_info Task Description(s)
  * This driver supports the following task(s):
  *
  *
  * \subsection section_task_desc_uart_emulator uartEmulator
  * No description entered
  *
  */




/** \addtogroup module_scif_driver_setup Driver Setup
  *
  * \section section_driver_setup_overview Overview
  *
  * This driver setup instance has been generated for:
  * - <b>Project name</b>:     uartEmulate
  * - <b>Code prefix</b>:      -
  *
  * The driver setup module contains the generated output from the Sensor Controller Studio project:
  * - Location of task control and scheduling data structures in AUX RAM
  * - The AUX RAM image, and the size the image
  * - Task data structure information (location, size and buffer count)
  * - I/O pin mapping translation table
  * - Task resource initialization and uninitialization functions
  * - Hooks for run-time logging
  *
  * @{
  */
#ifndef SCIF_H
#define SCIF_H

#include <stdint.h>
#include <stdbool.h>
#include "scif_framework.h"
#include "scif_osal_tirtos.h"


/// Target chip name
#define SCIF_TARGET_CHIP_NAME_CC2640
/// Target chip package
#define SCIF_TARGET_CHIP_PACKAGE_QFN32_4X4_RSM

/// Number of tasks implemented by this driver
#define SCIF_TASK_COUNT 1

/// uartEmulator: Task ID
#define SCIF_UART_EMULATOR_TASK_ID 0


/// uartEmulator I/O mapping: UART RX
#define SCIF_UART_EMULATOR_DIO_UART_RX 9
/// uartEmulator I/O mapping: UART TX
#define SCIF_UART_EMULATOR_DIO_UART_TX 8


// All shared data structures in AUX RAM need to be packed
#pragma pack(push, 2)


/// uartEmulator: Task configuration structure
typedef struct {
    uint16_t alertMask;            ///< Bit-vector selecting which UART events trigger ALERT interrupt
    uint16_t alertRxFifoThr;       ///< Number of bytes (or higher) in the RX FIFO that triggers ALERT interrupt
    uint16_t alertTxFifoThr;       ///< Number of bytes (or lower) in the TX FIFO that triggers ALERT interrupt
    uint16_t rxByteTimeout;        ///< Maximum number of idle half bit-periods after a received byte before indicating timeout
    uint16_t rxEnableReqIdleCount; ///< Required number of idle half-bits before enabling RX.
} SCIF_UART_EMULATOR_CFG_T;


/// uartEmulator: Task input data structure
typedef struct {
    uint16_t pTxBuffer[64]; ///< TX FIFO ring buffer
} SCIF_UART_EMULATOR_INPUT_T;


/// uartEmulator: Task output data structure
typedef struct {
    uint16_t pRxBuffer[64]; ///< RX FIFO ring buffer
} SCIF_UART_EMULATOR_OUTPUT_T;


/// uartEmulator: Task state structure
typedef struct {
    uint16_t alertBacklog; ///< Bit-vector of events not yet communicated to the application
    uint16_t alertEvents;  ///< Bit-vector of events communicated to the application in the last ALERT interrupt
    uint16_t alertMask;    ///< Currently used ALERT interrupt mask
    uint16_t exit;         ///< Set to exit the UART emulator
    uint16_t rxEnable;     ///< Set to enable RX, or clear to disable RX (controls whether start bits are detected)
    uint16_t rxEnabled;    ///< Is RX currently enabled?
    uint16_t rxHead;       ///< RX FIFO head index (updated by the Sensor Controller)
    uint16_t rxTail;       ///< RX FIFO tail index (updated by the application)
    uint16_t txHead;       ///< TX FIFO head index (updated by the application)
    uint16_t txTail;       ///< TX FIFO tail index (updated by the Sensor Controller)
} SCIF_UART_EMULATOR_STATE_T;


/// Sensor Controller task data (configuration, input buffer(s), output buffer(s) and internal state)
typedef struct {
    struct {
        SCIF_UART_EMULATOR_CFG_T cfg;
        SCIF_UART_EMULATOR_INPUT_T input;
        SCIF_UART_EMULATOR_OUTPUT_T output;
        SCIF_UART_EMULATOR_STATE_T state;
    } uartEmulator;
} SCIF_TASK_DATA_T;

/// Sensor Controller task generic control (located in AUX RAM)
#define scifTaskData    (*((volatile SCIF_TASK_DATA_T*) 0x400E00EA))


// Initialized internal driver data, to be used in the call to \ref scifInit()
extern const SCIF_DATA_T scifDriverSetup;


// Restore previous struct packing setting
#pragma pack(pop)


// AUX I/O re-initialization functions
void scifReinitTaskIo(uint32_t bvTaskIds);


/// Maximum number of characters that can be stored in the UART TX FIFO
#define SCIF_UART_TX_FIFO_MAX_COUNT                  63
/// Maximum number of characters that can be stored in the UART RX FIFO
#define SCIF_UART_RX_FIFO_MAX_COUNT                  63

/// RX character flag: RX timeout occurred after receiving this character
#define BV_SCIF_UART_RX_TIMEOUT                      0x0800
/// RX character flag: RX FIFO ran full when receiving this character (this character may be invalid)
#define BV_SCIF_UART_RX_OVERFLOW                     0x0400
/// RX character flag: Break occurred when receiving this character
#define BV_SCIF_UART_RX_BREAK                        0x0200
/// RX character flag: Framing error occurred when receiving this character
#define BV_SCIF_UART_RX_FRAMING_ERROR                0x0100

/// UART ALERT event: RX FIFO at or above the specified threshold
#define BV_SCIF_UART_ALERT_RX_FIFO_ABOVE_THR         0x0001
/// UART ALERT event: RX byte timeout occurred
#define BV_SCIF_UART_ALERT_RX_BYTE_TIMEOUT           0x0002
/// UART ALERT event: Break or framing error occurred
#define BV_SCIF_UART_ALERT_RX_BREAK_OR_FRAMING_ERROR 0x0004
/// UART ALERT event: TX FIFO at or below the specified threshold
#define BV_SCIF_UART_ALERT_TX_FIFO_BELOW_THR         0x0008


// UART control functions
void scifUartSetRxTimeout(uint16_t rxTimeout);
void scifUartSetRxEnableReqIdleCount(uint16_t rxEnableReqIdleCount);
void scifUartRxEnable(uint16_t rxEnable);
void scifUartStopEmulator(void);
void scifUartSetBaudRate(uint32_t baudRate);

// UART RX FIFO access functions
void scifUartSetRxFifoThr(uint16_t threshold);
uint32_t scifUartGetRxFifoCount(void);
uint16_t scifUartRxGetChar(void);
void scifUartRxGetChars(char* pBuffer, uint32_t count);
void scifUartRxGetCharsWithFlags(uint16_t* pBuffer, uint32_t count);

// UART TX FIFO access functions
void scifUartSetTxFifoThr(uint16_t threshold);
uint32_t scifUartGetTxFifoCount(void);
void scifUartTxPutChar(char c);
void scifUartTxPutCharDelayed(char c, uint8_t delay);
void scifUartTxPutChars(char* pBuffer, uint32_t count);

// UART event/interrupt functions
uint16_t scifUartGetEvents(void);
void scifUartClearEvents(void);
void scifUartSetEventMask(uint16_t mask);


#endif
//@}


// Generated by DESKTOP-4FB9335 at 2018-03-18 11:16:04.123
