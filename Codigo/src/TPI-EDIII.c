/*
 * @brief Sistema de control de iluminación en un recinto.
 *
 * @note Modulos Perifericos utilizados: ADC, DMA, UART2, TIMER0, TIMER1
 * */

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_uart.h"

#define INPUT	(uint8_t) 0
#define OUTPUT	(uint8_t) 1
#define PORT_0	(uint8_t) 0
#define PORT_2	(uint8_t) 2
#define	PIN_0	((uint32_t) (1<<0))
#define	PIN_1	((uint32_t) (1<<1))
#define	PIN_2	((uint32_t) (1<<2))
#define	PIN_3	((uint32_t) (1<<3))
#define	PIN_4	((uint32_t) (1<<4))
#define	PIN_5	((uint32_t) (1<<5))
#define	PIN_6	((uint32_t) (1<<6))
#define PIN_22  ((uint32_t) (1<<22))

#define NUM_DISPLAYS	(uint8_t) 4
#define NUM_MUESTRAS_PWM (uint8_t) 100

volatile uint16_t set_point = 1234;
volatile uint16_t adc_muestras = 0;
volatile uint8_t  duty_cycle = 80;

void cfgGPIO(void);
void cfgADC(void);
void cfgTimer(void);
void actDisplays(uint8_t nDisplays);

int main(void) {

    cfgGPIO();
    cfgADC();
    cfgTimer();

    while (1) {}

    return 0;
}

void cfgGPIO(void) {

	/* Pines de los display's */
	PINSEL_CFG_Type cfgPinQ1; //P0.0
	PINSEL_CFG_Type cfgPinQ2; //P0.1
	PINSEL_CFG_Type cfgPinQ3; //P0.2
	PINSEL_CFG_Type cfgPinQ4; //P0.3

	PINSEL_CFG_Type cfgPinA; //P2.0
	PINSEL_CFG_Type cfgPinB; //P2.1
	PINSEL_CFG_Type cfgPinC; //P2.2
	PINSEL_CFG_Type cfgPinD; //P2.3
	PINSEL_CFG_Type cfgPinE; //P2.4
	PINSEL_CFG_Type cfgPinF; //P2.5
	PINSEL_CFG_Type cfgPinG; //P2.6

	// P0.0
	cfgPinQ1.Portnum = PINSEL_PORT_0;
	cfgPinQ1.Pinnum = PINSEL_PIN_0;
	cfgPinQ1.Funcnum = PINSEL_FUNC_0;
	cfgPinQ1.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinQ1.OpenDrain = PINSEL_PINMODE_NORMAL;

	// P0.1
	cfgPinQ2.Portnum = PINSEL_PORT_0;
	cfgPinQ2.Pinnum = PINSEL_PIN_1;
	cfgPinQ2.Funcnum = PINSEL_FUNC_0;
	cfgPinQ2.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinQ2.OpenDrain = PINSEL_PINMODE_NORMAL;

	// P0.2
	cfgPinQ3.Portnum = PINSEL_PORT_0;
	cfgPinQ3.Pinnum = PINSEL_PIN_2;
	cfgPinQ3.Funcnum = PINSEL_FUNC_0;
	cfgPinQ3.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinQ3.OpenDrain = PINSEL_PINMODE_NORMAL;

	// P0.3
	cfgPinQ4.Portnum = PINSEL_PORT_0;
	cfgPinQ4.Pinnum = PINSEL_PIN_3;
	cfgPinQ4.Funcnum = PINSEL_FUNC_0;
	cfgPinQ4.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinQ4.OpenDrain = PINSEL_PINMODE_NORMAL;

	// P2.0
	cfgPinA.Portnum = PINSEL_PORT_2;
	cfgPinA.Pinnum = PINSEL_PIN_0;
	cfgPinA.Funcnum = PINSEL_FUNC_0;
	cfgPinA.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinA.OpenDrain = PINSEL_PINMODE_NORMAL;

	// P2.1
	cfgPinB.Portnum = PINSEL_PORT_2;
	cfgPinB.Pinnum = PINSEL_PIN_1;
	cfgPinB.Funcnum = PINSEL_FUNC_0;
	cfgPinB.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinB.OpenDrain = PINSEL_PINMODE_NORMAL;

	// P2.2
	cfgPinC.Portnum = PINSEL_PORT_2;
	cfgPinC.Pinnum = PINSEL_PIN_2;
	cfgPinC.Funcnum = PINSEL_FUNC_0;
	cfgPinC.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinC.OpenDrain = PINSEL_PINMODE_NORMAL;

	// P2.3
	cfgPinD.Portnum = PINSEL_PORT_2;
	cfgPinD.Pinnum = PINSEL_PIN_3;
	cfgPinD.Funcnum = PINSEL_FUNC_0;
	cfgPinD.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinD.OpenDrain = PINSEL_PINMODE_NORMAL;

	// P2.4
	cfgPinE.Portnum = PINSEL_PORT_2;
	cfgPinE.Pinnum = PINSEL_PIN_4;
	cfgPinE.Funcnum = PINSEL_FUNC_0;
	cfgPinE.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinE.OpenDrain = PINSEL_PINMODE_NORMAL;

	// P2.5
	cfgPinF.Portnum = PINSEL_PORT_2;
	cfgPinF.Pinnum = PINSEL_PIN_5;
	cfgPinF.Funcnum = PINSEL_FUNC_0;
	cfgPinF.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinF.OpenDrain = PINSEL_PINMODE_NORMAL;

	// P2.6
	cfgPinG.Portnum = PINSEL_PORT_2;
	cfgPinG.Pinnum = PINSEL_PIN_6;
	cfgPinG.Funcnum = PINSEL_FUNC_0;
	cfgPinG.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinG.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinQ1);
	PINSEL_ConfigPin(&cfgPinQ2);
	PINSEL_ConfigPin(&cfgPinQ3);
	PINSEL_ConfigPin(&cfgPinQ4);

	GPIO_SetDir(PORT_0, PIN_0, OUTPUT);
	GPIO_SetDir(PORT_0, PIN_1, OUTPUT);
	GPIO_SetDir(PORT_0, PIN_2, OUTPUT);
	GPIO_SetDir(PORT_0, PIN_3, OUTPUT);

	PINSEL_ConfigPin(&cfgPinA);
	PINSEL_ConfigPin(&cfgPinB);
	PINSEL_ConfigPin(&cfgPinC);
	PINSEL_ConfigPin(&cfgPinD);
	PINSEL_ConfigPin(&cfgPinE);
	PINSEL_ConfigPin(&cfgPinF);
	PINSEL_ConfigPin(&cfgPinG);

	GPIO_SetDir(PORT_2, PIN_0, OUTPUT);
	GPIO_SetDir(PORT_2, PIN_1, OUTPUT);
	GPIO_SetDir(PORT_2, PIN_2, OUTPUT);
	GPIO_SetDir(PORT_2, PIN_3, OUTPUT);
	GPIO_SetDir(PORT_2, PIN_4, OUTPUT);
	GPIO_SetDir(PORT_2, PIN_5, OUTPUT);
	GPIO_SetDir(PORT_2, PIN_6, OUTPUT);

	/* Pines de los Timers*/
	PINSEL_CFG_Type cfgPinTMR0MAT1;
	PINSEL_CFG_Type cfgPinTMR1MAT0;
	PINSEL_CFG_Type cfgPinTMR2MAT0;

	/* P1.22 como MAT1.0 */
	cfgPinTMR1MAT0.Portnum = PINSEL_PORT_1;
	cfgPinTMR1MAT0.Pinnum = PINSEL_PIN_22;
	cfgPinTMR1MAT0.Funcnum = PINSEL_FUNC_3;
	cfgPinTMR1MAT0.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinTMR1MAT0.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinTMR1MAT0);

	// Configuración del pin P1.29 MAT01
	cfgPinTMR0MAT1.Portnum = PINSEL_PORT_1;
	cfgPinTMR0MAT1.Pinnum = PINSEL_PIN_29;
	cfgPinTMR0MAT1.Funcnum = PINSEL_FUNC_3;
	cfgPinTMR0MAT1.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinTMR0MAT1.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinTMR0MAT1);

	/* P0.6 como MAT2.0 */
	cfgPinTMR2MAT0.Portnum = PINSEL_PORT_0;
	cfgPinTMR2MAT0.Pinnum = PINSEL_PIN_6;
	cfgPinTMR2MAT0.Funcnum = PINSEL_FUNC_3;
	cfgPinTMR2MAT0.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinTMR2MAT0.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinTMR2MAT0);

	/* Pin del ADC */
	PINSEL_CFG_Type cfgPinADC;

	// Configuración del pin P0.23 ADC Channel 0
	cfgPinADC.Portnum = PINSEL_PORT_0;
	cfgPinADC.Pinnum = PINSEL_PIN_23;
	cfgPinADC.Funcnum = PINSEL_FUNC_1;
	cfgPinADC.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinADC.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinADC);

	/* Pin del la lampara led +12v*/
	PINSEL_CFG_Type cfgPinLampara;

	// Configuración del LED en P0.4
	cfgPinLampara.Portnum = PINSEL_PORT_0;
	cfgPinLampara.Pinnum = PINSEL_PIN_4;
	cfgPinLampara.Funcnum = PINSEL_FUNC_0;
	cfgPinLampara.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinLampara.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinLampara);
	GPIO_SetDir(PORT_0, PIN_4, OUTPUT);
	GPIO_SetValue(PORT_0, PIN_4);

	/* Pin del Led Testigo */
	PINSEL_CFG_Type cfgPinLED;

	// Configuración del LED en P0.23
	cfgPinLED.Portnum = PINSEL_PORT_0;
	cfgPinLED.Pinnum = PINSEL_PIN_22;
	cfgPinLED.Funcnum = PINSEL_FUNC_0;
	cfgPinLED.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinLED.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinLED);
	GPIO_SetDir(PORT_0, PIN_22, OUTPUT);
	GPIO_SetValue(PORT_0, PIN_22);

    return;
}

void cfgTimer(void){

	/* Configuración del TMR0 en modo match1 - (TMR0MR1 cada 1s) */
	TIM_TIMERCFG_Type cfgTMR0Mode;
	TIM_MATCHCFG_Type cfgTMR0MAT1;

	// TMR0
	cfgTMR0Mode.PrescaleOption = TIM_PRESCALE_USVAL;
	cfgTMR0Mode.PrescaleValue = 1000; // 1 ms

	// MATCH 1
	cfgTMR0MAT1.MatchChannel = 1;
	cfgTMR0MAT1.MatchValue = 499;
	cfgTMR0MAT1.IntOnMatch = DISABLE;
	cfgTMR0MAT1.ResetOnMatch = ENABLE;
	cfgTMR0MAT1.StopOnMatch = DISABLE;
	cfgTMR0MAT1.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &cfgTMR0Mode);
	TIM_ConfigMatch(LPC_TIM0, &cfgTMR0MAT1);
	TIM_Cmd(LPC_TIM0, ENABLE);

	/* Configuración del TMR1 en modo match - (TMR1 cada 1ms) */
	TIM_TIMERCFG_Type cfgTMR1Mode;
	TIM_MATCHCFG_Type cfgTMR1MAT0;

	// TMR1
	cfgTMR1Mode.PrescaleOption = TIM_PRESCALE_USVAL;
	cfgTMR1Mode.PrescaleValue = 1000; // 1 ms

	// MATCH 0
	cfgTMR1MAT0.MatchChannel = 0;
    cfgTMR1MAT0.MatchValue = 1; // 1ms
	cfgTMR1MAT0.IntOnMatch = ENABLE;
	cfgTMR1MAT0.ResetOnMatch = ENABLE;
	cfgTMR1MAT0.StopOnMatch = DISABLE;
	cfgTMR1MAT0.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;

	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &cfgTMR1Mode);
	TIM_ConfigMatch(LPC_TIM1, &cfgTMR1MAT0);
	TIM_Cmd(LPC_TIM1, ENABLE);

	NVIC_EnableIRQ(TIMER1_IRQn);

	/* Configuración del TMR2 en modo match - (TMR2 cada 10us) */
	TIM_TIMERCFG_Type cfgTMR2Mode;
	TIM_MATCHCFG_Type cfgTMR2MAT0;

	// TMR2
	cfgTMR2Mode.PrescaleOption = TIM_PRESCALE_USVAL;
	cfgTMR2Mode.PrescaleValue = 1;

	// MATCH 0
	cfgTMR2MAT0.MatchChannel = 0;
	cfgTMR2MAT0.MatchValue = 10;
	cfgTMR2MAT0.IntOnMatch = ENABLE;
	cfgTMR2MAT0.ResetOnMatch = ENABLE;
	cfgTMR2MAT0.StopOnMatch = DISABLE;
	cfgTMR2MAT0.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;

	TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &cfgTMR2Mode);
	TIM_ConfigMatch(LPC_TIM2, &cfgTMR2MAT0);
	TIM_Cmd(LPC_TIM2, ENABLE);

	NVIC_EnableIRQ(TIMER2_IRQn);

	return;
}

void cfgADC(void){
	ADC_Init(LPC_ADC, 200000);

	ADC_BurstCmd(LPC_ADC, DISABLE);
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);
	ADC_ChannelCmd(LPC_ADC, 0, ENABLE);
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_RISING);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, ENABLE);

	NVIC_EnableIRQ(ADC_IRQn);

	return;
}

void TIMER1_IRQHandler(void){
	static uint8_t nDisplay = 0;

	if(TIM_GetIntStatus(LPC_TIM1, TIM_MR0_INT)){
		actDisplays(nDisplay);
		nDisplay = (nDisplay + 1) % NUM_DISPLAYS;

		TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
	}

	return;
}

void TIMER2_IRQHandler(void){
	static uint8_t current = 0;

	if(TIM_GetIntStatus(LPC_TIM2, TIM_MR0_INT)){
		if(current > duty_cycle){
			GPIO_SetValue(PORT_0, PIN_4);
		}else{
			GPIO_ClearValue(PORT_0, PIN_4);
		}

		current = (current + 1) % NUM_MUESTRAS_PWM;

		TIM_ClearIntPending(LPC_TIM2, TIM_MR0_INT);
	}

	return;
}

void ADC_IRQHandler(void){
	adc_muestras =  ADC_ChannelGetData(LPC_ADC, 0);

	return;
}

void actDisplays(uint8_t nDisplays){
	static uint8_t valor[4] = {0, 0, 0, 0};		// mil, cen, dec, uni

	valor[0] = set_point / 1000;
	valor[1] = (set_point % 1000) / 100;
	valor[2] = (set_point % 100) / 10;
	valor[3] = set_point % 10;

	switch(nDisplays){
		case 0:
			GPIO_SetValue(PORT_0, PIN_0);
			GPIO_ClearValue(PORT_0, PIN_1);
			GPIO_ClearValue(PORT_0, PIN_2);
			GPIO_ClearValue(PORT_0, PIN_3);
			break;
		case 1:
			GPIO_SetValue(PORT_0, PIN_1);
			GPIO_ClearValue(PORT_0, PIN_0);
			GPIO_ClearValue(PORT_0, PIN_2);
			GPIO_ClearValue(PORT_0, PIN_3);
			break;
		case 2:
			GPIO_SetValue(PORT_0, PIN_2);
			GPIO_ClearValue(PORT_0, PIN_0);
			GPIO_ClearValue(PORT_0, PIN_1);
			GPIO_ClearValue(PORT_0, PIN_3);
			break;
		case 3:
			GPIO_SetValue(PORT_0, PIN_3);
			GPIO_ClearValue(PORT_0, PIN_0);
			GPIO_ClearValue(PORT_0, PIN_1);
			GPIO_ClearValue(PORT_0, PIN_2);
			break;
		default:
			break;
	}

	switch(valor[nDisplays]){
		case 0:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_1);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_SetValue(PORT_2, PIN_3);
			GPIO_SetValue(PORT_2, PIN_4);
			GPIO_SetValue(PORT_2, PIN_5);
			GPIO_ClearValue(PORT_2, PIN_6);
			break;
		case 1:
			GPIO_ClearValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_1);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_ClearValue(PORT_2, PIN_3);
			GPIO_ClearValue(PORT_2, PIN_4);
			GPIO_ClearValue(PORT_2, PIN_5);
			GPIO_ClearValue(PORT_2, PIN_6);
			break;
		case 2:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_1);
			GPIO_ClearValue(PORT_2, PIN_2);
			GPIO_SetValue(PORT_2, PIN_3);
			GPIO_SetValue(PORT_2, PIN_4);
			GPIO_ClearValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		case 3:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_1);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_SetValue(PORT_2, PIN_3);
			GPIO_ClearValue(PORT_2, PIN_4);
			GPIO_ClearValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		case 4:
			GPIO_ClearValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_1);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_ClearValue(PORT_2, PIN_3);
			GPIO_ClearValue(PORT_2, PIN_4);
			GPIO_SetValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		case 5:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_ClearValue(PORT_2, PIN_1);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_SetValue(PORT_2, PIN_3);
			GPIO_ClearValue(PORT_2, PIN_4);
			GPIO_SetValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		case 6:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_ClearValue(PORT_2, PIN_1);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_SetValue(PORT_2, PIN_3);
			GPIO_SetValue(PORT_2, PIN_4);
			GPIO_SetValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		case 7:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_1);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_ClearValue(PORT_2, PIN_3);
			GPIO_ClearValue(PORT_2, PIN_4);
			GPIO_ClearValue(PORT_2, PIN_5);
			GPIO_ClearValue(PORT_2, PIN_6);
			break;
		case 8:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_1);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_SetValue(PORT_2, PIN_3);
			GPIO_SetValue(PORT_2, PIN_4);
			GPIO_SetValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		case 9:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_1);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_ClearValue(PORT_2, PIN_3);
			GPIO_ClearValue(PORT_2, PIN_4);
			GPIO_SetValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		default:
			break;
	}

	return;
}
