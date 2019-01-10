/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    UVCUSER.c
 *      Purpose: USB Video Device Class User module 
 *      Version: V1.00
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#include <string.h>
#include <LPC23xx.H>                                  /* LPC23xx definitions */

#include "type.h"
#include "usb.h"
#include "usbhw.h"
#include "usbcfg.h"
#include "usbcore.h"
#include "uvc.h"
#include "uvcuser.h"

static BYTE UVC_PowerMode          = 0xA0;           /* Power Mode */
static BYTE UVC_ErrorCode          = NO_ERROR_ERR;   /* Error Code */

static BYTE UVC_suSelectedInputPin = 0x01;   /* SelectorUnit   selcted input pin */

static WORD UVC_puBrightnessCur    = 0x0010; /* ProcessingUnit Brightness Current */
static WORD UVC_puBrightnessDef    = 0x0010; /* ProcessingUnit Brightness Default */
static WORD UVC_puBrightnessMin    = 0x0001; /* ProcessingUnit Brightness Minimum */
static WORD UVC_puBrightnessMax    = 0x00FF; /* ProcessingUnit Brightness Maximum */
static WORD UVC_puBrightnessRes    = 0x0001; /* ProcessingUnit Brightness Resolution */

static UVC_VIDEO_PROBE_AND_COMMIT_CONTROL UVC_vsVideoProbe = 
   {0x001F,                 // bitfield indicating what fields shall be kept fixed
    0x01,                   // Video format index from a format descriptor
    0x01,                   // Video frame index from a frame descriptor
    0x000A2C2A,             // Frame intervall in 100 ns units
    0x0000,                 // Key frame rate in key-frame per video-frame units
    0x0000,                 // PFrame rate i PFrame/key frame units
    0x0000,                 // Compression quality control in abstarct units (0..10000)
    0x0000,                 // Window size for average bit rate control
    0x00FF,                 // int. VS interface latency in ms from capture to presentation
    0x00009480,             // maximum video frame or codec specific segemet size in bytes
    0x00009480,             // max. bytes the device can transmit/receive in single payload transfer
#if (UVC_VERSION ==  0x0110)
    0x00000000,             // device clock frequency in Hz for sepcified format
    0x00,                   // bitfield control
    0x00,                   // preferred payload format version for specifide bFormatIndex
    0x00,                   // minimum payload format version for specifide bFormatIndex    
    0x00                    // maximum payload format version for specifide bFormatIndex
#endif   
  };


/*----------------------------------------------------------------------------
  UVC SetCur Request Callback
  Called automatically on UVC SET_CUR Request
  Parameters:   None                          (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
BOOL UVC_SetCur (void) {
  WORD wTemp = 0;

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 0:                                            // interface control requests
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case VC_VIDEO_POWER_MODE_CONTROL:
              UVC_PowerMode = EP0Buf[0];
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 1:                                            // InputTerminal (camera)         see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite)      see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal                 see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit                   see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              UVC_suSelectedInputPin = EP0Buf[0];
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit                 see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:                    // only Brightness is supported   see usbdesc.c
              wTemp = *((__packed WORD *)EP0Buf);
              if ((wTemp >= UVC_puBrightnessMin) &&        // check the value to set
                  (wTemp <= UVC_puBrightnessMax)   ) {
                   
                UVC_puBrightnessCur = wTemp; 
                UVC_ErrorCode = NO_ERROR_ERR;
              }
              else {
                UVC_ErrorCode = OUT_OF_RANGE_ERR;
              }
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case VS_PROBE_CONTROL :
            case VS_COMMIT_CONTROL:
              memcpy((void *)&UVC_vsVideoProbe, (const void *)EP0Buf, sizeof(UVC_VIDEO_PROBE_AND_COMMIT_CONTROL));
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_SetCur


/*----------------------------------------------------------------------------
  UVC GetCur Request Callback
  Called automatically on UVC GET_CUR Request
  Parameters:   None                          (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
BOOL UVC_GetCur (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 0:                                            // interface control requests
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case VC_VIDEO_POWER_MODE_CONTROL:
              EP0Buf[0] = UVC_PowerMode;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case VC_REQUEST_ERROR_CODE_CONTROL:
              EP0Buf[0] = UVC_ErrorCode;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 1:                                            // InputTerminal (camera)    see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite) see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
            break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit              see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              EP0Buf[0] = UVC_suSelectedInputPin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              *((__packed WORD *)EP0Buf) = UVC_puBrightnessCur;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case VS_PROBE_CONTROL :
            case VS_COMMIT_CONTROL:
              memcpy((void *)EP0Buf, (const void *)&UVC_vsVideoProbe, sizeof(UVC_VIDEO_PROBE_AND_COMMIT_CONTROL));
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetCur


/*----------------------------------------------------------------------------
  UVC GetMin Request Callback
  Called automatically on UVC GET_MIN Request
  Parameters:   None                          (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
BOOL UVC_GetMin (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 1:                                            // InputTerminal (camera)    see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite) see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit              see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              EP0Buf[0] = UVC_suSelectedInputPin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              *((__packed WORD *)EP0Buf) = UVC_puBrightnessMin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetMin

/*----------------------------------------------------------------------------
  UVC GetMax Request Callback
  Called automatically on UVC GET_MAX Request
  Parameters:   None                          (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
BOOL UVC_GetMax (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 1:                                            // InputTerminal (camera)    see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:                            
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite) see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit              see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              EP0Buf[0] = UVC_suSelectedInputPin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              *((__packed WORD *)EP0Buf) = UVC_puBrightnessMax;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetMax

/*----------------------------------------------------------------------------
  UVC GetRes Request Callback
  Called automatically on UVC GET_RES Request
  Parameters:   None                          (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
BOOL UVC_GetRes (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 1:                                            // InputTerminal (camera)    see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite) see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit              see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              EP0Buf[0] = UVC_suSelectedInputPin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              *((__packed WORD *)EP0Buf) = UVC_puBrightnessRes;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetRes

/*----------------------------------------------------------------------------
  UVC GetLen Request Callback
  Called automatically on UVC GET_LEN Request
  Parameters:   None                          (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
BOOL UVC_GetLen (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              UVC_ErrorCode = INVALID_REQUEST_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetLen

/*----------------------------------------------------------------------------
  UVC GetInfo Request Callback
  Called automatically on UVC GET_INFO Request
  Parameters:   None                          (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
BOOL UVC_GetInfo (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 0:                                            // interface control requests
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case VC_VIDEO_POWER_MODE_CONTROL:
              EP0Buf[0] = (SUPPORTS_GET | SUPPORTS_SET);
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case VC_REQUEST_ERROR_CODE_CONTROL:
              EP0Buf[0] = (SUPPORTS_GET);
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 1:                                            // InputTerminal (camera)    see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite) see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit              see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              EP0Buf[0] = (SUPPORTS_GET | SUPPORTS_SET);
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              EP0Buf[0] = (SUPPORTS_GET | SUPPORTS_SET);
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetInfo

/*----------------------------------------------------------------------------
  UVC GetDef Request Callback
  Called automatically on UVC GET_DEF Request
  Parameters:   None                          (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
BOOL UVC_GetDef (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 1:                                            // InputTerminal (camera)    see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite) see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit              see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              EP0Buf[0] = UVC_suSelectedInputPin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              *((__packed WORD *)EP0Buf) = UVC_puBrightnessDef;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetDef

