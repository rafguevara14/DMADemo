#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

#define LED_PIN 22
#define POT_PIN 27
#define POT_CHANNEL 1

#define PWM_MAX_COUNT 100

#define ADC_RESOLUTION 12

void init_led(){
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
}

uint init_pwm(){

	gpio_set_function(LED_PIN, GPIO_FUNC_PWM);

	// set to slowest frequency possible
	uint slice_num = pwm_gpio_to_slice_num(LED_PIN);
	pwm_set_clkdiv_int_frac(slice_num, 255, 0);

	// set wrap value to 100 so that channel value 
	// level directly corresponds to duty cycle
	pwm_set_wrap(slice_num, PWM_MAX_COUNT);
	pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);

	pwm_set_enabled(slice_num, true);

	return slice_num;
}
 
void init_adc(){
	adc_init();
	adc_gpio_init(POT_PIN);
	adc_select_input(POT_CHANNEL);
}

int main(){

	stdio_init_all();

	init_led();

	uint slice_num = init_pwm();

	init_adc();

	while (1){

		uint16_t raw_data = adc_read();

		uint duty_cycle = 100 * (raw_data) / ((1 << ADC_RESOLUTION)-1);

		pwm_set_chan_level(slice_num, PWM_CHAN_A, duty_cycle);

		sleep_ms(500);
	}
}
