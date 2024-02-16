#include "my_adc_thread.h"
#include "stdio.h"

adc_data_size_t adc_data = 100;
bool flag = false;  //flag to notify the ADC result is updated
static ioport_level_t g_led1 = IOPORT_LEVEL_HIGH;
bsp_leds_t Leds;

ioport_level_t toggle_led_level(ioport_level_t level);

/* MyAdc Thread entry function */
void my_adc_thread_entry(void)
{

    R_BSP_LedsGet (&Leds);

    while (1)
    {
        ssp_err_t status;
        // Initialize ADC Driver
        status = g_adc.p_api->open (g_adc.p_ctrl, g_adc.p_cfg);
        if (status != SSP_SUCCESS)
            while (1)
                ;

        //Configure ADC Channel
        status = g_adc.p_api->scanCfg (g_adc.p_ctrl, g_adc.p_channel_cfg);
        if (status != SSP_SUCCESS)
            while (1)
                ;
        while (1)
        {
            //Start the ADC scan
            status = g_adc.p_api->scanStart (g_adc.p_ctrl);
            if (status == SSP_SUCCESS)
            {
                //Check scan status
                status = g_adc.p_api->scanStatusGet (g_adc.p_ctrl);
            }

            if (SSP_SUCCESS == status)
            {
                // Stop the scan
                g_adc.p_api->scanStop (g_adc.p_ctrl);
            }

            if (flag == true)
            {
                g_led1 = toggle_led_level (g_led1);
                g_ioport.p_api->pinWrite (Leds.p_leds[BSP_LED_LED1], g_led1);
                flag = false;
                //printf("Test\n");
                tx_thread_sleep (100);
            }
        }
        //Close the channel
        g_adc.p_api->close (g_adc.p_ctrl);
    }
}
/* Callback function */
void adc_user_callback(adc_callback_args_t *p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);
    g_adc.p_api->read (g_adc.p_ctrl, ADC_REG_CHANNEL_0, &adc_data);
    flag = true;
}

ioport_level_t toggle_led_level(ioport_level_t level)
{
    return level == IOPORT_LEVEL_HIGH ? IOPORT_LEVEL_LOW : IOPORT_LEVEL_HIGH;
}


