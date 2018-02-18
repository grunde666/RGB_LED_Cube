#ifndef TIMER_H
#define TIMER_H

/* Dispatcher function to be called from main context */
void timer_dispatch(void);
/* Initialization of timer module */
void timer_init(void);

#endif
