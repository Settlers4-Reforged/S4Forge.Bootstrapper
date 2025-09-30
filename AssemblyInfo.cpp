#include "pch.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

[assembly:AssemblyTitleAttribute(L"NetModAPI")];
[assembly:AssemblyDescriptionAttribute(L"")];
[assembly:AssemblyConfigurationAttribute(L"")];
[assembly:AssemblyCompanyAttribute(L"")];
[assembly:AssemblyProductAttribute(L"NetModAPI")];
[assembly:AssemblyCopyrightAttribute(L"Copyright (c)  2023")];
[assembly:AssemblyTrademarkAttribute(L"")];
[assembly:AssemblyCultureAttribute(L"")];

[assembly:AssemblyVersionAttribute(FORGE_VERSION_STRING)] ;
[assembly:AssemblyFileVersionAttribute(FORGE_VERSION_STRING)] ;

[assembly:ComVisible(false)];

// Limits visibility of mostly everything to Forge for now
// This Wrapper should only really ever be used by Forge
// Especially because Forge wraps the full functionality (hopefully...)
[assembly:InternalsVisibleToAttribute("S4Forge")];
