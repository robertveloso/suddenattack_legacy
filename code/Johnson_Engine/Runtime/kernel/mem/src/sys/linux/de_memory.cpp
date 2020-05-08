
#include "bdefs.h"
// #include <new.h>
#include "de_memory.h"
// #include "build_options.h"
#include "sysdebugging.h"
#include "dsys.h"

//#define TRACK_MEMORY_USAGE
//#define TRACK_MEM_BOUNDS

typedef struct CleanupHandler_t
{
	cleanup_handler m_Fn;
	void *m_pUser;
	struct CleanupHandler_t *m_pNext;
} CleanupHandler;


class MemTrack
{
public:
	uint32	m_AllocSize;
	uint32	m_AllocNumber;
};


static uint32 g_MemoryUsage;
static uint32 g_nAllocations;
uint32 g_nTotalAllocations, g_nTotalFrees;
#ifdef TRACK_MEM_BOUNDS

static bool firstAlloc;
uint32 g_FirstAlloc;
uint32 g_MaxAlloc;
#endif
static bool s_bWorldAllocActive;
uint32 g_WorldMemUsage;
uint32 g_WorldMemAllocs;
uint32 g_nWorldTotalMemAllocs;

void mem_StartWorldTrack()
{
	s_bWorldAllocActive = TRUE;
	g_WorldMemUsage = 0;
	g_WorldMemAllocs = 0;
	g_nWorldTotalMemAllocs = 0;
}

void mem_FinishWorldTrack()
{
	s_bWorldAllocActive = FALSE;
	printf("World Mem Info\n");
	printf("g_WorldMemAllocs: %u\n", g_WorldMemAllocs);
	printf("g_TotalMemAllocs: %u\n", g_nWorldTotalMemAllocs);
	printf("g_WorldMemUsage : %u bytes %.2f Kb\n", g_WorldMemUsage, (float)g_WorldMemUsage/1024.0f);
}

static int g_MemRefCount=0;
static CleanupHandler_t *g_pCleanupHandlers;
// static _PNH g_OldNewHandler; //MJS

// PlayDemo profile info.
uint32 g_PD_Malloc=0;
uint32 g_PD_Free=0;


// Hook Stdlith's base allocators.
void* DefStdlithAlloc(uint32 size)
{
	return dalloc(size);
}

void DefStdlithFree(void *ptr)
{
	dfree(ptr);
}


void dm_HeapCompact()
{
	/*
	HANDEL hHeap;

	if(hHeap = GetProcessHeap())
	{
		HeapCompact(hHeap, 0);
	}
	*/
}


static void dm_CallCleanupHandlers()
{
	CleanupHandler *pCur;

	pCur = g_pCleanupHandlers;
	while(pCur)
	{
		pCur->m_Fn(pCur->m_pUser);
		pCur = pCur->m_pNext;
	}
}

static int dm_NewHandler(size_t size)
{
	dsi_OnMemoryFailure();
	return 0;
}


void* operator new(size_t size, void *ptr, char z)
{
	ptr=ptr;
	z=z;
	return dalloc(size);
}


void dm_Init()
{
	if(g_MemRefCount == 0)
	{
		g_pCleanupHandlers = 0;

//		g_OldNewHandler = _set_new_handler(dm_NewHandler);

		g_MemoryUsage = 0;
		g_nAllocations = 0;
		g_nTotalAllocations = g_nTotalFrees = 0;
	}

	g_MemRefCount++;
}


void dm_Term()
{
	CleanupHandler *pCur, *pNext;

	g_MemRefCount--;
	if(g_MemRefCount == 0)
	{
		// Free all the cleanup handlers.
		pCur = g_pCleanupHandlers;
		while(pCur)
		{
			pNext = pCur->m_pNext;
			LTMemFree(pCur);
			pCur = pNext;
		}

		// Restore the old new handler.
//		_set_new_handler(g_OldNewHandler);

		g_MemoryUsage = 0;
		g_nAllocations = 0;
	}
}


uint32 dm_GetBytesAllocated()
{
	return g_MemoryUsage;
}


uint32 dm_GetNumAllocations()
{
	return g_nAllocations;
}


void dm_AddCleanupHandler(cleanup_handler fn, void *pUser)
{
	CleanupHandler *pHandler;

	LT_MEM_TRACK_ALLOC(pHandler = (CleanupHandler*)LTMemAlloc(sizeof(CleanupHandler)),LT_MEM_TYPE_MISC);
	if(pHandler)
	{
		pHandler->m_Fn = fn;
		pHandler->m_pUser = pUser;
		pHandler->m_pNext = g_pCleanupHandlers;
		g_pCleanupHandlers = pHandler;
	}
}


void dm_RemoveCleanupHandler(cleanup_handler fn)
{
	CleanupHandler **ppPrev, *pCur;

	ppPrev = &g_pCleanupHandlers;
	pCur = g_pCleanupHandlers;
	while(pCur)
	{
		if(pCur->m_Fn == fn)
		{
			*ppPrev = pCur->m_pNext;
			LTMemFree(pCur);
		}

		ppPrev = &pCur->m_pNext;
		pCur = pCur->m_pNext;
	}
}


void* dalloc(uint32 size)
{
	char *ptr;
	uint32 fullAllocSize;


	if(size == 0)
		return 0;

	
	// Add 4 bytes if we're tracking memory usage.
	#ifdef TRACK_MEMORY_USAGE
		MemTrack *pMemTrack;
		fullAllocSize = size + sizeof(MemTrack);
	#else
		fullAllocSize = size;
	#endif

	
	// Try to allocate the memory.
	{
//		CountAdder cntAdd(&g_PD_Malloc); // deprecated
		LT_MEM_TRACK_ALLOC(ptr = (char*)LTMemAlloc((size_t)fullAllocSize),LT_MEM_TYPE_MISC);
	}

	if(!ptr)
	{
		// Free up as much memory as possible and try again.
		dm_CallCleanupHandlers();

		LT_MEM_TRACK_ALLOC(ptr = (char*)LTMemAlloc((size_t)fullAllocSize),LT_MEM_TYPE_MISC);
		if(!ptr)
		{
			dsi_OnMemoryFailure();
		}
	}


	// Store the size in there if we're tracking memory usage.
	#ifdef TRACK_MEMORY_USAGE
		// Used so you can have it stop on a certain allocation.
		static uint32 g_LookForAlloc = 0xFFFFFFFF;
		if(g_nAllocations == g_LookForAlloc)
		{
			pMemTrack = NULL;
		}
		
		g_MemoryUsage += size;
		pMemTrack = (MemTrack*)ptr;
		pMemTrack->m_AllocSize = size;
		pMemTrack->m_AllocNumber = g_nAllocations;
		ptr += sizeof(MemTrack);
	#endif
	#ifdef TRACK_MEM_BOUNDS
		// first available address which was allocated by malloc
		if (!firstAlloc)
		{
			g_FirstAlloc = (uint32)ptr;
			firstAlloc = true;
		}
		// calculate the address of the last block of allocated memory, when this 
		// is larger than 32Mb, the debug kit / production PS2 will be trashed!
		uint32 lastMBAddr = g_FirstAlloc + (uint32)ptr + size;
		if (lastMBAddr > g_MaxAlloc)
		{
			g_MaxAlloc = lastMBAddr;
		}
	#endif

	if (s_bWorldAllocActive)
	{
		g_WorldMemUsage += size;
		g_WorldMemAllocs++;
		g_nWorldTotalMemAllocs++;
	}
	++g_nAllocations;
	++g_nTotalAllocations;
	return ptr;
}


void* dalloc_z(uint32 size)
{
	void *ret;

	ret = dalloc(size);
	if(ret)
	{
		memset(ret, 0, size);
	}

	return ret;
}


void dfree(void *ptr)
{
	char *pCharPtr;

	if(!ptr)
		return;

	pCharPtr = (char*)ptr;

	#ifdef TRACK_MEMORY_USAGE
		MemTrack *pMemTrack;

		pCharPtr -= sizeof(MemTrack);
		pMemTrack = (MemTrack*)pCharPtr;

		if(pMemTrack->m_AllocSize > g_MemoryUsage)
		{
			dsi_PrintToConsole("Error: engine freed more memory than allocated!");
		}

		g_MemoryUsage -= pMemTrack->m_AllocSize;
		//uint32 delsize = pMemTrack->m_AllocSize - sizeof(MemTrack);
		// debug and wipe the memory with 0xDD so that it'll be easy to track
		//printf(" Free: %6u Size: %8u Ptr: 0x%08X (%6u %6u)\n", pMemTrack->m_AllocNumber, pMemTrack->m_AllocSize, ptr, g_nAllocations, g_nTotalFrees);
		//memset(ptr, 0xDD, delsize);
	#endif

	--g_nAllocations;
	++g_nTotalFrees;

	if (s_bWorldAllocActive)
	{
		g_WorldMemAllocs--;
	}
	
	{
//		CountAdder cntAdd(&g_PD_Free); // deprecated
		LTMemFree(pCharPtr);
	}
}


#ifndef DE_SERVER_COMPILE
	void* operator new(size_t size)
	{
		return dalloc(size);
	}

	void operator delete(void *ptr)
	{
		dfree(ptr);
	}
#endif
