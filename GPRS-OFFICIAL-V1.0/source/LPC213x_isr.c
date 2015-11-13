#include "LPC213x_isr.h"
#include <LPC213x.H>
#include <string.h>
#include "main.h"
#include "wdt.h"

extern _program_counter program_counter;
extern _system_flag system_flag;
extern _uart1_rx_frame uart1_rx;
extern unsigned char buffer_PLC[];
extern unsigned char check_frame;
extern unsigned char start_frame;
extern unsigned char stop_frame;
extern unsigned char stop_frame_immediately;

/******************************************************************************/
/*            LPC213x Peripherals Interrupt Handlers                        */
/******************************************************************************/
/**
 * @brief  This function handles TIM0 global interrupt request.
 * @param  None
 * @retval None
 */
__irq void myTimer0_ISR(void) {
	long int regVal;
	//volatile static uint16_t timer_read_command,timer_send_socket;
	regVal = T0IR; // read the current value in T0's Interrupt Register

	//Clear_Watchdog();
	program_counter.timer_send_socket++;
	program_counter.timer_delay++;
	program_counter.timer_wait_response_login++;

	if (program_counter.timer_send_socket >= 20 * TIME_SEND_SOCKET) {
		program_counter.timer_send_socket = 0;
		system_flag.bits.SEND_SOCKET = 1;
	}
	//----------------
	if (program_counter.timer_wait_response_login
			>= 20 * TIME_WAIT_RESPONSE_LOGIN) {
		program_counter.timer_wait_response_login = 0;
		system_flag.bits.TIMEOUT_WAIT_LOGIN = 1;
	}
	GPIO_Toggled(GPIO_P0, GPIO_PIN_18);
	//----------------
	T0IR = regVal; // write back to clear the interrupt flag
	VICVectAddr = 0x0; // Acknowledge that ISR has finished execution
}
//-------------------------------------------------------------------------------------------
__irq void myTimer1_ISR(void) {
	long int regVal;
	regVal = T1IR; // read the current value in T0's Interrupt Register

	T1IR = regVal; // write back to clear the interrupt flag
	VICVectAddr = 0x0; // Acknowledge that ISR has finished execution
}

/******************************************************************************/
/*            LPC213x Peripherals Interrupt Handlers                        */
/******************************************************************************/
/**
 * @brief  This function handles UART0 global interrupt request.
 */
__irq void myUart0_ISR(void) {
	uint8_t in_data, LSRValue;
	static bool start_OK = false;
	static uint32_t index = 0;
	LSRValue = U0LSR;
	in_data = U0RBR; // dummy read
	/* Receive Line Status */
	if (LSRValue & (LSR_OE | LSR_PE | LSR_FE | LSR_RXFE | LSR_BI)) {
		/* There are errors or break interrupt */
		/* Read LSR will clear the interrupt */
		VICVectAddr = 0; /* Acknowledge Interrupt */
		return;
	}
	if (LSRValue & LSR_RDR) /* Receive Data Ready */
	{
		/* If no error on RLS, normal ready, save into the data buffer. */
		/* Note: read RBR will clear the interrupt */
		//Recieve Data Available Interrupt has occured
		if (!start_OK) {
			if (in_data == start_frame) {
				if (stop_frame_immediately == true) {
					check_frame = true;
				} else {
					start_OK = true;
					index = 0;
				}
			}
		} else {
			if (in_data != stop_frame) {
				buffer_PLC[index++] = in_data;
				if (index >= MAX_BUFFER)
					index = 0;
			} else {
				buffer_PLC[index++] = 0;
				check_frame = true;
				start_OK = false;
			}
		}
	}

	VICVectAddr = 0x0; // Acknowledge that ISR has finished execution
}
/******************************************************************************/
/*            LPC213x Peripherals Interrupt Handlers                        */
/******************************************************************************/
/**
 * @brief  This function handles UART1 global interrupt request.
 * @param  None
 * @retval None
 */
__irq void myUart1_ISR(void) {
	uint8_t regVal, LSRValue;
	static unsigned char counter_rx;

	LSRValue = U1LSR;
	regVal = U1RBR; // dummy read
	/* Receive Line Status */
	if (LSRValue & (LSR_OE | LSR_PE | LSR_FE | LSR_RXFE | LSR_BI)) {
		/* There are errors or break interrupt */
		/* Read LSR will clear the interrupt */
		VICVectAddr = 0; /* Acknowledge Interrupt */
		return;
	}
	if (LSRValue & LSR_RDR) { /* Receive Data Ready */
		/* If no error on RLS, normal ready, save into the data buffer. */
		/* Note: read RBR will clear the interrupt */
		//Recieve Data Available Interrupt has occured
		//regVal = U1RBR; // dummy read
		//-------------------------------------------------------------------
		counter_rx++;
		if (regVal == '%') {
			system_flag.bits.CHECKING_NEW_COMMAND = 1;
			counter_rx = 0;
		}
		if ((system_flag.bits.CHECKING_NEW_COMMAND) && (regVal == 'A')
				&& (counter_rx < 10)) {
			system_flag.bits.CHECKING_NEW_COMMAND = 0;
			uart1_rx.para_rx.unread++;
		}
		//-------------------------------------------------------------------
		switch (uart1_rx.para_rx.state_uart) {
		//case UART_DO_NOTHING:
		//break;
		//case UART_IDLE: 		//Uart is IDLE STATE

		//break;
		case UART_WAIT_COMMAND_RESPONSE: //Uart is WAIT RESPONSE from Module GPRS
			if ((uart1_rx.para_rx.flag.bits.RESPONSE_RECEIVING)
					&& (uart1_rx.para_rx.counter_rx < 70)) {
				uart1_rx.buffer_rx.buf_response_command[uart1_rx.para_rx.counter_rx] =
						regVal; //contain data to buffer
				uart1_rx.para_rx.counter_rx++; //increase counter
			}
			//-----*****-----*****
			if ((regVal == LF)
					&& (uart1_rx.para_rx.flag.bits.RESPONSE_RECEIVING) == 1) {
				//----------
				if (strstr(uart1_rx.buffer_rx.buf_response_command, "OK")) {
					uart1_rx.para_rx.flag.bits.RESPONSE_OK = 1;
					uart1_rx.para_rx.flag.bits.RESPONSE_ERROR = 0;
				} else if (strstr(uart1_rx.buffer_rx.buf_response_command,
						"ERROR")) {
					uart1_rx.para_rx.flag.bits.RESPONSE_OK = 0;
					uart1_rx.para_rx.flag.bits.RESPONSE_ERROR = 1;
				}
				if (strstr(uart1_rx.buffer_rx.buf_response_command,
						"CONNECT")) {
					uart1_rx.para_rx.flag.bits.CONNECT_OK = 1;
				}
				uart1_rx.para_rx.state_uart = UART_IDLE;
				uart1_rx.para_rx.flag.bits.IDLE_RECEIVING = 0;
			}
			//-----*****-----*****
			else if ((regVal == LF)
					&& (uart1_rx.para_rx.flag.bits.RESPONSE_RECEIVING) == 0) {
				uart1_rx.para_rx.flag.bits.RESPONSE_RECEIVING = 1;
				uart1_rx.para_rx.counter_rx = 0;
			}
			//*****-----
			break;
		case UART_WAIT_DATA_GPRS: //Uart is waiting Data after Read Command Data.
			if ((uart1_rx.para_rx.flag.bits.DATA_RECEIVING)
					&& (uart1_rx.para_rx.counter_rx < MAX_BUFFER_RX)) {
				uart1_rx.buffer_rx.buf_rx_server[uart1_rx.para_rx.counter_rx] =
						regVal; //contain data to buffer
				uart1_rx.para_rx.counter_rx++; //increase counter
			}
			//-----*****-----*****
			if ((regVal == LF)
					&& (uart1_rx.para_rx.flag.bits.DATA_RECEIVING) == 1) {
				uart1_rx.para_rx.flag.bits.DATA_RECEIVE_FINISH = 1;	//finish received data
				uart1_rx.para_rx.state_uart = UART_IDLE;//switch to idle state
				uart1_rx.para_rx.flag.bits.IDLE_RECEIVING = 0;
			} else if ((regVal == LF)
					&& (uart1_rx.para_rx.flag.bits.DATA_RECEIVING) == 0) {
				uart1_rx.para_rx.flag.bits.DATA_RECEIVING = 1; //start receive data
				uart1_rx.para_rx.counter_rx = 0; //reset counter rx
			}
			break;
		case UART_SEND_DATA:
			if (regVal == 'O')
				uart1_rx.para_rx.flag.bits.WAIT_SENDING = 1;
			else if ((uart1_rx.para_rx.flag.bits.WAIT_SENDING)
					&& (regVal == 'K')) {
				uart1_rx.para_rx.flag.bits.SEND_DATA_OK = 1;
				uart1_rx.para_rx.state_uart = UART_IDLE; //switch to idle state
			} else if ((uart1_rx.para_rx.flag.bits.WAIT_SENDING)
					&& (regVal == 'R')) {
				uart1_rx.para_rx.flag.bits.SEND_DATA_ERROR = 1;
				uart1_rx.para_rx.state_uart = UART_IDLE; //switch to idle state
			}
			break;
		case UART_CHECK_PARA:
			if ((uart1_rx.para_rx.flag.bits.CHECK_PARA_RECEIVING)
					&& (uart1_rx.para_rx.counter_rx < 70)) {
				uart1_rx.buffer_rx.buf_response_command[uart1_rx.para_rx.counter_rx] =
						regVal; //contain data to buffer
				uart1_rx.para_rx.counter_rx++; //increase counter
			} else if (uart1_rx.para_rx.counter_rx > 70) {
				uart1_rx.para_rx.counter_rx = 0;
				uart1_rx.para_rx.state_uart = UART_IDLE;
			}
			//-----*****-----*****
			if (regVal == uart1_rx.para_rx.head_need_rx) {
				uart1_rx.para_rx.counter_rx = 0;
				uart1_rx.para_rx.flag.bits.CHECK_PARA_RECEIVING = 1;
			}
			break;
		case UART_START_MODULE_GPRS:

			if (uart1_rx.para_rx.counter_rx < 70) {
				uart1_rx.buffer_rx.buf_response_command[uart1_rx.para_rx.counter_rx] =
						regVal;
				uart1_rx.para_rx.counter_rx++;
			}
			//*****-----
			break;
		default:
			break;
		}
		//-------------------------------------------------------------------
	}
	VICVectAddr = 0x0; // Acknowledge that ISR has finished execution
}
//-----------------------------------------------------------------------------------------------------
void DefaultVICHandler(void)
__irq {
	VICVectAddr = 0; /* Acknowledge Interrupt */
}

