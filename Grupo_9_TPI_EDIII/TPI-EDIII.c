#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_pwm.h"


#define INPUT	(uint8_t) 0
#define OUTPUT	(uint8_t) 1
#define PORT_0	(uint8_t) 0
#define PORT_2	(uint8_t) 2
#define PORT_3 	(uint8_t) 3
#define	PIN_0	((uint32_t) (1<<0))
#define	PIN_1	((uint32_t) (1<<1))
#define	PIN_2	((uint32_t) (1<<2))
#define	PIN_3	((uint32_t) (1<<3))
#define	PIN_4	((uint32_t) (1<<4))
#define	PIN_5	((uint32_t) (1<<5))
#define	PIN_6	((uint32_t) (1<<6))
#define PIN_7 	((uint32_t) (1<<7))
#define PIN_22  ((uint32_t) (1<<22))
#define PIN_26  ((uint32_t) (1<<26))

#define NUM_DISPLAYS	(uint8_t)	4
#define VREF	(float) 3.3
#define MAX_VALOR	(uint16_t) 4095
#define NUM_ELEMENTOS 	26

#define BUFFER_1_START	0x2007C000
#define BUFFER_2_START	0x2007C014
#define BUFFER_SIZE		10

#define PWM_PERIOD	(uint16_t) 100  // Periodo del PWM en ticks (1 KHz con prescaler de 100us)

void cfgGPIO(void);
void cfgTimer(void);
void cfgUART(void);
void cfgADC(void);
void cfgDMA(void);
void cfgPWM(void);
void sendData(uint16_t lux_muestras);
uint16_t calculoLux(uint16_t adc_muestra);
void actDisplays(uint8_t nDisplays);
uint8_t obtenerDutyCycle(void);
void ajustarIluminancia(void);

volatile uint16_t set_point = 0;
volatile uint16_t lux_actual = 0;
volatile uint16_t *lux_muestras = (uint16_t *)BUFFER_1_START;
volatile uint16_t *buffer_uart = (uint16_t *)BUFFER_2_START;

typedef struct{
	float vout;
	uint16_t lux;
	uint8_t	dc;
} Elemento;

static const Elemento tabla[] = {
    {2.68, 295, 0},
    {2.58, 286, 5},
    {2.48, 258, 20},
    {2.38, 231, 35},
    {2.28, 203, 38},
    {2.18, 181, 42},
    {2.08, 168, 45},
    {1.98, 156, 48},
    {1.88, 144, 52},
    {1.78, 134, 55},
    {1.68, 124, 58},
    {1.58, 112, 63},
    {1.48, 99, 67},
    {1.38, 84, 75},
    {1.28, 68, 82},
    {1.18, 53, 84},
    {1.08, 42, 88},
    {0.98, 32, 92},
    {0.88, 19, 96},
    {0.78, 10, 98},
    {0.68, 9, 98},
    {0.58, 8, 98},
    {0.48, 7, 99},
    {0.38, 6, 99},
    {0.28, 5, 100},
    {0.18, 4, 100},
};

int main(void) {

    cfgGPIO();
    cfgTimer();
    cfgPWM();
    cfgADC();
    cfgUART();

    while (1) {}

    return 0;
}

void cfgGPIO(void) {
	/* Pines de los display's */
	PINSEL_CFG_Type cfgPinQ; //Transistores de los displays
	PINSEL_CFG_Type cfgPinSeg; //Segmentos

	// P0.0
	cfgPinQ.Portnum = PINSEL_PORT_0;
	cfgPinQ.Pinnum = PINSEL_PIN_0;
	cfgPinQ.Funcnum = PINSEL_FUNC_0;
	cfgPinQ.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinQ.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinQ);
	GPIO_SetDir(PORT_0, PIN_0, OUTPUT);

	// P0.1
	cfgPinQ.Pinnum = PINSEL_PIN_1;

	PINSEL_ConfigPin(&cfgPinQ);
	GPIO_SetDir(PORT_0, PIN_1, OUTPUT);

	// P0.2
	cfgPinQ.Pinnum = PINSEL_PIN_2;

	PINSEL_ConfigPin(&cfgPinQ);
	GPIO_SetDir(PORT_0, PIN_2, OUTPUT);

	// P0.3
	cfgPinQ.Pinnum = PINSEL_PIN_3;

	PINSEL_ConfigPin(&cfgPinQ);
	GPIO_SetDir(PORT_0, PIN_3, OUTPUT);

	// P2.0
	cfgPinSeg.Portnum = PINSEL_PORT_2;
	cfgPinSeg.Pinnum = PINSEL_PIN_0;
	cfgPinSeg.Funcnum = PINSEL_FUNC_0;
	cfgPinSeg.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinSeg.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinSeg);
	GPIO_SetDir(PORT_2, PIN_0, OUTPUT);

	// P2.7
	cfgPinSeg.Pinnum = PINSEL_PIN_7;

	PINSEL_ConfigPin(&cfgPinSeg);
	GPIO_SetDir(PORT_2, PIN_7, OUTPUT);

	// P2.2
	cfgPinSeg.Pinnum = PINSEL_PIN_2;

	PINSEL_ConfigPin(&cfgPinSeg);
	GPIO_SetDir(PORT_2, PIN_2, OUTPUT);

	// P2.3
	cfgPinSeg.Pinnum = PINSEL_PIN_3;

	PINSEL_ConfigPin(&cfgPinSeg);
	GPIO_SetDir(PORT_2, PIN_3, OUTPUT);

	// P2.4
	cfgPinSeg.Pinnum = PINSEL_PIN_4;

	PINSEL_ConfigPin(&cfgPinSeg);
	GPIO_SetDir(PORT_2, PIN_4, OUTPUT);

	// P2.5
	cfgPinSeg.Pinnum = PINSEL_PIN_5;

	PINSEL_ConfigPin(&cfgPinSeg);
	GPIO_SetDir(PORT_2, PIN_5, OUTPUT);

	// P2.6
	cfgPinSeg.Pinnum = PINSEL_PIN_6;

	PINSEL_ConfigPin(&cfgPinSeg);
	GPIO_SetDir(PORT_2, PIN_6, OUTPUT);

	/* Pines de los Timers*/
	PINSEL_CFG_Type cfgPinTMR0MAT1;
	PINSEL_CFG_Type cfgPinTMR1MAT0;

	/* P1.29 como MAT0.1 */
	cfgPinTMR0MAT1.Portnum = PINSEL_PORT_1;
	cfgPinTMR0MAT1.Pinnum = PINSEL_PIN_29;
	cfgPinTMR0MAT1.Funcnum = PINSEL_FUNC_3;
	cfgPinTMR0MAT1.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinTMR0MAT1.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinTMR0MAT1);

	/* P1.22 como MAT1.0 */
	cfgPinTMR1MAT0.Portnum = PINSEL_PORT_1;
	cfgPinTMR1MAT0.Pinnum = PINSEL_PIN_22;
	cfgPinTMR1MAT0.Funcnum = PINSEL_FUNC_3;
	cfgPinTMR1MAT0.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinTMR1MAT0.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinTMR1MAT0);

	/* Pin del Led Testigo */
	PINSEL_CFG_Type cfgPinLEDRed;
	PINSEL_CFG_Type cfgPinLEDBlue;

	// Configuración del LED en P0.22
	cfgPinLEDRed.Portnum = PINSEL_PORT_0;
	cfgPinLEDRed.Pinnum = PINSEL_PIN_22;
	cfgPinLEDRed.Funcnum = PINSEL_FUNC_0;
	cfgPinLEDRed.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinLEDRed.OpenDrain = PINSEL_PINMODE_NORMAL;

	// Configuración del LED en P3.26
	cfgPinLEDBlue.Portnum = PINSEL_PORT_3;
	cfgPinLEDBlue.Pinnum = PINSEL_PIN_26;
	cfgPinLEDBlue.Funcnum = PINSEL_FUNC_0;
	cfgPinLEDBlue.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinLEDBlue.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinLEDRed);
	PINSEL_ConfigPin(&cfgPinLEDBlue);

	GPIO_SetDir(PORT_0, PIN_22, OUTPUT);
	GPIO_SetValue(PORT_0, PIN_22);

	GPIO_SetDir(PORT_3, PIN_26, OUTPUT);
	GPIO_SetValue(PORT_3, PIN_26);

	/* Pines UART2 */
	PINSEL_CFG_Type cfgTXD2;
	PINSEL_CFG_Type cfgRXD2;

	cfgTXD2.Portnum = PINSEL_PORT_0;
	cfgTXD2.Pinnum = PINSEL_PIN_10;
	cfgTXD2.Funcnum = PINSEL_FUNC_1;
	cfgTXD2.OpenDrain = PINSEL_PINMODE_NORMAL;

	cfgRXD2.Portnum = PINSEL_PORT_0;
	cfgRXD2.Pinnum = PINSEL_PIN_11;
	cfgRXD2.Funcnum = PINSEL_FUNC_1;
	cfgRXD2.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgTXD2);
	PINSEL_ConfigPin(&cfgRXD2);

	/* Pin del ADC */
	PINSEL_CFG_Type cfgPinADC;

	// Configuración del pin P0.23 ADC Channel 0
	cfgPinADC.Portnum = PINSEL_PORT_0;
	cfgPinADC.Pinnum = PINSEL_PIN_23;
	cfgPinADC.Funcnum = PINSEL_FUNC_1;
	cfgPinADC.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinADC.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinADC);

	/* PWM1.2 en P2.1 que no interfiere con displays */
	PINSEL_CFG_Type cfgPinPWM;

	// PWM1.2 en P2.1
	cfgPinPWM.Portnum = PINSEL_PORT_2;
	cfgPinPWM.Pinnum = PINSEL_PIN_1;
	cfgPinPWM.Funcnum = PINSEL_FUNC_1;  // PWM1.2
	cfgPinPWM.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinPWM.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgPinPWM);

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

	/* Configuración del TMR1 en modo match - (TMR1 cada 1ms) */
	TIM_TIMERCFG_Type cfgTMR1Mode;
	TIM_MATCHCFG_Type cfgTMR1MAT0;

	// TMR1
	cfgTMR1Mode.PrescaleOption = TIM_PRESCALE_USVAL;
	cfgTMR1Mode.PrescaleValue = 1; // 1 us

	// MATCH 0
	cfgTMR1MAT0.MatchChannel = 0;
	cfgTMR1MAT0.MatchValue = 1000; // 1ms
	cfgTMR1MAT0.IntOnMatch = ENABLE;
	cfgTMR1MAT0.ResetOnMatch = ENABLE;
	cfgTMR1MAT0.StopOnMatch = DISABLE;
	cfgTMR1MAT0.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;

	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &cfgTMR1Mode);
	TIM_ConfigMatch(LPC_TIM1, &cfgTMR1MAT0);
	TIM_Cmd(LPC_TIM1, ENABLE);

	NVIC_SetPriority(TIMER1_IRQn, 0);
	NVIC_EnableIRQ(TIMER1_IRQn);

	return;
}

void cfgPWM(void){
	PWM_TIMERCFG_Type cfgPWMTimer;
	PWM_MATCHCFG_Type cfgPWMMatch;

	// Configurar PWM en modo timer
	cfgPWMTimer.PrescaleOption = PWM_TIMER_PRESCALE_USVAL;
	cfgPWMTimer.PrescaleValue = 100;  // 100 us por tick

	PWM_Init(LPC_PWM1, PWM_MODE_TIMER, &cfgPWMTimer);

	// Configurar MR6 como duty cycle (canal PWM1.2)
	cfgPWMMatch.MatchChannel = 2;
	cfgPWMMatch.IntOnMatch = DISABLE;
	cfgPWMMatch.ResetOnMatch = DISABLE;
	cfgPWMMatch.StopOnMatch = DISABLE;

	PWM_ConfigMatch(LPC_PWM1, &cfgPWMMatch);

	PWM_ChannelConfig(LPC_PWM1, 2, PWM_CHANNEL_SINGLE_EDGE);

	PWM_ChannelCmd(LPC_PWM1, 2, ENABLE);

	PWM_MatchUpdate(LPC_PWM1, 0, PWM_PERIOD, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 2, 0, PWM_MATCH_UPDATE_NEXT_RST);

	PWM_CounterCmd(LPC_PWM1, ENABLE);
	PWM_ResetCounter(LPC_PWM1);
	PWM_Cmd(LPC_PWM1, ENABLE);

	return;
}

void cfgUART(void){
	UART_CFG_Type	cfgPinUART2;
	UART_FIFO_CFG_Type	cfgUART2FIFO;

	UART_ConfigStructInit(&cfgPinUART2);
	UART_Init(LPC_UART2, &cfgPinUART2);
	UART_FIFOConfigStructInit(&cfgUART2FIFO);
	UART_FIFOConfig(LPC_UART2, &cfgUART2FIFO);
	UART_IntConfig(LPC_UART2, UART_INTCFG_RBR, ENABLE);
	UART_TxCmd(LPC_UART2, ENABLE);

	NVIC_SetPriority(UART2_IRQn, 1);
	NVIC_EnableIRQ(UART2_IRQn);

	return;
}

void cfgADC(void){
	ADC_Init(LPC_ADC, 200000);

	ADC_BurstCmd(LPC_ADC, DISABLE);
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_RISING);
	ADC_ChannelCmd(LPC_ADC, 0, ENABLE);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, ENABLE);

	NVIC_SetPriority(ADC_IRQn, 2);
	NVIC_EnableIRQ(ADC_IRQn);

	TIM_Cmd(LPC_TIM0, ENABLE);

	return;
}

void cfgDMA(void){
	GPDMA_Channel_CFG_Type cfgDMA;

	NVIC_DisableIRQ(DMA_IRQn);
	GPDMA_Init();

	cfgDMA.ChannelNum = 7;
	cfgDMA.SrcConn = 0;
	cfgDMA.DstConn = 0;
	cfgDMA.SrcMemAddr = (uint32_t)lux_muestras;
	cfgDMA.DstMemAddr = (uint32_t)buffer_uart;
	cfgDMA.TransferType = GPDMA_TRANSFERTYPE_M2M;
	cfgDMA.TransferSize = BUFFER_SIZE;
	cfgDMA.TransferWidth = GPDMA_WIDTH_HALFWORD;
	cfgDMA.DMALLI = 0;

	GPDMA_Setup(&cfgDMA);

	NVIC_SetPriority(DMA_IRQn, 0);

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

void UART2_IRQHandler(void){
	static uint8_t received_flag = 0;
	uint8_t receivedData = 0;

	if(UART_GetLineStatus(LPC_UART2)& UART_LSR_RDR){

		receivedData = UART_ReceiveByte(LPC_UART2);

		if(received_flag == 0){
			set_point = (uint16_t) (receivedData << 8);
			received_flag = 1;
		} else if(received_flag == 1){
			set_point = set_point | (uint16_t) receivedData;
			received_flag = 0;
		}
	}

	return;
}

void ADC_IRQHandler(void){
	static uint8_t cMuestras = 0;
	uint16_t adc_muestra = 0;
	uint16_t lux_valor = 0;

	while(!(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_DONE)));

	adc_muestra = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
	lux_valor = calculoLux(adc_muestra);

	*((uint16_t *)(BUFFER_1_START) + cMuestras) = lux_valor;
	GPIO_ClearValue(PORT_0, PIN_22);

	ajustarIluminancia();

	cMuestras++;

	if(cMuestras >= BUFFER_SIZE){
		GPIO_SetValue(PORT_0, PIN_22);

		cfgDMA();

		NVIC_EnableIRQ(DMA_IRQn);
		GPDMA_ChannelCmd(7, ENABLE);

		cMuestras = 0;
	}

	return;
}

void DMA_IRQHandler(void){
	if(GPDMA_IntGetStatus(GPDMA_STAT_INT, 7)){
		if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 7)){
			for(uint8_t i = 0; i < BUFFER_SIZE; i++){
				uint16_t dato = *((uint16_t *)(BUFFER_1_START) + i);
				sendData(dato);
			}

			GPDMA_ClearIntPending(GPDMA_STATCLR_INTTC, 7);
		}
		if(GPDMA_IntGetStatus(GPDMA_STAT_INTERR, 7)){
			GPDMA_ClearIntPending(GPDMA_STATCLR_INTERR, 7);
		}
	}

	GPDMA_ChannelCmd(7, DISABLE);
	NVIC_DisableIRQ(DMA_IRQn);
	return;
}

void sendData(uint16_t lux_muestras){
	static uint8_t buffer[2] = {0, 0};

	buffer[0] = (uint8_t)(lux_muestras >> 8);	//Byte mas alto
	buffer[1] = (uint8_t)(lux_muestras & 0xFF);	//Byte mas bajo

	UART_Send(LPC_UART2, buffer, sizeof(buffer), BLOCKING);

	return;
}

uint16_t calculoLux(uint16_t adc_muestra){
	float vout = 0;
	uint16_t lux_valor = 0;

	vout = (adc_muestra * VREF) / (float)MAX_VALOR;

	if (vout >= tabla[0].vout){
		 return tabla[0].lux;
	 }

	if (vout <= tabla[NUM_ELEMENTOS - 1].vout){
	        return tabla[NUM_ELEMENTOS - 1].lux;
	    }

	for (int i = 0; i < NUM_ELEMENTOS - 1; i++){
		if ( (vout <= tabla[i].vout) && (vout > tabla[i+1].vout) ){
			lux_valor = tabla[i].lux;
			return lux_valor;
	    }
	}

	return 0;
}

uint8_t obtenerDutyCycle(void){

	if(set_point >= tabla[0].lux){
		return tabla[0].dc;
	}

	if(set_point <= tabla[NUM_ELEMENTOS - 1].lux){
		return tabla[NUM_ELEMENTOS - 1].dc;
	}

	for (int i = 0; i < NUM_ELEMENTOS - 1; i++){
		if ((set_point <= tabla[i].lux) && (set_point > tabla[i+1].lux) ){
			return tabla[i].dc;
		}
	}

	return 0;
}

void ajustarIluminancia(void){
	uint8_t duty_cycle = 0;

	duty_cycle = obtenerDutyCycle();

	uint32_t match_value = (PWM_PERIOD * duty_cycle) / 100;

	PWM_MatchUpdate(LPC_PWM1, 2, match_value, PWM_MATCH_UPDATE_NEXT_RST);

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
			GPIO_SetValue(PORT_2, PIN_7);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_SetValue(PORT_2, PIN_3);
			GPIO_SetValue(PORT_2, PIN_4);
			GPIO_SetValue(PORT_2, PIN_5);
			GPIO_ClearValue(PORT_2, PIN_6);
			break;
		case 1:
			GPIO_ClearValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_7);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_ClearValue(PORT_2, PIN_3);
			GPIO_ClearValue(PORT_2, PIN_4);
			GPIO_ClearValue(PORT_2, PIN_5);
			GPIO_ClearValue(PORT_2, PIN_6);
			break;
		case 2:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_7);
			GPIO_ClearValue(PORT_2, PIN_2);
			GPIO_SetValue(PORT_2, PIN_3);
			GPIO_SetValue(PORT_2, PIN_4);
			GPIO_ClearValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		case 3:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_7);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_SetValue(PORT_2, PIN_3);
			GPIO_ClearValue(PORT_2, PIN_4);
			GPIO_ClearValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		case 4:
			GPIO_ClearValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_7);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_ClearValue(PORT_2, PIN_3);
			GPIO_ClearValue(PORT_2, PIN_4);
			GPIO_SetValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		case 5:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_ClearValue(PORT_2, PIN_7);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_SetValue(PORT_2, PIN_3);
			GPIO_ClearValue(PORT_2, PIN_4);
			GPIO_SetValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		case 6:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_ClearValue(PORT_2, PIN_7);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_SetValue(PORT_2, PIN_3);
			GPIO_SetValue(PORT_2, PIN_4);
			GPIO_SetValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		case 7:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_7);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_ClearValue(PORT_2, PIN_3);
			GPIO_ClearValue(PORT_2, PIN_4);
			GPIO_ClearValue(PORT_2, PIN_5);
			GPIO_ClearValue(PORT_2, PIN_6);
			break;
		case 8:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_7);
			GPIO_SetValue(PORT_2, PIN_2);
			GPIO_SetValue(PORT_2, PIN_3);
			GPIO_SetValue(PORT_2, PIN_4);
			GPIO_SetValue(PORT_2, PIN_5);
			GPIO_SetValue(PORT_2, PIN_6);
			break;
		case 9:
			GPIO_SetValue(PORT_2, PIN_0);
			GPIO_SetValue(PORT_2, PIN_7);
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
