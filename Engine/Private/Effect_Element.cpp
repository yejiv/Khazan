#include "Effect_Element.h"
#include "GameInstance.h"

CEffect_Element::CEffect_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CGameObject{pDevice, pDeviceContext}
{
}

CEffect_Element::CEffect_Element(const CEffect_Element& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CEffect_Element::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Element::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(&pArg)))
        return E_FAIL;
    
    return S_OK;
}

void CEffect_Element::Priority_Update(_float fTimeDelta)
{ 
}

void CEffect_Element::Update(_float fTimeDelta)
{ 
    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CEffect_Element::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::WEIGHT_BLEND, this);
}

HRESULT CEffect_Element::Render()
{
    return S_OK;
}

void CEffect_Element::SetSpreadData(void* pArg)
{
}

void CEffect_Element::SetRotateData(void* pArg)
{
}

void CEffect_Element::SetTwinkleData(void* pArg)
{
}

void CEffect_Element::SetUpwardData(void* pArg)
{
}

void CEffect_Element::SetScrollData(void* pArg)
{
}

void CEffect_Element::SetLoopOff()
{
}

void CEffect_Element::SetStop()
{
}

void CEffect_Element::SetData(_uint eventType,_float fDurTime)
{
    TRACK_DATA data;
    data.EventType = eventType;
    data.fDurTime = fDurTime;
    data.fCurTime = 0;
    m_bRunning = true;
    m_TimeTracks.push_back(data);
}

void CEffect_Element::Active()
{
    TRACK_DATA data;
    data.EventType = 0;
    m_TimeTracks.push_back(data);
}

void CEffect_Element::Reset()
{
    m_TimeTracks.clear();
    m_bRunning = false;
}

HRESULT CEffect_Element::Ready_Component()
{
    return S_OK;
}

HRESULT CEffect_Element::Bind_ShaderResources()
{
    return S_OK;
}

void CEffect_Element::Apply(void* pArg)
{
}


CGameObject* CEffect_Element::Clone(void* pArg)
{
    return nullptr;
}

void CEffect_Element::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
}





