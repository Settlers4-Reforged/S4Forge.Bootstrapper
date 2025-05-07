#include "pch.h"
#include "Helper.h"
#include "Logger.h"


#pragma unmanaged
#pragma optimize( "g", off )
#pragma warning( push )
#pragma warning( disable : 4748 )


HRESULT CaptureStackTrace(PSTACKTRACE StackTrace, PEXCEPTION_POINTERS exception_pointers = nullptr) {
    DWORD MaxFrames = TRACE_MAX_STACK_FRAMES;
    DWORD MachineType;
    CONTEXT Context;
    STACKFRAME64 StackFrame;

    if (exception_pointers != nullptr) {
        Context = *exception_pointers->ContextRecord;
    } else {
        // Use current context.
        ZeroMemory(&Context, sizeof(CONTEXT));
        Context.ContextFlags = CONTEXT_CONTROL;
        __asm
        {
        Label:
            mov[Context.Ebp], ebp;
            mov[Context.Esp], esp;
            mov eax, [Label];
            mov[Context.Eip], eax;
        }
    }

    // Set up stack frame.
    ZeroMemory(&StackFrame, sizeof(STACKFRAME64));
    MachineType = IMAGE_FILE_MACHINE_I386;
    StackFrame.AddrPC.Offset = Context.Eip;
    StackFrame.AddrPC.Mode = AddrModeFlat;
    StackFrame.AddrFrame.Offset = Context.Ebp;
    StackFrame.AddrFrame.Mode = AddrModeFlat;
    StackFrame.AddrStack.Offset = Context.Esp;
    StackFrame.AddrStack.Mode = AddrModeFlat;

    StackTrace->FrameCount = 0;
    SymInitialize(GetCurrentProcess(), NULL, TRUE);

    while (StackTrace->FrameCount < MaxFrames) {
        if (!StackWalk64(
            MachineType,
            GetCurrentProcess(),
            GetCurrentThread(),
            &StackFrame,
            &Context,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL)) {
            //
            // Maybe it failed, maybe we have finished walking the stack.
            //
            break;
        }

        if (StackFrame.AddrPC.Offset != 0) {
            //
            // Valid frame.
            //
            StackTrace->Frames[StackTrace->FrameCount++] =
                StackFrame.AddrPC.Offset;
        } else {
            //
            // Base reached.
            //
            break;
        }
    }

    return S_OK;
}
#pragma warning( pop )
#pragma optimize( "g", on )
#pragma managed

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

String^ GetStackTrace(PEXCEPTION_POINTERS exception_pointers = nullptr, int skip = 0, bool skipToFirstManaged = false) {
#ifndef PUBLIC
    List<String^>^ output = gcnew List<String^>();

    STACKTRACE stacktrace{};
    CaptureStackTrace(&stacktrace, exception_pointers);
    auto process = GetCurrentProcess();

    char buf[sizeof(SYMBOL_INFO) + (TRACE_MAX_FUNCTION_NAME_LENGTH - 1) * sizeof(TCHAR)];
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)buf;
    symbol->MaxNameLen = TRACE_MAX_FUNCTION_NAME_LENGTH;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    auto StackType = System::Diagnostics::StackFrame::typeid;
    // Hidden/Private Method to translate a managed address into a MethodBase
    auto GetMethodBaseFromNativeIP_Method = StackType->GetMethod("GetMethodFromNativeIP", System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::NonPublic);

    bool managedTransition = false;
    int skips = skip; //Try to allow skipping... well this here
    bool skipNative = skipToFirstManaged && GetMethodBaseFromNativeIP_Method != nullptr; //Only actually skip, when we can differentiate between managed and native
    for (unsigned int i = 0; i < stacktrace.FrameCount; i++) {
        DWORD64 address = stacktrace.Frames[i];
        const bool foundSymbol = SymFromAddr(process, address, nullptr, symbol);

        if (--skips > 0)
            continue;

        if (!foundSymbol || symbol->NameLen == 0) {
            // Try mixed mode/managed address translation instead, when SymFromAddr can't help...
            if (GetMethodBaseFromNativeIP_Method != nullptr) {
                auto managedSymbol = GetMethodBaseFromNativeIP_Method->Invoke(nullptr, gcnew array<Object^> { (System::IntPtr)(int)address });
                // It can happen, that we actually not know where the symbol come from. It is at least not managed...
                if (managedSymbol != nullptr) {
                    System::Reflection::MethodBase^ managedMethod = (System::Reflection::MethodBase^)managedSymbol;
                    auto symbolName = managedMethod->DeclaringType->Name + "::" + managedMethod->Name;
                    auto moduleName = managedMethod->Module->Name;

                    // First real managed method, no longer skip
                    skipNative = false;

                    if (!managedTransition && output->Count != 0) {
                        // Transitioning from managed to native
                        output[output->Count - 1] = "  [Managed -> Native]";
                    }

                    output->Add(
                        String::Format("  {0} @ <Managed>({2}) | Addr: 0x{1}",
                            symbolName,
                            address.ToString("X"),
                            moduleName)
                    );

                    managedTransition = true;
                    continue;
                }
            }

            if (skipNative) {
                managedTransition = false; // Handle transitions in skip too
                continue;
            }

            if (managedTransition) {
                // Transitions from and to managed are not handled by the above check, but still come out as "no symbol"
                // manually override as transition are
                // Sometimes this will not actually happen (e.g. when transitioning to a clr runtime method through their magic P/Invoke
                output->Add("  [Native -> Managed]");
                managedTransition = false;
            } else {
                output->Add(String::Format("  ? @ 0x{0}", address.ToString("X")));
            }
        } else {
            if (skipNative) {
                continue;
            }

            MEMORY_BASIC_INFORMATION mbi;
            VirtualQuery(reinterpret_cast<void*>(address), &mbi, sizeof(mbi));
            char moduleName[MAX_PATH];
            DWORD success = GetModuleFileNameA((HMODULE)mbi.AllocationBase, moduleName, sizeof(moduleName));

            String^ moduleFileName = "?";
            if (success != 0) {
                moduleFileName = IO::Path::GetFileName(gcnew String(moduleName));
            }

            output->Add(String::Format("  {0} @ 0x{1} ({2}) | Addr: 0x{3}",
                gcnew String(symbol->Name),
                symbol->Address.ToString("X"),
                moduleFileName,
                address.ToString("X")));
        }
    }

    return String::Join("\n", output);
#endif
}


void LogCall(System::String^ name) {
    NetModAPI::Logger::LogInfo("Called " + name + "!", nullptr);

    System::String^ output = "Stack Trace for " + name + ":\n";

    output += GetStackTrace();
    NetModAPI::Logger::LogInfo(output, nullptr);
}