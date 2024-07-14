/*
 * init.h
 */

#ifndef INC_INIT_H_
#define INC_INIT_H_



#include <stdint.h>
#include <stdbool.h>


// 공용 매크로
#define CUSTOM_FALSE				0
#define CUSTOM_TRUE					1
#define ABS(x) 						( ((x) < 0) ? (-1 * (x)) : (x) )
#define GET_MIN(x, y)				( ((x) < (y)) ? (x) : (y) )


#define MAX_OLED_LEN	15


#define TIME_MAX	20
#define TIME_MIN	5



#define READ_STATE_IDLE		0
#define READ_STATE_DOING	1
#define READ_STATE_DONE		2

#define WRITE_STATE_IDLE	0
#define WRITE_STATE_DOING	1
#define WRITE_STATE_DONE	2



#define DO_READING_SIG		0
#define DO_NOT_READING_SIG	1

#define DO_WRITING_SIG		0
#define DO_NOT_WRITING_SIG	1



#define NORMAL_SIG			0
#define RESET_SIG			1




void	Init(void);





typedef struct	s_menuData
{
	char menuName[MAX_OLED_LEN];
	void (*func)(void);
}				t_menuData;







#endif /* INC_INIT_H_ */
