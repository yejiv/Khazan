#include "MeshTrail.h"
#include "GameInstance.h"

CMeshTrail::CMeshTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CGameObject{pDevice, pDeviceContext}
{
}

CMeshTrail::CMeshTrail(const CMeshTrail& Prototype)
    : CGameObject(Prototype)
    , m_iTextureIdx { Prototype.m_iTextureIdx}
    , m_fLifeTime{ Prototype.m_fLifeTime }
    , m_iDivisionCount{ Prototype.m_iDivisionCount }
{
}

HRESULT CMeshTrail::Initialize_Prototype()
{
    m_iTextureIdx = 0;
    m_fLifeTime = 0.4f;
    m_iDivisionCount = 5;
    m_vColor = { 1.f, 1.f, 1.f };

    return S_OK;
}

HRESULT CMeshTrail::Initialize_Clone(void* pArg)
{
    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (pArg)
    {
        TRAIL_DESC* dsc = static_cast<TRAIL_DESC*>(pArg);

        m_iTextureIdx = dsc->iTextureIdx;
        m_fLifeTime = dsc->fLifeTime;
        m_iDivisionCount = dsc->iDivisionCount;
        m_vColor = dsc->vColor;
        if (m_iDivisionCount < 1)
        {
            MSG_BOX(TEXT("Division Count is too low"));
            return E_FAIL;
        }
    }

    return S_OK;
}

void CMeshTrail::Priority_Update(_float fTimeDelta)
{
    //일정 시간 지난 TrailPoints들은 삭제

    for (auto it = m_ControlPoints.begin(); it != m_ControlPoints.end(); )
    {
        it->fLifeTime += fTimeDelta;

        if (it->fLifeTime > m_fLifeTime) 
            it = m_ControlPoints.erase(it); 
        else 
            ++it; 
    }
}

void CMeshTrail::Update(_float fTimeDelta)
{
    //캣멀룸으로 TrailPoints구성
    if (m_ControlPoints.size() < 2)
        return;
    
    m_TrailPoints.clear();

    m_TrailPoints.push_back(m_ControlPoints[0]);

    for (_int i = 0; i < m_ControlPoints.size() - 1; ++i)
    {
        for (_int j = 0; j < m_iDivisionCount; ++j)
        {
            _float weight = (float)(j + 1) / (float)(m_iDivisionCount);

            _vector top = XMVectorCatmullRom( XMLoadFloat4(&m_ControlPoints[(i - 1) < 0 ? 0 : i - 1].vTop),
                                              XMLoadFloat4(&m_ControlPoints[i].vTop),
                                              XMLoadFloat4(&m_ControlPoints[i + 1].vTop),
                                              XMLoadFloat4(&m_ControlPoints[(i + 2) > m_ControlPoints.size() - 1 ? i + 1 : i + 2].vTop),
                                              weight);

           _vector bottom = XMVectorCatmullRom( XMLoadFloat4(&m_ControlPoints[(i - 1) < 0 ? 0 : i - 1].vBottom),
                                             XMLoadFloat4(&m_ControlPoints[i].vBottom),
                                             XMLoadFloat4(&m_ControlPoints[i + 1].vBottom),
                                             XMLoadFloat4(&m_ControlPoints[(i + 2) > m_ControlPoints.size() - 1 ? i + 1 : i + 2].vBottom),
                                             weight);
           CVIBuffer_QuadTrail::QUAD_TRAIL_POINT SplineNode;
           XMStoreFloat4(&SplineNode.vTop, top);
           XMStoreFloat4(&SplineNode.vBottom, bottom);

           m_TrailPoints.push_back(SplineNode);
        }
    }
}

void CMeshTrail::Late_Update(_float fTimeDelta)
{
    m_pVIBufferCom->Update(m_TrailPoints);

    if (m_ControlPoints.size() > 1)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::WEIGHT_BLEND, this);
}

HRESULT CMeshTrail::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(2);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CMeshTrail::Add_ControlPoint(_fvector top, _gvector bottom)
{
    if (m_ControlPoints.size() 
        && XMVector4Equal(XMLoadFloat4(&m_ControlPoints.back().vTop), top)
        && XMVector4Equal(XMLoadFloat4(&m_ControlPoints.back().vBottom), bottom))
        return;

    CVIBuffer_QuadTrail::QUAD_TRAIL_POINT newPoint;

    XMStoreFloat4(&newPoint.vTop, top);
    XMStoreFloat4(&newPoint.vBottom, bottom);
    newPoint.fLifeTime = 0.f;

    m_ControlPoints.push_back(newPoint);
}

const TRAIL_CONFIG& CMeshTrail::Get_TrailConfig() const
{
    TRAIL_CONFIG Config{};

    Config.fLifeTime = m_fLifeTime;
    Config.iTextureIdx = m_iTextureIdx;
    Config.iDivisionCount = m_iDivisionCount;
    Config.vColor = m_vColor;

    return Config;
}

void CMeshTrail::Set_TrailConfig(const TRAIL_CONFIG& Config)
{
    m_fLifeTime = Config.fLifeTime;
    m_iTextureIdx = Config.iTextureIdx;
    m_iDivisionCount = Config.iDivisionCount;
    m_vColor = Config.vColor;
}

_uint CMeshTrail::Get_NumTrailTextures()
{
    return m_pTextureCom->Get_NumTextures();
}

ID3D11ShaderResourceView* CMeshTrail::Get_TrailTexture(_uint iIndex)
{
    return m_pTextureCom->Get_Texture(iIndex);
}

HRESULT CMeshTrail::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;
    
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Slash"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;
    
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_QuadTrail"),
        TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMeshTrail::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vTrailColor", &m_vColor, sizeof(_float3))))
        return E_FAIL;

    return S_OK;
}

CMeshTrail* CMeshTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMeshTrail* pInstance = new CMeshTrail(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMeshTrail"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMeshTrail::Clone(void* pArg)
{
    CMeshTrail* pInstance = new CMeshTrail(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CMeshTrail"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMeshTrail::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
}





