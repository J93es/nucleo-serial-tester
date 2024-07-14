/*
 * init.c
 */

#include "init.h"

#include "main.h"
#include "custom_delay.h"
#include "custom_gpio.h"
#include "custom_oled.h"
#include "custom_switch.h"
#include "custom_exception.h"
#include "custom_filesystem.h"



uint8_t doWriting = DO_NOT_WRITING_SIG;
uint8_t doReading = DO_NOT_READING_SIG;





/*
 * syscalls.c에 있는 _io_putchar 재정의
 */
int __io_putchar(int ch)
{
	while (!LL_USART_IsActiveFlag_TXE(USART2));
	LL_USART_TransmitData8(USART2, (char)ch);
	return ch;
}




void Read_State_Machine(uint8_t cntlSig) {

	static uint8_t	readState = READ_STATE_IDLE;
	static uint8_t	readCnt = 0;
	int32_t	readData;

	if (cntlSig == RESET_SIG) {

		readCnt = 0;
		doReading = DO_NOT_READING_SIG;

		readState = READ_STATE_IDLE;
		Custom_OLED_Printf("/0READ IDLE");
	}

	switch(readState) {

		case READ_STATE_IDLE:
			if (doReading == DO_READING_SIG) {

				Custom_OLED_Printf("/0READ IDLE");
				readState = READ_STATE_DOING;
			}

			break;



		case READ_STATE_DOING:
			Custom_OLED_Printf("/0READ DO  ");
			readData = LL_USART_ReceiveData8(USART2);
			Custom_OLED_Printf("/1%dth read: %d", ++readCnt, readData);
			readState = READ_STATE_DONE;

			break;



		case READ_STATE_DONE:
			Custom_OLED_Printf("/0READ DONE");
			doReading = DO_NOT_READING_SIG;
			readState = READ_STATE_IDLE;

			break;
	}
}




void Write_State_Machine(uint8_t writeData, uint8_t cntlSig) {

	static uint8_t	writeState = WRITE_STATE_IDLE;
	static uint8_t	writeCnt = 0;

	if (cntlSig == RESET_SIG) {

		writeCnt = 0;
		doReading = DO_NOT_READING_SIG;

		writeState = WRITE_STATE_IDLE;
		Custom_OLED_Printf("/4WRITE IDLE");
	}

	switch(writeState) {

		case WRITE_STATE_IDLE:
			if (doWriting == DO_WRITING_SIG) {

				Custom_OLED_Printf("/4WRITE IDLE");
				writeState = WRITE_STATE_DOING;
			}

			break;



		case WRITE_STATE_DOING:
			Custom_OLED_Printf("/4WRITE DO  ");
			LL_USART_TransmitData8(USART2, writeData);
			Custom_OLED_Printf("/5%dth write: %4d", ++writeCnt, writeData);
			writeState = WRITE_STATE_DONE;

			break;



		case WRITE_STATE_DONE:
			Custom_OLED_Printf("/4WRITE DONE");
			doWriting = DO_NOT_WRITING_SIG;
			writeState = WRITE_STATE_IDLE;

			break;
	}
}





void UART_Loopback() {

	static int i = 0;
	uint8_t writeData = 1;
	uint8_t cntlSig = NORMAL_SIG;
	uint8_t sw = Custom_Switch_Read();


	if (sw == CUSTOM_SW_BOTH) {
		cntlSig = RESET_SIG;
	}

	else if (sw == CUSTOM_SW_1) {

		doWriting = DO_WRITING_SIG;
		writeData = i++;
	}

	else if (sw == CUSTOM_SW_2) {

		doReading = DO_READING_SIG;
	}

	Write_State_Machine((uint8_t)writeData, cntlSig);
	Read_State_Machine(cntlSig);

}



void Init() {
	/*
	 * STM 보드와 컴퓨터 간 UART 통신을 통해 컴퓨터 터미널로 디버깅할 수 있도록 USART2를 활성화한다.
	 */
	LL_USART_Enable(USART2);

	/*
	* 1ms 주기로 동작하는 SysTick을 기화한다. custom_delay.h 파일의 Custom_Delay_Get_SysTick
	* 현재 SvsTick이 초기화된 시점부터 경과한 시간을 구할 수 있다.
	*/
	Custom_Delay_Init_SysTick();

	/*
	 * OLED를 사용하기 전에는 Custom_OLED_Init 함수를 호출하여 여러가지 초기화를 수행해야 한다.
	 * 이 함수는 OLED 처음 쓰기 전에 딱 한 번만 호출하면 된다.
	 */
	Custom_OLED_Init();

	/*
	 * 플래시를 사용하기 전에는 Custom_FileSystem_Load 함수를 호출하여 플래시 정보를 불러와야 한다.
	 * 이 함수는 플래시를 처음 쓰기 전에 딱 한 번만 호출하면 된다.
	 */
	Custom_FileSystem_Load();

	/**
	 * Custom_OLED_Printf 함수는 C언어에서 printf와 동일하게 동작한다.
	 * 즉, %d, %f 등의 서식 문자를 사용하여 숫자를 출력할 수 있다.
	 * 다만 특수한 기능들 추가되어있는데, /0이라는 부분이 있으면 첫 번째 줄의 첫 번째 칸으로 돌아가고,
	 * /1이라는 부분이 있으면 두 번째 줄의 첫 번째 칸으로 돌아간다.
	 * 그리고 /r, /g, /b라는 부분이 있으면 각각 문자를 빨강, 초록, 파랑으로 출력한다.
	 * 즉, 아래 예제에서는 첫 번째 줄에 "Hello"를 출력한 후, 두 번째 줄의 첫 번째 칸으로 커서가 이동하고 파란색 글씨로 "ZETIN!"을 쓴다.
	 */
	Custom_OLED_Init();
	Custom_OLED_Printf("/0Hello, /1/bZETIN!");
	Custom_Delay_ms(1000);


	while(1) {

		UART_Loopback();

	}
}
