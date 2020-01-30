# MOWTHREAD
MOW THREAD is a cross platform, easy to use and a basic thread library.It is a basic header file you can include it and start using.
You don't need to use CMakeFile.txt or fips build tool.
On Windows threads return 32bit values but on linux you can return 64bit. This is a windows only limitation because of Windows CRT.
Here some functions are explained:

	--WINDOWS
	mowthread mthread(uint32_t(*func)(void*), void* params);  //CREATE A THREAD
	int mthread_join(mowthread thread, uint32_t* retValue);	//ON WINDOWS THREAD EXIT CODE IS 32 BITS UNSIGNED
	--LINUX
	mowthread mthread(void* (*func)(void*), void* params);  //CREATE A THREAD
	int mthread_join(mowthread thread, void** retValue);    WAIT THREAD TO FINISH

	int mthread_detach(mowthread thread)	//DETACH THE THREAD SO YOU DON'T NEED TO WAIT FOR IT.
	
	mowmutex mmutex_init()  //CREATE A MUTEX
	int mmutex_lock(mowmutex mutex) 	//LOCK THE MUTEX
	int mmutex_unlock(mowmutex mutex) 	//UNLOCK THE MUTEX
	int mmutex_lock_to(mowmutex mutex, uint32_t sec, uint32_t nsec)	//LOCK WITH A TIMEOUT
	static inline void msleep_ms(uint32_t ms) 	//SLEEP IN MILLISECONDS (1/1000 SECONDS)
	static inline uint64_t mprecise_time_of_day_ns(void)  //THIS IS FILESYSTEM TIME
