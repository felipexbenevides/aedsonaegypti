/* 
 * File:   main.c
 * Author: Edson Felipe Costa Benevides
 *
 * Created on 19 de Junho de 2018, 20:32
 */
#include "18F4550.h"
#device adc=10
#fuses HS,NOWDT,PUT
#use delay(crystal=8mhz)
#use rs232 (baud = 115200, xmit=pin_c6, rcv=pin_c7)

#define ADC_CHANNEL 0

int32 digital_value;
int change;

//Prototipos
void init_adc(void);
void init_serial(void);
void init_interrupt(void);
void convert(void);

#int_timer0
void trata_t0(){
    disable_interrupts(GLOBAL);
    //PULSE
    output_high(PIN_B0);
    output_low(PIN_B0);
    //RESET TIMER
    change = 1;
    set_timer0(65135); //4301 Hz
    enable_interrupts(GLOBAL); 
}

/*------------------------------------
 * MAIN
 *------------------------------------*/
int main() {
    //INIT
    init_serial();
    init_interrupt();
    init_adc();  

    //TRISB OUTPUT
    set_tris_b(0b00000000);
    
    //Teste serial
    printf("Teste serial, baud = 115200 ");
    while(1) {       
        delay_us(10);
        convert();
    }
    return 0;
}
/*------------------------------------
 *INIT ADC
 *------------------------------------*/
void init_adc(void){
    //modo adc
    setup_adc(ADC_CLOCK_INTERNAL);
    //entrada analogica
    setup_adc_ports(AN0);
    //canal de conversao
    set_adc_channel(ADC_CHANNEL);
    delay_us(10);
}

/*------------------------------------
 *INIT SERIAL
 *------------------------------------*/
void init_serial(void){
    
}

/*------------------------------------
 *INIT INTERRUPT
 *------------------------------------*/
void init_interrupt(void){
    setup_timer_0(RTCC_INTERNAL | RTCC_DIV_1 );
    enable_interrupts(GLOBAL | INT_TIMER0);
    set_timer0(0);
    //Para 1 segundo set [65535 - FOSC/(4*PRESCALER)]
    set_timer0(65135);    
}

void convert(void){
    //CONVERSAO A/D
    digital_value = read_adc();
    if(change == 1){
        printf("%04d ", (int)digital_value/4);
        change = 0;
    }
    
}