#include "stdio.h"
#include "init.h"
#include "usbh_conf.h"
#include "usbh_hid.h"
#include "usbh_core.h"
#include "ff_gen_drv.h"
#include "usbh_diskio.h"

void USBH_UserProcess (USBH_HandleTypeDef *phost, uint8_t id);
void read_flash();

USBH_HandleTypeDef husbh;
HID_MOUSE_Info_TypeDef* mouseinfo;
float xorigin, yorigin;
int8_t xnow, ynow;
int msc = 0;
int hid = 0;


char mynewdiskPath[4];

int main()
{
	Sys_Init();

	USBH_Init(&husbh, USBH_UserProcess, 0);
	FATFS_LinkDriver(&USBH_Driver, mynewdiskPath);
	USBH_RegisterClass(&husbh, USBH_HID_CLASS);
	USBH_RegisterClass(&husbh, USBH_MSC_CLASS);
	USBH_Start(&husbh);

	while (1)
	{
	 /*Application background process */
	 /* USB Host process : should be called in the main loop to handle host
	 stack*/
		USBH_Process(&husbh);
		if (msc)
		{
			read_flash(mynewdiskPath);
			msc = 0;
		}
		else if (hid)
		{
			USBH_Process(&husbh);
		}


	}
}


void USBH_UserProcess (USBH_HandleTypeDef *phost, uint8_t id)
{
	switch (id)
	{
		case HOST_USER_DISCONNECTION:
			printf("HID is disconnected. \r\n");
			//Appli_state = APPLICATION_DISCONNECT;
			msc = 0;
			hid = 0;
			break;

/* when HOST_USER_CLASS_ACTIVE event is received, application can start
 communication with device*/
		case HOST_USER_CLASS_ACTIVE:
			//Appli_state = APPLICATION_READY;
			if(phost->pActiveClass == USBH_MSC_CLASS)
			{
				printf("MSC device detected\r\n");
				msc = 1;
			}
			if(phost->pActiveClass == USBH_HID_CLASS){
				printf("HID device detected\r\n");
				hid = 1;
			}
			break;

		case HOST_USER_CONNECTION:
			printf("USB is conected. \r\n");
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


void read_flash(char* path)
{
	//FRESULT res;
	FATFS mynewdiskFatFs;
	DIR dp;
	FILINFO fno;

	if(f_mount(&mynewdiskFatFs, (TCHAR const*)path, 1) == FR_OK) // mount the flash
	{
		printf("Mount success! \r\n");
	}
	if(f_opendir(&dp, (TCHAR const*)path) == FR_OK) // open the top directory
	{
		while(1)
		{
			if(f_readdir(&dp, &fno) != FR_OK || fno.fname[0] == 0)  // read the top directory until end
			{
				printf("end of the driver. \r\n");
				break;
			}
			else
			{
				if (fno.fattrib & AM_DIR)  // fno data is a directory 
				{
					//printf("%s", fno.fname);
					printf("%s is a directory. \r\n", fno.fname);
				}
				else // fno data is a file 
				{
					printf("%s is a file with size: %d \r\n", fno.fname, fno.fdate, fno.fsize);
				}
			}
		}
	}

}

