#include "pch.h"

#include "IForge.h"
#include "NetModAPI.h"

#include "Logger.h"
#include <vcclr.h>

using namespace System;
using namespace IO;
using namespace Reflection;
using namespace Linq;
using namespace Collections::Generic;

#define LogInfo(msg) LogMessage(LogLevelInfo, L"S4ForgeBootstrapper", L"Bootstrap", msg)
#define LogError(msg) LogMessage(LogLevelError, L"S4ForgeBootstrapper", L"Bootstrap", msg)

bool IsIPlugin(Type^ type) {
    if(type->IsInterface)
        return false;

    Type^ pluginInterface = NetModAPI::IForge::typeid;

    for each(Type^ interf in type->GetInterfaces()) {
        if(interf->FullName == pluginInterface->FullName)
            return true;
    }

    return false;
}

bool NetModAPI::NetModAPI::LoadForge() {
#pragma warning(suppress : 4947)

    String^ file = "plugins\\Forge\\S4Forge.dll";
    if(!File::Exists(file)) {
        LogError(L"S4Forge.dll not found in plugins folder!");
        return false;
    }

    LogInfo(L"Loading S4Forge Assembly...");

    try {
        Assembly^ pluginAssembly = Assembly::LoadFrom(file);
        auto      types = gcnew List<Type^>(pluginAssembly->GetTypes());

        Type^ pluginClass = Linq::Enumerable::SingleOrDefault(Linq::Enumerable::Where(types, gcnew Func<Type^, bool>(&IsIPlugin)));
        if(pluginClass == nullptr)
            return false;

        IForge^ plugin = static_cast<IForge^>(Activator::CreateInstance(pluginClass));

        LogInfo(L"Initializing S4Forge...");
        plugin->Initialize();
    } catch(Exception^ e) {
        String^ stackTrace = e->StackTrace;
        while(e->InnerException != nullptr) {
            e = e->InnerException;
        }

        String^ errorMsg = String::Format("Error during load of Forge \nError: {0}\n\n============= Stack Trace =============\n{1}", e->Message, stackTrace);
        pin_ptr<const wchar_t> wch = PtrToStringChars(errorMsg);
        LogMessage((int)LogLevelError, L"S4ForgeBootstrapper", L"Bootstrap", wch);

        CrashHandling::DebugService::GetReporter()->ReportException(CrashHandling::DebugReportSource{ "S4Forge" }, "Error during load of Forge", e, true);
    }
    return true;
}
