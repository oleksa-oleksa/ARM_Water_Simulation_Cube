/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USBDESC.C
 *      Purpose: USB Descriptors
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#include "type.h"
#include "usb.h"
#include "usbcfg.h"
#include "usbdesc.h"
#include "uvc.h"

/*----------------------------------------------------------------------------
 * based on
 * USB Device Class Definition for Video Devices: Video Device Examples
 *---------------------------------------------------------------------------*/
 
/* USB Standard Device Descriptor */
const BYTE USB_DeviceDescriptor[] = {
  USB_DEVICE_DESC_SIZE,                      // bLength                 18
  USB_DEVICE_DESCRIPTOR_TYPE,                // bDescriptorType          1
  WBVAL(0x0200),                             // bcdUSB                2.00
  0xEF,                                      // bDeviceClass           239 Miscellaneous Device
  0x02,                                      // bDeviceSubClass          2 Common Class
  0x01,                                      // bDeviceProtocol          1 Interface Association
  USB_MAX_PACKET0,                           // bMaxPacketSize0
  WBVAL(0xC251),                             // idVendor
  WBVAL(0x1706),                             // idProduct                  Keil MCB2300, Video Device
  WBVAL(0x0100),                             // bcdDevice             1.00
  0x01,                                      // iManufacturer            1 Keil Software
  0x02,                                      // iProduct                 2 Keil MCB2300 UVC
  0x03,                                      // iSerialNumber            3 Demo 1.00
  0x01                                       // bNumConfigurations       1
};

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const BYTE USB_ConfigDescriptor[] = {
/* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,               // bLength                  9
  USB_CONFIGURATION_DESCRIPTOR_TYPE,         // bDescriptorType          2
  WBVAL(                                     // wTotalLength 
    USB_CONFIGUARTION_DESC_SIZE +            //
    UVC_INTERFACE_ASSOCIATION_DESC_SIZE +    //
    USB_INTERFACE_DESC_SIZE +                //                          VC interface
    UVC_VC_INTERFACE_HEADER_DESC_SIZE(1) +   //                          header + 1 interface
    UVC_CAMERA_TERMINAL_DESC_SIZE(2) +       //                          camera sensor + 2 controls
#ifdef COMPOSITE_VIDEO
    UVC_INPUT_TERMINAL_DESC_SIZE(0) +        //                          composite video input
#endif
    UVC_OUTPUT_TERMINAL_DESC_SIZE(0) +       //
#ifdef COMPOSITE_VIDEO
    UVC_SELECTOR_UNIT_DESC_SIZE(2) +         //                          selector + 2 input pins
#else
    UVC_SELECTOR_UNIT_DESC_SIZE(1) +         //                          selector + 1 input pins
#endif
    UVC_PROCESSING_UNIT_DESC_SIZE(2) +       //                          processing + 2 control bytes
    USB_ENDPOINT_DESC_SIZE +                 //                          standard interrupt EP
    UVC_VC_ENDPOINT_DESC_SIZE +              //                          class-specific interrupt EP
    USB_INTERFACE_DESC_SIZE +                //                          VS interface alternate setting 0
    UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1) +//                     VS input header
    0x0B +                                   //                          VS Format Descriptor
    0x26 +                                   //                          VS Frame Descriptor
    USB_INTERFACE_DESC_SIZE +                //                          VS interface alternate setting 1
    USB_ENDPOINT_DESC_SIZE +                 //                          Video data EP
    0x0000                                   // der ganze Rest 
      ),
  0x02,                                      // bNumInterfaces           2
  0x01,                                      // bConfigurationValue      1 ID of this configuration
  0x00,                                      // iConfiguration           0 no description available
  USB_CONFIG_BUS_POWERED ,                   // bmAttributes          0x80 Bus Powered
  USB_CONFIG_POWER_MA(100),                  // bMaxPower              100 mA

/* Interface Association Descriptor */
  UVC_INTERFACE_ASSOCIATION_DESC_SIZE,       // bLength                  8
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, // bDescriptorType         11
  0x00,                                      // bFirstInterface          0
  0x02,                                      // bInterfaceCount          2
  CC_VIDEO,                                  // bFunctionClass          14 Video
  SC_VIDEO_INTERFACE_COLLECTION,             // bFunctionSubClass        3 Video Interface Collection
  PC_PROTOCOL_UNDEFINED,                     // bInterfaceProtocol       0 (protocol undefined)
  0x04,                                      // iFunction                4 LPC23xx UVC Device

/* VideoControl Interface Descriptor */

/* Standard VC Interface Descriptor  = interface 0 */
  USB_INTERFACE_DESC_SIZE,                   // bLength                  9
  USB_INTERFACE_DESCRIPTOR_TYPE,             // bDescriptorType          4    
  USB_UVC_VCIF_NUM,                          // bInterfaceNumber         0 index of this interface
  0x00,                                      // bAlternateSetting        0 index of this setting
  0x01,                                      // bNumEndpoints            1 one interrupt endpoint
  CC_VIDEO,                                  // bInterfaceClass         14 Video
  SC_VIDEOCONTROL,                           // bInterfaceSubClass       1 Video Control
  PC_PROTOCOL_UNDEFINED,                     // bInterfaceProtocol       0 (protocol undefined)
  0x04,                                      // iFunction                4 LPC23xx UVC Device

/* Class-specific VC Interface Descriptor */
  UVC_VC_INTERFACE_HEADER_DESC_SIZE(1),      // bLength                 13 12 + 1 (header + 1*interface
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_HEADER,                                 // bDescriptorSubtype       1 (HEADER)
  WBVAL(UVC_VERSION),                        // bcdUVC                1.10 or 1.00
  WBVAL(                                     // wTotalLength               header+units+terminals (no Endpoints)
    UVC_VC_INTERFACE_HEADER_DESC_SIZE(1) +   //                          header + 1 interface
    UVC_CAMERA_TERMINAL_DESC_SIZE(2) +       //                          camera sensor + 2 controls
#ifdef COMPOSITE_VIDEO
    UVC_INPUT_TERMINAL_DESC_SIZE(0) +        //                          composite video input
#endif
    UVC_OUTPUT_TERMINAL_DESC_SIZE(0) +       //
#ifdef COMPOSITE_VIDEO
    UVC_SELECTOR_UNIT_DESC_SIZE(2) +         //                          selector + 2 input pins
#else
    UVC_SELECTOR_UNIT_DESC_SIZE(1) +         //                          selector + 1 input pins
#endif
    UVC_PROCESSING_UNIT_DESC_SIZE(2)         //                          processing + 2 control bytes
      ),
  DBVAL(0x005B8D80),                         // dwClockFrequency  6.000000 MHz
  0x01,                                      // bInCollection            1 one streaming interface
  0x01,                                      // baInterfaceNr( 0)        1 VS interface 1 belongs to this VC interface

/* Input Terminal Descriptor (Camera) */
  UVC_CAMERA_TERMINAL_DESC_SIZE(2),          // bLength                 17 15 + 2 controls
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_INPUT_TERMINAL,                         // bDescriptorSubtype       2 (INPUT_TERMINAL)
  0x01,                                      // bTerminalID              1 ID of this Terminal
  WBVAL(ITT_CAMERA),                         // wTerminalType       0x0201 Camera Sensor
  0x00,                                      // bAssocTerminal           0 no Terminal assiciated
  0x00,                                      // iTerminal                0 no description available                                      
  WBVAL(0x0000),                             // wObjectiveFocalLengthMin 0
  WBVAL(0x0000),                             // wObjectiveFocalLengthMax 0
  WBVAL(0x0000),                             // wOcularFocalLength       0
  0x02,                                      // bControlSize             2
  0x00, 0x00,                                // bmControls          0x0000 no controls supported

#ifdef COMPOSITE_VIDEO
/* Input Terminal Descriptor (Composite) */   
  UVC_INPUT_TERMINAL_DESC_SIZE(0),           // bLength                  8
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_INPUT_TERMINAL,                         // bDescriptorSubtype       2 (INPUT_TERMINAL)
  0x02,                                      // bTerminalID              2 ID of this Terminal
  WBVAL(COMPOSITE_CONNECTOR),                // wTerminalType       0x0401 Composite video input
  0x00,                                      // bAssocTerminal           0 no Terminal assiciated
  0x00,                                      // iTerminal                0 no description available                                      
#endif
 
/* Output Terminal Descriptor */ 
  UVC_OUTPUT_TERMINAL_DESC_SIZE(0),          // bLength                  9
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_OUTPUT_TERMINAL,                        // bDescriptorSubtype       3 (OUTPUT_TERMINAL)
  0x03,                                      // bTerminalID              3 ID of this Terminal
  WBVAL(TT_STREAMING),                       // wTerminalType       0x0101 USB streaming terminal
  0x00,                                      // bAssocTerminal           0 no Terminal assiciated
  0x05,                                      // bSourceID                5 input pin connected to output pin unit 5
  0x00,                                      // iTerminal                0 no description available                                      

#ifdef COMPOSITE_VIDEO
/* Selector Unit Descriptor */
  UVC_SELECTOR_UNIT_DESC_SIZE(2),            // bLength                  8 6 + 2 input pins
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_SELECTOR_UNIT,                          // bDescriptorSubtype       4 (SELECTOR UNIT)
  0x04,                                      // bUnitID                  4
  0x02,                                      // bNrInPins                2
  0x01,                                      // baSourceID(0)            1 see Input Terminal Descriptor (Camera)
  0x02,                                      // baSourceID(1)            2 see Input Terminal Descriptor (Composite)
  0x00,                                      // iSelector                0 no description available
#else
/* Selector Unit Descriptor */
  UVC_SELECTOR_UNIT_DESC_SIZE(1),            // bLength                  8 6 + 1 input pins
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_SELECTOR_UNIT,                          // bDescriptorSubtype       4 (SELECTOR UNIT)
  0x04,                                      // bUnitID                  4
  0x01,                                      // bNrInPins                1
  0x01,                                      // baSourceID(0)            1 see Input Terminal Descriptor (Camera)
  0x00,                                      // iSelector                0 no description available
#endif

/* Processing Unit Descriptor */
  UVC_PROCESSING_UNIT_DESC_SIZE(2),          // bLength                 12 10 + 2 control bytes
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_PROCESSING_UNIT,                        // bDescriptorSubtype       5 (PROCESSING_UNIT)
  0x05,                                      // bUnitID                  5
  0x04,                                      // bSourceID                4 input pin connected to output pin unit 4
  WBVAL(0x0000),                             // wMaxMultiplier           0 not used
  0x02,                                      // bControlSize             2 two control bytes
  WBVAL(0x0001),                             // bmControls          0x0001 Brightness
  0x00,                                      // iProcessing              0 no description available
#if (UVC_VERSION == 0x0110)
  0x00,                                      // bmVideoStandards         0 none
#endif

/* Standard Interrupt Endpoint Descriptor */
// we use an interrupt endpoint for notification
  USB_ENDPOINT_DESC_SIZE,                   // bLength                  7 
  USB_ENDPOINT_DESCRIPTOR_TYPE,             // bDescriptorType          5 (ENDPOINT)
  USB_ENDPOINT_IN(1),                       // bEndpointAddress      0x81 EP 1 IN
  USB_ENDPOINT_TYPE_INTERRUPT,              // bmAttributes             3 interrupt transfer type
  WBVAL(0x0008),                            // wMaxPacketSize      0x0008 1x 8 bytes
  0x20,                                     // bInterval               32 ms polling interval
        
/* Class-Specific Interrupt Endpoint Descriptor */
// mandatory if Standard Interrupt Endpoint is used
  UVC_VC_ENDPOINT_DESC_SIZE,                // bLength                  5
  CS_ENDPOINT,                              // bDescriptorType       0x25 (CS_ENDPOINT)
  EP_INTERRUPT,                             // bDescriptorSubtype       3 (EP_INTERRUPT)
  WBVAL(0x0008),                            // wMaxTransferSize         8 8-Byte status packet

/* Video Streaming Interface Descriptor */

/* Standard VS Interface Descriptor  = interface 1 */
// alternate setting 0 = Zero Bandwidth
  USB_INTERFACE_DESC_SIZE,                   // bLength                  9
  USB_INTERFACE_DESCRIPTOR_TYPE,             // bDescriptorType          4    
  USB_UVC_VSIF_NUM,                          // bInterfaceNumber         1 index of this interface
  0x00,                                      // bAlternateSetting        0 index of this setting
  0x00,                                      // bNumEndpoints            0 no EP used
  CC_VIDEO,                                  // bInterfaceClass         14 Video
  SC_VIDEOSTREAMING,                         // bInterfaceSubClass       2 Video Streaming
  PC_PROTOCOL_UNDEFINED,                     // bInterfaceProtocol       0 (protocol undefined)
  0x00,                                      // iInterface               0 no description available

/* Class-specific VS Header Descriptor (Input) */
  UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1),// bLength               14 13 + (1*1) (no specific controls used)
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VS_INPUT_HEADER,                           // bDescriptorSubtype       5 (INPUT_HEADER)
  0x01,                                      // bNumFormats              1 one format descriptor follows
  WBVAL(                                     // wTotalLength               header+frame/format descriptors
    UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1) +//                     VS input header
    0x0B +                                   //                          VS Format Descriptor
    0x26                                     //                          VS Frame Descriptor
      ),
  USB_ENDPOINT_IN(3),                        // bEndPointAddress      0x83 EP 3 IN
  0x00,                                      // bmInfo                   0 no dynamic format change supported
  0x03,                                      // bTerminalLink            3 supplies terminal ID 3 (Output terminal)
  0x01,                                      // bStillCaptureMethod      1 supports still image capture method1
  0x01,                                      // bTriggerSupport          1 HW trigger supported for still image capture
  0x00,                                      // bTriggerUsage            0 HW trigger initiate a still image capture
  0x01,                                      // bControlSize             1 one byte bmaControls field size
  0x00,                                      // bmaControls(0)           0 no VS specific controls

/* Class-specific VS Format Descriptor */
  0x0B,                                      // bLength                 11
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VS_FORMAT_MJPEG,                           // bDescriptorSubtype       6 (VS_FORMAT_MJPEG)
  0x01,                                      // bFormatIndex             1 first (and only) format descriptor
  0x01,                                      // bNumFrameDescriptors     1 one frame descriptor follows
  0x01,                                      // bmFlags                  1 uses fixed size samples
  0x01,                                      // bDefaultFrameIndex       1 default frame index is 1
  0x00,                                      // bAspectRatioX            0 non-interlaced stream - not required
  0x00,                                      // bAspectRatioY            0 non-interlaced stream - not required
  0x00,                                      // bmInterlaceFlags         0 non-interlaced stream
  0x00,                                      // bCopyProtect             0 no restrictions

/* Class specific VS Frame Descriptor */
  0x26,                                      // bLength                 38
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VS_FRAME_MJPEG,                            // bDescriptorSubtype       7 (VS_FRAME_MJPEG)
  0x01,                                      // bFrameIndex              1 first (and only) Frame Descripot
#if (UVC_VERSION == 0x0110)
  0x03,                                      // bmCapabilities        0x03 Still images using capture method 1, fixed frame rate
#else
  0x01,                                      // bmCapabilities        0x01 Still images using capture method 1
#endif
  WBVAL(0x00B0),                             // wWidth                 176 width of frame is 176 pixels
  WBVAL(0x0090),                             // wHeight                144 hight of frame is 144 pixels
  DBVAL(0x000DEC00),                         // dwMinBitRate        912384 min bit rate in bits/s
  DBVAL(0x000DEC00),                         // dwMaxBitRate        912384 max bit rate in bits/s
  DBVAL(0x00009480),                         // dwMaxVideoFrameBufferSize   38016 max video/still frame size in bytes
  DBVAL(0x000A2C2A),                         // dwDefaultFrameInterval     666666 default frame interval is 666666ns (15fps)
  0x00,                                      // bFrameIntervalType              0 continuous frame interval
  DBVAL(0x000A2C2A),                         // dwMinFrameInterval         666666 min frame interval is 666666ns (15fps) 
  DBVAL(0x000A2C2A),                         // dwMaxFrameInterval         666666 max frame interval is 666666ns (15fps)
  DBVAL(0x00000000),                         // dwFrameIntervalStep             0 no frame interval step supported

/* Standard VS Interface Descriptor  = interface 1 */
// alternate setting 1 = operational setting
  USB_INTERFACE_DESC_SIZE,                   // bLength                  9
  USB_INTERFACE_DESCRIPTOR_TYPE,             // bDescriptorType          4    
  USB_UVC_VSIF_NUM,                          // bInterfaceNumber         1 index of this interface
  0x01,                                      // bAlternateSetting        1 index of this setting
  0x01,                                      // bNumEndpoints            1 one EP used
  CC_VIDEO,                                  // bInterfaceClass         14 Video
  SC_VIDEOSTREAMING,                         // bInterfaceSubClass       2 Video Streaming
  PC_PROTOCOL_UNDEFINED,                     // bInterfaceProtocol       0 (protocol undefined)
  0x00,                                      // iInterface               0 no description available

/* Standard VS Isochronous Video data Endpoint Descriptor */
  USB_ENDPOINT_DESC_SIZE,                   // bLength                  7 
  USB_ENDPOINT_DESCRIPTOR_TYPE,             // bDescriptorType          5 (ENDPOINT)
  USB_ENDPOINT_IN(3),                       // bEndpointAddress      0x83 EP 3 IN
  USB_ENDPOINT_TYPE_ISOCHRONOUS |           // bmAttributes             5 isochronous transfer type
  USB_ENDPOINT_SYNC_ASYNCHRONOUS,           //                            asynchronous synchronizationtype
  WBVAL(0x01FE),                            // wMaxPacketSize      0x01FE max packet 510 bytes
  0x01,                                     // bInterval                1 one frame interval
/* Terminator */
  0x00                                      // bLength                  0
};



/* USB String Descriptor (optional) */
const BYTE xUSB_StringDescriptor[] = {
/* Index 0x00: LANGID Codes */
  0x04,                                     // bLength                  4
  USB_STRING_DESCRIPTOR_TYPE,               // bDescriptorType          3 (STRING)
  WBVAL(0x0409),                            // wLANGID             0x0409 US English

/* Index 0x04 (04): Manufacturer */
  0x1C,                                     // bLength                 28
  USB_STRING_DESCRIPTOR_TYPE,               // bDescriptorType          3 (STRING)
  'K',0,
  'e',0,
  'i',0,
  'l',0,
  ' ',0,
  'S',0,
  'o',0,
  'f',0,
  't',0,
  'w',0,
  'a',0,
  'r',0,
  'e',0,

/* Index 0x20 (32): Product */
  0x24,                                     // bLength                 36
  USB_STRING_DESCRIPTOR_TYPE,               // bDescriptorType          3 (STRING)
  'K',0,
  'e',0,
  'i',0,
  'l',0,
  ' ',0,
  'M',0,
  'C',0,
  'B',0,
  '2',0,
  '3',0,
  '0',0,
  '0',0,
  ' ',0,
  'U',0,
  'V',0,
  'C',0,
  ' ',0,

/* Index 0x44 (68): Serial Number */
  0x14,                                     // bLength                  20
  USB_STRING_DESCRIPTOR_TYPE,               // bDescriptorType          3 (STRING)
  'D',0,
  'e',0,
  'm',0,
  'o',0,
  ' ',0,
  '1',0,
  '.',0,
  '0',0,
  '0',0,

/* Index 0x58 (88): Interface Association Descriptor  */
  0x26,                                     // bLength                 38
  USB_STRING_DESCRIPTOR_TYPE,               // bDescriptorType          3 (STRING)
  'L',0,
  'P',0,
  'C',0,
  '2',0,
  '3',0,
  'x',0,
  'x',0,
  ' ',0,
  'U',0,
  'V',0,
  'C',0,
  ' ',0,
  'D',0,
  'e',0,
  'v',0,
  'i',0,
  'c',0,
  'e',0,
};

#define MAX_STRDESC_SIZE  0x26              // see IAD string
const BYTE USB_StringDescriptor[][MAX_STRDESC_SIZE] = { // zweidimensionales feld
  {
    /* Index 0x00: LANGID Codes */
    0x04,                                   // bLength                  4
    USB_STRING_DESCRIPTOR_TYPE,             // bDescriptorType          3 (STRING)
    WBVAL(0x0409)                          // wLANGID             0x0409 US English
  },
  {
    /* Index 0x01: Manufacturer */
    0x1C,                                   // bLength                 28
    USB_STRING_DESCRIPTOR_TYPE,             // bDescriptorType          3 (STRING)
    'K',0,
    'e',0,
    'i',0,
    'l',0,
    ' ',0,
    'S',0,
    'o',0,
    'f',0,
    't',0,
    'w',0,
    'a',0,
    'r',0,
    'e',0 
  },
  {
    /* Index 0x02: Product */
    0x24,                                   // bLength                 36
    USB_STRING_DESCRIPTOR_TYPE,             // bDescriptorType          3 (STRING)
    'K',0,
    'e',0,
    'i',0,
    'l',0,
    ' ',0,
    'M',0,
    'C',0,
    'B',0,
    '2',0,
    '3',0,
    '0',0,
    '0',0,
    ' ',0,
    'U',0,
    'V',0,
    'C',0,
    ' ',0
  },
  {
    /* Index 0x03: Serial Number */
    0x14,                                   // bLength                  20
    USB_STRING_DESCRIPTOR_TYPE,             // bDescriptorType          3 (STRING)
    'D',0,
    'e',0,
    'm',0,
    'o',0,
    ' ',0,
    '1',0,
    '.',0,
    '0',0,
    '0',0
  },
  {
    /* Index 0x04: Interface Association Descriptor  */
    0x26,                                   // bLength                 38
    USB_STRING_DESCRIPTOR_TYPE,             // bDescriptorType          3 (STRING)
    'L',0,
    'P',0,
    'C',0,
    '2',0,
    '3',0,
    'x',0,
    'x',0,
    ' ',0,
    'U',0,
    'V',0,
    'C',0,
    ' ',0,
    'D',0,
    'e',0,
    'v',0,
    'i',0,
    'c',0,
    'e',0
  },
  {
    /* Index 0x05: endof string  */
    0x04,                                   // bLength                  4
    USB_STRING_DESCRIPTOR_TYPE,             // bDescriptorType          3 (STRING)
    0,0
  }
};
