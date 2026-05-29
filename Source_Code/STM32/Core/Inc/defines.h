#ifndef __DEFINES_H__
#define __DEFINES_H__

#define micro_times_ellapse     DWT->CYCCNT / (SystemCoreClock / 1000000)
#define PRIORITY_LOW            1
#define PRIORITY_LOW_MEDIUM     2
#define PRIORITY_MEDIUM         3
#define PRIORITY_MEDIUM_HIGH    4
#define PRIORITY_HIGH           5
#define PRIORITY_VERY_HIGH      6

#endif
