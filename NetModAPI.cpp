#include "pch.h"

#include "IForge.h"
#include "NetModAPI.h"

#include "Logger.h"

using namespace System;
using namespace IO;
using namespace Reflection;
using namespace Linq;
using namespace Collections::Generic;

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

    String^ file = "plugins\\S4Forge.dll";
    if(!File::Exists(file)) {
        Logger::LogError("S4Forge.dll not found in plugins folder!", nullptr, nullptr);
        return false;
    }

    Logger::LogInfo("Loading S4Forge Assembly...", nullptr);

    try {
        Assembly^ pluginAssembly = Assembly::LoadFrom(file);
        auto      types = gcnew List<Type^>(pluginAssembly->GetTypes());

        Type^ pluginClass = Enumerable::SingleOrDefault(Enumerable::Where(types, gcnew Func<Type^, bool>(&IsIPlugin)));
        if(pluginClass == nullptr)
            return false;

        IForge^ plugin = static_cast<IForge^>(Activator::CreateInstance(pluginClass));

        Logger::LogInfo("Initializing S4Forge...", nullptr);
        plugin->Initialize();
    } catch(Exception^ e) {
        String^ stackTrace = e->StackTrace;
        while(e->InnerException != nullptr) {
            e = e->InnerException;
        }

        String^ errorMsg = String::Format("Error during load of Forge \nError: {1}\n\n============= Stack Trace =============\n{2}", e->Message, stackTrace);
        Logger::LogError(errorMsg, e, nullptr);
    }
    return true;
}
