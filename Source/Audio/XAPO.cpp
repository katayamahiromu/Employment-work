#include"XAPO.h"

XAPO_REGISTRATION_PROPERTIES TestXAPO::xapoRegProp =
{
	__uuidof(TestXAPO),
	L"TestXAPO",
	L"abc",
	1.0,
	XAPOBASE_DEFAULT_FLAG,
	1,1,1,1
};

TestXAPO::TestXAPO():CXAPOBase(&xapoRegProp)
{
}

HRESULT TestXAPO::LockForProcess(
	UINT32 inputLockParameterCount,
	const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pInputLockParameter,
	UINT32 OutputLockedParaneterCount,
	const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pOutputLockParameter
	)
{

	inputFmt = *pInputLockParameter[0].pFormat;
	outputFmt = *pOutputLockParameter[0].pFormat;

	return CXAPOBase::LockForProcess(
		inputLockParameterCount,
		pInputLockParameter,
		OutputLockedParaneterCount,
		pOutputLockParameter
	);
}

void TestXAPO::Process(
	UINT32 InputProcessParameterCount,
	const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameter,
	UINT32 outputProcessParameterCount,
	XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameter,
	BOOL IsEnable
)
{
	const XAPO_PROCESS_BUFFER_PARAMETERS& inputParam = pInputProcessParameter[0];
	XAPO_PROCESS_BUFFER_PARAMETERS& outputParam = pOutputProcessParameter[0];

	memcpy(outputParam.pBuffer, inputParam.pBuffer, outputFmt.nBlockAlign * inputParam.ValidFrameCount);

	outputParam.ValidFrameCount = inputParam.ValidFrameCount;
	outputParam.BufferFlags = inputParam.BufferFlags;
}