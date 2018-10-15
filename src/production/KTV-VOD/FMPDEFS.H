/*******************************************************************************
*
*   COPYRIGHT   :   (c) 1995-1996 Sigma Designs, Inc. All rights reserved.
*
*   FILENAME    :   FMPDefs.h
*
*   DESCRIPTION :   This header file contains FMP specific definitions.
*
*   PLATFORM    :   DOS, Windows
*
*   PROGRAMMER  :   ECY
*
*   COMMENTS    :   NONE
*
*   NOTES       :   NONE
*
*   CHANGES     :
*   08/10/95    -   Created.
*    (ECY)
*
*******************************************************************************/


#ifndef __FMPDEFS_H__
#define __FMPDEFS_H__


/*******************************************************************************
* FMP Commands
*******************************************************************************/
#define FMP_OPEN                        0x01
#define FMP_CLOSE                       0x02
#define FMP_PLAY                        0x03
#define FMP_PAUSE                       0x04
#define FMP_STOP                        0x05
#define FMP_SEEK                        0x06
#define FMP_STEP                        0x07
#define FMP_GROUP                       0x08
#define FMP_SET                         0x09
#define FMP_GET                         0x0A
#define FMP_CALLBACK                    0x0B
#define FMP_SIGNAL                      0x0C
#define FMP_UNLOAD                      0x0D
#define FMP_INIT                        0x0E
#define FMP_CAPTURE                     0x0F
#define FMP_UPDATE                      0x10
#define FMP_FREEZE                      0x12


/*******************************************************************************
* FMP Errors
*******************************************************************************/
#define FMPE_DOS                        0x0100
#define FMPE_INVALID_FLAGS              0x0200
#define FMPE_HANDLE                     0x0300
#define FMPE_NOT_IMPLEMENT              0x0400
#define FMPE_INVALID_CMD                0x0500
#define FMPE_OUT_OF_MEM                 0x0600
#define FMPE_INDEX                      0x0700
#define FMPE_TYPE                       0x0800
#define FMPE_WRITE                      0x0900
#define FMPE_TOO_MANY                   0x0A00
#define FMPE_ITEM_INDEX                 0x0B00
#define FMPE_ITEM_HANDLE                0x0C00
#define FMPE_ERROR                      0x0D00
#define FMPE_STREAM_DATA                0x0E00
#define FMPE_NOT_CDXA_DRV               0x0F00
#define FMPE_HARDWARE                   0x1000
#define FMPE_NA                         0x1100
#define FMPE_VALUE                      0x1200
#define FMPE_TIME_FMT                   0x1300
#define FMPE_NOT_READY                  0x1400
#define FMPE_POS			0x1500
#define FMPE_REFUSE			0x1600
#define FMPE_INVALID_INDEX		0x1700

/*******************************************************************************
* FMP Messages
*******************************************************************************/
#define FMPM_BUF_POS                    0x01
#define FMPM_BUF_EMPTY                  0x02
#define FMPM_BUF_SEEK                   0x03
#define FMPM_BUF_CREATE                 0x04
#define FMPM_BUF_CLOSE                  0x05
#define FMPM_BUF_TOTALSIZE              0x06
#define FMPM_COMPLETED                  0x07
#define FMPM_CANCELED                   0x08
#define FMPM_ERROR                      0x09
#define FMPM_MEM_ALLOC                  0x0A
#define FMPM_MEM_FREE                   0x0B
#define FMPM_SIGNAL                     0x0C
#define FMPM_FIFO_START			0x0D
#define FMPM_FIFO_LOW			0x0E
#define FMPM_FIFO_HIGH			0x0F
#define FMPM_FIFO_DROP			0x10
#define FMPM_PUSH_READ			0x11
#define FMPM_PUSH_SEEK			0x12
#define FMPM_PUSH_CREATE		0x13

/*******************************************************************************
* FMP Flags
*******************************************************************************/
#define FMPF_BUF_LOW                    0x00000000L
#define FMPF_BUF_EMS                    0x00010000L
#define FMPF_BUF_XMS                    0x00020000L

#define FMPF_PASCAL                     0x1000
#define FMPF_C                          0x2000

#define FMPF_TEST                       0x8000

#define FMPF_INSERT                     0x0001
#define FMPF_REMOVE                     0x0002
#define FMPF_GET                        0x0003
#define FMPF_SELECT                     0x0004
#define FMPF_UNSELECT                   0x0005
#define FMPF_HANDLE                     0x0000
#define FMPF_INDEX                      0x0020

#define FMPF_FILE                       0x0001
#define FMPF_BUFFERS                    0x0002
#define FMPF_GROUP                      0x0003
#define FMPF_BROADCAST					0x0004
#define FMPF_EXTEND                     0x0010
#define FMPF_NOACCESS                   0x0100

#define FMPF_UNKNOWN                    0x0000
#define FMPF_AUDIO                      0x0001
#define FMPF_VIDEO                      0x0002

#define FMPF_POS_START                  0x0100
#define FMPF_POS_SET                    0x0200
#define FMPF_POS_END                    0x0300
#define FMPF_POS_CUR                    0x0400

#define FMPF_DONT_UPDATE                0x1000
#define FMPF_UPDATE_ALL                 0x2000

#define FMPF_SIG_AT                     0x0001
#define FMPF_SIG_EVERY                  0x0002
#define FMPF_SIG_REMOVE                 0x0003
#define FMPF_SIG_REMOVE_AT              0x0004
#define FMPF_SIG_REMOVE_ALL             0x0005

#define FMPE_HRD                        0x2000
#define FMPE_HRD_NO_DMA                 0x2100
#define FMPE_HRD_NO_INT                 0x2200
#define FMPE_HRD_NO_PORT                0x2300
#define FMPE_HRD_NOT_FOUND              0x2400
#define FMPE_HRD_DONT_USE               0x2500

#define FMPF_END_STOP                   0x0000
#define FMPF_END_PAUSE                  0x0001
#define FMPF_END_KEEP                   0x0002
#define FMPF_END_REPEAT                 0x0004
#define FMPF_END_CLOSE                  0x0005

#define FMPF_PAUSED                     0x0001
#define FMPF_STOPPED                    0x0002
#define FMPF_PLAYING                    0x0004
#define FMPF_SEEKING                    0x0008
#define FMPF_STEPPING                   0x0010
#define FMPF_CLOSED                     0x0020
#define FMPF_READY                      (FMPF_PAUSED | FMPF_STOPPED)

#define FMPF_BYTES                      0x0001
#define FMPF_SAMPLES                    0x0002
#define FMPF_MSEC                       0x0003
#define FMPF_HMSF                       0x0004
#define FMPF_HMSC                       0x0005
#define FMPF_FRAMES                     FMPF_SAMPLES
#define FMPF_TIME                       FMPF_HMSC
#define FMPF_SMPTE                      FMPF_HMSF

#define FMPF_BUF_32_BITS                0x0001
#define FMPF_BUF_LOOP                   0x0002

#define FMPF_VID_UNKNOWN                0x0000
#define FMPF_VID_MPEG                   0x0001
#define FMPF_VID_AVI                    0x0002
#define FMPF_VID_MPEG2                  0x0004

#define FMPF_ALL_VGA                    0x0001
#define FMPF_ALL_VID                    0x0002
#define FMPF_KEY_VGA                    0x0004
#define FMPF_KEY_VID                    0x0008
#define FMPF_KEY_MIX                    0x000C
#define FMPF_KEY_CALIBRATE				0x000D

#define FMPF_AUD_UNKNOWN                0x0000
#define FMPF_AUD_MPEG                   0x0001
#define FMPF_AUD_MPEG_L1                0x0002
#define FMPF_AUD_MPEG_L2                0x0004
#define FMPF_AUD_MPEG_L3                0x0008
#define FMPF_AUD_MPEG2                  0x0010
#define FMPF_AUD_MPEG2_L1               0x0020
#define FMPF_AUD_MPEG2_L2               0x0040
#define FMPF_AUD_MPEG2_L3               0x0080
#define FMPF_AUD_DOLBY_AC3				0x0100

#define FMPF_AUD_WAVE                   0x0010
#define FMPF_AUD_VOC                    0x0020
#define FMPF_AUD_PCM                    0x0040
#define FMPF_AUD_ADPCM                  0x0080

#define FMPF_AUD_STEREO                 0x0001
#define FMPF_AUD_JSTEREO                0x0002
#define FMPF_AUD_DUAL                   0x0003
#define FMPF_AUD_SINGLE                 0x0004
#define FMPF_AUD_11		                0x0000
#define FMPF_AUD_10                     0x0001
#define FMPF_AUD_20                     0x0002
#define FMPF_AUD_30                     0x0003
#define FMPF_AUD_21                     0x0004
#define FMPF_AUD_31                     0x0005
#define FMPF_AUD_22                     0x0006
#define FMPF_AUD_32                     0x0007

#define FMPF_AUD_NO_EMPH                0x0001
#define FMPF_AUD_EMPH_50                0x0002
#define FMPF_AUD_EMPH_J17               0x0003

#define FMPF_AUD_COPYRIGHT              0x0001
#define FMPF_AUD_ORIGINAL               0x0002

#define FMPF_GRP_UNKNOWN                0x0000
#define FMPF_GRP_MPEG                   0x0001
#define FMPF_GRP_AVI                    0x0002
#define FMPF_GRP_CUSTOM                 0x0004
#define FMPF_GRP_MPEG2_PROGRAM          0x0008
#define FMPF_GRP_MPEG2_TRANSPORT        0x0010

#define FMPF_PIC_BMP                    0x0001
#define FMPF_PIC_DIB24                  0x0002
#define FMPF_PIC_TOFILE                 0x0004

#define FMPF_REFUSE						0x0001
#define FMPF_IGNORE						0x0002


/*******************************************************************************
* FMP Index
*******************************************************************************/
#define FMPI_DRV_PRODUCT                0x0101
#define FMPI_DRV_VERSION                0x0102
#define FMPI_DRV_MAX_CHAN               0x0103
#define FMPI_DRV_VID_SUP                0x0104
#define FMPI_DRV_AUD_SUP                0x0105
#define FMPI_DRV_GRP_SUP                0x0106
#define FMPI_DRV_HDR_STAT               0x0107
#define FMPI_DRV_MEMORY                 0x0108
#define FMPI_DRV_ID						0x0109
#define FMPI_DRV_CAPABILITY				0x010A

#define FMPI_STM_TYPE                   0x0202
#define FMPI_STM_SOURCE                 0x0203
#define FMPI_STM_MODE                   0x0204
#define FMPI_STM_TIME_FMT               0x0205
#define FMPI_STM_POSITION               0x0206
#define FMPI_STM_SPEED                  0x0207
#define FMPI_STM_USER                   0x0208
#define FMPI_STM_SIZE                   0x0209
#define FMPI_STM_SLIDE                  0x020A
#define FMPI_STM_PASSWD                 0x0210
#define FMPI_STM_FILETYPE               0x0211
#define FMPI_STM_MEMFLAGS               0x0212
#define FMPI_STM_FILESIZE               0x0213
#define FMPI_STM_FILEORG                0x0214
#define FMPI_STM_BIT_RATE				0x0215
#define FMPI_STM_NOCACHE				0x0216

#define FMPI_BUF_LEFT                   0x0301
#define FMPI_BUF_POS                    0x0302
#define FMPI_BUF_ADDRESS                0x0303
#define FMPI_BUF_SIZE                   0x0304
#define FMPI_BUF_MODE                   0x0305
#define FMPI_BUF_TOTALSIZE              0x0306

#define FMPI_VID_TYPE                   0x0401
#define FMPI_VID_RATE                   0x0402
#define FMPI_VID_SIZE                   0x0403
#define FMPI_VID_ASPECT                 0x0404
#define FMPI_VID_BIT_RATE               0x0405
#define FMPI_VID_SRC_POS                0x0406
#define FMPI_VID_SRC_SIZE               0x0407
#define FMPI_VID_DEST_POS               0x0408
#define FMPI_VID_DEST_SIZE              0x0409
#define FMPI_VID_KEY_MIN                0x040A
#define FMPI_VID_KEY_MAX                0x040B
#define FMPI_VID_KEY_MASK               0x040C
#define FMPI_VID_KEY_COL                0x040D
#define FMPI_VID_KEY_MODE               0x040E
#define FMPI_VID_KEY_TYPE               0x040F
#define FMPI_VID_CONTRAST				0x0410
#define FMPI_VID_BRIGHTNESS				0x0411
#define FMPI_VID_SATURATION				0x0412
#define FMPI_VID_SLIDE					0x0420
#define	FMPI_VID_HWND					0x0413
#define FMPI_VID_KEY_COL_INDEX          0x0414

#define FMPI_VID_DEFAULTS				0x1000
#define FMPI_VID_REGISTRY				0x2000

#define FMPI_AUD_TYPE                   0x0501
#define FMPI_AUD_RATE                   0x0502
#define FMPI_AUD_VOLUME                 0x0503
#define FMPI_AUD_BIT_RATE               0x0504
#define FMPI_AUD_TREBLE                 0x0505
#define FMPI_AUD_BASS                   0x0506
#define FMPI_AUD_CHANNELS               0x0507
#define FMPI_AUD_EMPH                   0x0508
#define FMPI_AUD_RIGHTS                 0x0509
#define FMPI_AUD_SHIFT                  0x0510
#define FMPI_AUD_BAL_L                  0x0511
#define FMPI_AUD_BAL_R                  0x0512

#define FMPI_AUD_DEFAULTS				0x1000
#define FMPI_AUD_REGISTRY				0x2000

#define FMPI_GRP_TYPE                   0x0601
#define FMPI_GRP_NB                     0x0602

#define FMPI_FIFO_FCNTRL				0x0701
#define FMPI_FIFO_SIZE					0x0702
#define FMPI_FIFO_MAX					0x0703
#define FMPI_FIFO_VID_SIZE				0x0704
#define FMPI_FIFO_AUD_SIZE				0x0705
#define FMPI_FIFO_START					0x0706
#define FMPI_FIFO_LOW					0x0707
#define FMPI_FIFO_HIGH					0x0708
#define FMPI_FIFO_DROP					0x0709
#define FMPI_FIFO_UNIT					0x070A

#define FMPI_PUSH_ASYNC					0x0801
#define FMPI_PUSH_SYNC					0x0802

#define FMPI_OVLY_XOFFSET				0x0901
#define FMPI_OVLY_YOFFSET				0x0902
#define FMPI_OVLY_VGACORRECTION			0x0903
#define FMPI_OVLY_STABILITY				0x0904
#define FMPI_OVLY_HFREQUENCY			0x0905
#define FMPI_OVLY_USEDEFAULTFREQ		0x0906
#define FMPI_OVLY_COLOR_SETTING			0x0907
#define FMPI_OVLY_LOWERLEVEL			0x0908
#define FMPI_OVLY_UPPERLEVEL			0x0909
#define FMPI_OVLY_DELTA_CALIBRATE		0x090A
#define FMPI_OVLY_COLOR_CALIBRATION		0x090B

#define FMPI_OVLY_DEFAULTS				0x1000
#define FMPI_OVLY_REGISTRY				0x2000

#define FMPI_UPD_PALETTE                0x0001
#define FMPI_UPD_VGA_MODE               0x0007
#define FMPI_UPD_FRAME_POS              0x0008
#define FMPI_UPD_FRAME_SIZE             0x0009

/*******************************************************************************
* FMP Constants
*******************************************************************************/

#define FMP_MIN_AUD_VOLUME 0
#define FMP_MAX_AUD_VOLUME 100
#define FMP_DEF_AUD_VOLUME 50

#define FMP_MIN_BSC 0			// Brightness, Saturation & Contrast
#define FMP_MAX_BSC 1000
#define FMP_DEF_BSC 500

// REALmagic capabilities
#define FMP_CAPABILITY_BORDER_ADJUST	0x0001
#define FMP_CAPABILITY_KEYCOLOR_ADJUST	0x0002
#define FMP_CAPABILITY_BSC_ADJUST		0x0004	// Brightness, Saturation & contrast

// RGB values used for min. medium and max color calibration
#define FMP_MIN_COLOR_LEVEL				0x00000000
#define FMP_MEDIUM_COLOR_LEVEL			0x00808080
#define FMP_MAX_COLOR_LEVEL				0x00FFFFFF
#define FMP_MEDIUM_COLOR				0x80

#define FMP_LOWER_RED					0x01
#define FMP_LOWER_GREEN					0x02
#define FMP_LOWER_BLUE					0x04
#define FMP_UPPER_RED					0x10
#define FMP_UPPER_GREEN					0x20
#define FMP_UPPER_BLUE					0x40

/*******************************************************************************
* FMP Structures
*******************************************************************************/

typedef struct {
	LPTSTR lpFileName;
	DWORD dwCallBack;
	BYTE Reserved[8];
} FMP_OPEN_STRUCT;

typedef struct {
	DWORD dwSize;
	void* lpBuffer;
} FMP_PUSH_STRUCT;

typedef struct {
	COLORREF RGBColorKey;
	COLORREF RGBUpper;
	COLORREF RGBLower;
	USHORT Mask;
} FMP_COLOR_SETTING_STRUCT;

#endif  /* __FMPDEFS_H__ */
