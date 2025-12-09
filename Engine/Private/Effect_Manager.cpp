#include "Effect_Manager.h"
#include "Prefab.h"
#include "GameInstance.h"


CEffect_Manager::CEffect_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect_Manager::Initialize(_uint iNumLevels)
{
	m_pEffectLayers = new unordered_map<_wstring, vector<class CPrefab*>>[iNumLevels];
	m_pEffectPools = new unordered_map<_wstring, deque<class CPrefab*>>[iNumLevels];
	m_pRunningEffects = new unordered_map<_wstring, list<class CPrefab*>>[iNumLevels];
	m_iNumLevels = iNumLevels;

	return S_OK;
}
 
void CEffect_Manager::Add_Effect_ToPool(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint iPoolSize)
{
	CPrefab* effect;
	m_iCurLevel = iLayerLevelIndex;

	auto Layer = Find_Effect_Layer(iLayerLevelIndex, strPrototypeTag);
	m_pEffectPools[iLayerLevelIndex][strPrototypeTag];
	m_pRunningEffects[iLayerLevelIndex][strPrototypeTag];
	auto Pool = Find_Effect_Pool(iLayerLevelIndex, strPrototypeTag);

	if (Layer == nullptr)
	{
		//MSG_BOX(TEXT("Effect Layer NULL"));
		return;
	}

	if (Pool == nullptr)
	{
		//MSG_BOX(TEXT("Effect Pool NULL"));
		return;
	}

	for (_uint i = 0; i < iPoolSize; ++i)
	{
		effect = dynamic_cast<CPrefab*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iLayerLevelIndex, strPrototypeTag));

		if (effect == nullptr)
		{
			//MSG_BOX(TEXT("EFFECT ::NULL"));
			return;
		}

		_uint ID = Layer->size();
		
		Layer->push_back(effect);
		Pool->push_back(effect);
		Safe_AddRef(effect);
		effect->SetID(ID);
	}
}

_uint CEffect_Manager::Spawn_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _fvector SpwanPos)
{
    CPrefab* effect{ nullptr };

	auto Pool = Find_Effect_Pool(iLayerLevelIndex, strPrototypeTag);
	auto RunningLayer = Find_RunningEffect_Layer(strPrototypeTag);

    if (nullptr == Pool || Pool->size() == 0 || nullptr == RunningLayer)
    {
        //MSG_BOX(TEXT("Effect Pool이 없거나 Pool에 객체가 없어서 Spwan 실패!!! 아마도 객체가 모자를 확률이 큼"));
        return 0;
	}

    effect = Pool->back();

	Pool->pop_back();
	RunningLayer->push_back(effect);
	effect->ResetChildren();
	effect->UpdatePosition(SpwanPos);

	return effect->GetID();
}

_uint CEffect_Manager::Spawn_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _fvector Quaternion, _gvector Position)
{
    CPrefab* effect{ nullptr };

	auto Pool = Find_Effect_Pool(iLayerLevelIndex, strPrototypeTag);
	auto RunningLayer = Find_RunningEffect_Layer(strPrototypeTag);

	if (nullptr == Pool || Pool->size() == 0 || nullptr == RunningLayer)
	{
		//MSG_BOX(TEXT("Effect Pool이 없거나 Pool에 객체가 없어서 Spwan 실패!!! 아마도 객체가 모자를 확률이 큼"));
		return 0;
	}
    
    effect = Pool->back();

	Pool->pop_back();
	RunningLayer->push_back(effect);
	effect->ResetChildren();
	effect->UpdateWorldMatrix(Quaternion, Position);

	return effect->GetID();
}

_uint CEffect_Manager::Spawn_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _matrix worldmatrix, _gvector Position)
{
    CPrefab* effect{ nullptr };

    auto Pool = Find_Effect_Pool(iLayerLevelIndex, strPrototypeTag);
    auto RunningLayer = Find_RunningEffect_Layer(strPrototypeTag);

    if (nullptr == Pool || Pool->size() == 0 || nullptr == RunningLayer)
    {
        //MSG_BOX(TEXT("Effect Pool이 없거나 Pool에 객체가 없어서 Spwan 실패!!! 아마도 객체가 모자를 확률이 큼"));
        return 0;
    }

    effect = Pool->back();

    Pool->pop_back();
    RunningLayer->push_back(effect);
    effect->ResetChildren();

    _vector Q = Decompose_Rotation(worldmatrix); 
    effect->UpdateWorldMatrix(Q, Position);

    return effect->GetID();
}

void CEffect_Manager::Update_Effect_Position(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID, _fvector SpwanPos)
{
	auto Layer = Find_Effect_Layer(iLayerLevelIndex, strPrototypeTag);

    if (nullptr == Layer || Layer->size() <= ID || nullptr == (*Layer)[ID])
        return;

	(*Layer)[ID]->UpdatePosition(SpwanPos);
}

void CEffect_Manager::Update_Effect_World(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID, _fvector Quaternion, _gvector Position)
{
	auto Layer = Find_Effect_Layer(iLayerLevelIndex, strPrototypeTag);

    if (nullptr == Layer || Layer->size() <= ID || nullptr == (*Layer)[ID])
        return;

	(*Layer)[ID]->UpdateWorldMatrix(Quaternion, Position); 
}

void CEffect_Manager::Update_Effect_World(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID, _matrix worldmatrix, _gvector Position)
{
    auto Layer = Find_Effect_Layer(iLayerLevelIndex, strPrototypeTag);

    if (nullptr == Layer || Layer->size() <= ID || nullptr == (*Layer)[ID])
        return;

    _vector Q = Decompose_Rotation(worldmatrix);

    (*Layer)[ID]->UpdateWorldMatrix(Q, Position);
}

void CEffect_Manager::Stop_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID)
{
	auto Layer = Find_Effect_Layer(iLayerLevelIndex, strPrototypeTag);

    if (nullptr == Layer || Layer->size() <= ID || nullptr == (*Layer)[ID])
        return;

	(*Layer)[ID]->SetClose();
}

void CEffect_Manager::Stop_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag)
{ 
    auto Layer = Find_RunningEffect_Layer(strPrototypeTag);

    if (nullptr == Layer)
        return;

    for (auto effect : *Layer) 
        effect->SetClose(); 
}

void CEffect_Manager::Priority_Update(_float fEffectTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pRunningEffects[m_iCurLevel])
		{
			for (auto& effect : Pair.second)
			{
				if (nullptr != effect)
					effect->Priority_Update(fEffectTimeDelta);
			} 
		}
	}
}

void CEffect_Manager::Update(_float fEffectTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)	//?덈꺼
	{
		for (auto& Pair : m_pRunningEffects[i])	//?댄럺?몃퀎
		{
			for (auto iter = Pair.second.begin(); iter != Pair.second.end();)
			{
				if (nullptr != *iter)
				{
					(*iter)->Update(fEffectTimeDelta);
					if ((*iter)->IsActive() == false)
					{
						auto pool = Find_Effect_Pool(i, Pair.first);
                        if (nullptr == pool)    //임시
                        {
                            iter ++;
                            continue;
                        }
						pool->push_back(*iter);
						iter = Pair.second.erase(iter);
					}
					else
						iter++;
				}
			}
		}
		
	}
}

void CEffect_Manager::Late_Update(_float fEffectTimeDelta)
{
	for (auto& Pair : m_pRunningEffects[m_iCurLevel])
	{
		for (auto& effect : Pair.second)
		{
			if (nullptr != effect)
				effect->Late_Update(fEffectTimeDelta);
		}
	}
}

void CEffect_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pEffectLayers[iLevelIndex])
	{	
		for (auto effect : Pair.second)
			Safe_Release(effect);
		Pair.second.clear();
	}
	m_pEffectLayers[iLevelIndex].clear();

	for (auto Pair : m_pEffectPools[iLevelIndex])
	{
		//for (auto& effect : Pair.second)
		//	Safe_Release(effect);
		Pair.second.clear();
	}
	m_pEffectPools[iLevelIndex].clear();

	for (auto Pair : m_pRunningEffects[iLevelIndex])
	{
		//for (auto& effect : Pair.second)
		//	Safe_Release(effect);
		Pair.second.clear();
	}
	
    m_pRunningEffects[iLevelIndex].clear();
}

deque<class CPrefab*>* CEffect_Manager::Find_Effect_Pool(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	auto	iter = m_pEffectPools[iLayerLevelIndex].find(strLayerTag);

	if (iter == m_pEffectPools[iLayerLevelIndex].end())
	{
		//MSG_BOX(TEXT("Effect Pool??紐살갼?? ?꾨쭏??Tag Error"));
		return nullptr;
	}

	return &iter->second;
}

vector<class CPrefab*>* CEffect_Manager::Find_Effect_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	auto	iter = m_pEffectLayers[iLayerLevelIndex].find(strLayerTag);

	if (iter == m_pEffectLayers[iLayerLevelIndex].end())
	{
		m_pEffectLayers[iLayerLevelIndex][strLayerTag]; 
		iter = m_pEffectLayers[iLayerLevelIndex].find(strLayerTag);
	}

	return &iter->second;
}

list<class CPrefab*>* CEffect_Manager::Find_RunningEffect_Layer(const _wstring& strLayerTag)
{
	auto	iter = m_pRunningEffects[m_iCurLevel].find(strLayerTag);

	if (iter == m_pRunningEffects[m_iCurLevel].end())
	{
		//MSG_BOX(TEXT("Running?섎뒗 Effect Layer ?녿떎?붾뜲 洹몃윺 由ш? ?녿떎;"));
		return nullptr;
	}

	return &iter->second;
}

_vector CEffect_Manager::Decompose_Rotation(_matrix W)
{
    _vector S, Q, T;

    if (!XMMatrixDecompose(&S, &Q, &T, W))
    {
        XMFLOAT4X4 m;
        XMStoreFloat4x4(&m, W);
        _vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
        _vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
        _vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f));

        _matrix RotationMatrix(r0, r1, r2, XMVectorSet(0.f, 0.f, 0.f, 1.f));
        Q = XMQuaternionRotationMatrix(RotationMatrix);
    }

    return Q;
}


CEffect_Manager* CEffect_Manager::Create(_uint iNumLevels)
{
	CEffect_Manager* pInstance = new CEffect_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CEffect_Manager::Free()
{
	__super::Free();	 

	Safe_Release(m_pGameInstance); 

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pEffectLayers[i])
		{
			for (auto effect : Pair.second)
				Safe_Release(effect);
			Pair.second.clear();
		}
		m_pEffectLayers[i].clear();

		for (auto Pair : m_pEffectPools[i])
		{
			for (auto& effect : Pair.second)
                //effect = nullptr;
				Safe_Release(effect);
			Pair.second.clear();
		}
		m_pEffectPools[i].clear();

		for (auto Pair : m_pRunningEffects[i])
		{
			for (auto& effect : Pair.second)
                //effect = nullptr;
                Safe_Release(effect);
			Pair.second.clear();
		}
		m_pRunningEffects[i].clear();
	}

	Safe_Delete_Array(m_pEffectLayers);
	Safe_Delete_Array(m_pEffectPools);
	Safe_Delete_Array(m_pRunningEffects);
}
