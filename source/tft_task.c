/******************************************************************************
*
* File Name: tft_task.c
*
* Description: This file contains task and functions related to the tft-task
* for the Water Feature Controller
*******************************************************************************/

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "GUI.h"
#include "mtb_st7789v.h"
#include "mtb_light_sensor.h"
#include "tft_task.h"
#include "FreeRTOS.h"
#include "task.h"

/* The pins are defined by the st7789v library. If the display is being used
 *  on different hardware the mappings will be different. */
const mtb_st7789v_pins_t tft_pins =
{
    .db08 = CYBSP_J2_2,
    .db09 = CYBSP_J2_4,
    .db10 = CYBSP_J2_6,
    .db11 = CYBSP_J2_10,
    .db12 = CYBSP_J2_12,
    .db13 = CYBSP_D7,
    .db14 = CYBSP_D8,
    .db15 = CYBSP_D9,
    .nrd  = CYBSP_D10,
    .nwr  = CYBSP_D11,
    .dc   = CYBSP_D12,
    .rst  = CYBSP_D13
};

#define LIGHT_SENSOR_PIN (CYBSP_A0)

cyhal_adc_t adc;
mtb_light_sensor_t light_sensor;

/*******************************************************************************
* Forward Function Prototypes
*******************************************************************************/

/*******************************************************************************
* Function Name: void tft_task(void *arg)
********************************************************************************
*
* Parameters:
*  arg: task argument
*
* Return:
*  None
*
*******************************************************************************/
void tft_task(void *arg)
{
    cy_rslt_t result;

    /* Initialize the User Button */
    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Initialize the display controller */
    result = mtb_st7789v_init8(&tft_pins);
    CY_ASSERT(result == CY_RSLT_SUCCESS);
    
    /* Initialize the adc and light sensor */
      result = cyhal_adc_init(&adc, LIGHT_SENSOR_PIN, NULL);
      CY_ASSERT(result == CY_RSLT_SUCCESS);

      result = mtb_light_sensor_init(&light_sensor, &adc, LIGHT_SENSOR_PIN);
      CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* To avoid compiler warning */
    (void)result;
    
    bool tdState = 0;
    bool pdState = 0;

    uint8_t light = mtb_light_sensor_light_level(&light_sensor);

    GUI_Init();
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
    GUI_SetTextMode(GUI_TM_NORMAL);
    GUI_SetFont(&GUI_Font32B_1);
    GUI_DispStringHCenterAt("Water Feature" , 160, 50);
    GUI_DispStringHCenterAt("Controller" , 160, 90);
    GUI_SetFont(&GUI_Font16B_1);

    for(;;)
    {
    	light = mtb_light_sensor_light_level(&light_sensor);
    	GUI_DispStringAt("Ambient Light:  ", 100, 150);   //90,180
    	GUI_DispDec(light, 3);

    	tdState = cyhal_gpio_read(CYBSP_A7);
    	pdState = cyhal_gpio_read(CYBSP_A15);
    	cyhal_gpio_write(CYBSP_USER_LED, tdState);
    	cyhal_gpio_write(CYBSP_USER_LED2, pdState);

    	if (!tdState)
    	{
    		GUI_DispStringAt("Presence Detected", 100, 170);
    		if (pdState)
    		{
				GUI_DispStringAt("Approaching", 100, 190);
			} else
			{
				GUI_ClearRect(90, 190, 250, 250);
			}
    	} else
    	{
    		GUI_ClearRect(90, 170, 250, 250);
    	}


    	cyhal_system_delay_ms(100);
    }
}

/*******************************************************************************
* Function Name: void wait_for_switch_press_and_release(void)
********************************************************************************
*
* Summary: This implements a simple "Wait for button press and release"
*           function.  It first waits for the button to be pressed and then
*           waits for the button to be released.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  This is a blocking function and exits only on a button press and release
*
*******************************************************************************/
void wait_for_switch_press_and_release(void)
{
    /* Wait for SW2 to be pressed */
    while(cyhal_gpio_read(CYBSP_USER_BTN) != CYBSP_BTN_PRESSED);

    /* Wait for SW2 to be released */
    while(cyhal_gpio_read(CYBSP_USER_BTN) == CYBSP_BTN_OFF);
}

/* END OF FILE */
