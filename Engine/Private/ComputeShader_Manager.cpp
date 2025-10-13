#include "ComputeShader_Manager.h"
#include "ComputeShader.h"

CComputeShader_Manager::CComputeShader_Manager()
{
}

HRESULT CComputeShader_Manager::Initialize()
{
	m_Jobs.resize(ENUM_CLASS(COMPUTEJOP::END));

    return S_OK;
}

void CComputeShader_Manager::Add_Job(COMPUTEJOP eJobTag, const COMPUTE_JOB_DESC& Desc, _bool isExecuteNow) // Default Parameter false
{
	if (isExecuteNow)
	{
		Desc.pShader->Execute(Desc.PassDesc);
		return;
	}

	COMPUTE_JOB_DESC CopyDesc = Desc;

	for (auto& pSRV : Desc.PassDesc.SRVs)
		Safe_AddRef(pSRV);
	for (auto& pUAV : Desc.PassDesc.UAVs)
		Safe_AddRef(pUAV);
	for (auto& pConstantBuffer : Desc.PassDesc.ConstantBuffers)
		Safe_AddRef(pConstantBuffer);
	
	m_Jobs[ENUM_CLASS(eJobTag)].emplace_back(Desc);
}

void CComputeShader_Manager::Execute_Job(COMPUTEJOP eJobTag)
{
	for (auto& Job : m_Jobs[ENUM_CLASS(eJobTag)])
	{
		if (Job.pShader)
			Job.pShader->Execute(Job.PassDesc);

		for (auto& pSRV : Job.PassDesc.SRVs)
			Safe_Release(pSRV);
		for (auto& pUAV : Job.PassDesc.UAVs)
			Safe_Release(pUAV);
		for (auto& pConstantBuffer : Job.PassDesc.ConstantBuffers)
			Safe_Release(pConstantBuffer);
	}

	m_Jobs[ENUM_CLASS(eJobTag)].clear();
}

CComputeShader_Manager* CComputeShader_Manager::Create()
{
	CComputeShader_Manager* pInstance = new CComputeShader_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CComputeShader_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CComputeShader_Manager::Free()
{
    __super::Free();

	for (auto& Jobs : m_Jobs)
	{
		for (auto& Job : Jobs)
		{
			for (auto& pSRV : Job.PassDesc.SRVs)
				Safe_Release(pSRV);
			for (auto& pUAV : Job.PassDesc.UAVs)
				Safe_Release(pUAV);
			for (auto& pConstantBuffer : Job.PassDesc.ConstantBuffers)
				Safe_Release(pConstantBuffer);
		
			Safe_Release(Job.pShader);
		}
	}
}
