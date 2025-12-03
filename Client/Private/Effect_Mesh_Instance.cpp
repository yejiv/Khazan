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
    , m_bIsNormal {Prototype.m_bIsNormal}
{
    m_pVIBufferCom = dynamic_cast<CVIBuffer_Mesh_Instance*>(Prototype.m_pVIBufferCom->Clone(nullptr));
}

HRESULT CEffect_Mesh_Instance::Initialize_Prototype(void* pArg)
{
    __super::Initialize_Prototype();

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_sData = *static_cast<PARTICLE_DESC*>(pArg);
    m_bIsNormal = (m_sData.sDissolveData.bIsDissolve && m_sData.sDissolveData.iDissolveTextureIdx > 4) ? 1 : 0;

    const char* NoiseFormat = "../Bin/Resources/Effect/Noise/Noise%d.png";

    char finalNoisePathBuffer[MAX_PATH] = {};
    ZeroMemory(m_sData.pNoiseFilePath, sizeof(m_sData.pNoiseFilePath));
    sprintf_s(finalNoisePathBuffer, MAX_PATH, NoiseFormat, m_sData.iTurbulenceTextureIdx);
    strcpy_s(m_sData.pNoiseFilePath, MAX_PATH, finalNoisePathBuffer);

    m_pVIBufferCom = CVIBuffer_Mesh_Instance::Create(m_pDevice, m_pContext, &m_sData);  //이걸 프로토타입으로 바꾸고 객체 클론할 땐 m_pVIBuffer->Clone 직접 호출

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
    m_fAccTime += fTimeDelta;

    for(auto it = m_TimeTracks.begin(); it != m_TimeTracks.end();)
    {
        it->fCurTime += fTimeDelta;

        if(it->fCurTime > it->fDurTime)
        {
            if (m_pVIBufferCom->isLoop() && m_TimeTracks.size() == 1)
            {
                ++it;
                continue;
            }

            dynamic_cast<CVIBuffer_Mesh_Instance*>(m_pVIBufferCom)->Remove_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE(it->EventType));
            it = m_TimeTracks.erase(it);
        }
        else
            ++it;
    }

    m_bRunning = (m_pVIBufferCom->Update(fTimeDelta) == true && m_TimeTracks.size() == 0) ? false : true;
    
    if (m_sData.bGravity)
        m_pVIBufferCom->UpdateGravity(fTimeDelta);


    if (m_sData.bIsTurbulence)
        m_pVIBufferCom->UpdateTurbulence(fTimeDelta, m_fAccTime);

    __super::Update(fTimeDelta);
     
}

void CEffect_Mesh_Instance::Late_Update(_float fTimeDelta)
{
    if (m_bIsNormal)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);
    else
        __super::Late_Update(fTimeDelta);
}

HRESULT CEffect_Mesh_Instance::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (m_bIsNormal)
        m_pShaderCom->Begin(2);
    else
        m_pShaderCom->Begin((_uint)m_sData.bIsFresnel);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CEffect_Mesh_Instance::SetLoopOff()
{
    m_pVIBufferCom->Setting_Loop(false); 
}

void CEffect_Mesh_Instance::Reset()
{
    __super::Reset();
    m_fAccTime = 0.f;
    m_pVIBufferCom->Reset();
}

void CEffect_Mesh_Instance::SetSpreadData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE::SPREAD_SPEED, data.fSpreadSpeed);
    m_pVIBufferCom->Setting_Pivot(data.fPivot);
    m_sData.bGravity = data.bGravity;
    SetData(ENUM_CLASS(CVIBuffer_Mesh_Instance::SPEED_VALUE::SPREAD_SPEED), data.fDuration);
}

void CEffect_Mesh_Instance::SetRotateData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE::ROTATION_SPEED, data.fRotationSpeed);
    m_pVIBufferCom->Setting_Pivot(data.fPivot);
    SetData(ENUM_CLASS(CVIBuffer_Mesh_Instance::SPEED_VALUE::ROTATION_SPEED),data.fDuration);
}

void CEffect_Mesh_Instance::SetTwinkleData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE::SCALE_SPEED, data.fScaleSpeed);
    SetData(ENUM_CLASS(CVIBuffer_Mesh_Instance::SPEED_VALUE::SCALE_SPEED),data.fDuration);
}

void CEffect_Mesh_Instance::SetUpwardData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE::UPWARD_SPEED, data.fUpwardSpeed);
    m_sData.bGravity = data.bGravity;
    SetData(ENUM_CLASS(CVIBuffer_Mesh_Instance::SPEED_VALUE::UPWARD_SPEED),data.fDuration);
}

void CEffect_Mesh_Instance::SetScrollData(void* pArg)
{
    m_fScrollSpeed.x = *static_cast<_float*>(pArg);
    m_fScrollSpeed.y = *(static_cast<_float*>(pArg) + 1);
}

void CEffect_Mesh_Instance::SetStop()
{
    m_pVIBufferCom->Setting_Loop(false);
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

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_MeshEffect_Dissolve"),
        TEXT("Com_TextureDissolve"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_MeshEffect_Normal"),
        TEXT("Com_TextureNormal"), reinterpret_cast<CComponent**>(&m_pNormalTextureCom), nullptr)))
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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_ScrollSpeed", &m_sData.iScrollSpeed, sizeof(_float2))))
        return E_FAIL; 

    _bool ScrollDir = m_sData.bIsScrollVertical;
    if (FAILED(m_pShaderCom->Bind_Bool("g_MaskScrollYDir", &ScrollDir)))
        return E_FAIL;
    
    _bool IsScrollInv = m_sData.bIsScrollInverse;
    if (FAILED(m_pShaderCom->Bind_Bool("g_MaskScrollInv", &IsScrollInv)))
        return E_FAIL;

    _bool IsDissolve = m_sData.sDissolveData.bIsDissolve;
    if (FAILED(m_pShaderCom->Bind_Bool("g_IsDissolve", &IsDissolve)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_EdgeWidth", &m_sData.sDissolveData.fDissolveEdgeWidth, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_EdgeColor", &m_sData.sDissolveData.fDissolveEdgeColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskScrollSpeed", &m_sData.fMaskScrollSpeed, sizeof(_float))))
        return E_FAIL;
    
    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", m_sData.iTextureIdx)))
        return E_FAIL;
    
    if (FAILED(m_pMaskTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", m_sData.iMaskTextureIdx)))
        return E_FAIL;
     
    if (!m_bIsNormal)
    {
        if (FAILED(m_pDissolveTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", m_sData.sDissolveData.iDissolveTextureIdx)))
            return E_FAIL;
    }
    else
    {
        _bool IsFresnel = m_sData.bIsFresnel;
        if (FAILED(m_pNormalTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_NormalTexture", m_sData.sDissolveData.iDissolveTextureIdx - 5)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Bool("g_IsFresnel", &IsFresnel)))
            return E_FAIL;
    }


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
    Safe_Release(m_pDissolveTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pNormalTextureCom);
}
