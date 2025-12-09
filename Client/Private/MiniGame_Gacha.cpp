#include "MiniGame_Gacha.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "RandomBox.h"
#include "UI_Gacha_Selete.h"


CMiniGame_Gacha::CMiniGame_Gacha(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMiniGame_Gacha::CMiniGame_Gacha(const CMiniGame_Gacha& Prototype)
    : CGameObject( Prototype )
{
}

HRESULT CMiniGame_Gacha::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;
    CHECK_FAILED(Ready_Prototype(), E_FAIL);
    return S_OK;
}

HRESULT CMiniGame_Gacha::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Setting_Object(), E_FAIL);

    return S_OK;
}

void CMiniGame_Gacha::Priority_Update(_float fTimeDelta)
{   
    for (auto pBox : m_pBox)
        pBox->Priority_Update(fTimeDelta);
}

void CMiniGame_Gacha::Update(_float fTimeDelta)
{
    if (m_eState == END)
        Input_Key();
    else if (m_eState == SUCCES_NOTICE)
        Update_Notice(fTimeDelta);
    else if (m_eState == SHUFFLE_SET)
        Setting_Suffle();
    else if (m_eState == SHUFFLE)
        Update_Suffle(fTimeDelta);

    for (auto pBox : m_pBox)
        pBox->Update(fTimeDelta);
}

void CMiniGame_Gacha::Late_Update(_float fTimeDelta)
{
    for (auto pBox : m_pBox)
        pBox->Late_Update(fTimeDelta);

    _vector vPos = m_pBox[m_iSeleteNum]->Get_Position();
    if (m_eState == END || m_eState == SUCCES_NOTICE)
        m_pSeleteUI->Late_Update(fTimeDelta, XMVectorSetY(vPos, XMVectorGetY(vPos) + m_fGuidePosY));
}

HRESULT CMiniGame_Gacha::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_MiniGame_RandomBox"),
        CRandomBox::Create(m_pDevice, m_pContext, m_iPrototypeIndex)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_MiniGame_Gacha_Selete"),
        CUI_Gacha_Selete::Create(m_pDevice, m_pContext)), E_FAIL);

    return S_OK;
}

HRESULT CMiniGame_Gacha::Setting_Object()
{
    CUIObject::UIOBJECT_DESC IconDesc = {};
    IconDesc.fDepth = 1.f;
    IconDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    IconDesc.szName = "Icon";
    IconDesc.vLocalPos = _float2{ 0.f, 0.f };
    IconDesc.vLocalSize = { 32.f, 32.f };
    IconDesc.vColor = { 1.f, 1.f, 1.f, 1.f };

    m_pSeleteUI = static_cast<CUI_Gacha_Selete*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iPrototypeIndex, TEXT("Prototype_PartObject_MiniGame_Gacha_Selete"), &IconDesc));
    CHECK_NULLPTR(m_pSeleteUI, E_FAIL);
    
    CGameObject::GAMEOBJECT_DESC Desc{};
    Desc.fRotationPerSec = 1.f;
    Desc.fSpeedPerSec = 1.f;

    for (_int i = 0; i < 3; ++i)
    {
        CRandomBox* pRandomBox = static_cast<CRandomBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iPrototypeIndex, TEXT("Prototype_PartObject_MiniGame_RandomBox"), &Desc));
        CHECK_NULLPTR(pRandomBox, E_FAIL);
        m_pBox.push_back(pRandomBox);
        pRandomBox->Set_Index(i);
    }

    m_pBox[0]->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-1.f, 0.2f, 5.f, 1.f));
    m_pBox[1]->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.2f, 5.f, 1.f));
    m_pBox[2]->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(1.f, 0.2f, 5.f, 1.f));

    return S_OK;
}

void CMiniGame_Gacha::Setting_Suffle()
{
    --m_iSuffleCount;
    ++m_iSpeedCount;
    if (m_iSpeedCount >= 5)
    {
        m_fSpeed += 5.f;
        m_iSpeedCount = 0.f;
    }

    if (m_iSuffleCount <= 0)
    {
        m_eState = END;
        m_iSeleteNum = 0;
        m_fGuidePosY = 1.5f;
    }
    else
    {
        m_vSuffleVector1.clear();
        m_vSuffleVector2.clear();
        m_iCulSuffleIndex = 0;
        m_fAcctime = 0.f;
        m_iSuffleIndex1 = m_pGameInstance->Rand(0, 2);
        m_iSuffleIndex2 = m_pGameInstance->Rand(0, 2);

        while (m_iSuffleIndex1 == m_iSuffleIndex2)
        {
            m_iSuffleIndex2 = m_pGameInstance->Rand(0, 2);
        }

        m_eState = SHUFFLE;

        _vector vposA = m_pBox[m_iSuffleIndex1]->Get_Position();
        _vector vposB = m_pBox[m_iSuffleIndex2]->Get_Position();

        m_vSuffleVector1.push_back(vposA);
        m_vSuffleVector1.push_back(XMVectorSetY((vposA + vposB) * 0.5f, XMVectorGetY(vposA) + 1.f));
        m_vSuffleVector1.push_back(vposB);

        if (abs(m_iSuffleIndex1 - m_iSuffleIndex2) == 1)
        {
            switch (m_pGameInstance->Rand(0, 2))
            {
            case 0:
                m_vSuffleVector2.push_back(vposB);
                m_vSuffleVector2.push_back((vposA + vposB) * 0.5f);
                m_vSuffleVector2.push_back(vposA);
                break;
            case 1:
                m_vSuffleVector2.push_back(vposB);
                m_vSuffleVector2.push_back(XMVectorSetZ((vposA + vposB) * 0.5f, XMVectorGetZ(vposA) + 1.f));
                m_vSuffleVector2.push_back(vposA);
                break;
            default:
                m_vSuffleVector2.push_back(vposB);
                m_vSuffleVector2.push_back(XMVectorSetZ((vposA + vposB) * 0.5f, XMVectorGetZ(vposA) - 1.f));
                m_vSuffleVector2.push_back(vposA);
                break;
            }
        }
        else
        {
            if (m_pGameInstance->Rand(0, 1) == 0)
            {
                m_vSuffleVector2.push_back(vposB);
                m_vSuffleVector2.push_back(XMVectorSetZ((vposA + vposB) * 0.5f, XMVectorGetZ(vposA) + 1.f));
                m_vSuffleVector2.push_back(vposA);
            }
            else
            {
                m_vSuffleVector2.push_back(vposB);
                m_vSuffleVector2.push_back(XMVectorSetZ((vposA + vposB) * 0.5f, XMVectorGetZ(vposA) - 1.f));
                m_vSuffleVector2.push_back(vposA);
            }
        }
    }
}

void CMiniGame_Gacha::Update_Suffle(_float fTimeDelta)
{
    _bool isSwap = false;
    m_fAcctime += fTimeDelta * m_fSpeed;
    if (m_fAcctime >= 1.f)
    {
        m_fAcctime = 0.f;
        ++m_iCulSuffleIndex;
        if (m_iCulSuffleIndex >= 2)
        {
            m_iCulSuffleIndex = 1;
            m_fAcctime = 1.f;
            m_eState = SHUFFLE_SET;
            isSwap = true;
        }
    }
    _float fShuffleTime = {};
    if (m_iCulSuffleIndex == 0)
        fShuffleTime = m_fAcctime * m_fAcctime;
    else
        fShuffleTime = 1.f - (1.f - m_fAcctime) * (1.f - m_fAcctime);

    _vector vRandomBox1Pos = XMVectorCatmullRom(m_vSuffleVector1[0], m_vSuffleVector1[m_iCulSuffleIndex], m_vSuffleVector1[m_iCulSuffleIndex + 1], m_vSuffleVector1[2], fShuffleTime);
    _vector vRandomBox2Pos = XMVectorCatmullRom(m_vSuffleVector2[0], m_vSuffleVector2[m_iCulSuffleIndex], m_vSuffleVector2[m_iCulSuffleIndex + 1], m_vSuffleVector2[2], fShuffleTime);

    m_pBox[m_iSuffleIndex1]->Get_Transform()->Set_State(STATE::POSITION, XMVectorSetW(vRandomBox1Pos, 1.f));
    m_pBox[m_iSuffleIndex2]->Get_Transform()->Set_State(STATE::POSITION, XMVectorSetW(vRandomBox2Pos, 1.f));

    if (isSwap)
    {
        CRandomBox* pTemp = m_pBox[m_iSuffleIndex1];
        m_pBox[m_iSuffleIndex1] = m_pBox[m_iSuffleIndex2];
        m_pBox[m_iSuffleIndex2] = pTemp;
    }
}

void CMiniGame_Gacha::Update_Notice(_float fTimeDelta)
{
    if (m_fGuideCount <= 0)
        m_eState = SHUFFLE_SET;

    m_fAcctime += fTimeDelta;
    
    if (m_fAcctime >= 1.f)
    {
        m_fAcctime = 0.f;
        --m_fGuideCount;
    }

    if (m_fAcctime <= 0.5f)
        m_fGuidePosY -= fTimeDelta * 0.5f;
    else
        m_fGuidePosY += fTimeDelta * 0.5f;


}

void CMiniGame_Gacha::Input_Key()
{
    //m_pGameInstance->Change_InputType(INPUT_TYPE::UI);
    if (m_pGameInstance->Key_Down(DIK_BACKSPACE))
    {
        m_iSuffleCount = m_pGameInstance->Rand(30, 45);
        m_eState = SUCCES_NOTICE;
        m_iSpeedCount = 0;
        m_fSpeed = 5.f;
        m_iSeleteNum = m_pGameInstance->Rand(0, 2);
        m_iSuccesNum = m_pBox[m_iSeleteNum]->Get_Index();
        m_fAcctime = 0.f;
        m_fGuidePosY = 1.5f;
        m_fGuideCount = 3;
    }
    if (m_pGameInstance->Key_Down(DIK_A))
    {
        --m_iSeleteNum;
        m_iSeleteNum < 0 ? m_iSeleteNum = 2 : m_iSeleteNum;
    }
    else if (m_pGameInstance->Key_Down(DIK_D))
    {
        ++m_iSeleteNum;
        m_iSeleteNum > 2 ? m_iSeleteNum = 0 : m_iSeleteNum;
    }
    else if (m_pGameInstance->Key_Down(DIK_F) && m_iSuccesNum == m_pBox[m_iSeleteNum]->Get_Index())
    {
        m_iSuffleCount = m_pGameInstance->Rand(30, 45);
        m_eState = SUCCES_NOTICE;
        m_iSpeedCount = 0;
        m_fSpeed = 5.f;
        m_iSeleteNum = m_pGameInstance->Rand(0, 2);
        m_iSuccesNum = m_pBox[m_iSeleteNum]->Get_Index();
        m_fAcctime = 0.f;
        m_fGuidePosY = 1.5f;
        m_fGuideCount = 3;
    }
}

CMiniGame_Gacha* CMiniGame_Gacha::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CMiniGame_Gacha* pInstance = new CMiniGame_Gacha(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed to Created : CMiniGame_Gacha"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMiniGame_Gacha::Clone(void* pArg)
{
    CMiniGame_Gacha* pInstance = new CMiniGame_Gacha(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CMiniGame_Gacha"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMiniGame_Gacha::Free()
{
    __super::Free();
    Safe_Release(m_pSeleteUI);
    for (auto pbox : m_pBox)
        Safe_Release(pbox);
    m_pBox.clear();
}
