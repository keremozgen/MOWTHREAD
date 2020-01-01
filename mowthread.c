#include "mowthread.h"

mowmutex m = 0;

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
	m = mmutex_init();
	mowthread t = mthread(deneme, 1);
	mowthread l = mthread(deneme, 2);
	mowthread a = mthread(deneme, 3);
	mowthread aa = mthread(deneme, 4);
	mowthread aaa = mthread(deneme, 5);

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