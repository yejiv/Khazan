#include "UI_Gacha_Selete.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Gacha_Selete::CUI_Gacha_Selete(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Texture{ pDevice, pContext }
{
}

CUI_Gacha_Selete::CUI_Gacha_Selete(const CUI_Gacha_Selete& Prototype)
    : CUI_Texture(Prototype)
{
}

HRESULT CUI_Gacha_Selete::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Gacha_Selete::Initialize_Clone(void* pArg)
{
    m_iShaderPass = 1;

    m_vColor = { 1.f,1.f,1.f,1.f };
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_vDefaultPos = m_vLocalPos;
    return S_OK;
}

void CUI_Gacha_Selete::Late_Update(_float fTimeDelta, _vector vPos)
{
    static _bool itemEffect = false;

    if (m_pGameInstance->Key_Pressing(DIK_TAB, fTimeDelta, INPUT_TYPE::FORCE))
    {
        if (m_pGameInstance->Key_Down(DIK_Q, INPUT_TYPE::FORCE))
        {
            if (itemEffect == false)
            {
                /*[1] 아이템 켜기 */
                m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Item_normal_Gacha"), vPos);
                itemEffect = true;
            }
            else
            {
                /*[2] 아이템 끄기 */
                m_pGameInstance->Stop_Effect_Force(ENUM_CLASS(LEVEL::EMBARS), TEXT("Item_normal_Gacha"));
                itemEffect = false;
            }
        }

        if (m_pGameInstance->Key_Down(DIK_W, INPUT_TYPE::FORCE))
        {
            /*[2] 아이템 터지기 */
            m_pGameInstance->Stop_Effect_Force(ENUM_CLASS(LEVEL::EMBARS), TEXT("Item_normal_Gacha"));
            itemEffect - false;
            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("item_blust"), vPos);
        }

        if (m_pGameInstance->Key_Down(DIK_E, INPUT_TYPE::FORCE))
        {
            /*[3] 당첨된 아이템 끄기 -> 이거 나중에 내가 시간 조절해서 알아서 꺼지게 만들 수 있음! 그럼 굳이 호출 안 해도 되는데 의견 부탁! */
            m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("item_blust"));
            itemEffect - false;
        }
    }
    
    Update_WorldPos(vPos);
    
    //계속 호출해도 상관 없지만 꺼져있을 땐 안 해도 좋음
    m_pGameInstance->Update_Effect_Position(ENUM_CLASS(LEVEL::EMBARS), TEXT("Item_normal_Gacha"), 0, vPos);
    
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_Gacha_Selete::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(m_iShaderPass);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CUI_Gacha_Selete::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Guide_Press"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

void CUI_Gacha_Selete::Update_WorldPos(_vector vPos)
{
    _matrix OldVeiw = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
    _matrix OldProj = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);

    _matrix vVPMatrix = OldVeiw * OldProj;
    _vector vWinPos = XMVector3TransformCoord(vPos, vVPMatrix);

    _float fWinPosX = (XMVectorGetX(vWinPos) + 1.f) * 0.5f * g_iWinSizeX;
    _float fWinPosY = (1.f - XMVectorGetY(vWinPos)) * 0.5f * g_iWinSizeY;

    m_vWorldPos.x = fWinPosX + m_vLocalPos.x;
    m_vWorldPos.y = fWinPosY + m_vLocalPos.y;

    if (m_vWorldPos.x < 0)
        m_vWorldPos.x = 0;
    else if (m_vWorldPos.x > g_iWinSizeX)
        m_vWorldPos.x = g_iWinSizeX;

    if (m_vWorldPos.y < 0)
        m_vWorldPos.y = 0;
    else if (m_vWorldPos.y > g_iWinSizeY)
        m_vWorldPos.y = g_iWinSizeY;


    Update_Transform(nullptr, m_vWorldPos);
}

CUI_Gacha_Selete* CUI_Gacha_Selete::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Gacha_Selete* pInstance = new CUI_Gacha_Selete(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Gacha_Selete"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Gacha_Selete::Clone(void* pArg)
{
    CUI_Gacha_Selete* pInstance = new CUI_Gacha_Selete(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Gacha_Selete"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Gacha_Selete::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
