#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

#define ONBOARD_LED 25
#define ONBOARD_LED_SLICE 4
#define ONBOARD_LED_CHANNEL PWM_CHAN_B

#define EXTERNAL_LED 22
#define EXTERNAL_LED_SLICE 3 // pg 524 rp2040 datasheet
#define EXTERNAL_LED_CHANNEL PWM_CHAN_A

#define INDICATOR EXTERNAL_LED
#define INDICATOR_SLICE EXTERNAL_LED_SLICE

#define OUTPUT_LED ONBOARD_LED
#define OUTPUT_SLICE ONBOARD_LED_SLICE
#define OUTPUT_CHANNEL ONBOARD_LED_CHANNEL

#define POT_PIN 27
#define POT_CHANNEL 1

// matched with the max adc value so that the raw adc
// value directly maps to a PWM duty cycle
#define PWM_MAX_COUNT 4095 

#define ADC_RESOLUTION 12
#define ADC_IRQ_FIFO 22 // pg 60 rp2040 datasheet

void init_led(){
	gpio_init(EXTERNAL_LED);
	gpio_set_dir(EXTERNAL_LED, GPIO_OUT);

	gpio_init(ONBOARD_LED);
	gpio_set_dir(ONBOARD_LED, GPIO_OUT);
}

void init_pwm(){

	gpio_set_function(OUTPUT_LED, GPIO_FUNC_PWM);

	// set to slowest frequency possible
	pwm_set_clkdiv_int_frac(OUTPUT_SLICE, 255, 0);

	// set wrap value to 100 so that channel value 
	// level directly corresponds to duty cycle
	pwm_set_wrap(OUTPUT_SLICE, PWM_MAX_COUNT);
	pwm_set_chan_level(OUTPUT_SLICE, OUTPUT_CHANNEL, 0);

	pwm_set_enabled(OUTPUT_SLICE, true);
}
 
void adc_callback();
 
void init_adc(){

	adc_init();

	adc_gpio_init(POT_PIN);

	adc_select_input(POT_CHANNEL);

	adc_fifo_setup( 
		true, // enable FIFO
		false, // disable DMA
		1, // trigger irq on one sample
		false, // disable error sample flag
		true // shift down to 8 bytes
	);

	adc_set_clkdiv(1E20); // arbitrary large value to be slow enough

	// enable interrupt

	adc_irq_set_enabled(true);

	irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_callback);

	irq_set_enabled(ADC_IRQ_FIFO, true);
}


int main(){

	init_led();
	init_pwm();
	init_adc();

	adc_run(true);

	while (1){
		tight_loop_contents();
	}
}

void adc_callback(){

	uint16_t raw_data = adc_fifo_get();
	adc_fifo_drain(); // resets event

	pwm_set_chan_level(OUTPUT_SLICE, OUTPUT_CHANNEL, raw_data);

	// toggle gpio indicator to measure sampling rate
	gpio_xor_mask(1 << INDICATOR);

	irq_clear(ADC_IRQ_FIFO);
}
