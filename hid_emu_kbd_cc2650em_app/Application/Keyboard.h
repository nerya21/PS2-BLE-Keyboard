/******************************************************************************

 @file  Keyboard.h

 @brief This file contains the interface to the Keyboard service.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************

 Copyright (c) 2014-2016, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 *****************************************************************************/

#ifndef KEYBOARD_H
#define KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */

#include "board.h"

/*********************************************************************
 * MACROS
 */

// Task Events
#define BOARD_KEY_CHANGE_EVT			0x01
#define BOARD_MOD_CHANGE_EVT			0x02
#define BOARD_BREAK_CODE_EVT			0x04

#define SCAN_CODE_SET_2_TO_HID		{						\
										{ 0x0	,	0x0	 },	\
										{ 0x42	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x3E	,	0x0	 },	\
										{ 0x3C	,	0x0	 },	\
										{ 0x3A	,	0x0	 },	\
										{ 0x3B	,	0x0	 },	\
										{ 0x45	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x43	,	0x0	 },	\
										{ 0x41	,	0x0	 },	\
										{ 0x3F	,	0x0	 },	\
										{ 0x3D	,	0x0	 },	\
										{ 0x2B	,	0x0	 },	\
										{ 0x35	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x04	,	0x40 },	\
										{ 0x02	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x01	,	0x10 },	\
										{ 0x14	,	0x0	 },	\
										{ 0x1E	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x1D	,	0x0	 },	\
										{ 0x16	,	0x0	 },	\
										{ 0x04	,	0x0	 },	\
										{ 0x1A	,	0x0	 },	\
										{ 0x1F	,	0x0	 },	\
										{ 0x0	,	0x8  },	\
										{ 0x0	,	0x0	 },	\
										{ 0x06	,	0x81 },	\
										{ 0x1B	,	0x0	 },	\
										{ 0x07	,	0x7F },	\
										{ 0x08	,	0x0	 },	\
										{ 0x21	,	0x0	 },	\
										{ 0x20	,	0x0	 },	\
										{ 0x0	,	0x80 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x2C	,	0x0	 },	\
										{ 0x19	,	0x0	 },	\
										{ 0x09	,	0x0	 },	\
										{ 0x17	,	0x0	 },	\
										{ 0x15	,	0x0	 },	\
										{ 0x22	,	0x0	 },	\
										{ 0x0	,	0x65 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x11	,	0x0	 },	\
										{ 0x05	,	0x80 },	\
										{ 0x0B	,	0x0	 },	\
										{ 0x0A	,	0x74 },	\
										{ 0x1C	,	0x0	 },	\
										{ 0x23	,	0x0	 },	\
										{ 0x0	,	0x66 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x10	,	0x0	 },	\
										{ 0x0D	,	0x78 },	\
										{ 0x18	,	0x0	 },	\
										{ 0x24	,	0x0	 },	\
										{ 0x25	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x36	,	0x0	 },	\
										{ 0x0E	,	0x0	 },	\
										{ 0x0C	,	0x0	 },	\
										{ 0x12	,	0x0	 },	\
										{ 0x27	,	0x0	 },	\
										{ 0x26	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x37	,	0x0	 },	\
										{ 0x38	,	0x54 },	\
										{ 0xF	,	0x0	 },	\
										{ 0x33	,	0x0	 },	\
										{ 0x13	,	0x0	 },	\
										{ 0x2D	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x34	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x2F	,	0x0	 },	\
										{ 0x2E	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x39	,	0x0	 },	\
										{ 0x20	,	0x0	 },	\
										{ 0x28	,	0x58 },	\
										{ 0x30	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x31	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x2A	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x59	,	0x4D },	\
										{ 0x0	,	0x0	 },	\
										{ 0x5C	,	0x50 },	\
										{ 0x5F	,	0x4A },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x62	,	0x49 },	\
										{ 0x63	,	0x4C },	\
										{ 0x5A	,	0x51 },	\
										{ 0x5D	,	0x0	 },	\
										{ 0x5E	,	0x4F },	\
										{ 0x60	,	0x52 },	\
										{ 0x29	,	0x0	 },	\
										{ 0x53	,	0x0	 },	\
										{ 0x44	,	0x0	 },	\
										{ 0x57	,	0x0	 },	\
										{ 0x5B	,	0x4E },	\
										{ 0x56	,	0x0	 },	\
										{ 0x55	,	0x0	 },	\
										{ 0x61	,	0x4B },	\
										{ 0x47	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x40	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 },	\
										{ 0x0	,	0x0	 }	\
									 };

/*********************************************************************
 * TYPEDEFS
 */
typedef void (*keysPressedCB_t)(uint8_t event, uint8_t keysPressed);


/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      Keyboard_init
 *
 * @brief   initialize keyboard
 *
 * @param   appKeyCB:	HID key pressed callback function
 */
void Keyboard_init(keysPressedCB_t appKeyCB);

/*********************************************************************
 * @fn      Keyboard_changeLedState
 *
 * @brief   Change LED state
 *
 * @param   state:	LED new state in USB HID format: [0,0,0,0,0,SCROLL,CAPS,NUM]
 */
void Keyboard_changeLedState(uint8_t state);
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* BOARD_KEY_H */
