/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  |                                                                         |
  | NOTE: This class is derived from CSerialPort from the MRPT project:     |
  |                     Mobile Robot Programming Toolkit (MRPT)             |
  |                          http://www.mrpt.org/                           |
  |                                                                         |
  | Copyright (c) 2005-2015, Individual contributors, see AUTHORS file      |
  | See: http://www.mrpt.org/Authors - All rights reserved.                 |
  | Released under BSD License. See details in http://www.mrpt.org/License  |
  +-------------------------------------------------------------------------+  */


#include "threads.h"
#include "CTicTac.h"
#include "CSemaphore.h"
#include <iostream>
#include <stdint.h>

#ifdef _WIN32
    #include <conio.h>
	#include <windows.h>
	#include <process.h>
	#include <tlhelp32.h>
	#include <sys/utime.h>
	#include <io.h>
	#include <direct.h>
#else
    #include <pthread.h>
    #include <termios.h>
    #include <unistd.h>
    #include <sys/select.h>
    #include <sys/time.h>
    #include <time.h>
	#include <unistd.h>
	#include <utime.h>
	#include <errno.h>
	#include <signal.h>
#endif

#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __APPLE__
	#include <sys/sysctl.h>
	#include <mach/mach_init.h>
	#include <mach/thread_act.h>
#endif

using namespace libredaq;
using namespace libredaq::utils;
using namespace libredaq::system;
using namespace std;

/*---------------------------------------------------------------
						sleep
---------------------------------------------------------------*/
void libredaq::system::sleep( int time_ms )
{
#ifdef _WIN32
	Sleep( time_ms );
#else
	// We will wake up on signals: Assure the desired time has passed:
	CTicTac tictac;
	tictac.Tic();
	int timeLeft_ms = time_ms - (int)(tictac.Tac()*1000);
	while ( timeLeft_ms>0 )
	{
		usleep( timeLeft_ms * 1000 );
		timeLeft_ms = time_ms - (int)(tictac.Tac()*1000);
	}
#endif
}

/*---------------------------------------------------------------
						createThread
---------------------------------------------------------------*/
namespace libredaq
{
	namespace system
	{
		struct TAuxThreadLaucher
		{
	#ifdef _WIN32
			TAuxThreadLaucher() : ptrFunc(NULL),param(NULL),win_sem(0,10)
			{
			}
	#else
			TAuxThreadLaucher() : ptrFunc(NULL),param(NULL) { };
	#endif
			void    (*ptrFunc) (void *);
			void    *param;

	#ifdef _WIN32
			// These for windows only:
			unsigned long		myWindowsId;
			libredaq::synch::CSemaphore win_sem;
	#endif
		};

		void *auxiliary_thread_launcher_LIN( void *param )
		{
			try
			{
			TAuxThreadLaucher   *d = reinterpret_cast<TAuxThreadLaucher*>(param);

			TAuxThreadLaucher	localCopy = *d;

	#ifdef _WIN32
			// Signal that the thread has started:
			d->myWindowsId = (unsigned long)GetCurrentThreadId();
			d->win_sem.release();
			// Our parent thread will release the memory of "param".
	#else
			// LINUX: We have to free here this memory:
			delete d;
			d = NULL;
	#endif

			// Now start the user code:
			localCopy.ptrFunc( localCopy.param );
			}
			catch(std::exception &e)
			{
				std::cout << "Exception in [auxiliary_thread_launcher_LIN/WIN]!!!:\n" << e.what();
			}
			catch(...)
			{
				std::cout << "Untyped exception in [auxiliary_thread_launcher_LIN/WIN]!!!\n";
			}
			return NULL;
		}

		void auxiliary_thread_launcher_WIN( void *param )
		{
			auxiliary_thread_launcher_LIN(param);
		}
	} // end namespace
} // end namespace


libredaq::system::TThreadHandle libredaq::system::detail::createThreadImpl(
    void       ( *func )( void * ),
    void       *param
    )
{
	

    TAuxThreadLaucher   *auxData=new TAuxThreadLaucher();
    auxData->ptrFunc = func;
    auxData->param = param;

#ifdef _WIN32
	TThreadHandle		threadHandle;

	HANDLE h= (HANDLE)_beginthread( auxiliary_thread_launcher_WIN,0, auxData);
	if (h== ((HANDLE) -1))
	{
		delete auxData;
		throw std::runtime_error("Error creating new thread");
	}

	threadHandle.hThread = h;

	// Wait until the thread starts so we know its ID:
	auxData->win_sem.waitForSignal();
	threadHandle.idThread = auxData->myWindowsId;

	delete auxData; auxData = NULL;

	return threadHandle;

#else
	TThreadHandle		threadHandle;

    pthread_t   newThreadId;
    int iRet = pthread_create( &newThreadId,NULL,auxiliary_thread_launcher_LIN,auxData);
	if (iRet!=0) throw std::runtime_error("Error invoking pthread_create");

	threadHandle.idThread = (unsigned long)newThreadId;
	return threadHandle;
#endif

	
}


/*---------------------------------------------------------------
						joinThread
---------------------------------------------------------------*/
void libredaq::system::joinThread( const TThreadHandle &threadHandle )
{
	if (threadHandle.isClear()) return;
#ifdef _WIN32
	int prio = GetThreadPriority((HANDLE) threadHandle.hThread);
	if (THREAD_PRIORITY_ERROR_RETURN==prio)
		return; // It seems this is not a running thread...

	DWORD ret = WaitForSingleObject( (HANDLE) threadHandle.hThread , INFINITE );
	if (ret!=WAIT_OBJECT_0)
		cerr << "[libredaq::system::joinThread] Error waiting for thread completion!" << endl;
#elif defined(__APPLE__)
	pthread_join(reinterpret_cast<pthread_t>(threadHandle.idThread), NULL);
#else
	pthread_join(threadHandle.idThread, NULL);
#endif
}

/*---------------------------------------------------------------
						getCurrentThreadId
---------------------------------------------------------------*/
unsigned long libredaq::system::getCurrentThreadId()
{
#ifdef _WIN32
	return GetCurrentThreadId();

/* Jerome Monceaux 2011/03/08: bilock@gmail.com
 * The next precompilation directive didn't compile under osx
 * added defined(__APPLE__) solved the probleme
 */
//#elif __APPLE__
#elif defined(__APPLE__)
	return reinterpret_cast<unsigned long>(pthread_self());
#else
	return pthread_self();
#endif
}

/*---------------------------------------------------------------
						getCurrentThreadHandle
---------------------------------------------------------------*/
TThreadHandle libredaq::system::getCurrentThreadHandle()
{
	TThreadHandle h;
#ifdef _WIN32
	// Win32:
	h.hThread = GetCurrentThread();
	h.idThread = GetCurrentThreadId();
#elif defined(__APPLE__)
	h.idThread = reinterpret_cast<long unsigned int>(pthread_self());
#else
	// pthreads:
	h.idThread = pthread_self();
#endif
	return h;
}

/*---------------------------------------------------------------
					changeThreadPriority
---------------------------------------------------------------*/
void libredaq::system::changeThreadPriority(
	const TThreadHandle &threadHandle,
	TThreadPriority priority )
{
#ifdef _WIN32
	// TThreadPriority is defined to agree with numbers expected by Win32 API:
	SetThreadPriority( threadHandle.hThread, priority);
#else

	const pthread_t tid =
	#ifdef __APPLE__
		reinterpret_cast<pthread_t>(threadHandle.idThread);
	#else
		threadHandle.idThread;
	#endif

	int policy;
	struct sched_param param;

	if (pthread_getschedparam(tid,&policy,&param)) {
		cerr << "[libredaq::system::changeThreadPriority] Warning: Failed call to pthread_getschedparam" << endl;
		return;
	}

	int prio = 0;
	switch(priority)
	{
		case tpLowests: prio=40; break;
		case tpLower : prio=41; break;
		case tpLow : prio=49; break;
		case tpNormal: prio=50; break;
		case tpHigh : prio=51; break;
		case tpHigher: prio=55; break;
		case tpHighest: prio=60; break;
	}

	param.sched_priority = prio;

	if (pthread_setschedparam(tid, policy, &param)) {
		cerr << "[libredaq::system::changeThreadPriority] Warning: Failed call to pthread_getschedparam" << endl;
		return;
	}

#endif
}

/*---------------------------------------------------------------
					changeCurrentProcessPriority
---------------------------------------------------------------*/
void libredaq::system::changeCurrentProcessPriority( TProcessPriority priority  )
{
#ifdef _WIN32
	DWORD dwPri;
	switch (priority)
	{
	case ppIdle:	dwPri = IDLE_PRIORITY_CLASS; break;
	case ppNormal:	dwPri = NORMAL_PRIORITY_CLASS; break;
	case ppHigh:	dwPri = HIGH_PRIORITY_CLASS; break;
	case ppVeryHigh: dwPri= REALTIME_PRIORITY_CLASS; break;
	default:
		throw std::runtime_error("Invalid priority value");
	}
	SetPriorityClass( GetCurrentProcess(), dwPri );
#else
	std::cout << "[libredaq::system::changeCurrentProcessPriority] Warning: Not implemented in Linux yet" << std::endl;
#endif
}

/*---------------------------------------------------------------
					libredaq::system::getCurrentThreadTimes
---------------------------------------------------------------*/
void libredaq::system::getCurrentThreadTimes(
	time_t			&creationTime,
	time_t			&exitTime,
	double			&cpuTime )
{
	

#ifdef _WIN32
	FILETIME	timCreat,timExit, timKernel, timUser;
	uint64_t	t;

	HANDLE threadHandle;

#if !defined(HAVE_OPENTHREAD) // defined(_MSC_VER) && (_MSC_VER<1300)
	// In MSVC6/GCC the ID is just the HANDLE:
	threadHandle = reinterpret_cast<HANDLE>( libredaq::system::getCurrentThreadId() );
#else
	// Get the handle from the ID:
	threadHandle = OpenThread( READ_CONTROL | THREAD_QUERY_INFORMATION, FALSE,  GetCurrentThreadId() );  // threadId);
	if (!threadHandle)	 throw std::runtime_error("Cannot open the thread with the given 'threadId'");
#endif

	if (!GetThreadTimes( threadHandle , &timCreat, &timExit, &timKernel, &timUser ))
	{
		CloseHandle(threadHandle);
		throw std::runtime_error("Error accessing thread times!");
	}

#if defined(HAVE_OPENTHREAD) // _MSC_VER) && (_MSC_VER>=1300)
	// From OpenThread...
	CloseHandle(threadHandle);
#endif

	// Formula is derived from:
	//  http://support.microsoft.com/kb/167296
	t = (((uint64_t)timCreat.dwHighDateTime) << 32) | timCreat.dwLowDateTime;
	creationTime = (t - 116444736000000000ULL)/10000000;

	t = (((uint64_t)timExit.dwHighDateTime) << 32) | timExit.dwLowDateTime;
	exitTime = (t - 116444736000000000ULL)/10000000;

	// CPU time is user+kernel:
	int64_t	t1 = (((uint64_t)timKernel.dwHighDateTime) << 32) | timKernel.dwLowDateTime;
	int64_t	t2 = (((uint64_t)timUser.dwHighDateTime) << 32) | timUser.dwLowDateTime;

	cpuTime = ((double)(t1+t2)) * 100e-9;	// FILETIME counts intervals of 100ns

#endif

#ifdef MRPT_OS_LINUX
	MRPT_UNUSED_PARAM(creationTime);
	MRPT_UNUSED_PARAM(exitTime);
	// Unix:
#	ifdef HAVE_GETTID
		pid_t 	id = gettid();
#	else
		// gettid is:
		//  186 in 64bit
		//  224 in 32bit
		#if MRPT_WORD_SIZE==64
			pid_t 	id = (long int)syscall(186);
		#elif MRPT_WORD_SIZE==32
			pid_t 	id = (long int)syscall(224);
		#else
			#error MRPT_WORD_SIZE must be 32 or 64.
		#endif
#	endif

	// (JL) Refer to: /usr/src/linux/fs/proc/array.c
	long unsigned 	tms_utime=0, tms_stime=0;
	ifstream is(format("/proc/self/task/%i/stat", id).c_str() );

	if (is.is_open())
	{
		string s;
		getline(is,s);

		size_t idx = s.find(")");

		if (idx!=string::npos)
		{
			vector_string	tokens;
			libredaq::system::tokenize( string(s.c_str()+idx+1)," ",tokens);

			if (tokens.size()>=13)
			{
				sscanf(tokens[11].c_str(), "%lu" ,&tms_utime);
				sscanf(tokens[12].c_str(), "%lu", &tms_stime);
			}
		}
	}

	// Compute cpuTime:
	double clockTicksPerSecond = (double)sysconf(_SC_CLK_TCK);
	if (clockTicksPerSecond>0)
		cpuTime = (tms_utime + tms_stime) / clockTicksPerSecond;
#endif

#ifdef __APPLE__
	thread_basic_info info;
	mach_msg_type_number_t count = THREAD_BASIC_INFO_COUNT;
	if(thread_info(mach_thread_self(), THREAD_BASIC_INFO, (thread_info_t)&info, &count)==0)
	{
		double utime = info.user_time.seconds + info.user_time.microseconds * 1e-6;
		double stime = info.system_time.seconds + info.system_time.microseconds * 1e-6;
		cpuTime = utime + stime;
	}
#endif

	
}

/*---------------------------------------------------------------
					launchProcess
  ---------------------------------------------------------------*/
bool libredaq::system::launchProcess( const std::string & command )
{
#ifdef _WIN32
	STARTUPINFOA			SI;
	PROCESS_INFORMATION		PI;

	memset(&SI,0,sizeof(STARTUPINFOA) );
	SI.cb = sizeof(STARTUPINFOA);

	if (CreateProcessA( NULL, (LPSTR)command.c_str(), NULL, NULL, true, 0,	NULL, NULL,	&SI, &PI) )
	{
		// Wait:
		WaitForSingleObject( PI.hProcess, INFINITE );
		return true;
	} // End of process executed OK
	else
	{
		char str[300];
		DWORD e = GetLastError();

		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,0,e,0,str,sizeof(str), NULL);

		// ERROR:
		std::cerr << "[launchProcess] Couldn't spawn process. Error msg: " << str << std::endl;
		return false;
	}
#else
	return 0== ::system(command.c_str());
#endif
} // end launchProcess

// Return number of processors in the system
unsigned int libredaq::system::getNumberOfProcessors()
{
    static unsigned int ret = 0;

    if (!ret)
    {
#ifdef _WIN32
        SYSTEM_INFO si;  // Requires Win200 or above.
        GetSystemInfo(&si);
        ret=si.dwNumberOfProcessors;
        if (ret<1) ret=1;
#elif defined(__APPLE__)
        size_t len=sizeof(int);
        if(sysctlbyname("hw.logicalcpu", &ret, &len, NULL, 0) != 0)
            ret = 1; // failed
#else
        // This assumes a Linux kernel 2.6
        ifstream f;
        f.open("/proc/cpuinfo");
        if (!f.is_open())
            return 1; // No info...

        std::string lin;
        unsigned int nProc = 0;
        while (!f.fail() && !f.eof())
        {
            std::getline(f,lin);
            if (!f.fail() && !f.eof())
                if (lin.find("processor")!=std::string::npos)
                    nProc++;
        }
        ret = nProc ? nProc : 1;
#endif
    }
    return ret;
}

/*---------------------------------------------------------------
					exitThread
  ---------------------------------------------------------------*/
void libredaq::system::exitThread()
{
#ifdef _WIN32
	ExitThread(0);
#else
	pthread_exit(NULL);
#endif
}

/*---------------------------------------------------------------
					terminateThread
  ---------------------------------------------------------------*/
void libredaq::system::terminateThread(TThreadHandle &threadHandle)
{
	if (threadHandle.isClear()) return; // done

#ifdef _WIN32
	TerminateThread(threadHandle.hThread,-1);
#elif defined(__APPLE__)
	pthread_cancel(reinterpret_cast<pthread_t>(threadHandle.idThread));
#else
	pthread_cancel(threadHandle.idThread);
#endif
	threadHandle.clear();
}

