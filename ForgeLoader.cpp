#include "pch.h"
#include "ForgeLoader.h"
#include "NetS4ModApi.h"
#include "CrashHandler.h"
#include "Logger.h"
#include <vcclr.h>


using namespace System;
using namespace IO;
using namespace Reflection;
using namespace Linq;
using namespace Collections::Generic;
using namespace Forge::Engine;

#define LogInfo(msg) LogMessage(LogLevelInfo, L"S4ForgeBootstrapper", L"Bootstrap", msg)
#define LogError(msg) LogMessage(LogLevelError, L"S4ForgeBootstrapper", L"Bootstrap", msg)

bool IsForge(Type^ type) {
    if (type->IsInterface)
        return false;

    Type^ pluginInterface = IForge::typeid;

    for each (Type ^ interf in type->GetInterfaces()) {
        if (interf->FullName == pluginInterface->FullName)
            return true;
    }

    return false;
}

//void SendLogCallback(LogLevel level, string assembly, string category, string message);
// To
//void __stdcall LogMessage(int loglevel, const wchar_t* source, const wchar_t* category, const wchar_t* message)
void LogMessageWrapper(Forge::Logging::LogLevel loglevel, String^ source, String^ category, String^ message) {
    pin_ptr<const wchar_t> wchSource = PtrToStringChars(source);
    pin_ptr<const wchar_t> wchCategory = PtrToStringChars(category);
    pin_ptr<const wchar_t> wchMessage = PtrToStringChars(message);
    LogMessage((int)loglevel, wchSource, wchCategory, wchMessage);
}

bool CreateForgeInstance(System::Reflection::Assembly^ pluginAssembly) {
    auto types = gcnew List<Type^>(pluginAssembly->GetTypes());

    Type^ pluginClass = Linq::Enumerable::SingleOrDefault(Linq::Enumerable::Where(types, gcnew Func<Type^, bool>(&IsForge)));
    if (pluginClass == nullptr)
        return false;

    IForge^ forge = static_cast<IForge^>(Activator::CreateInstance(pluginClass));

    LogInfo(L"Initializing S4Forge...");
    forge->Initialize(gcnew NetModAPI::NetS4ModApi(), CrashHandling::CrashRptDebugReporter::Instance, gcnew Forge::Logging::CLogger::SendLogCallback(LogMessageWrapper));

    return true;
}

void ReportForgeLoadException(System::Exception^ e) {
    CrashHandling::CrashRptDebugReporter::Instance->ReportException(Forge::Logging::DebugReportSource{ "S4Forge" }, "Error during load of Forge", e, true);
}


bool LoadForge() {
#pragma warning(suppress : 4947)

    String^ file = "plugins\\Forge\\S4Forge.dll";
    if (!File::Exists(file)) {
        LogError(L"S4Forge.dll not found in plugins folder!");
        return false;
    }

    LogInfo(L"Loading S4Forge Assembly...");

    try {
        Assembly^ pluginAssembly = Assembly::LoadFrom(file);
        return CreateForgeInstance(pluginAssembly);
    } catch (Exception^ e) {
        String^ stackTrace = e->StackTrace;
        while (e->InnerException != nullptr) {
            e = e->InnerException;
        }

        String^ errorMsg = String::Format("Error during load of Forge \nError: {0}\n\n============= Stack Trace =============\n{1}", e->Message, stackTrace);
        pin_ptr<const wchar_t> wch = PtrToStringChars(errorMsg);
        LogMessage((int)LogLevelError, L"S4ForgeBootstrapper", L"Bootstrap", wch);
        ReportForgeLoadException(e);
    }
    return true;
}
