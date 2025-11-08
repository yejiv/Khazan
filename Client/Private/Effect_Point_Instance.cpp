#include "Effect_Point_Instance.h"
#include "Effect_Prefab.h"
#include "GameInstance.h"

CEffect_Point_Instance::CEffect_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CEffect_Element{pDevice, pDeviceContext}
{
}

CEffect_Point_Instance::CEffect_Point_Instance(const CEffect_Point_Instance& Prototype)
    : CEffect_Element(Prototype)
    , m_sData { Prototype.m_sData }
{
    m_pVIBufferCom = dynamic_cast<CVIBuffer_Point_Instance*>(Prototype.m_pVIBufferCom->Clone(nullptr));
}

HRESULT CEffect_Point_Instance::Initialize_Prototype(void* pArg)
{
    __super::Initialize_Prototype();
    m_sData = *static_cast<PARTICLE_DESC*>(pArg);

    const char* NoiseFormat = "../Bin/Resources/Effect/Noise/Noise%d.png";

    char finalNoisePathBuffer[MAX_PATH] = {};
    ZeroMemory(m_sData.pNoiseFilePath, sizeof(m_sData.pNoiseFilePath));
    sprintf_s(finalNoisePathBuffer, MAX_PATH, NoiseFormat, m_sData.iTurbulenceTextureIdx);
    strcpy_s(m_sData.pNoiseFilePath, MAX_PATH, finalNoisePathBuffer);

    m_pVIBufferCom = CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &m_sData);
    m_iEffect_Type = 0; //필요할까

    return S_OK;
}

HRESULT CEffect_Point_Instance::Initialize_Clone()
{
    __super::Initialize_Clone(nullptr);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CEffect_Point_Instance::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CEffect_Point_Instance::Update(_float fTimeDelta)
{
    m_fAccTime += fTimeDelta;
    m_fSpriteTime += fTimeDelta;

    for(auto it = m_TimeTracks.begin(); it != m_TimeTracks.end();)
    {
        it->fCurTime += fTimeDelta;

        if(it->fCurTime > it->fDurTime && it->EventType != 0)
        {
            dynamic_cast<CVIBuffer_Point_Instance*>(m_pVIBufferCom)->Remove_Speed(CVIBuffer_Point_Instance::SPEED_VALUE(it->EventType - 1));
            it = m_TimeTracks.erase(it);
        }
        else
            ++it;
    }

    m_bRunning = (m_pVIBufferCom->Update(fTimeDelta) == true && m_TimeTracks.size() == 0) ? false : true;

    if (m_sData.bGravity)
        m_pVIBufferCom->UpdateGravity(fTimeDelta);

    if (m_sData.bIsTurbulence)
    {
        m_fAccTime += fTimeDelta;
        m_pVIBufferCom->UpdateTurbulence(fTimeDelta, m_fAccTime);
    }

    if (m_fSpriteTime > m_sData.fSpriteSpeed)
    {
        ++m_iUVIdx;
        m_fSpriteTime = 0.f;
    }

    if (m_iUVIdx == (m_sData.iCol * m_sData.iRow))
        m_iUVIdx = 0;

    __super::Update(fTimeDelta);
}

void CEffect_Point_Instance::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CEffect_Point_Instance::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    //m_pShaderCom->Begin((_uint)m_Data.TextureBindType);
    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CEffect_Point_Instance::Reset()
{
    __super::Reset();
    m_pVIBufferCom->Reset();
    m_fAccTime = 0.f;
    m_fSpriteTime = 0.f;
    m_iUVIdx = 0;
    m_bRunning = true;
}

void CEffect_Point_Instance::SetSpreadData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Point_Instance::SPEED_VALUE::SPREAD_SPEED, data.fSpreadSpeed);
    m_pVIBufferCom->Setting_Pivot(data.fPivot);
    m_sData.bGravity = data.bGravity;
    SetData(ENUM_CLASS(data.eEventType), data.fDuration); 
}

void CEffect_Point_Instance::SetRotateData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Point_Instance::SPEED_VALUE::ROTATION_SPEED, data.fRotationSpeed);
    m_pVIBufferCom->Setting_Pivot(data.fPivot);
    SetData(ENUM_CLASS(data.eEventType),data.fDuration);
}

void CEffect_Point_Instance::SetTwinkleData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Point_Instance::SPEED_VALUE::SCALE_SPEED, data.fScaleSpeed);
    SetData(ENUM_CLASS(data.eEventType),data.fDuration);
}

void CEffect_Point_Instance::SetUpwardData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Point_Instance::SPEED_VALUE::UPWARD_SPEED, data.fUpwardSpeed);
    m_sData.bGravity = data.bGravity;
    SetData(ENUM_CLASS(data.eEventType),data.fDuration);
}

void CEffect_Point_Instance::SetScrollData(void* pArg)
{
    m_fScrollSpeed.x = *static_cast<_float*>(pArg);
    m_fScrollSpeed.y = *(static_cast<_float*>(pArg) + 1);
}

HRESULT CEffect_Point_Instance::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Particle_Prototype"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_MeshEffect_Masking"),
        TEXT("Com_TextureMask"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_MeshEffect_Dissolve"),
        TEXT("Com_TextureDissolve"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sprite_Effect"),
        TEXT("Com_TextureSprite"), reinterpret_cast<CComponent**>(&m_pSpriteTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Point_Instance::Bind_ShaderResources()
{
    _float iCol = static_cast<_float>(m_sData.iCol);
    _float iRow = static_cast<_float>(m_sData.iRow);
    _float UVIdx = static_cast<_float>(m_iUVIdx);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vSourceColor", &m_sData.vColor, sizeof(_float4))))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fSizeRatio", &m_sData.fSizeRatio, sizeof(_float))))
        return E_FAIL;

    _bool ScrollDir = m_sData.bIsScrollVertical;
    if (FAILED(m_pShaderCom->Bind_Bool("g_MaskScrollYDir", &ScrollDir)))
        return E_FAIL;

    _bool IsScrollInv = m_sData.bIsScrollInverse;
    if (FAILED(m_pShaderCom->Bind_Bool("g_MaskScrollYDir", &IsScrollInv)))
        return E_FAIL;

    _bool IsDissolve = m_sData.sDissolveData.bIsDissolve;
    if (FAILED(m_pShaderCom->Bind_Bool("g_IsDisolve", &IsDissolve)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_EdgeWidth", &m_sData.sDissolveData.fDissolveEdgeWidth, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_EdgeColor", &m_sData.sDissolveData.fDissolveEdgeColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskScrollSpeed", &m_sData.fMaskScrollSpeed, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_numCols", &iCol, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_numRows", &iRow, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_FrameIdx", &UVIdx, sizeof(_float))))
        return E_FAIL;

    if (m_sData.iCol > 1 || m_sData.iRow > 1)
    {
        if (FAILED(m_pSpriteTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", m_sData.iTextureIdx)))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", m_sData.iTextureIdx)))
            return E_FAIL;
    }


    if (FAILED(m_pMaskTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", m_sData.iMaskTextureIdx)))
        return E_FAIL;

    if (FAILED(m_pDissolveTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DisolveTexture", m_sData.sDissolveData.iDissolveTextureIdx)))
        return E_FAIL;

    return S_OK;
}

CEffect_Point_Instance* CEffect_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CEffect_Point_Instance* pInstance = new CEffect_Point_Instance(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Point_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

Engine::CEffect_Element* CEffect_Point_Instance::Clone()
{
    CEffect_Point_Instance* pInstance = new CEffect_Point_Instance(*this);

    if (FAILED(pInstance->Initialize_Clone()))
    {
        MSG_BOX(TEXT("CEffect_Point_Instance :: Clone Error!!!!"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Point_Instance::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pSpriteTextureCom);
    Safe_Release(m_pMaskTextureCom);
    Safe_Release(m_pDissolveTextureCom);
    Safe_Release(m_pVIBufferCom);
}

