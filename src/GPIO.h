#ifndef GPIO_h
#define GPIO_h

#define F_CPU 96000000UL  //задаем частоту МК по datasheet
#define setPort_INPUT(x,y) x|=(1<<y)    //макрос для быстрого назначения пина порта
#define setPort_OUTPUT(x,y) x|=(1<<y)

void GPIO_init(void);
void ADC_init(void);
void PWM_start(void);
void PWM_stop(void);
void Tim_0_init(void);
void Smooth_light_run(void);
bool Light_control(void);


#endif