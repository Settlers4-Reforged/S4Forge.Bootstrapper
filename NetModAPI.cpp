#include "pch.h"

#include "IPlugin.h"
#include "NetModAPI.h"


using namespace System;
using namespace System::IO;
using namespace System::Reflection;
using namespace System::Linq;
using namespace System::Collections::Generic;

bool IsIPlugin(Type^ type) {
	if (type->IsInterface)
		return false;

	Type^ pluginInterface = NetModAPI::IPlugin::typeid;

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

	if (args->RequestingAssembly == nullptr) return nullptr;

	String^ folderPath = Path::GetDirectoryName(args->RequestingAssembly->Location);
	String^ rawAssemblyPath = Path::Combine(folderPath, (gcnew System::Reflection::AssemblyName(args->Name))->Name);

	String^ assemblyPath = rawAssemblyPath + ".dll";

	if (!File::Exists(assemblyPath))
	{
		assemblyPath = rawAssemblyPath + ".asi";
		if (!File::Exists(assemblyPath)) return nullptr;
	}

	auto assembly = System::Reflection::Assembly::LoadFrom(assemblyPath);
	return assembly;
}

bool NetModAPI::NetModAPI::LoadAllPlugins() {
	MessageBox(nullptr, L"Loading..", L"Loader", 0);
	array<String^>^ files = Directory::GetFiles(Environment::CurrentDirectory + "/plugins/", PluginExtension);

	AppDomain::CurrentDomain->AppendPrivatePath("plugins\\");

	AppDomain^ currentDomain = AppDomain::CurrentDomain;
	currentDomain->AssemblyResolve += gcnew ResolveEventHandler(&AssemblyLoader);

	for each (String^ file in files) {
		try {
			String^ msg = String::Format("Loading Plugin Assembly '{0}'...", file);
			Console::WriteLine(msg);
			MessageBox(nullptr, static_cast<LPCWSTR>(Runtime::InteropServices::Marshal::StringToHGlobalUni(msg).ToPointer()), L"Loader", 0);
			Assembly^ pluginAssembly = Assembly::LoadFrom(file);

			auto types = gcnew List<Type^> (pluginAssembly->GetTypes());

			Type^ pluginClass = Enumerable::SingleOrDefault(Enumerable::Where(types, gcnew Func<Type^, bool>(&IsIPlugin)));
			IPlugin^ plugin = static_cast<IPlugin^>(Activator::CreateInstance(pluginClass));
			plugin->Initialize();
		} catch(Exception^ e) {
			String^ errorMsg = String::Format("Error during load of Assembly '{0}'\nError: {1}!", file, e->Message);
			MessageBox(nullptr, static_cast<LPCWSTR>(Runtime::InteropServices::Marshal::StringToHGlobalUni(errorMsg).ToPointer()), L"Loader", 0);
		}
	}

	return true;
}
