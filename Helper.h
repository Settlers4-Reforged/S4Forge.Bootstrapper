#pragma once

#pragma unmanaged
#pragma comment(lib,"Dbghelp.lib")
#include <dbghelp.h>
#include <iostream>

#define TRACE_MAX_STACK_FRAMES 1024
#define TRACE_MAX_FUNCTION_NAME_LENGTH 1024

typedef struct _STACKTRACE {
    //
    // Number of frames in Frames array.
    //
    UINT FrameCount;

    //
    // PC-Addresses of frames. Index 0 contains the topmost frame.
    //
    ULONGLONG Frames[1000];
} STACKTRACE, * PSTACKTRACE;

//
// Disable global optimization and ignore /GS waning caused by 
// inline assembly.
//
extern HRESULT CaptureStackTrace(PSTACKTRACE StackTrace, PEXCEPTION_POINTERS exception_pointers);

#pragma managed

extern System::String^ GetStackTrace(PEXCEPTION_POINTERS exception_pointers, int skip, bool skipToFirstManaged);

#define LOG_CALL() LogCall(__FUNCTION__ )
extern void LogCall(System::String^ name);

