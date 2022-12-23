#include "stdio.h"
#include "init.h"
#include "usbh_conf.h"
#include "usbh_hid.h"
#include "usbh_core.h"
#include "ff_gen_drv.h"
#include "usbh_diskio.h"

void USBH_UserProcess (USBH_HandleTypeDef *phost, uint8_t id);

USBH_HandleTypeDef hUSBHost;
HID_MOUSE_Info_TypeDef* mouseinfo;
float xorigin, yorigin;
int8_t xnow, ynow;


int main()
{
	Sys_Init();

	USBH_Init(&hUSBHost, USBH_UserProcess, 0);
	USBH_RegisterClass(&hUSBHost, USBH_HID_CLASS);
	USBH_Start(&hUSBHost);

	while (1)
	{
	 /*Application background process */
	 /* USB Host process : should be called in the main loop to handle host
	 stack*/
		USBH_Process(&hUSBHost);
	}
}


void USBH_UserProcess (USBH_HandleTypeDef *phost, uint8_t id)
{
	switch (id)
	{
		case HOST_USER_DISCONNECTION:
			printf("HID is disconnected. \r\n");
			//Appli_state = APPLICATION_DISCONNECT;
			break;

/* when HOST_USER_CLASS_ACTIVE event is received, application can start
 communication with device*/
		case HOST_USER_CLASS_ACTIVE:
			//Appli_state = APPLICATION_READY;
			break;

		case HOST_USER_CONNECTION:
			printf("HID is conected. \r\n");
			//Appli_state = APPLICATION_START;
			break;
		default:
			break;
	}
}

void USBH_HID_EventCallback(USBH_HandleTypeDef *phost) {
	HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;

	if (HID_Handle->Init == USBH_HID_MouseInit) // device is mouse
	{
		mouseinfo = USBH_HID_GetMouseInfo(phost); // gets three variables: x, y, and button[left, right, middle]
		xnow = mouseinfo->x;
		ynow = mouseinfo->y;
		xorigin += xnow * 0.02;    //change the cursor speed
		yorigin += ynow * 0.0015;
		printf("\033[%d;%dH", (uint8_t)yorigin, (uint8_t)xorigin);
		fflush(stdout);
		if (mouseinfo->buttons[0]) // left click
		{
			printf("O");
			fflush(stdout);
		}
		else if (mouseinfo->buttons[1]) // left click
		{
			printf(" ");
			fflush(stdout);
		}
		fflush(stdout);
	}
}

