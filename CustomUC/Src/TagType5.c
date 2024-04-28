/**
  ******************************************************************************
  * @file    TagType5.c
  * @author  System Research & Applications Team - Catania & Agrate Lab.
  * @version 1.0.0
  * @brief   Minimal NDEF header APIs implementation
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "SmartNFC.h"
#include "TagType5.h"


#if USE_MEMORY

/* Local defines -------------------------------------------------------------*/
#define SMARTNFC_PRINTF(...)            PRINTF_VERBOSE(__VA_ARGS__)

/* st.com:smartag == 14 bytes */
#define SMART_TYPE_STRING_LENGTH  14
  
/* We need to save the TLV termination byte */
#define SMARTNFC_END_ADDR_COMPACT_DATA   (STSMART_NFC_MAX_SIZE - 4)



/** @brief Memory size value indicating that this is a 8-bytes Capability Container */
#define NFCT5_EXTENDED_CCFILE             0x00
/** @brief Capability container version 1.0 */
#define NFCT5_VERSION_V1_0                0x40
/** @brief Read access condition mask for the Capability Container byte1 */
#define NFCT5_READ_ACCESS                 0x0C
/** @brief Write access condition mask for the Capability Container byte1 */
#define NFCT5_WRITE_ACCESS                0x03

/** @brief Type5 Tag NDEF message TLV-Type. */
#define NFCT5_NDEF_MSG_TLV                ((uint8_t) 0x03)
/** @brief Type5 Tag Proprietary message TLV-Type. */
#define NFCT5_PROPRIETARY_TLV             ((uint8_t) 0xFD)
/** @brief Type5 Tag Terminator TLV-Type. */
#define NFCT5_TERMINATOR_TLV              ((uint8_t) 0xFE)
/** @brief TLV-Length indicating a 4-bytes TLV (Length coded on 2 bytes). */
#define NFCT5_3_BYTES_L_TLV               ((uint8_t) 0xFF)


/* Error codes for Higher level */
#define NDEF_OK                     0
#define NDEF_ERROR                  1
#define NDEF_ERROR_NOT_FORMATED     2

/* Max PayLoad Size == NFC TAG Size - ST25DV_CC_SIZE
                                    - ST25DV_TL_SIZE
                                    - ST25DV_NDEF1_SIZE
                                    - ST25DV_NDEF2_SIZE
                                    - ST25DV_NDEF3_HEADER_SIZE
* And because we write 8 bytes for each sample... it must be a multiply of 8!
*/
#define SMARTNFC_MAX_PAYLOAD_SIZE (((STSMART_NFC_MAX_SIZE -ST25DV_CC_SIZE - ST25DV_TL_SIZE - ST25DV_NDEF3_HEADER_SIZE - ST25DV_NDEF1_SIZE - ST25DV_NDEF2_SIZE)>>3)<<3)

/* Local types ---------------------------------------------------------------*/

/**
  * @brief  Tag Type 5 State enumeration definition.
  */
typedef enum
{
  TT5_NO_NDEF = 0,  /**< No data detected in the tag. */
  TT5_INITIALIZED,  /**< Capability container detected. */
  TT5_READ_WRITE,   /**< Read-Write data detected. */
  TT5_READ          /**< Read-Only data message detected. */
} TT5_State;

/** @brief Type5 Tag Capability Container Magic numbers as defined by the NFC Forum. */
typedef enum {
  NFCT5_MAGICNUMBER_E1_CCFILE = 0xE1, /**<  Complete data area can be read by 1-byte block adrdess commands. */
  NFCT5_MAGICNUMBER_E2_CCFILE = 0xE2  /**<  Last part of the data area can be only read by 2-bytes block address commands.\n
                                            The first 256 blocks can be read by 1-byte block address commands. */
} TT5_MagicNumber_t;

/**
  * @brief  Type5 Tag Capability Container structure.
  */
typedef struct
{
  TT5_MagicNumber_t MagicNumber;  /**< CCfile[0]: Magic Number should be E1h or E2h (for extended API) */
  uint8_t Version;                /**< CCfile[1]: Capability container version (b7-b4) and access conditions (b3-b0) */
  uint8_t MemorySize;             /**< CCfile[2]: Memory size, expressed in 8 bytes blocks, set to 0 if tag size is greater than 16kbits. */
  uint8_t TT5Tag;                 /**< CCfile[3]: Additionnal information on the Type5 Tag:\n
                                                  b0: supports `read multiple block` commands\n
                                                  b1: RFU\n
                                                  b2: RFU\n
                                                  b3: supports `lock block` commands\n
                                                  b4: requires the `special frame` format
                                    */
  uint8_t RSevd1;                 /**< RFU */
  uint8_t RSevd2;                 /**< RFU */
  uint16_t ExtMemorySize;         /**< CCfile[6],CCfile[7]: Memory size, expressed in 8 bytes blocks, when tag size is greater than 16kbits. */
  TT5_State State;                /**< Indicates if a NDEF message is present. */
  uint32_t NDEF_offset;           /**< Indicates the address of a NDEF message in the tag. */
}sCCFileInfo;

/** @brief Type5 Tag Type-Length-Value structure as defined by the NFC Forum */
typedef struct
{
  uint8_t   Type;     /**< NFC Forum message Type */
  uint8_t   Length;   /**< Message length if lesser than 255 bytes */
  uint16_t  Length16; /**< Message length if greater than or equal to 255 bytes */
} TT5_TLV_t;

/* Local variables -----------------------------------------------------------*/
static sCCFileInfo CCFileStruct;
static uint32_t NDEFPayLoadLength;
static uint32_t MaxSamplesNumber;
static uint32_t BeginAddrCompactData;

/* Local functions -----------------------------------------------------------*/

static  uint16_t NfcType5_TT5Init(void);
static uint16_t NfcType5_WriteCCFile( const uint8_t * const pCCBuffer );
static uint16_t NfcType5_NDEFDetection( void );
static uint16_t NfcType5_ReadCCFile( uint8_t * const pCCBuffer );

/**
  * @brief  Init NDEF Header
  * @param  None
  * @retval None
  */
void NfcType5_NDEFInitHeader(void)
{
  ST25DV_UID pUid;
  uint16_t MemoryAddressNDEF = 0x0000;
  
  /* Check if no NDEF detected, init mem in Tag Type 5 */
  if( NfcType5_NDEFDetection( ) != NDEF_OK ) {
    CCFileStruct.MagicNumber = (ST25DV_CC_SIZE==8) ? NFCT5_MAGICNUMBER_E2_CCFILE : NFCT5_MAGICNUMBER_E1_CCFILE;
    CCFileStruct.Version = NFCT5_VERSION_V1_0;
    CCFileStruct.MemorySize = ( STSMART_NFC_MAX_SIZE / 8 ) & 0xFF;
    CCFileStruct.ExtMemorySize = (( STSMART_NFC_MAX_SIZE / 8 )-1);
    CCFileStruct.TT5Tag = 0x01;
    while( NfcType5_TT5Init( ) != NFCTAG_OK ) {
      ;
    }
    
    MemoryAddressNDEF = ST25DV_CC_SIZE;
    
    /* Read the NFC UID */
    if(BSP_NFCTAG_ReadUID(BSP_NFCTAG_INSTANCE, &pUid)!=NFCTAG_OK ) {
      STNFC_Error_Handler(STNFC_READING_ERROR);
    } else {
      SMARTNFC_PRINTF("SmarTagUID= %lx%lx\r\n",pUid.MsbUid,pUid.LsbUid);
    }
    
    /*
    The fields in an NDEF Message header are as follows:
    ______________________________
    | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0|
    |------------------------------|
    | MB| ME| CF| SR| IL|    TNF   |  NDEF StatusByte, 1 byte
    |------------------------------|
    |        TYPE_LENGTH           |  1 byte, hex value
    |------------------------------|
    |        PAYLOAD_LENGTH        |  1 or 4 bytes (determined by SR) (LSB first)
    |------------------------------|
    |        ID_LENGTH             |  0 or 1 bytes (determined by IL)
    |------------------------------|
    |        TYPE                  |  (determined by TYPE_LENGTH)
    |------------------------------|
    |        ID                    |  0 or 1 byte  (determined by IL & ID_LENGTH)
    |------------------------------|
    |        PAYLOAD               |  X bytes (determined by PAYLOAD_LENGTH)
    |------------------------------|
    
    DEF Status Byte : size = 1byte : has multiple bit fields that contain meta data bout the rest of the header fields.
    MB : Message Begin flag
    ME : Message End flag
    CF : Chunk Flag (1 = Record is chunked up across multiple messages)
    SR : Short Record flag ( 1 = Record is contained in 1 message)
    IL : ID Length present flag ( 0 = ID field not present, 1 = present)
    TNF: Type Name Format code - one of the following
    0x00 : Empty
    0x01 : NFC Well Known Type [NFC RTD] (Use This One)
    0x02 : Media Type [RFC 2046]
    0x03 : Absolute URI [RFC 3986]
    0x04 : External Type [NFC RTD]
    0x05 : UnKnown
    0x06 : UnChanged
    0x07 : Reserved
    */
    
    
    /* NDEF message header */
    {
      uint8_t NDEFMessageHeader[4];
      
      /* TLV */
      NDEFMessageHeader[0] = 0x03;
      NDEFMessageHeader[1] = NFCT5_3_BYTES_L_TLV;
      NDEFMessageHeader[2] = (ST25DV_TLV_PAYLOAD_SIZE >> 8) & 0xFF; 
      NDEFMessageHeader[3] = ST25DV_TLV_PAYLOAD_SIZE & 0xFF;
      
      if(BSP_NFCTAG_WriteData(BSP_NFCTAG_INSTANCE, NDEFMessageHeader, MemoryAddressNDEF, ST25DV_TL_SIZE)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      
      /* NDEF HEADER */
      SMARTNFC_PRINTF("Creating NDEF External record Type for saving log data\r\n");
      SMARTNFC_PRINTF("\tST NFC Protocol Ver %d Rev %d\r\n",SMARTNFC_RECORD_VERSION, SMARTNFC_RECORD_REVISION);  
      
      MemoryAddressNDEF += ST25DV_TL_SIZE;
    }
    
    /* ASTRA customized BLE URI NDEF */  
    {
      uint8_t NDEFHeaderBLE[ST25DV_NDEF1_SIZE] = {0};
      NDEFHeaderBLE[0] = (1<<7 /* MB */) |
                         (0<<6 /* ME */) |
                         (0<<5 /* CF */) |
                         (1<<4 /* SR */) |
                         (0<<3 /* IL */) |
                         (0x01  /* TNF well known type*/);              /* Value 0x91 */
      
      NDEFHeaderBLE[1] = 0x01; /* Type lenght */
      NDEFHeaderBLE[2] = ST25DV_NDEF1_PAYLOAD_SIZE; /* Playload Length */
      NDEFHeaderBLE[3] = 0x55; /* Playload type U */
      NDEFHeaderBLE[4] = 0x00; /* URI header identifier NONE */
      
      uint8_t BleMacAddress[6] = { 0 };
      AstraGetMacAddress(BleMacAddress);
      uint32_t securepin = 0;
      AstraGetSecurePin(&securepin);
      sprintf((char*)&NDEFHeaderBLE[5],"stapplication://connect?Pin=%ld&Add=%02x:%02x:%02x:%02x:%02x:%02x",
              securepin,
              BleMacAddress[5],
              BleMacAddress[4],
              BleMacAddress[3],
              BleMacAddress[2],
              BleMacAddress[1],
              BleMacAddress[0]);
      
      if(BSP_NFCTAG_WriteData(BSP_NFCTAG_INSTANCE, NDEFHeaderBLE, MemoryAddressNDEF, ST25DV_NDEF1_SIZE)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      
      MemoryAddressNDEF += ST25DV_NDEF1_SIZE;
      
    }
    
    
    /* ASTRA External record NDEF */  
    {
      uint8_t NDEFHeaderASTRA[ST25DV_NDEF2_SIZE]={0};
      NDEFHeaderASTRA[0] = (0<<7 /* MB */) |
                           (0<<6 /* ME */) |
                           (0<<5 /* CF */) |
                           (1<<4 /* SR */) |
                           (0<<3 /* IL */) |
                           (0x04  /* TNF External Record*/);            /* Value 0x14 */
      
      NDEFHeaderASTRA[1 ] = 0x0C;                         /* Type lenght */
      NDEFHeaderASTRA[2 ] = ST25DV_NDEF2_PAYLOAD_SIZE;    /* Playload Length*/
      NDEFHeaderASTRA[3 ] = 's';                          /* "st.com:astra" */
      NDEFHeaderASTRA[4 ] = 't';
      NDEFHeaderASTRA[5 ] = '.';
      NDEFHeaderASTRA[6 ] = 'c';
      NDEFHeaderASTRA[7 ] = 'o';
      NDEFHeaderASTRA[8 ] = 'm';
      NDEFHeaderASTRA[9 ] = ':';
      NDEFHeaderASTRA[10] = 'a';
      NDEFHeaderASTRA[11] = 's';
      NDEFHeaderASTRA[12] = 't';
      NDEFHeaderASTRA[13] = 'r';
      NDEFHeaderASTRA[14] = 'a';
      
      if(BSP_NFCTAG_WriteData(BSP_NFCTAG_INSTANCE, NDEFHeaderASTRA, MemoryAddressNDEF, ST25DV_NDEF2_SIZE)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      
      MemoryAddressNDEF += ST25DV_NDEF2_SIZE;
    }   
    
    
    /* SMARTAG External record NDEF */  
    {
      uint8_t NDEFHeader[ST25DV_NDEF3_HEADER_SIZE];
      uint16_t NDEFHeaderPlayloadLength = ST25DV_NDEF3_PAYLOAD_SIZE;
      
      NDEFHeader[0] = (0<<7 /* MB */) |
                      (1<<6 /* ME */) |
                      (0<<5 /* CF */) |
                      (0<<4 /* SR */) |
                      (1<<3 /* IL */) |
                      (0x04  /* TNF External Record*/);           /* Value 0x4C */
      
      NDEFHeader[1 ] = SMART_TYPE_STRING_LENGTH;                  /* Type lenght */
      NDEFHeader[2 ] = 0x00;                                      /* Playload Length MSB*/
      NDEFHeader[3 ] = 0x00;
      NDEFHeader[4 ] = (NDEFHeaderPlayloadLength >> 8) & 0xFF;
      NDEFHeader[5 ] = (NDEFHeaderPlayloadLength & 0xFF);         /* Playload Length LSB*/
      NDEFHeader[6 ] = SMARTNFC_UID_EXTRA_LENGHT;
      NDEFHeader[7] = 's'; /* "st.com:smartag" */
      NDEFHeader[8] = 't';
      NDEFHeader[9] = '.';
      NDEFHeader[10] = 'c';
      NDEFHeader[11] = 'o';
      NDEFHeader[12] = 'm';
      NDEFHeader[13] = ':';
      NDEFHeader[14] = 's';
      NDEFHeader[15] = 'm';
      NDEFHeader[16] = 'a';
      NDEFHeader[17] = 'r';
      NDEFHeader[18] = 't';
      NDEFHeader[19] = 'a';
      NDEFHeader[20] = 'g';
      NDEFHeader[21] = 0x00; /* 3 Padding bytes */
      NDEFHeader[22] = 0x00;
      NDEFHeader[23] = 0x00;
      NDEFHeader[24] = PlatformStatus.s.STM32_UID[0]; /*  MCU ID */
      NDEFHeader[25] = PlatformStatus.s.STM32_UID[1];
      NDEFHeader[26] = PlatformStatus.s.STM32_UID[2];
      NDEFHeader[27] = PlatformStatus.s.STM32_UID[3];
      NDEFHeader[28] = PlatformStatus.s.STM32_UID[4];
      NDEFHeader[29] = PlatformStatus.s.STM32_UID[5];
      NDEFHeader[30] = PlatformStatus.s.STM32_UID[6];
      NDEFHeader[31] = PlatformStatus.s.STM32_UID[7];
      NDEFHeader[32] = PlatformStatus.s.STM32_UID[8];
      NDEFHeader[33] = PlatformStatus.s.STM32_UID[9];
      NDEFHeader[34] = PlatformStatus.s.STM32_UID[10];
      NDEFHeader[35] = PlatformStatus.s.STM32_UID[11];
      
      if(BSP_NFCTAG_WriteData(BSP_NFCTAG_INSTANCE, NDEFHeader, MemoryAddressNDEF , ST25DV_NDEF3_HEADER_SIZE)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
      
      MemoryAddressNDEF += ST25DV_NDEF3_HEADER_SIZE;
      
    }
    
    /* Close NDEF at the end of Minimal NDEF Record */
    {
      uint8_t CloseNDEF = NFCT5_TERMINATOR_TLV;
      if(BSP_NFCTAG_WriteData(BSP_NFCTAG_INSTANCE, &CloseNDEF, ST25DV_CC_SIZE + ST25DV_TL_SIZE + ST25DV_TLV_PAYLOAD_SIZE, 1)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
    }
  }
}


/**
  * @brief  Update NDEF Header and added Term for Tag Length Value Field (TLV).
  * A TLV Block consist of one or more bytes, depending on which of these three fields is present.
  * 
  * Note: The TLV Block will always be at least one byte since the T Field is mandatory in every case
  *
  * @param  uint32_t BytesAdded: Bytes to be added to header NDEF for moving and updating TLV and related NDEF header. 
  * @retval None
  */
void NfcType5_NDEFUpdateHeaderAddTermTLV(uint32_t BytesAdded)
{
  /* Increment the NDEF Payload Length */
  NDEFPayLoadLength +=BytesAdded;

  // + SMARTNFC_START_ADDR_OFFSET
  if(NDEFPayLoadLength<=SMARTNFC_MAX_PAYLOAD_SIZE)
  {
    uint8_t NDEFHeaderSection[2];
    uint32_t SizeForTLV;

    SizeForTLV = (NDEFPayLoadLength + ST25DV_NDEF1_SIZE + ST25DV_NDEF2_SIZE + ST25DV_NDEF3_HEADER_SIZE);
      
    /* Control if we need to update the NDEF session */
    if(NDEFPayLoadLength>ST25DV_NDEF3_PAYLOAD_SIZE) 
    {
      NDEFHeaderSection[0] = (NDEFPayLoadLength>>8)&0xFF;
      NDEFHeaderSection[1] = (NDEFPayLoadLength   )&0xFF;
      {
        static int32_t OnlyOneTime=1;
        if(OnlyOneTime) {
          OnlyOneTime=0;
          PRINTF_INFO("NDEF payload length>%u\r\n", ST25DV_NDEF3_PAYLOAD_SIZE);
        }
      }
          PRINTF_INFO("New NDEF payload length %2x %2x saved at address %u\r\n", NDEFHeaderSection[0], NDEFHeaderSection[1], (ST25DV_CC_SIZE + ST25DV_TL_SIZE + ST25DV_NDEF1_SIZE + ST25DV_NDEF2_SIZE + 4));
          PRINTF_INFO("New NDEF end charachter at address %u\r\n", (ST25DV_CC_SIZE + ST25DV_TL_SIZE + SizeForTLV));
      
      /* Update NDEF header */
      if(BSP_NFCTAG_WriteData(BSP_NFCTAG_INSTANCE, NDEFHeaderSection, ST25DV_CC_SIZE + ST25DV_TL_SIZE + ST25DV_NDEF1_SIZE + ST25DV_NDEF2_SIZE + 4, 2)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      } 
      
      /* Write the Terminator TLV */
      {
        uint8_t CloseNDEF = NFCT5_TERMINATOR_TLV;
        if(BSP_NFCTAG_WriteData(BSP_NFCTAG_INSTANCE, &CloseNDEF, ST25DV_CC_SIZE + ST25DV_TL_SIZE + SizeForTLV, 1)!=NFCTAG_OK){
          STNFC_Error_Handler(STNFC_WRITING_ERROR);
        }
      }
    }

    /* Control if we need to update the TLV session */
    if(SizeForTLV>ST25DV_TLV_PAYLOAD_SIZE) {
      NDEFHeaderSection[0] = (SizeForTLV>>8)&0xFF;
      NDEFHeaderSection[1] = (SizeForTLV   )&0xFF;
      PRINTF_INFO("New NDEF message payload length %2x %2x saved at address %u\r\n", NDEFHeaderSection[0], NDEFHeaderSection[1], (ST25DV_CC_SIZE+2));
      /* Update TLV header */
      if(BSP_NFCTAG_WriteData(BSP_NFCTAG_INSTANCE, NDEFHeaderSection, ST25DV_CC_SIZE+2, 2)!=NFCTAG_OK){
        STNFC_Error_Handler(STNFC_WRITING_ERROR);
      }
    }
  }
}

/* Local Functions -----------------------------------------------------------*/
/**
  * @brief  This function initializes the Capability Container and an empty NDEF message in a NFC Tag.
  * @details The Capability Container content is defined by the global variable `CCFileStruct`.
  * @retval NDEF_ERROR The Tag has not been initialized.
  * @retval NDEF_OK    The Tag has been successfully initialized.
  */
static uint16_t NfcType5_TT5Init( void )
{
  uint16_t Status;
  uint8_t CCBuffer[8];
  uint8_t CData;

  /* Prepare buffer for updating CCFile */

  CCBuffer[0] = CCFileStruct.MagicNumber;
  CCBuffer[1] = CCFileStruct.Version;
  CCBuffer[2] = CCFileStruct.MemorySize;
  CCBuffer[3] = CCFileStruct.TT5Tag;
  CCFileStruct.NDEF_offset = 0x04;

  /* If extended memory prepare the length bytes */
  if( CCFileStruct.MemorySize == NFCT5_EXTENDED_CCFILE ) {
    CCBuffer[4] = 0x00;
    CCBuffer[5] = 0x00;
    CCBuffer[6] = (uint8_t)(CCFileStruct.ExtMemorySize >> 8);
    CCBuffer[7] = (uint8_t)(CCFileStruct.ExtMemorySize & 0xFF);
    CCFileStruct.NDEF_offset = 0x08;
  }

  /* Update CCFile */
  Status = NfcType5_WriteCCFile( CCBuffer );
  if( Status != NDEF_OK ) {
    return Status;
  }

  /* Update NDEF TLV for INITIALIZED state */
  CData = NFCT5_NDEF_MSG_TLV;
  if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, &CData, CCFileStruct.NDEF_offset, 1 )!= NFCTAG_OK ) {
    STNFC_Error_Handler(STNFC_WRITING_ERROR);
    return NDEF_ERROR;
  }

  CData = 0x00;
  if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, &CData, (CCFileStruct.NDEF_offset + 1), 1 )!= NFCTAG_OK ) {
    STNFC_Error_Handler(STNFC_WRITING_ERROR);
    return NDEF_ERROR;
  }

  return NDEF_OK;
}

/**
  * @brief  This functions writes the Capability Container in the NFC Tag.
  * @param  pCCBuffer Pointer on the buffer containnig the Capability Container.
  * @retval NDEF_ERROR Error when writing the Tag.
  * @retval NDEF_OK    The CC has been successfully written.
  */
static uint16_t NfcType5_WriteCCFile( const uint8_t * const pCCBuffer )
{
  /* Write first block of CCFile */
  if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, pCCBuffer, 0x00, 0x4 )!= NFCTAG_OK ) {
    STNFC_Error_Handler(STNFC_WRITING_ERROR);
    return NDEF_ERROR;
  }

  /* If extended memory writes the next 4 bytes */
  if(pCCBuffer[2] == 0x00) {
    if(BSP_NFCTAG_WriteData( BSP_NFCTAG_INSTANCE, pCCBuffer + 4, 0x04, 4 )!= NFCTAG_OK ) {
      STNFC_Error_Handler(STNFC_WRITING_ERROR);
      return NDEF_ERROR;
    }
  }

  return NDEF_OK;
}

/**
  * @brief    This function detects a NDEF message in a Type 5 Tag.
  * @details  It first detects the Capability Container and then look for the NDEF TLV.
  *           The `CCfileStruct` global variable is updated accordingly with what is detected.
  * @retval NDEF_OK                 NDEF message Tag Type 5 detected.
  * @retval NDEF_ERROR_NOT_FORMATED Device is not a NFC Tag Type 5 Tag.
  */
static uint16_t NfcType5_NDEFDetection( void )
{
  uint8_t CCBuffer[8];
  TT5_TLV_t TLVDetect;
  uint16_t Status;
  uint32_t MemorySize;
  
  CCFileStruct.State = TT5_NO_NDEF;
  
  /* Read CCFile */
  Status = NfcType5_ReadCCFile( CCBuffer );
  if( Status != NDEF_OK )
  {
    return Status;
  }
  
  /* Check Byte 0 is equal to magic number  correct for the current board */
  if (((ST25DV_CC_SIZE == 8) && (CCBuffer[0] != NFCT5_MAGICNUMBER_E2_CCFILE)) || ((ST25DV_CC_SIZE != 8) && (CCBuffer[0] != NFCT5_MAGICNUMBER_E1_CCFILE)))
  { 
    return NDEF_ERROR_NOT_FORMATED;
  }
  /* Check Version number */
  else if( ( (CCBuffer[1]&0xFC) != 0x40 ) )
  {
    return NDEF_ERROR_NOT_FORMATED;
  }
  
  /* Check if CCFile is on 4 Bytes or 8 Bytes */
  if( CCBuffer[2] == 0x00 )
  {
    /* Update CCFIle structure */
    CCFileStruct.MemorySize = 0x0;
    CCFileStruct.ExtMemorySize = (uint16_t)CCBuffer[6];
    CCFileStruct.ExtMemorySize = ( CCFileStruct.ExtMemorySize << 8 ) |  CCBuffer[7];
    MemorySize = CCFileStruct.ExtMemorySize;
    CCFileStruct.NDEF_offset = 8;
  }
  else
  {
    return NDEF_ERROR_NOT_FORMATED;
  }
  
  /* Update CCFIle structure */
  CCFileStruct.MagicNumber = (TT5_MagicNumber_t)CCBuffer[0];
  CCFileStruct.Version = CCBuffer[1];
  CCFileStruct.TT5Tag = CCBuffer[3];
  
  /* Search for position of NDEF TLV in memory and tag Status */
  while( ( BSP_NFCTAG_ReadData( BSP_NFCTAG_INSTANCE, (uint8_t *)&TLVDetect, CCFileStruct.NDEF_offset, sizeof(TT5_TLV_t) ) == NFCTAG_OK ) && ( CCFileStruct.NDEF_offset < MemorySize ) )
  {
    /* Detect first NDEF Message in memory */
    if( TLVDetect.Type == NFCT5_NDEF_MSG_TLV )
    {
      if( TLVDetect.Length == 0x00 )
      {
        CCFileStruct.State = TT5_INITIALIZED;
      }
      else
      {
        if( CCFileStruct.Version & 0x3 )
        {
          CCFileStruct.State = TT5_READ;
        }
        else
        {
          CCFileStruct.State = TT5_READ_WRITE;
        }
      }
      return NDEF_OK;
    }
    /* If Proprietary NDEF jump to end of proprietary message */
    else if( TLVDetect.Type == NFCT5_PROPRIETARY_TLV )
    {
      if( TLVDetect.Length == NFCT5_3_BYTES_L_TLV )
      {
        CCFileStruct.NDEF_offset = CCFileStruct.NDEF_offset + TLVDetect.Length16;
        continue;
      }
      else
      {
        CCFileStruct.NDEF_offset = CCFileStruct.NDEF_offset + TLVDetect.Length;
        continue;
      }
    }
    /* if Terminator no NDEF detected */
    else if( TLVDetect.Type == NFCT5_TERMINATOR_TLV )
    {
      return NDEF_ERROR_NOT_FORMATED;
    }
    
    CCFileStruct.NDEF_offset++;
  }
  
  return NDEF_ERROR_NOT_FORMATED;
}

/**
  * @brief  This functions reads the Capability Container from the NFC Tag.
  * @param  pCCBuffer Pointer on the buffer used to store the CC.
  * @retval NDEF_ERROR Error when reading the Tag.
  * @retval NDEF_OK    The CC has been successfully read.
  */
static uint16_t NfcType5_ReadCCFile( uint8_t * const pCCBuffer )
{

  /* Read 4 bytes of CC File */
  if(BSP_NFCTAG_ReadData( BSP_NFCTAG_INSTANCE, pCCBuffer, 0x00, 4 )!=NFCTAG_OK) {
     STNFC_Error_Handler(STNFC_READING_ERROR);
     return NDEF_ERROR;
  }

  /* If extended memory reads the next 4 bytes */
  if(pCCBuffer[2] == 0x00) {
    if(BSP_NFCTAG_ReadData( BSP_NFCTAG_INSTANCE, pCCBuffer + 4, 0x04, 4 )!=NFCTAG_OK) {
      STNFC_Error_Handler(STNFC_READING_ERROR);
      return NDEF_ERROR;
    }
  }

  return NDEF_OK;
}

/**
  * @brief  Initialize NDEF PayLoad Size value
  * @param  uint32_t Address for the first sample
  * @retval None
  */
void NfcType5_SetInitialNDEFPayLoadLengthValue(uint32_t Address)
{
  /* We will start saving the space for Config/Min/Max section */
  NDEFPayLoadLength = Address - SMARTNFC_START_ADDR_OFFSET;
  
  /* This is the Address where we save the first sample */
  BeginAddrCompactData = Address;
  
  /* Total Number of Samples== (end -start)/8 because we use 8 bytes for each sample */
  MaxSamplesNumber = (SMARTNFC_END_ADDR_COMPACT_DATA - BeginAddrCompactData)>>3;
  
  SMARTNFC_PRINTF("\r\nNfcType5_SetInitialNDEFPayLoadLengthValue:\r\n");
  SMARTNFC_PRINTF("\tNDEFPayLoadLength=%d\r\n",NDEFPayLoadLength);
  SMARTNFC_PRINTF("\tBeginAddrCompactData=%d\r\n",BeginAddrCompactData);
  SMARTNFC_PRINTF("\tEndAddrCompactData=%d\r\n",SMARTNFC_END_ADDR_COMPACT_DATA);
  SMARTNFC_PRINTF("\tMaxSamplesNumber=%d\r\n",MaxSamplesNumber);
}


/**
  * @brief  Update the Sample Counter Until the End of the NFC Size
  * @param  SNFC_LogDefinition_t SampleCounter Current Sample Counter
  * @param  uint32_t BytesAdded How Many bytes we are adding to the tag
  * @retval None
  */
void NfcType5_UpdateSampleCounter(SNFC_LogDefinition_t *LogDef,uint32_t BytesAdded)
{
  if(LogDef->SampleCounter<MaxSamplesNumber) {
    LogDef->SampleCounter++;
    /* Update the NDEF Header if it's necessary */
    NfcType5_NDEFUpdateHeaderAddTermTLV(BytesAdded);
  } else {
    static int32_t OnlyOneTime=1;
    if(OnlyOneTime) {
      OnlyOneTime=0;
      SMARTNFC_PRINTF("----->Stop Updating Sample Counter\r\n");
    }
  }
  
  if (LogDef->LastSamplePointer >= SMARTNFC_END_ADDR_COMPACT_DATA) {
    /* Overwrite oldest sample */
    SMARTNFC_PRINTF("\r\n---> Reached the End of the TAG\r\n\toverwriting oldest samples\r\n");
    LogDef->LastSamplePointer = BeginAddrCompactData; 
  }
}

/**
  * @brief  Update NDEF PayLoad Size value
  * @param  uint32_t LastSamplePointer
  * @param  uint32_t SampleCounter
  * @retval None
  */
void NfcType5_ComputeNDEFPayLoadSize(uint32_t LastSamplePointer,uint32_t SampleCounter) 
{
  if(SampleCounter == MaxSamplesNumber) {
    NDEFPayLoadLength = SMARTNFC_MAX_PAYLOAD_SIZE;
  } else {
    NDEFPayLoadLength = (LastSamplePointer - SMARTNFC_START_ADDR_OFFSET);
  }
}

#endif //USE_MEMORY
