//GLOBAL INCLUDES HERE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//GENERAL DEFINES HERE
#define MOWTHREADOK 1
#define MOWTHREADERR 0
#define MOWTHREADTIMEOUT -1

#ifdef _WIN32
char MTHREAD_WIN_ERR_BUFFER[1024] = { 0 };
uint32_t MTHREAD_WIN_ERR_CODE = 0;
#define MOW_THREAD_STRERROR(...) MTHREAD_WIN_ERR_CODE = GetLastError();\
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,NULL,MTHREAD_WIN_ERR_CODE, \
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), MTHREAD_WIN_ERR_BUFFER,sizeof(MTHREAD_WIN_ERR_BUFFER),NULL);\
	MTHREAD_WIN_ERR_BUFFER[1023] = 0;\
	printf("\nERROR CODE: %u %s %s %d\n", MTHREAD_WIN_ERR_CODE, MTHREAD_WIN_ERR_BUFFER,__FILE__,__LINE__);\
MTHREAD_WIN_ERR_CODE = 0; *MTHREAD_WIN_ERR_BUFFER = 0
#else
#define MOW_THREAD_STRERROR(x) printf("\n%s %s %d\n",strerror(x),__FILE__,__LINE__)
#endif

//GENERAL DEBUG DEFINES HERE
#ifdef _DEBUG
#define MOW_THREAD_ERROR(x) printf("\nERROR %s %d: %s\n",__FILE__,__LINE__,x);

#else
#define MOW_THREAD_ERROR(x) ((void)0)
#endif


#ifdef _WIN32
#include <Windows.h>

#ifdef _DEBUG


#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
#else

#endif



//IF ANDROID INCLUDE ANDROID ONLY LIBRARIES
#if defined(__ANDROID__) && !defined(ANDROIDPRINT)
#define ANDROIDPRINT
#include <jni.h>
#include <android/log.h>
#include "android/android_native_app_glue.h"

//ANDROID ONLY DEFINES HERE


#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, "MOW", __VA_ARGS__);
#endif


//DEFINE MOWFILE VARIABLES HERE
typedef uint64_t* mowthread;
typedef void* mowmutex;

//DEFINE MOWFILE FUNCTIONS HERE
#ifdef _WIN32
mowthread mthread(uint32_t(*func)(void*), void* params);
int mthread_join(mowthread thread, uint32_t* retValue);	//ON WINDOWS THREAD EXIT CODE IS 32 BITS UNSIGNED
#else
mowthread mthread(void* (*func)(void*), void* params);
int mthread_join(mowthread thread, void** retValue);
#endif

int mthread_detach(mowthread thread);

mowmutex mmutex_init();

int mmutex_lock(mowmutex mutex);

int mmutex_unlock(mowmutex mutex);

//IMPLEMENT MOW FUNCTIONS HERE

#ifdef _WIN32
mowmutex mmutex_init() {
	HANDLE mutex = CreateSemaphoreA(NULL, 1, 1, NULL);
	if (NULL == mutex) {
		MOW_THREAD_STRERROR();
		return MOWTHREADERR;
	}
	return (mowmutex)mutex;
}
int mmutex_lock(mowmutex mutex) {
	if (NULL == mutex) {
		MOW_THREAD_ERROR("mmutex_lock mutex is NULL\n");
		return MOWTHREADERR;
	}
	DWORD wfso = WaitForSingleObject((HANDLE)mutex, INFINITE);
	if (0 != wfso) {
		MOW_THREAD_STRERROR();
		return MOWTHREADERR;
	}return MOWTHREADOK;
}
int mmutex_unlock(mowmutex mutex) {
	if (NULL == mutex) {
		MOW_THREAD_ERROR("mmutex_lock mutex is NULL\n");
		return MOWTHREADERR;
	}
	if (0 == ReleaseSemaphore((HANDLE)mutex, 1, NULL)) {
		MOW_THREAD_STRERROR();
		return MOWTHREADERR;
	}return MOWTHREADOK;
}
int mmutex_destroy(mowmutex mutex) {
	if (NULL == mutex) {
		MOW_THREAD_ERROR("mmutex_lock mutex is NULL\n");
		return MOWTHREADERR;
	}
	if (0 == CloseHandle((HANDLE)mutex)) {
		MOW_THREAD_STRERROR();
		return MOWTHREADERR;
	}return MOWTHREADOK;
}
#else
mowmutex mmutex_init() {
	pthread_mutex_t* mutex = calloc(1, sizeof(pthread_mutex_t));
	if (NULL == mutex) {
		MOW_THREAD_ERROR("mmutex_init can't allocate memory\n");
		return NULL;
	}
	int r = pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
	if (0 != r) {
		MOW_THREAD_STRERROR(r);
		return NULL;
	}
	return (mowmutex)mutex;
}
int mmutex_lock(mowmutex mutex) {
	if (NULL == mutex) {
		MOW_THREAD_ERROR("mmutex_lock mutex is NULL\n");
		return MOWTHREADERR;
	}
	int r = pthread_mutex_lock((pthread_mutex_t*)mutex);
	if (0 != r) {
		MOW_THREAD_STRERROR(r);
		return MOWTHREADERR;
	}return MOWTHREADOK;
}
int mmutex_unlock(mowmutex mutex) {
	if (NULL == mutex) {
		MOW_THREAD_ERROR("mmutex_lock mutex is NULL\n");
		return MOWTHREADERR;
	}
	int r = pthread_mutex_unlock((pthread_mutex_t*)mutex);
	if (0 != r) {
		MOW_THREAD_STRERROR(r);
		return MOWTHREADERR;
	}return MOWTHREADOK;
}
int mmutex_destroy(mowmutex mutex) {
	if (NULL == mutex) {
		MOW_THREAD_ERROR("mmutex_lock mutex is NULL\n");
		return MOWTHREADERR;
	}
	int r = pthread_mutex_destroy((pthread_mutex_t*)mutex);
	if (0 != r) {
		MOW_THREAD_STRERROR(r);
		return MOWTHREADERR;
	}
	free(mutex);
	return MOWTHREADOK;
}
#endif

int mthread_detach(mowthread thread) {
	int ret = MOWTHREADERR;
	if (NULL == mthread) {
		printf("mthread_detach thread is NULL\n");
		return MOWTHREADERR;
	}
#ifdef _WIN32
	ret = CloseHandle(thread);	//If the function succeeds, the return value is nonzero.
	if (0 == ret) {
		goto MTHREADDETACH_ERROR;
	}
#else
	ret = pthread_detach(thread);	//On success, pthread_detach() returns 0; on error, it returns an error number.
	if (0 != ret) {
		goto MTHREADDETACH_ERROR;
	}
#endif
	return MOWTHREADOK;
MTHREADDETACH_ERROR:
	MOW_THREAD_STRERROR(ret);
	return MOWTHREADERR;
}


#ifdef _WIN32
int mthread_join(mowthread thread, uint32_t* retValue) {
	if (NULL == thread) {
		MOW_THREAD_ERROR("mthread_join thread is null\n");
		return MOWTHREADERR;
	}
	DWORD wfso = WaitForSingleObject(thread, INFINITE);
	uint32_t ret = MOWTHREADERR;
	if (WAIT_FAILED == wfso) {
		goto MTHREADJOIN_ERROR;
	}
	/*else if (WAIT_TIMEOUT == wfso) {
		ret = MOWTHREADTIMEOUT;
		goto MTHREADJOIN_ERROR;
	}*/
	else if (0 != wfso) {
		goto MTHREADJOIN_ERROR;
	}
	if (FALSE == GetExitCodeThread(thread, (LPDWORD)&ret)) {
		goto MTHREADJOIN_ERROR;
	}
	if (NULL != retValue) *retValue = ret;
	if (0 == CloseHandle(thread))
	{
		goto MTHREADJOIN_ERROR;
	}
	return MOWTHREADOK;
MTHREADJOIN_ERROR:
	MOW_THREAD_STRERROR();
	return MOWTHREADERR;
}
#else
int mthread_join(mowthread thread, void** retValue) {
	if (NULL == thread) {
		MOW_THREAD_ERROR("mthread_join thread is null\n");
		return MOWTHREADERR;
	}
	int ret;
	ret = pthread_join(thread, retValue);
	if (0 == ret) {	//SUCCESS
		return MOWTHREADOK;
	}
	else {	//ERROR
		MOW_THREAD_STRERROR(ret);
		return MOWTHREADERR;
	}
}
#endif

#ifdef _WIN32
mowthread mthread(uint32_t(*func)(void*), void* params) {
#else
mowthread mthread(void* (*func)(void*), void* params) {
#endif
	if (NULL == func) {
		MOW_THREAD_ERROR("mthread function is NULL\n");
		return MOWTHREADERR;
	}
	mowthread thread;
#ifdef _WIN32
	thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, params, 0, NULL);
	if (NULL == thread) {
		MOW_THREAD_STRERROR();
		return MOWTHREADERR;
	}return thread;
#else
	int r = pthread_create(&thread, NULL, func, params);
	if (0 != r) {
		MOW_THREAD_STRERROR(r);
		return MOWTHREADERR;
	}return thread;
#endif


}


