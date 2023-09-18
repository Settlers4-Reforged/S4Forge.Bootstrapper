#pragma once

namespace NetModAPI {
	
	interface class IPatch : public System::IDisposable {
		void Apply();
		void Revert();
	};

	ref class Patch : public IPatch {
	protected:
		hlib::AbstractPatch* m_handle;

		Patch(hlib::AbstractPatch* handle);
		Patch() {}
	public:
		Patch(int address, array<System::Byte> ^patch);
		Patch(int address, array<System::Byte> ^patch, array<System::Byte> ^expectedOrigin);
		Patch(int address, System::Int32 patch); // patch 4 bytes
		Patch(int address, System::Int32 patch, System::Int32 expectedOrig);
		Patch(int address, System::Byte patch); // patch 1 bytes
		Patch(int address, System::Byte patch, System::Byte expected); // patch 1 bytes

		~Patch() {
			delete this->m_handle;
		}

		virtual void Apply();
		virtual void Revert();
	};

	ref class JmpPatch : public Patch {
	public:
		JmpPatch(System::IntPtr address, System::IntPtr jumpTargetAddress, int nops);
		JmpPatch(System::IntPtr address, System::IntPtr jumpTargetAddress, array<System::Byte> ^expectedOrigin, int nops);
	};

	ref class CallPatch : public Patch {
	public:
		CallPatch(System::IntPtr address, System::IntPtr callTargetAddress, int nops);
		CallPatch(System::IntPtr address, System::IntPtr callTargetAddress, array<System::Byte> ^expectedOrigin, int nops);
	};

	ref class NopPatch : public Patch {
	public:
		NopPatch(System::IntPtr address, int nops);
		NopPatch(System::IntPtr address, array<System::Byte> ^expectedOrigin, int nops);
	};
	
}
