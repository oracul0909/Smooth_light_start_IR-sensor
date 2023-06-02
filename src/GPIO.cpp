#include "Arduino.h"
#include "GPIO.h"
#include "util/delay.h"

#define PIR PB0       // пин датчика движения
#define PWM PB1       // ШИМ пин ключа транзистора
#define BRIGHT PB4    // пин фоторезистора
#define PWM_VAL OCR0B // значение шима
#define PWM_SPEED 20
#define PIR_DETECT (PINB & ((1 << PIR)))

/*
Инициализация пинов контроллера
*/
void GPIO_init(void)
{
    setPort_OUTPUT(PORTB, PIR); // пин PB0 порта B как вход (PORTB)
    setPort_OUTPUT(PORTB, BRIGHT);
    setPort_INPUT(DDRB, PWM); // пин PB1 порта B как выход (DDRB)
    PORTB &= ~((1 << PIR));   // опустить бит PB0 порта B
    PORTB &= ~((1 << BRIGHT));
}

/*
Инициализация АЦП
*/
void ADC_init(void)
{
    cli();
    ADMUX = 0x00;
    ADCSRA = 0x00;
    ADMUX |= ((0 << REFS0)) | ((1 << ADLAR)) | ((1 << MUX1)) | ((0 << MUX0));
    ADCSRA |= ((1 << ADEN)) | ((1 << ADSC)) | ((1 << ADIE));
    sei();
}

/*
Запуск ШИМ
*/
void PWM_start(void)
{
    TCCR0A |= ((1 << COM0B0)) | ((1 << COM0B1)) | ((1 << WGM01)) | ((1 << WGM00)); // настройки выхода процессора (режим работы таймера)
    TCCR0B |= ((1 << CS02));                                                       // настройка таймера (WGM - режим работы, CS02 - частота)
}

/*
Остановка ШИМ
*/
void PWM_stop(void)
{
    TCCR0A |= ((0 << COM0B0)) | ((0 << COM0B1)) | ((0 << WGM01)) | ((0 << WGM00)); // настройки выхода процессора (режим работы таймера)
    TCCR0B |= ((0 << CS02));                                                       // настройка таймера (WGM - режим работы, CS02 - частота)
}

void Tim_0_init(void)
{
    PWM_start();
    PWM_VAL = 0xFF; // установка заполнения ШИМ (0 - нет, 255 - максимальное заполнение)
    // PORTB |= ((1<<PB0));                   //поднять бит PB0 порта B
}

/*
Плавный пуск/остановка освещения по срабатыванию датчика движения
*/
void Smooth_light_run(void)
{
    if (Light_control())
    {
        if (PIR_DETECT && PWM_VAL > 0)
        {
            PWM_VAL--;
            _delay_ms(PWM_SPEED);
        }
        if (!PIR_DETECT && PWM_VAL < 255)
        {
            PWM_VAL++;
            _delay_ms(PWM_SPEED);
        }
    }
    else
    {
        if(PWM_VAL < 255)
        {
            PWM_VAL++;
            _delay_ms(PWM_SPEED);
        }
    }
}

/*
Контроль естественной освещенности помещения
*/
bool Light_control(void)
{
    static bool flag;
    if (ADCH < 120 && flag == 1)
        flag = 0;
    if (ADCH > 140 && flag == 0)
        flag = 1;
    if (flag)
        return true;
    else
        return false;
}

//непрерывный опрос аналогового входа датчика освещенности через прерывания
static volatile uint8_t ADC_value;
ISR(ADC_vect)
{
    ADC_value = ADCH;
    ADCSRA |= ((1 << ADSC));
}
