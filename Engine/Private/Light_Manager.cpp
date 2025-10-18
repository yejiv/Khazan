#include "Light_Manager.h"
#include "Light.h"
#include "GameInstance.h"

CLight_Manager::CLight_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLight_Manager::Initialize(_uint iNumLevels)
{
	m_pLights = new unordered_map<_wstring, class CLight*>[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc, _bool isEnable)
{
	if (nullptr != Find_Light(strLightTag, iLevelIndex))
		return E_FAIL;

	CLight* pLight = CLight::Create(LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	pLight->Set_Enable(isEnable);
	m_pLights[iLevelIndex].emplace(strLightTag, pLight);

	return S_OK;
}

void CLight_Manager::Set_LightDesc(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc)
{
	CLight* pLight = Find_Light(strLightTag, iLevelIndex);
	if (nullptr == pLight)
		return;

	pLight->Set_LightDesc(LightDesc);
}

void CLight_Manager::Set_LightPosition(const _wstring& strLightTag, _uint iLevelIndex, const _float4& vPosition)
{
	CLight* pLight = Find_Light(strLightTag, iLevelIndex);
	if (nullptr == pLight)
		return;

	pLight->Set_Position(vPosition);
}

void CLight_Manager::Set_LightEnable(const _wstring& strLightTag, _uint iLevelIndex, _bool isEnable)
{
	CLight* pLight = Find_Light(strLightTag, iLevelIndex);
	if (nullptr == pLight)
		return;

	pLight->Set_Enable(isEnable);
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer, _uint iLevelIndex)
{
	for (auto& pLight : m_pLights[iLevelIndex])
	{
		if (true == pLight.second->isEnable())
		{
#pragma region 단비가 친 야매 코드래요

			// if (!m_pGameInstance->isIn_Frustum_WorldSpace(XMLoadFloat4(&pLight.second->Get_LightDesc()->vPosition), 3.f) &&
			// 	LIGHT_DESC::POINT == pLight.second->Get_LightDesc()->eType)
			// 	continue;

#pragma endregion

			pLight.second->Render(pShader, pVIBuffer);
		}
	}

	return S_OK;
}

void CLight_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pLights[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLights[iLevelIndex].clear();
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(const _wstring& strLightTag, _uint iLevelIndex)
{
	CLight* pLight = Find_Light(strLightTag, iLevelIndex);
	if (nullptr == pLight)
		return nullptr;

	return pLight->Get_LightDesc();
}

CLight* CLight_Manager::Find_Light(const _wstring& strLightTag, _uint iLevelIndex)
{
	auto iter = m_pLights[iLevelIndex].find(strLightTag);

	if (iter == m_pLights[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CLight_Manager* CLight_Manager::Create(_uint iNumLevels)
{
	CLight_Manager* pInstance = new CLight_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Create : CLight_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight_Manager::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLights[i])
			Safe_Release(Pair.second);
		m_pLights[i].clear();
	}

	Safe_Delete_Array(m_pLights);

	Safe_Release(m_pGameInstance);
}
