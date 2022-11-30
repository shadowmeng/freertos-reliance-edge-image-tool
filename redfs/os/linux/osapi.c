#include <redfs.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>

pthread_mutex_t mutexLock;

REDSTATUS RedOsClockInit(void)
{
    return 0;
}

uint32_t RedOsClockGetTime(void)
{
    /*  FreeRTOS does not provide an RTC abstraction since most of the systems
        it targets have no RTC hardware.  If your hardware includes an RTC that
        you would like to use, this function must be customized.
    */
    return 0;
}

uint32_t RedOsTaskId(void)
{
	return syscall(SYS_gettid);
}

REDSTATUS RedOsClockUninit(void)
{
    return 0;
}


REDSTATUS RedOsMutexInit(void)
{
	pthread_mutex_init(&mutexLock, NULL);
}

REDSTATUS RedOsMutexUninit(void)
{
	pthread_mutex_destroy(&mutexLock);
}

void RedOsMutexAcquire(void)
{
	pthread_mutex_lock(&mutexLock);
}

void RedOsMutexRelease(void)
{
	pthread_mutex_unlock(&mutexLock);
}
