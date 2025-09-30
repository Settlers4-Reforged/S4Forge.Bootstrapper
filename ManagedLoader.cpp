#include "pch.h"
#include "ManagedIncludes.h"
#include "ManagedLoader.h"
#include "ForgeLoader.h"


using namespace System;
using namespace IO;
using namespace Reflection;
using namespace Linq;
using namespace Collections::Generic;

Assembly^ AssemblyLoader(Object^ sender, ResolveEventArgs^ args) {
    try {
        auto loadedAssemblies = System::AppDomain::CurrentDomain->GetAssemblies();

        for each (Assembly ^ a in loadedAssemblies) {
            if (a->FullName == args->Name)
                return a;
        }

        String^ resourceName = (gcnew AssemblyName(args->Name))->Name + ".dll";


        // Check for assemblies in the bootstrapper assembly:
        String^ resource = nullptr;
        auto    embeddedAssemblies = Assembly::GetExecutingAssembly()->GetManifestResourceNames();
        for each (String ^ a in embeddedAssemblies) {
            if (a->EndsWith(resourceName)) {
                resource = a;
                break;
            }
        }

        if (resource != nullptr) {
            auto stream = Assembly::GetExecutingAssembly()->GetManifestResourceStream(resource);

            array<Byte>^ assemblyData = gcnew array<Byte>(safe_cast<int>(stream->Length));
            stream->Read(assemblyData, 0, assemblyData->Length);
            stream->Close();
            return Assembly::Load(assemblyData);
        }

        if (args->RequestingAssembly == nullptr) return nullptr;

        String^ folderPath = Path::GetDirectoryName(args->RequestingAssembly->Location);
        if (folderPath == nullptr)
            folderPath = "";
        String^ rawAssemblyPath = Path::Combine(folderPath, (gcnew AssemblyName(args->Name))->Name);

        String^ assemblyPath = rawAssemblyPath + ".dll";

        if (!File::Exists(assemblyPath)) {
            assemblyPath = rawAssemblyPath + ".asi";
            if (!File::Exists(assemblyPath)) return nullptr;
        }

        auto assembly = Assembly::LoadFrom(assemblyPath);
        return assembly;
    } catch (Exception^ e) {
        Diagnostics::Debugger::Break();
        throw e;
    }
}

DWORD __stdcall InitPlugins(void* param) {
#pragma warning(disable:4947)
    AppDomain::CurrentDomain->AppendPrivatePath("plugins\\");

    AppDomain^ currentDomain = AppDomain::CurrentDomain;
    currentDomain->AssemblyResolve += gcnew ResolveEventHandler(&AssemblyLoader);

    CrashHandling::InstallCrashHandler();
    LoadForge();
    return 0;
}
