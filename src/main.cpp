#include <Arduino.h>
#include "GPIO.h"


#define F_CPU 96000000UL


int main(void)
{
  GPIO_init();
  ADC_init();
  Tim_0_init();

  while(1)
  {
    Smooth_light_run();
  }
}