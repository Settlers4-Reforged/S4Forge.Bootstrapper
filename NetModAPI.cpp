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
	if (type->IsInterface)
		return false;

	Type^ pluginInterface = NetModAPI::IForge::typeid;

	for each(Type^ interf in type->GetInterfaces()) {
		if (interf->FullName == pluginInterface->FullName)
			return true;
	}

	return false;
}

Assembly^ AssemblyLoader(Object^ sender, ResolveEventArgs^ args) {
	auto loadedAssemblies = System::AppDomain::CurrentDomain->GetAssemblies();
	
	for each(Assembly^ a in loadedAssemblies) {
		if (a->FullName == args->Name)
			return a;
	}

	String^ resourceName = (gcnew AssemblyName(args->Name))->Name + ".dll";

	String^ resource = nullptr;
	auto embeddedAssemblies = (NetModAPI::IForge::typeid)->Assembly->GetManifestResourceNames();
	for each(String^ a in embeddedAssemblies) {
		if(a->EndsWith(resourceName)) {
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

	if (!File::Exists(assemblyPath))
	{
		assemblyPath = rawAssemblyPath + ".asi";
		if (!File::Exists(assemblyPath)) return nullptr;
	}

	auto assembly = Assembly::LoadFrom(assemblyPath);
	return assembly;
}

bool NetModAPI::NetModAPI::LoadForge() {
#pragma warning(suppress : 4947)
	AppDomain::CurrentDomain->AppendPrivatePath("plugins\\");

	AppDomain^ currentDomain = AppDomain::CurrentDomain;
	currentDomain->AssemblyResolve += gcnew ResolveEventHandler(&AssemblyLoader);

	String^ file = "plugins\\S4Forge.dll";
	if (!File::Exists(file)) {
		Logger::LogError("S4Forge.dll not found in plugins folder!", nullptr);
		return false;
	}

	Logger::LogInfo("Loading S4Forge Assembly...");

	try {
		Assembly^ pluginAssembly = Assembly::LoadFrom(file);
		auto types = gcnew List<Type^>(pluginAssembly->GetTypes());

		Type^ pluginClass = Enumerable::SingleOrDefault(Enumerable::Where(types, gcnew Func<Type^, bool>(&IsIPlugin)));
		if (pluginClass == nullptr)
			return false;

		IForge^ plugin = static_cast<IForge^>(Activator::CreateInstance(pluginClass));

		Logger::LogInfo("Initializing S4Forge...");
		plugin->Initialize();
	} catch (Exception^ e) {
		String^ stackTrace = e->StackTrace;
		while (e->InnerException != nullptr) {
			e = e->InnerException;
		}

		String^ errorMsg = String::Format("Error during load of Forge \nError: {1}\n\n============= Stack Trace =============\n{2}", e->Message, stackTrace);
		Logger::LogError(errorMsg, e);
	}
	return true;
}
