#include "Effect_Sprite.h"
#include "Effect_Prefab.h"
#include "GameInstance.h"

CEffect_Sprite::CEffect_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CEffect_Element{pDevice, pDeviceContext}
{
}

CEffect_Sprite::CEffect_Sprite(const CEffect_Sprite& Prototype)
    : CEffect_Element(Prototype)
{
}

HRESULT CEffect_Sprite::Initialize_Clone(void* pArg)
{
    __super::Initialize_Clone(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    Apply(pArg);

    m_pTransformCom->Scale(_float3(2.f, 2.f, 2.f));
    //m_pTransformCom->Scale(_float3(m_sData.fSize, m_sData.fSize * m_sData.fSizeRatio, m_sData.fSize));

    return S_OK;
}

void CEffect_Sprite::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CEffect_Sprite::Update(_float fTimeDelta)
{
    m_fCurTime += fTimeDelta * 10.f;

    if (m_fCurTime > m_sData.fSpriteSpeed)
    {
        ++ m_iUVIdx;
        m_fCurTime = 0.f;
    }

    m_pTransformCom->Scaling(_float3(m_sData.ScalingValue, m_sData.ScalingValue, m_sData.ScalingValue));
    const _float4* CamPos = m_pGameInstance->Get_CamPosition();
    m_pTransformCom->LookAt(XMLoadFloat4(CamPos));
 
    if (m_iUVIdx == (m_sData.iCol * m_sData.iRow))
    {
        if (m_sData.IsLoop == false)
            m_TimeTracks.pop_back(); 
        m_iUVIdx = 0;
        m_bRunning = false;
    }
    //(뭔가 끝내라는 이벤트 -> 이거 루프 세팅 false로 바꿔주기)
    
    __super::Update(fTimeDelta);
}

void CEffect_Sprite::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CEffect_Sprite::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    //m_pShaderCom->Begin((_uint)m_Data.TextureBindType);
    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CEffect_Sprite::Save_Data(ofstream& os)
{
    os.write(reinterpret_cast<char*>(&m_iEffect_Type), sizeof(_uint));
    os.write(reinterpret_cast<char*>(&m_sData), sizeof(SPRITE_DESC));
}

void CEffect_Sprite::Edit_Element()
{
    _bool            loop = (_int)m_sEditingData.IsLoop;

    //checkBox - isloop
    ImGui::InputFloat("Size : ", reinterpret_cast<_float*>(&m_sEditingData.fSize));
    ImGui::InputFloat("Size Ratio : ", &m_sEditingData.fSizeRatio);
    ImGui::InputFloat("Sprite Speed : ", reinterpret_cast<_float*>(&m_sEditingData.fSpriteSpeed));

    ImGui::ColorEdit4("MyColorWithAlpha",(float*)&m_sEditingData.vColor);
    
    const char* textures[] = {"test0","test1"};
    ImGui::Combo("Textures", reinterpret_cast<int*>(&m_sEditingData.iTextureIdx), textures, IM_ARRAYSIZE(textures));
    ImGui::InputFloat("Scaling Value : ", reinterpret_cast<_float*>(&m_sEditingData.ScalingValue));
    ImGui::InputInt("Col : ", reinterpret_cast<int*>(&m_sEditingData.iCol));
    ImGui::InputInt("Row : ", reinterpret_cast<int*>(&m_sEditingData.iRow));
    ImGui::Checkbox("Sprite Loop", &loop);

    m_sEditingData.IsLoop = loop;

    if (ImGui::Button("Apply"))
        Apply(&m_sEditingData);
    if (ImGui::Button("Revert"))
        RevertChanges();
}

void CEffect_Sprite::RevertChanges()
{
    m_sEditingData = m_sData;
}

void CEffect_Sprite::Reset()
{
    __super::Reset();
    //m_pVIBufferCom->Reset();
}

void CEffect_Sprite::Active()
{
    __super::Active();

    m_fCurTime = 0.f;
    m_iUVIdx = 0;
    m_bRunning = true;
}

HRESULT CEffect_Sprite::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosSpriteTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Sprite_Effect"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Sprite::Bind_ShaderResources()
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

    if(FAILED(m_pShaderCom->Bind_RawValue("g_vSourceColor", &m_sData.vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_numCols", &iCol, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_numRows", &iRow, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_FrameIdx", &UVIdx, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fSizeRatio", &m_sData.fSizeRatio, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fSize", &m_sData.fSize, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_sData.iTextureIdx)))
        return E_FAIL;


    return S_OK;
}

void CEffect_Sprite::Apply(void* pArg)
{
    m_sData = *static_cast<SPRITE_DESC*>(pArg); 
    m_sEditingData = m_sData;
}

CEffect_Sprite* CEffect_Sprite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CEffect_Sprite* pInstance = new CEffect_Sprite(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Sprite"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Sprite::Clone(void* pArg)
{
    CEffect_Sprite* pInstance = new CEffect_Sprite(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Sprite"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Sprite::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}





