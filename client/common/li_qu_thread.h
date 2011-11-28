/* 
 * File:   li_qu_thread.h
 * Author: sysprog
 *
 * Created on November 28, 2011, 5:08 PM
 */

#ifndef LI_QU_THREAD_H
#define	LI_QU_THREAD_H

#ifdef	__cplusplus
extern "C" {
#endif

pthread_t get_quThread();
void* listener_thread();

#ifdef	__cplusplus
}
#endif

#endif	/* LI_QU_THREAD_H */

