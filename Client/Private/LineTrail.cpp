#include "LineTrail.h"
#include "GameInstance.h"

CLineTrail::CLineTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CGameObject{pDevice, pDeviceContext}
{
}

CLineTrail::CLineTrail(const CLineTrail& Prototype)
    : CGameObject(Prototype)
    , m_iTextureIdx{ Prototype.m_iTextureIdx }
    , m_fLifeTime{ Prototype.m_fLifeTime }
    , m_iDivisionCount{ Prototype.m_iDivisionCount }
{
}

HRESULT CLineTrail::Initialize_Prototype()
{
    m_iTextureIdx = 0;
    m_fLifeTime = 0.4f;
    m_iDivisionCount = 5;

    return S_OK;
}

HRESULT CLineTrail::Initialize_Clone(void* pArg)
{
    if (FAILED(Ready_Component(pArg)))
        return E_FAIL;

    if (pArg)
    {
        LINE_TRAIL_DESC* dsc = static_cast<LINE_TRAIL_DESC*>(pArg);
    
        m_iTextureIdx = dsc->iTextureIdx;
        m_fLifeTime = dsc->fLifeTime;
        m_iDivisionCount = dsc->iDivisionCount;
        if (m_iDivisionCount < 1)
        {
            MSG_BOX(TEXT("Division Count is too low"));
            return E_FAIL;
        }
    }

    return S_OK;
}

void CLineTrail::Priority_Update(_float fTimeDelta)
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

void CLineTrail::Update(_float fTimeDelta)
{
    //캣멀룸으로 TrailPoints구성
    if (m_ControlPoints.size() < 2)
        return;
    
    m_TrailPoints.clear();

    m_TrailPoints.push_back(m_ControlPoints[0].vPos);

    for (_int i = 0; i < m_ControlPoints.size() - 1; ++i)
    {
        for (_int j = 0; j < m_iDivisionCount; ++j)
        {
            _float weight = (float)(j + 1) / (float)(m_iDivisionCount);

            _vector pos = XMVectorCatmullRom( XMLoadFloat4(&m_ControlPoints[(i - 1) < 0 ? 0 : i - 1].vPos),
                                              XMLoadFloat4(&m_ControlPoints[i].vPos),
                                              XMLoadFloat4(&m_ControlPoints[i + 1].vPos),
                                              XMLoadFloat4(&m_ControlPoints[(i + 2) > m_ControlPoints.size() - 1 ? i + 1 : i + 2].vPos),
                                              weight);
           _float4 NewNode;
           XMStoreFloat4(&NewNode, pos);
           m_TrailPoints.push_back(NewNode);
        }
    }
}

void CLineTrail::Late_Update(_float fTimeDelta)
{
    m_pVIBufferCom->Update(m_TrailPoints, m_pGameInstance->Get_CamPosition());

    if (m_ControlPoints.size() > 1)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
}

HRESULT CLineTrail::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(2);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CLineTrail::Add_ControlPoint(_fvector pos)
{
    if (m_ControlPoints.size() && XMVector4Equal(XMLoadFloat4(&m_ControlPoints.back().vPos), pos))
        return;

    CVIBuffer_LineTrail::LINE_TRAIL_POINT newNode;

    XMStoreFloat4(&newNode.vPos, pos);
    newNode.fLifeTime = 0.f;

    m_ControlPoints.push_back(newNode);
}

HRESULT CLineTrail::Ready_Component(void* pArg)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;
    
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Slash"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;
    
    LINE_TRAIL_DESC dsc{};
    if (pArg == nullptr)
    {
        dsc.fOffset = 0.5f;
        pArg = &dsc;
    }

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_LineTrail"),
        TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLineTrail::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
        return E_FAIL;


    return S_OK;
}

CLineTrail* CLineTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLineTrail* pInstance = new CLineTrail(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLineTrail"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLineTrail::Clone(void* pArg)
{
    CLineTrail* pInstance = new CLineTrail(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CLineTrail"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLineTrail::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
}
