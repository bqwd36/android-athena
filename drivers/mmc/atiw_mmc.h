#ifndef __ATIW_MMC_H
#define __ATIW_MMC_H

#define DRIVER_NAME	"atiw_mmc"

#ifdef CONFIG_MMC_DEBUG
#define DBG(x...)       printk(DRIVER_NAME ": " x)
#else
#define DBG(x...)       do { } while (0)
#endif

#define ATIW_SDIO_Base	0x2000

#define SD_CTRL_SOFTWARE_RESET_CLEAR            (1<<0)

#define SD_CTRL_TRANSACTIONCONTROL_SET          (1<<8) // 0x0100

#define SD_CTRL_CARDCLOCKCONTROL_ENABLE_CLOCK   (1<<8) // 0x0100
#define SD_CTRL_CARDCLOCKCONTROL_CLK_DIV_512    (1<<7) // 0x0080
#define SD_CTRL_CARDCLOCKCONTROL_CLK_DIV_256    (1<<6) // 0x0040
#define SD_CTRL_CARDCLOCKCONTROL_CLK_DIV_128    (1<<5) // 0x0020
#define SD_CTRL_CARDCLOCKCONTROL_CLK_DIV_64     (1<<4) // 0x0010
#define SD_CTRL_CARDCLOCKCONTROL_CLK_DIV_32     (1<<3) // 0x0008
#define SD_CTRL_CARDCLOCKCONTROL_CLK_DIV_16     (1<<2) // 0x0004
#define SD_CTRL_CARDCLOCKCONTROL_CLK_DIV_8      (1<<1) // 0x0002
#define SD_CTRL_CARDCLOCKCONTROL_CLK_DIV_4      (1<<0) // 0x0001
#define SD_CTRL_CARDCLOCKCONTROL_CLK_DIV_2      (0<<0) // 0x0000

#define MEM_CARD_OPTION_REQUIRED                   0x000e
#define MEM_CARD_OPTION_DATA_RESPONSE_TIMEOUT(x)   (((x)&0x0f)<<4)      /* Four bits */
#define MEM_CARD_OPTION_C2_MODULE_NOT_PRESENT      (1<<14) // 0x4000
#define MEM_CARD_OPTION_DATA_XFR_WIDTH_1           (1<<15) // 0x8000
#define MEM_CARD_OPTION_DATA_XFR_WIDTH_4           (0<<15) //~0x8000

#define SD_CTRL_COMMAND_INDEX(x)                   ((x)&0x3f)           /* 0=CMD0, 1=CMD1, ..., 63=CMD63 */
#define SD_CTRL_COMMAND_TYPE_CMD                   (0 << 6)
#define SD_CTRL_COMMAND_TYPE_ACMD                  (1 << 6) // 0x0040
#define SD_CTRL_COMMAND_TYPE_AUTHENTICATION        (2 << 6) // 0x0080
#define SD_CTRL_COMMAND_RESPONSE_TYPE_NORMAL       (0 << 8) //~0x0100
#define SD_CTRL_COMMAND_RESPONSE_TYPE_IDLE3	   (3 << 8)
#define SD_CTRL_COMMAND_RESPONSE_TYPE_EXT_R1       (4 << 8)
#define SD_CTRL_COMMAND_RESPONSE_TYPE_EXT_R1B      (5 << 8)
#define SD_CTRL_COMMAND_RESPONSE_TYPE_EXT_R2       (6 << 8)
#define SD_CTRL_COMMAND_RESPONSE_TYPE_EXT_R3       (7 << 8)
#define SD_CTRL_COMMAND_DATA_PRESENT               (1 << 11) // 0x0800
#define SD_CTRL_COMMAND_TRANSFER_READ              (1 << 12) // 0x1000
#define SD_CTRL_COMMAND_TRANSFER_WRITE             (0 << 12) //~0x1000
#define SD_CTRL_COMMAND_MULTI_BLOCK                (1 << 13) // 0x2000
#define SD_CTRL_COMMAND_SECURITY_CMD               (1 << 14) // 0x4000

#define SD_CTRL_STOP_INTERNAL_ISSSUE_CMD12         (1 << 0)
#define SD_CTRL_STOP_INTERNAL_AUTO_ISSUE_CMD12     (1 << 8)

#define SD_CTRL_CARDSTATUS_RESPONSE_END            (1 << 0) // 0x0001
#define SD_CTRL_CARDSTATUS_RW_END                  (1 << 2) // 0x0004
#define SD_CTRL_CARDSTATUS_CARD_REMOVED_0          (1 << 3) // 0x0008
#define SD_CTRL_CARDSTATUS_CARD_INSERTED_0         (1 << 4) // 0x0010
#define SD_CTRL_CARDSTATUS_SIGNAL_STATE_PRESENT_0  (1 << 5) // 0x0020
#define SD_CTRL_CARDSTATUS_WRITE_PROTECT           (1 << 7) // 0x0080
#define SD_CTRL_CARDSTATUS_CARD_REMOVED_3          (1 << 8) // 0x0100
#define SD_CTRL_CARDSTATUS_CARD_INSERTED_3         (1 << 9) // 0x0200
#define SD_CTRL_CARDSTATUS_SIGNAL_STATE_PRESENT_3  (1 << 10)// 0x0400

#define SD_CTRL_BUFFERSTATUS_CMD_INDEX_ERROR       (1 << 0) // 0x0001
#define SD_CTRL_BUFFERSTATUS_CRC_ERROR             (1 << 1) // 0x0002
#define SD_CTRL_BUFFERSTATUS_STOP_BIT_END_ERROR    (1 << 2) // 0x0004
#define SD_CTRL_BUFFERSTATUS_DATA_TIMEOUT          (1 << 3) // 0x0008
#define SD_CTRL_BUFFERSTATUS_BUFFER_OVERFLOW       (1 << 4) // 0x0010
#define SD_CTRL_BUFFERSTATUS_BUFFER_UNDERFLOW      (1 << 5) // 0x0020
#define SD_CTRL_BUFFERSTATUS_CMD_TIMEOUT           (1 << 6) // 0x0040
#define SD_CTRL_BUFFERSTATUS_UNK7                  (1 << 7) // 0x0080
#define SD_CTRL_BUFFERSTATUS_BUFFER_READ_ENABLE    (1 << 8) // 0x0100
#define SD_CTRL_BUFFERSTATUS_BUFFER_WRITE_ENABLE   (1 << 9) // 0x0200
#define SD_CTRL_BUFFERSTATUS_ILLEGAL_FUNCTION      (1 << 13)// 0x2000
#define SD_CTRL_BUFFERSTATUS_CMD_BUSY              (1 << 14)// 0x4000
#define SD_CTRL_BUFFERSTATUS_ILLEGAL_ACCESS        (1 << 15)// 0x8000

#define SD_CTRL_INTMASKCARD_RESPONSE_END           (1 << 0) // 0x0001
#define SD_CTRL_INTMASKCARD_RW_END                 (1 << 2) // 0x0004
#define SD_CTRL_INTMASKCARD_CARD_REMOVED_0         (1 << 3) // 0x0008
#define SD_CTRL_INTMASKCARD_CARD_INSERTED_0        (1 << 4) // 0x0010
#define SD_CTRL_INTMASKCARD_SIGNAL_STATE_PRESENT_0 (1 << 5) // 0x0020
#define SD_CTRL_INTMASKCARD_UNK6                   (1 << 6) // 0x0040
#define SD_CTRL_INTMASKCARD_WRITE_PROTECT          (1 << 7) // 0x0080
#define SD_CTRL_INTMASKCARD_CARD_REMOVED_3         (1 << 8) // 0x0100
#define SD_CTRL_INTMASKCARD_CARD_INSERTED_3        (1 << 9) // 0x0200
#define SD_CTRL_INTMASKCARD_SIGNAL_STATE_PRESENT_3 (1 << 10)// 0x0400

#define SD_CTRL_INTMASKBUFFER_CMD_INDEX_ERROR      (1 << 0) // 0x0001
#define SD_CTRL_INTMASKBUFFER_CRC_ERROR            (1 << 1) // 0x0002
#define SD_CTRL_INTMASKBUFFER_STOP_BIT_END_ERROR   (1 << 2) // 0x0004
#define SD_CTRL_INTMASKBUFFER_DATA_TIMEOUT         (1 << 3) // 0x0008
#define SD_CTRL_INTMASKBUFFER_BUFFER_OVERFLOW      (1 << 4) // 0x0010
#define SD_CTRL_INTMASKBUFFER_BUFFER_UNDERFLOW     (1 << 5) // 0x0020
#define SD_CTRL_INTMASKBUFFER_CMD_TIMEOUT          (1 << 6) // 0x0040
#define SD_CTRL_INTMASKBUFFER_UNK7                 (1 << 7) // 0x0080
#define SD_CTRL_INTMASKBUFFER_BUFFER_READ_ENABLE   (1 << 8) // 0x0100
#define SD_CTRL_INTMASKBUFFER_BUFFER_WRITE_ENABLE  (1 << 9) // 0x0200
#define SD_CTRL_INTMASKBUFFER_ILLEGAL_FUNCTION     (1 << 13)// 0x2000
#define SD_CTRL_INTMASKBUFFER_CMD_BUSY             (1 << 14)// 0x4000
#define SD_CTRL_INTMASKBUFFER_ILLEGAL_ACCESS       (1 << 15)// 0x8000

#define SD_CTRL_DETAIL0_RESPONSE_CMD_ERROR                   (1 << 0) // 0x0001
#define SD_CTRL_DETAIL0_END_BIT_ERROR_FOR_RESPONSE_NON_CMD12 (1 << 2) // 0x0004
#define SD_CTRL_DETAIL0_END_BIT_ERROR_FOR_RESPONSE_CMD12     (1 << 3) // 0x0008
#define SD_CTRL_DETAIL0_END_BIT_ERROR_FOR_READ_DATA          (1 << 4) // 0x0010
#define SD_CTRL_DETAIL0_END_BIT_ERROR_FOR_WRITE_CRC_STATUS   (1 << 5) // 0x0020
#define SD_CTRL_DETAIL0_CRC_ERROR_FOR_RESPONSE_NON_CMD12     (1 << 8) // 0x0100
#define SD_CTRL_DETAIL0_CRC_ERROR_FOR_RESPONSE_CMD12         (1 << 9) // 0x0200
#define SD_CTRL_DETAIL0_CRC_ERROR_FOR_READ_DATA              (1 << 10)// 0x0400
#define SD_CTRL_DETAIL0_CRC_ERROR_FOR_WRITE_CMD              (1 << 11)// 0x0800

#define SD_CTRL_DETAIL1_NO_CMD_RESPONSE                      (1 << 0) // 0x0001
#define SD_CTRL_DETAIL1_TIMEOUT_READ_DATA                    (1 << 4) // 0x0010
#define SD_CTRL_DETAIL1_TIMEOUT_CRS_STATUS                   (1 << 5) // 0x0020
#define SD_CTRL_DETAIL1_TIMEOUT_CRC_BUSY                     (1 << 6) // 0x0040

#define ATIW_SDIO_CTRL_Cmd                  0x00
#define ATIW_SDIO_CTRL_CardPortSel          0x04
#define ATIW_SDIO_CTRL_Arg0                 0x08
#define ATIW_SDIO_CTRL_Arg1                 0x0C
#define ATIW_SDIO_CTRL_StopInternal         0x10
#define ATIW_SDIO_CTRL_TransferBlockCount   0x14
#define ATIW_SDIO_CTRL_Response0            0x18
#define ATIW_SDIO_CTRL_Response1            0x1C
#define ATIW_SDIO_CTRL_Response2            0x20
#define ATIW_SDIO_CTRL_Response3            0x24
#define ATIW_SDIO_CTRL_Response4            0x28
#define ATIW_SDIO_CTRL_Response5            0x2C
#define ATIW_SDIO_CTRL_Response6            0x30
#define ATIW_SDIO_CTRL_Response7            0x34
#define ATIW_SDIO_CTRL_CardStatus           0x38
#define ATIW_SDIO_CTRL_BufferCtrl           0x3C
#define ATIW_SDIO_CTRL_IntMaskCard          0x40
#define ATIW_SDIO_CTRL_IntMaskBuffer        0x44
#define ATIW_SDIO_CTRL_CardClockCtrl        0x48
#define ATIW_SDIO_CTRL_CardXferDataLen      0x4C
#define ATIW_SDIO_CTRL_CardOptionSetup      0x50
#define ATIW_SDIO_CTRL_ErrorStatus0         0x54
#define ATIW_SDIO_CTRL_ErrorStatus1         0x58
#define ATIW_SDIO_CTRL_DataPort             0x60
#define ATIW_SDIO_CTRL_TransactionCtrl      0x68
#define ATIW_SDIO_CTRL_CardIntCtrl          0x6C
#define ATIW_SDIO_CTRL_ClocknWaitCtrl       0x70
#define ATIW_SDIO_CTRL_HostInformation      0x74
#define ATIW_SDIO_CTRL_ErrorCtrl            0x78
#define ATIW_SDIO_CTRL_LEDCtrl              0x7C
#define ATIW_SDIO_CTRL_SoftwareReset        0x1C0

#define DONT_CARE_CARD_BITS ( \
      SD_CTRL_INTMASKCARD_SIGNAL_STATE_PRESENT_3 \
    | SD_CTRL_INTMASKCARD_WRITE_PROTECT \
    | SD_CTRL_INTMASKCARD_UNK6 \
    | SD_CTRL_INTMASKCARD_SIGNAL_STATE_PRESENT_0 \
  )
#define DONT_CARE_BUFFER_BITS ( SD_CTRL_INTMASKBUFFER_UNK7 | SD_CTRL_INTMASKBUFFER_CMD_BUSY )

#endif // __ATIW_MMC_H
