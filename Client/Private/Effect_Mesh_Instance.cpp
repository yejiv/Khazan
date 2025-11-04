#include "Effect_Mesh_Instance.h"
#include "Effect_Prefab.h"
#include "GameInstance.h"

CEffect_Mesh_Instance::CEffect_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CEffect_Element{pDevice, pDeviceContext}
{
}

CEffect_Mesh_Instance::CEffect_Mesh_Instance(const CEffect_Mesh_Instance& Prototype)
    : CEffect_Element(Prototype)
    , m_sData{ Prototype.m_sData }
{
    m_pVIBufferCom->Clone(Prototype.m_pVIBufferCom);
}

HRESULT CEffect_Mesh_Instance::Initialize_Prototype(void* pArg)
{
    __super::Initialize_Prototype();

    if (FAILED(Ready_Component()))
        return E_FAIL;

    //Apply(pArg);
    m_sData = *static_cast<PARTICLE_DESC*>(pArg);
    m_pVIBufferCom = CVIBuffer_Mesh_Instance::Create(m_pDevice, m_pContext, &m_sData);  //이걸 프로토타입으로 바꾸고 객체 클론할 땐 m_pVIBuffer->Clone 직접 호출
    m_iEffect_Type = 1; //필요할까
    m_fScrollSpeed = m_sData.iScrollSpeed;

    return S_OK;
}

HRESULT CEffect_Mesh_Instance::Initialize_Clone()
{
    __super::Initialize_Clone(nullptr);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CEffect_Mesh_Instance::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CEffect_Mesh_Instance::Update(_float fTimeDelta)
{
    for(auto it = m_TimeTracks.begin(); it != m_TimeTracks.end();)
    {
        it->fCurTime += fTimeDelta;

        if(it->fCurTime > it->fDurTime && it->EventType != 0)
        {
            dynamic_cast<CVIBuffer_Mesh_Instance*>(m_pVIBufferCom)->Remove_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE(it->EventType - 1));
            it = m_TimeTracks.erase(it);
        }
        else
            ++it;
    }

    m_bRunning = (m_pVIBufferCom->Update(fTimeDelta) == true && m_TimeTracks.size() == 0) ? false : true;
    
    if (m_sData.bGravity)
        m_pVIBufferCom->UpdateGravity(fTimeDelta);

    __super::Update(fTimeDelta);

    /* Edit */
    if (m_TimeTracks.size() == 0)
        m_pVIBufferCom->Remove_Speed(); 
}

void CEffect_Mesh_Instance::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CEffect_Mesh_Instance::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;
    //m_pShaderCom->Begin((_uint)m_Data.TextureBindType);
    m_pShaderCom->Begin((_uint)m_sData.bIsFresnel);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CEffect_Mesh_Instance::Reset()
{
    __super::Reset();
    m_pVIBufferCom->Reset();
}

void CEffect_Mesh_Instance::SetSpreadData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE::SPREAD_SPEED, data.fSpreadSpeed);
    m_pVIBufferCom->Setting_Pivot(data.fPivot);
    m_sData.bGravity = data.bGravity;
    SetData(ENUM_CLASS(data.eEventType), data.fDuration); 
}

void CEffect_Mesh_Instance::SetRotateData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE::ROTATION_SPEED, data.fRotationSpeed);
    m_pVIBufferCom->Setting_Pivot(data.fPivot);
    SetData(ENUM_CLASS(data.eEventType),data.fDuration);
}

void CEffect_Mesh_Instance::SetTwinkleData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE::SCALE_SPEED, data.fScaleSpeed);
    SetData(ENUM_CLASS(data.eEventType),data.fDuration);
}

void CEffect_Mesh_Instance::SetUpwardData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE::UPWARD_SPEED, data.fUpwardSpeed);
    m_sData.bGravity = data.bGravity;
    SetData(ENUM_CLASS(data.eEventType),data.fDuration);
}

void CEffect_Mesh_Instance::SetScrollData(void* pArg)
{
    m_fScrollSpeed.x = *static_cast<_float*>(pArg);
    m_fScrollSpeed.y = *(static_cast<_float*>(pArg) + 1);
}

HRESULT CEffect_Mesh_Instance::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_Particle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Slash"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_MeshEffect_Masking"),
        TEXT("Com_TextureMask"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Mesh_Instance::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if(FAILED(m_pShaderCom->Bind_RawValue("g_vSourceColor", &m_sData.vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_ScrollSpeed", &m_fScrollSpeed, sizeof(_float2))))
        return E_FAIL; 

    _bool ScrollDir = m_sData.bIsScrollVertical;
    if (FAILED(m_pShaderCom->Bind_Bool("g_MaskScrollYDir", &ScrollDir)))
        return E_FAIL;
    
    _bool IsScrollInv = m_sData.bIsScrollInverse;
    if (FAILED(m_pShaderCom->Bind_Bool("g_MaskScrollInv", &IsScrollInv)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskScrollSpeed", &m_sData.fMaskScrollSpeed, sizeof(_float))))
        return E_FAIL;
    
    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", m_sData.iTextureIdx)))
        return E_FAIL;
    
    if (FAILED(m_pMaskTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", m_sData.iMaskTextureIdx)))
        return E_FAIL;

    return S_OK;
}

CEffect_Mesh_Instance* CEffect_Mesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CEffect_Mesh_Instance* pInstance = new CEffect_Mesh_Instance(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Mesh_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

Engine::CEffect_Element* CEffect_Mesh_Instance::Clone()
{
    CEffect_Mesh_Instance* pInstance = new CEffect_Mesh_Instance(*this);

    if (FAILED(pInstance->Initialize_Clone()))
    {
        MSG_BOX(TEXT("CEffect_Mesh_Instance :: Clone Error!!!!"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Mesh_Instance::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pMaskTextureCom);
    Safe_Release(m_pVIBufferCom);
}
