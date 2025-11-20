#pragma once
#include<xapobase.h>

class __declspec(uuid("12345678-1234-5678-90AB-1234567890AB"))
TestXAPO : public CXAPOBase
{
public:
	TestXAPO();
	~TestXAPO() {};

	STDMETHOD(LockForProcess)(
		UINT32 inputLockParameterCount,
		const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pInputLockParameter,
		UINT32 OutputLockedParaneterCount,
		const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pOutputLockParameter
		);

	STDMETHOD_(void, Process)(
		UINT32 InputProcessParameterCount,
		const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameter,
		UINT32 outputProcessParameterCount,
		XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameter,
		BOOL IsEnable
		);

private:
	static XAPO_REGISTRATION_PROPERTIES xapoRegProp;//プロパティ
	WAVEFORMATEX inputFmt;
	WAVEFORMATEX outputFmt;
};