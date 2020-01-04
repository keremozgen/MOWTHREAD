#include "mowthread.h"

mowmutex m = 0;
volatile static int start = 0;
uint64_t testtolock(int k) {
	//while (0 == start);
	int kk = mmutex_lock_to(m, 0, 9999999);
	if (MOWTHREADOK == kk) {
		msleep_ms(2);
		printf("%d got the lock\n", k);
		mmutex_unlock(m);
	}
	else {
		//printf("%d returned from lockto\n", kk);
	}
}

uint64_t deneme(int k) {
	mmutex_lock(m);
	printf("Locked mutex %d\n",k);
	mmutex_unlock(m);
	return UINT64_MAX;
}
void main(){
#if defined(_WIN32) && defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
	printf("Hello\n");
	uint64_t f = mprecise_time_of_day_ns();
	msleep_ms(2000);
	uint64_t s = mprecise_time_of_day_ns();
	printf("%llu is the difference\n",(s-f));
	m = mmutex_init();
	
	mowthread t = mthread(testtolock, 1);
	mowthread l = mthread(testtolock, 2);
	mowthread a = mthread(testtolock, 3);
	mowthread aa = mthread(testtolock, 4);
	mowthread aaa = mthread(testtolock, 5);
	start = 1;
	//mthread_detach(t);
	mthread_join(t, 0);
	mthread_join(l, 0);
	mthread_join(a, 0);
	mthread_join(aa, 0);
	mthread_join(aaa, 0);

	mmutex_destroy(m);

#if defined(_WIN32) && defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif
}