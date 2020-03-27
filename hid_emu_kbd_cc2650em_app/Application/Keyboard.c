/******************************************************************************

 @file  Keyboard.c

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

/*********************************************************************
 * INCLUDES
 */

#include <xdc/runtime/System.h>
#include <stdbool.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

#ifdef USE_ICALL
#include <icall.h>
#endif

#include <inc/hw_ints.h>

#include "util.h"
#include "Keyboard.h"
#include "LED.h"

/*********************************************************************
 * MACROS
 */

// Delay
#define delay_us(i) ( CPUdelay(8*(i)) )
#define delay_ms(i) ( CPUdelay(8000*(i)) )

/*********************************************************************
 * CONSTANTS
 */

// Largest scan code value
#define SCAN_CODE_SET_2_TO_HID_END		0x83

// Host to device
#define BOARD_RESET_SEND_EVT			0xFF
#define BOARD_RESPONSE_SEND_EVT			0x00
#define BOARD_LED_CHANGE_SEND_EVT		0xED
#define BOARD_ECHO_SEND_EVT				0xEE
#define BOARD_RESEND_SEND_EVT			0xFE
#define BOARD_ACK						0xFA
#define BOARD_TEST_PASSED				0xAA

// Keyboard connections
#define KB_CLK							PINCC26XX_DIO14	//clock pin
#define KB_DATA							PINCC26XX_DIO15	//data pin

// Keyboard buffer
#define BOARD_KB_BUFFER_SIZE			128

// Task configuration
#define BOARD_TASK_PRIORITY				2
#ifndef BOARD_TASK_STACK_SIZE
#define BOARD_TASK_STACK_SIZE			644
#endif

// Keyboard scan code special values
#define BREAK_CODE	0xF0
#define EXT_CODE	0xE0
#define RESEND_CODE	0xFE

// Modifier keys (scan code - set 2)
#define L_CTRL							0x14
#define R_CTRL							0x14	//EXT_CODE
#define L_SHIFT							0x12
#define R_SHIFT							0x59
#define L_ALT							0x11
#define R_ALT							0x11	//EXT_CODE
#define L_GUI							0x1F	//EXT_CODE
#define R_GUI							0x27	//EXT_CODE

// LED state bitmap
#define USB_LED_NUM_LOCK		        0x01
#define USB_LED_CAPS_LOCK				0x02
#define USB_LED_SCROLL_LOCK				0x04
#define PS2_LED_SCROLL_LOCK				0x01
#define PS2_LED_NUM_LOCK				0x02
#define PS2_LED_CAPS_LOCK				0x04

/*********************************************************************
 * FUNCTIONS DECLARATION
 */

static void rxCallback(PIN_Handle hPin, PIN_Id pinId);
static void txCallback(PIN_Handle hPin, PIN_Id pinId);

/*******************************************************************************
 * VARIABLES
 */

// Keyboard buffer
uint8_t board_kb_buffer[BOARD_KB_BUFFER_SIZE] = { 0 };
uint8_t buffer_write_pos = 0;

// Task configuration
Task_Params keyboardTaskParams;
Task_Struct keyboardTask;
Char keyboardTaskStack[BOARD_TASK_STACK_SIZE];

// Semaphore configuration
Semaphore_Struct boardSemaphore;
Semaphore_Handle boardSemaphoreHandle;

// Host to device
uint8_t tx_byte;

// Scan Code set 2 to USB HID table
uint8_t ps2ToUsbTable[256][2] = SCAN_CODE_SET_2_TO_HID;

// Task configuration
Task_Params keyboardTaskParams;
Task_Struct keyboardTask;
Char keyboardTaskStack[BOARD_TASK_STACK_SIZE];

// Semaphore configuration
Semaphore_Struct boardSemaphore;
Semaphore_Handle boardSemaphoreHandle;

// Pointer to application callback
keysPressedCB_t appKeyChangeHandler = NULL;

// Memory for the GPIO module to construct a Hwi
Hwi_Struct callbackHwiKeys;

// PIN configuration
PIN_Config keyboardPinsCfg[] =
{
		KB_CLK	| PIN_GPIO_OUTPUT_DIS	| PIN_INPUT_DIS	| PIN_PULLUP,
		KB_DATA	| PIN_GPIO_OUTPUT_DIS	| PIN_INPUT_DIS	| PIN_PULLUP,
		PIN_TERMINATE
};
PIN_State	keyboardPins;
PIN_Handle	keyboardPinsHandler;

// TX request
uint8_t txRequestPending = 0;
uint8_t	txRequestEvent;
uint8_t txRequestResponse;


// LED change request
uint8_t pendingLedRequest = 0;
uint8_t latestLedState;
uint8_t rxTxBusy = 1;

/*********************************************************************
 * PRIVATE FUNCTIONS
 */

/*********************************************************************
 * @fn      bufferRead
 *
 * @brief   read one scan code from buffer
 *
 * @ret		scan code
 */
uint8_t bufferRead(){
	static uint8_t buffer_read_pos = 0;

	while(buffer_read_pos==buffer_write_pos);
	uint8_t key = board_kb_buffer[buffer_read_pos];
	buffer_read_pos = (buffer_read_pos + 1) % BOARD_KB_BUFFER_SIZE;

	return key;
}

/*********************************************************************
 * @fn      bufferWrite
 *
 * @brief   write one scan code from buffer
 *
 * @param   key:	scan code to write
 */
void bufferWrite(uint8_t key){
	board_kb_buffer[buffer_write_pos] = key;
	buffer_write_pos = (buffer_write_pos + 1) % BOARD_KB_BUFFER_SIZE;
}

/*********************************************************************
 * @fn      startTx
 *
 * @brief   start Host to Device transfer
 */
void startTx() {
	PIN_setConfig(keyboardPinsHandler, PIN_BM_INPUT_EN			, KB_DATA	| PIN_INPUT_DIS);
	PIN_setConfig(keyboardPinsHandler, PIN_BM_GPIO_OUTPUT_EN	, KB_DATA	| PIN_GPIO_OUTPUT_EN);

	delay_us(100);

	PINCC26XX_setOutputValue(KB_DATA, 0);

	PIN_setConfig(keyboardPinsHandler, PIN_BM_GPIO_OUTPUT_EN	, KB_CLK	| PIN_GPIO_OUTPUT_DIS);
	PIN_setConfig(keyboardPinsHandler, PIN_BM_INPUT_EN			, KB_CLK	| PIN_INPUT_EN);

	PIN_registerIntCb(keyboardPinsHandler, &txCallback);
	PIN_setConfig(keyboardPinsHandler, PIN_BM_IRQ				, KB_CLK	| PIN_IRQ_NEGEDGE);
}

/*********************************************************************
 * @fn      stopTx
 *
 * @brief   stop Host to Device transfer
 */
void stopTx(){
	PIN_setConfig(keyboardPinsHandler, PIN_BM_IRQ				, KB_CLK	| PIN_IRQ_DIS);
	PIN_registerIntCb(keyboardPinsHandler, &rxCallback);
	PIN_setConfig(keyboardPinsHandler, PIN_BM_IRQ				, KB_CLK	| PIN_IRQ_NEGEDGE);
}

/*********************************************************************
 * @fn      usbToPS2LedState
 *
 * @brief   convert USB HID LED state to PS/2 LED state bitmask
 *
 * @param   usb_state:	USB HID LED state
 */
uint8_t usbToPS2LedState(uint8_t usb_state) {
	uint8_t ps2_state = 0;

	if (usb_state & USB_LED_NUM_LOCK){
		ps2_state |= PS2_LED_NUM_LOCK;
	} else {
		ps2_state &= ~PS2_LED_NUM_LOCK;
	}

	if (usb_state & USB_LED_CAPS_LOCK){
		ps2_state |= PS2_LED_CAPS_LOCK;
	} else {
		ps2_state &= ~PS2_LED_CAPS_LOCK;
	}

	if (usb_state & USB_LED_SCROLL_LOCK){
		ps2_state |= PS2_LED_SCROLL_LOCK;
	} else {
		ps2_state &= ~PS2_LED_SCROLL_LOCK;
	}

	return ps2_state;
}

/*********************************************************************
 * @fn      rxRestoreClock
 *
 * @brief   restore clock line for Device to Host transfer
 */
void rxRestoreClock(){
	PINCC26XX_setOutputValue(KB_CLK, 1);
	PIN_setConfig(keyboardPinsHandler, PIN_BM_GPIO_OUTPUT_EN	, KB_CLK	| PIN_GPIO_OUTPUT_DIS);
	PIN_setConfig(keyboardPinsHandler, PIN_BM_INPUT_EN			, KB_CLK	| PIN_INPUT_EN);
	PIN_setConfig(keyboardPinsHandler, PIN_BM_IRQ				, KB_CLK	| PIN_IRQ_NEGEDGE);
}

/*********************************************************************
 * @fn      keyboardTx
 *
 * @brief   initiate Host to Device transfer
 *
 * @param   event:		BOARD_RESPONSE_SEND_EVT-	response for previous request
 * 						BOARD_LED_CHANGE_SEND_EVT-	change LED state request
 * 						BOARD_RESEND_SEND_EVT-		resend last scan code request
 * 						BOARD_RESET_SEND_EVT-		reset keyboard request
 *
 * @param   response:	case event is BOARD_RESPONSE_SEND_EVT-		the scan code response
 * 						case event is BOARD_LED_CHANGE_SEND_EVT-	the requested LED state
 * 						otherwise ignored
 */
void keyboardTx(uint8_t event, uint8_t response){
	static uint8_t led_state, cur_event = 0, response_num;


	if (event != BOARD_RESPONSE_SEND_EVT){
		cur_event = event;
		led_state = response;
	}

	switch (cur_event){
		case BOARD_LED_CHANGE_SEND_EVT:
			if (event == cur_event){
				response_num = 0;
				tx_byte = BOARD_LED_CHANGE_SEND_EVT;
				startTx();
			} else if (event == BOARD_RESPONSE_SEND_EVT){
				if (response_num == 0 && response == BOARD_ACK) {
					response_num++;
					tx_byte = usbToPS2LedState(led_state);
					startTx();
				} else if (response_num == 1 && response == BOARD_ACK) {
					rxRestoreClock();
				} else {

				}
			}
			break;

		case BOARD_RESET_SEND_EVT:
			if (event == cur_event){
				response_num = 0;
				tx_byte = BOARD_RESET_SEND_EVT;
				startTx();
			} else if (event == BOARD_RESPONSE_SEND_EVT){
				if(response_num == 0 && response == BOARD_ACK){
					response_num++;
					rxRestoreClock();
				} else if (response_num == 1 && response == BOARD_TEST_PASSED) {
					LED_changeState(Board_LED0, 0);
					rxRestoreClock();
				} else {
					System_abort("Failed initializing keyboard\n");
				}
			}
			break;

		case BOARD_RESEND_SEND_EVT:
			if (event == cur_event){
				tx_byte = BOARD_RESEND_SEND_EVT;
				startTx();
			}
			break;
	}


}

/*********************************************************************
 * @fn      isModifier
 *
 * @brief   is the scan code represent modifier key
 *
 * @param   key:		scan code
 * 			extended:	is the scan code extended
 *
 * @ret		is the scan code represent modifier key
 */
uint8_t isModifier(uint8_t key, uint8_t extended) {
	if (extended == 1) {
		if (key == L_GUI || key == R_CTRL || key == R_ALT || key == R_GUI) {
			return 1;
		}
	} else {
		if (key == L_SHIFT || key == L_CTRL || key == L_ALT || key == R_SHIFT) {
			return 1;
		}
	}
	return 0;
}

/*********************************************************************
 * @fn      taskFxn
 *
 * @brief   main loop of program
 * 			handling scan codes received from keyboard and sends it to HID handler
 *
 * @param   a0:		D/C
 * 			a1:		D/C
 */
void taskFxn(UArg a0, UArg a1) {
	uint8_t HID_key, key, event, extended;

	// Application main loop.
	for (;;) {
		Semaphore_pend(boardSemaphoreHandle, BIOS_WAIT_FOREVER);

		event = 0;
		extended = 0;

		key = bufferRead();
		if (key == EXT_CODE) {
			key = bufferRead();
			extended = 1;
		}

		if (key == BREAK_CODE) {
			key = bufferRead();
			event |= BOARD_BREAK_CODE_EVT;
		}

		HID_key = ps2ToUsbTable[key][extended];

		event |= isModifier(key, extended) ? BOARD_MOD_CHANGE_EVT : BOARD_KEY_CHANGE_EVT;

		if (HID_key != 0) {
			(*appKeyChangeHandler)(event, HID_key);
		}
	}
}

/*********************************************************************
 * @fn      createTask
 *
 * @brief   create task for handling pressed keys
 */
void createTask(void) {
	// Configure task
	Task_Params_init(&keyboardTaskParams);
	keyboardTaskParams.stack = keyboardTaskStack;
	keyboardTaskParams.stackSize = BOARD_TASK_STACK_SIZE;
	keyboardTaskParams.priority = BOARD_TASK_PRIORITY;

	Task_construct(&keyboardTask, taskFxn, &keyboardTaskParams, NULL);
}

/*********************************************************************
 * @fn      createSemaphore
 *
 * @brief   create semaphore for taskFxn
 */
void createSemaphore(void) {
	Semaphore_Params semParams;

	// Configure semaphore
	Semaphore_Params_init(&semParams);
	Semaphore_construct(&boardSemaphore, 0, &semParams);
	boardSemaphoreHandle = Semaphore_handle(&boardSemaphore);
}

/*********************************************************************
 * @fn      resetKeyboard
 *
 * @brief   send reset request for keyboard
 */
void resetKeyboard() {
	PIN_setConfig(keyboardPinsHandler, PIN_BM_GPIO_OUTPUT_EN	, KB_CLK	| PIN_GPIO_OUTPUT_EN);
	PINCC26XX_setOutputValue(KB_CLK, 0);
	keyboardTx(BOARD_RESET_SEND_EVT,0);
}

/*********************************************************************
 * @fn      rxCompleteCallback
 *
 * @brief   callback function for the last rise of clock line
 *
 * @param   hPin:		D/C
 * 			pinId:		D/C
 */
void rxCompleteCallback(PIN_Handle hPin, PIN_Id pinId) {
	PIN_setConfig(keyboardPinsHandler, PIN_BM_IRQ				, KB_CLK	| PIN_IRQ_DIS);
	PIN_setConfig(keyboardPinsHandler, PIN_BM_INPUT_EN			, KB_CLK	| PIN_INPUT_DIS);
	PIN_setConfig(keyboardPinsHandler, PIN_BM_GPIO_OUTPUT_EN	, KB_CLK	| PIN_GPIO_OUTPUT_EN);
	PINCC26XX_clrPendInterrupt(KB_CLK);
	PINCC26XX_setOutputValue(KB_CLK, 0);

	if (txRequestPending == 1) {
		txRequestPending = 0;
		keyboardTx(txRequestEvent, txRequestResponse);
	} else if (pendingLedRequest == 1 && rxTxBusy == 0) {
		pendingLedRequest = 0;
		keyboardTx(BOARD_LED_CHANGE_SEND_EVT, latestLedState);
	} else {
		delay_us(105);
		PIN_setConfig(keyboardPinsHandler, PIN_BM_IRQ				, KB_CLK	| PIN_IRQ_NEGEDGE);
		PIN_setConfig(keyboardPinsHandler, PIN_BM_GPIO_OUTPUT_EN	, KB_CLK	| PIN_GPIO_OUTPUT_DIS);
		PIN_setConfig(keyboardPinsHandler, PIN_BM_INPUT_EN			, KB_CLK	| PIN_INPUT_EN);
		PIN_registerIntCb(keyboardPinsHandler, &rxCallback);
	}
}

/*********************************************************************
 * @fn      rxCallback
 *
 * @brief   callback function for Device to Host transfer
 *
 * @param   hPin:		D/C
 * 			pinId:		D/C
 */
static void rxCallback(PIN_Handle hPin, PIN_Id pinId)
{
	static uint_t dataValue,parity =1, bit = 0, resendRequest = 0;
	static char key;

	delay_us(17);
	dataValue=PINCC26XX_getInputValue(KB_DATA);

	if (bit == 0) {
		key = 0;
		parity = 1;
		bit++;
		if (dataValue == 1) {
			resendRequest = 1;
		}
	} else if (bit > 0 && bit < 9) {
		key |= dataValue << (bit - 1);
		parity ^= dataValue;
		bit++;
	} else if (bit == 9) {
		if (parity != dataValue || (key > SCAN_CODE_SET_2_TO_HID_END && key != BOARD_TEST_PASSED && key != BOARD_ACK && key != BREAK_CODE && key != EXT_CODE)) {System_printf("P:%d\n",key);
			resendRequest = 1;
		}
		bit++;
	} else if (bit == 10) {
		PIN_setConfig(keyboardPinsHandler, PIN_BM_IRQ, KB_CLK	| PIN_IRQ_POSEDGE);
		PIN_registerIntCb(keyboardPinsHandler,rxCompleteCallback);
		rxTxBusy = 0;

		if (resendRequest == 1){
			resendRequest = 0;
			txRequestPending = 1;
			txRequestEvent = BOARD_RESEND_SEND_EVT;
		} else if (key == BOARD_TEST_PASSED || key == BOARD_ACK) {
			txRequestPending = 1;
			txRequestEvent = BOARD_RESPONSE_SEND_EVT;
			txRequestResponse = key;
		} else {
			bufferWrite(key);System_printf("%d\n",key);
			if (key != BREAK_CODE && key != EXT_CODE) {
				Semaphore_post(boardSemaphoreHandle);
				rxTxBusy = 0;
			}
		}
		bit = 0;
	}
}

/*********************************************************************
 * @fn      txCallback
 *
 * @brief   callback function for Host to Device transfer
 *
 * @param   hPin:		D/C
 * 			pinId:		D/C
 */
static void txCallback(PIN_Handle hPin, PIN_Id pinId) {
	static uint_t data_value, parity = 1, bit = 0;

	if (bit < 8) {
		data_value = (tx_byte >> bit) & 0x1;
		PINCC26XX_setOutputValue(KB_DATA, data_value);
		parity ^= data_value;
		bit++;
	} else if (bit == 8) {
		PINCC26XX_setOutputValue(KB_DATA, parity);
		bit++;
	} else if (bit == 9) {
		PIN_setConfig(keyboardPinsHandler, PIN_BM_GPIO_OUTPUT_EN	, KB_DATA | PIN_GPIO_OUTPUT_DIS);
		PIN_setConfig(keyboardPinsHandler, PIN_BM_INPUT_EN			, KB_DATA | PIN_INPUT_EN);
		bit++;
	} else if (bit == 10) {
		bit = 0;
		parity = 1;
		stopTx();
	}
}

/*********************************************************************
 * API FUNCTIONS
 */

void Keyboard_init(keysPressedCB_t appKeyCB){

  LED_changeState(Board_LED0, 1);

  // Initialize KEY pins. Enable int after callback registered
  keyboardPinsHandler = PIN_open(&keyboardPins, keyboardPinsCfg);

  // Set the application callback
  appKeyChangeHandler = appKeyCB;

  // Create semaphore
  createSemaphore();

  // Create task
  createTask();

  // Reset keyboard
  resetKeyboard();
}

void Keyboard_changeLedState(uint8_t state){
	latestLedState = state;
	pendingLedRequest = 1;
}
