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

void CLight_Manager::Update(_float fTimeDelta)
{
    for (auto& Pair : m_pLights[m_pGameInstance->Get_CurrentLevelID()])
    {
        if (Pair.second->isEnable())
            Pair.second->Update(fTimeDelta);
    }
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

_bool CLight_Manager::Is_LightEnable(const _wstring& strLightTag, _uint iLevelIndex)
{
	CLight* pLight = Find_Light(strLightTag, iLevelIndex);
	CHECK_NULLPTR(pLight, false);

	return pLight->isEnable();
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer, _uint iLevelIndex)
{
	for (auto& pLight : m_pLights[iLevelIndex])
	{
        CLight* pEnableLight = pLight.second;
        if (nullptr == pEnableLight)
            continue;

		if (true == pEnableLight->isEnable())
		{
            const LIGHT_DESC LightDesc = *pEnableLight->Get_LightDesc();

            //  if (LIGHT_DESC::POINT == LightDesc.eType && false == m_pGameInstance->isIn_Frustum_WorldSpace(XMLoadFloat4(&LightDesc.vPosition), LightDesc.fRange * 1.1f))
            //  {
            //      continue;
            //  }

			pEnableLight->Render(pShader, pVIBuffer);
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

void CLight_Manager::Start_LightTransition(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_TRANSITION_DESC& Desc, _bool isRestore)
{
    CLight* pLight = Find_Light(strLightTag, iLevelIndex);
    if (nullptr == pLight || nullptr != m_pTransLight)
        return;
    
    pLight->Start_LightTransition(Desc, isRestore);
}

void CLight_Manager::Backup_LightDesc(const _wstring& strLightTag, _uint iLevelIndex)
{
    CLight* pLight = Find_Light(strLightTag, iLevelIndex);
    if (nullptr == pLight)
        return;
    
    LIGHT_DESC LightDesc = *pLight->Get_LightDesc();
    m_OriginalLightDesc[strLightTag] = LightDesc; // 덮어씌우기
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(const _wstring& strLightTag, _uint iLevelIndex)
{
	CLight* pLight = Find_Light(strLightTag, iLevelIndex);
	if (nullptr == pLight)
		return nullptr;

	return pLight->Get_LightDesc();
}

const vector<_wstring>& CLight_Manager::Get_LightTags(_uint iLevelIndex)
{
    m_strLightTags.clear();

    for (auto& Pair : m_pLights[iLevelIndex])
    {
        if (nullptr != Pair.second)
            m_strLightTags.push_back(Pair.first);
    }

    return m_strLightTags;
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

    Safe_Release(m_pTransLight);

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLights[i])
			Safe_Release(Pair.second);
		m_pLights[i].clear();
	}

	Safe_Delete_Array(m_pLights);

	Safe_Release(m_pGameInstance);
}
