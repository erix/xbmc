
#include "stdafx.h"
#include "dll_tracker.h"
#include "dll_tracker_memory.h"
#include "dll_tracker_library.h"
#include "dll_tracker_file.h"
#include "dll_tracker_socket.h"
#include "dll_tracker_critical_section.h"
#include "DllLoader.h"

#ifdef _cplusplus
extern "C"
{
#endif

TrackedDllList g_trackedDlls;

void tracker_dll_add(DllLoader* pDll)
{
  DllTrackInfo* trackInfo = new DllTrackInfo;
  trackInfo->pDll = pDll;
  trackInfo->lMinAddr = 0;
  trackInfo->lMaxAddr = 0;
  g_trackedDlls.push_back(trackInfo);
}

void tracker_dll_free(DllLoader* pDll)
{
  for (TrackedDllsIter it = g_trackedDlls.begin(); it != g_trackedDlls.end(); ++it)
  {
    if ((*it)->pDll == pDll)
    {
      try
      {
        tracker_memory_free_all(*it);
        tracker_heapobjects_free_all(*it);
        tracker_library_free_all(*it);
        tracker_file_free_all(*it);
        tracker_socket_free_all(*it);
        tracker_critical_section_free_all(*it);
      }
      catch (...)
      {
        CLog::Log(LOGFATAL, "Error freeing tracked dll resources");
      }
      // free all functions which where created at the time we loaded the dll
	    DummyListIter dit = (*it)->dummyList.begin();
	    while (dit != (*it)->dummyList.end()) { free((void*)*dit); dit++;	}
	    (*it)->dummyList.clear();
	
      delete (*it);
      it = g_trackedDlls.erase(it);
    }
  }
}

void tracker_dll_set_addr(DllLoader* pDll, uintptr_t min, uintptr_t max)
{
  for (TrackedDllsIter it = g_trackedDlls.begin(); it != g_trackedDlls.end(); ++it)
  {
    if ((*it)->pDll == pDll)
    {
      (*it)->lMinAddr = min;
      (*it)->lMaxAddr = max;
      break;
    }
  }
}

char* tracker_getdllname(uintptr_t caller)
{
  DllTrackInfo *track = tracker_get_dlltrackinfo(caller);
  if(track)
    return track->pDll->GetFileName();
  return "";
}

DllTrackInfo* tracker_get_dlltrackinfo(uintptr_t caller)
{
  for (TrackedDllsIter it = g_trackedDlls.begin(); it != g_trackedDlls.end(); ++it)
  {
    if (caller >= (*it)->lMinAddr && caller <= (*it)->lMaxAddr)
    {
      return *it;
    }
  }

  // crap not in any base address, check if it may be in virtual spaces
  for (TrackedDllsIter it = g_trackedDlls.begin(); it != g_trackedDlls.end(); ++it)
  {
    for(VAllocListIter it2 = (*it)->virtualList.begin(); it2 != (*it)->virtualList.end(); ++it2)
    {
      if( it2->first <= caller && caller < it2->first + it2->second.size )
        return *it;

    }
  }

  return NULL;
}

DllTrackInfo* tracker_get_dlltrackinfo_byobject(DllLoader* pDll)
{
  for (TrackedDllsIter it = g_trackedDlls.begin(); it != g_trackedDlls.end(); ++it)
  {
    if ((*it)->pDll == pDll)
    {
      return *it;
    }
  }
  return NULL;
}

void tracker_dll_data_track(DllLoader* pDll, uintptr_t addr)
{
  for (TrackedDllsIter it = g_trackedDlls.begin(); it != g_trackedDlls.end(); ++it)
  {
    if (pDll == (*it)->pDll)
    {
      (*it)->dummyList.push_back((uintptr_t)addr);
      break;
    }
  }
}

#ifdef _cplusplus
}
#endif