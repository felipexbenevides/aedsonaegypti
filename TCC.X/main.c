/* 
 * File:   main.c
 * Author: Edson Felipe Costa Benevides
 *
 * Created on 19 de Junho de 2018, 20:32
 */
#include "18F4550.h"
#device adc=10
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN,PUT,BROWNOUT
#use delay(CLOCK=48M, CRYSTAL=20M) 
#use rs232 (baud = 115200, xmit=pin_c6, rcv=pin_c7)

#include<usb_cdc.h>

#define ADC_CHANNEL 0
#define RESET_TIMER0 18660
#define RESET_TIMER1 317



int32 digital_value;
int change;


//Prototipos
void init_adc(void);
void init_serial(void);
void init_interrupt(void);
void convert(void);

/*------------------------------------
 * GLOBAL
 *------------------------------------*/
int flag, flag1;
int timer1_control;

/*------------------------------------
 * INTERRUPT
 *------------------------------------*/
/*
 * Timer0
 * para 1 segundo o timer disparado 1 vez com inicializacao
 * de 18660 definido em RESET_TIMER0
 * @ CLOCK=48M; CRYSTAL=20M; T0_DIV_BY_256
 */
#int_timer0
void trata_t0(){
    disable_interrupts(GLOBAL);
    flag = 1;
    set_timer0(RESET_TIMER0);
    enable_interrupts(GLOBAL); 
}
/*
 * Timer1
 * para 1 segundo o timer disparado 23 vezes com inicializacao
 * de 317 definido em RESET_TIMER1
 * @ CLOCK=48M; CRYSTAL=20M; T1_DIV_BY_8
 */
#int_timer1
void trata_t1(){
    disable_interrupts(GLOBAL);
    timer1_control++;
    if(timer1_control == 23){
        flag1 = 1;
        timer1_control = 0;
    }
    set_timer0(RESET_TIMER1);
    enable_interrupts(GLOBAL); 
}

/*------------------------------------
 * MAIN
 *------------------------------------*/
int main() {
    int temp = 8;
    //INIT
    init_serial();
    init_interrupt();
    init_adc();  
    
    flag = 0;
    flag1 = 0;
    //TRISB OUTPUT
    set_tris_b(0b00000000);
    output_low(PIN_B0);
 
    //Loop
    while(1) {       
        usb_task();
        if(usb_enumerated()){
           printf(usb_cdc_putc, "ADC0\r\n");
        }
        if(flag1){ 
            //PULSE
            output_high(PIN_B0);
            delay_ms(10);
            output_low(PIN_B0);
            flag1 = 0;
            if(usb_enumerated()){
               printf(usb_cdc_putc, "PULSE\r\n");
            }
        }
        delay_us(10);
    }
    return 0;
}
/*------------------------------------
 *INIT
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


void init_serial(void){
    
}

void init_interrupt(void){
    setup_timer_0(T0_INTERNAL | RTCC_DIV_256 );
    setup_timer_1(T1_INTERNAL | T1_DIV_BY_8 );
    enable_interrupts(GLOBAL);
    enable_interrupts(INT_TIMER0);
    enable_interrupts(INT_TIMER1);
    //Para 1 segundo set_timer0([65535 - FOSC/(4*PRESCALER)])
    set_timer0(RESET_TIMER0);
    set_timer1(RESET_TIMER1);    
}

void convert(void){
    //CONVERSAO A/D
    digital_value = read_adc();
    if(change == 1){
        //printf("%04d ", (int)digital_value/4);
        change = 0;
    }
    
}