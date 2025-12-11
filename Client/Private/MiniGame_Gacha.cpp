#include "MiniGame_Gacha.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "RandomBox.h"
#include "UI_Gacha_Selete.h"
#include "UI_Inven.h"

#include "Minigame_Mirrorball.h"

void CMiniGame_Gacha::Start_MiniGame(MINIGAME_LEVEL eLevel)
{
    m_eMiniGameLevel = eLevel;
    if (eLevel == MINIGAME_LEVEL::EASY)
    {
        m_iSuffleCount = m_pGameInstance->Rand(10, 15);
        m_fSpeed = 2.f;
        m_fSpeedCount = 3.f;
        m_fAddSpeed = 0.5f;
        CClientInstance::GetInstance()->Embars_Club_Shuffle_0();
    }
    else if (eLevel == MINIGAME_LEVEL::NORMAL)
    {
        m_iSuffleCount = m_pGameInstance->Rand(15, 20);
        m_fSpeed = 2.f;
        m_fSpeedCount = 3.f;
        m_fAddSpeed = 2.f;
        CClientInstance::GetInstance()->Embars_Club_Shuffle_1();
    }
    else if (eLevel == MINIGAME_LEVEL::HARD)
    {
        m_iSuffleCount = m_pGameInstance->Rand(30, 45);
        m_fSpeed = 4.f;
        m_fSpeedCount = 2.f;
        m_fAddSpeed = 1.5f;
        m_isHardCount++;
        CClientInstance::GetInstance()->Embars_Club_Shuffle_2();
    }
    m_eState = SUCCES_NOTICE;
    m_iSpeedCount = 0;
    m_iSeleteNum = m_pGameInstance->Rand(0, 2);
    m_iSuccesNum = m_pBox[m_iSeleteNum]->Get_Index();
    m_fAcctime = 0.f;
    m_fGuidePosY = 1.5f;
    m_fGuideCount = 3;
    m_pGameInstance->Set_LightEnable(TEXT("GachaSelect"), ENUM_CLASS(LEVEL::EMBARS), true);
    _float4 vLightPos{};
    XMStoreFloat4(&vLightPos, m_pBox[m_iSeleteNum]->Get_Position());
    m_pGameInstance->Set_LightPosition(TEXT("GachaSelect"), ENUM_CLASS(LEVEL::EMBARS), vLightPos);

    //이펙트 스폰
    _vector vPos = m_pBox[m_iSeleteNum]->Get_Position();
    vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + m_fGuidePosY);
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Item_normal_Gacha"), vPos);

    for(auto pBox : m_pBox)
        pBox->Get_Model()->Set_Animation(1);
    // 미니게임 위치로 화면 전환
    CAMERA_POSE Pose = CClientInstance::GetInstance()->Camera_MakePose(_float3(-62.071f, -89.988f, -41.670f), _float3(-0.934f, -0.358f, 0.008f));
    CClientInstance::GetInstance()->Camera_SubShot(Pose, 0.3f, 0.3f);
    m_pMirrorball->End_Mirrorball();
}

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
    if (m_eState == SELETE_NUM)
        Update_Selete(fTimeDelta);
    else if (m_eState == SUCCES_NOTICE)
        Update_Notice(fTimeDelta);
    else if (m_eState == SHUFFLE_SET)
        Setting_Suffle();
    else if (m_eState == SHUFFLE)
        Update_Suffle(fTimeDelta);
    else if (m_eState == SELECT_END0)
        Update_Selete_End0(fTimeDelta);
    else if (m_eState == SELECT_END1)
        Update_Selete_End1(fTimeDelta);
    else if (m_eState == SELECT_END2)
        Update_Selete_End2(fTimeDelta);        
    else
        for (auto pBox : m_pBox)
            pBox->Get_Model()->Set_Animation(1);

    for (auto pBox : m_pBox)
        pBox->Update(fTimeDelta);

    m_pMirrorball->Update(fTimeDelta);
}

void CMiniGame_Gacha::Late_Update(_float fTimeDelta)
{
    for (auto pBox : m_pBox)
        pBox->Late_Update(fTimeDelta);

    if (m_eState == SELETE_NUM)
    {
        _vector vPos = m_pBox[m_iSeleteNum]->Get_Position();
        m_pSeleteUI->Late_Update(fTimeDelta, XMVectorSetY(vPos, XMVectorGetY(vPos) + m_fGuidePosY));
    }
    else if (m_eState == SUCCES_NOTICE || m_eState == SELECT_END0 || m_eState == SELECT_END1 || m_eState == SELECT_END2)
    {
        _vector vPos = m_pBox[m_iSeleteNum]->Get_Position();
        m_pGameInstance->Update_Effect_Position(ENUM_CLASS(LEVEL::EMBARS), TEXT("Item_normal_Gacha"), 0, XMVectorSetY(vPos, XMVectorGetY(vPos) + m_fGuidePosY));
    }
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

    m_pBox[0]->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-67.325f, -92.26f, -44.777f, 1.f));
    m_pBox[1]->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-67.325f, -92.26f, -41.831f, 1.f));
    m_pBox[2]->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-67.325f, -92.26f, -38.842, 1.f));

    m_pGameInstance->Set_LightPosition(TEXT("GachaSelect1"), ENUM_CLASS(LEVEL::EMBARS), { -67.325f, -92.26f, -44.777f, 1.f });
    m_pGameInstance->Set_LightPosition(TEXT("GachaSelect2"), ENUM_CLASS(LEVEL::EMBARS), { -67.325f, -92.26f, -41.831f, 1.f });
    m_pGameInstance->Set_LightPosition(TEXT("GachaSelect3"), ENUM_CLASS(LEVEL::EMBARS), { -67.325f, -92.26f, -38.842, 1.f});

    m_pBox[0]->Get_Transform()->Rotation(0.f, XMConvertToRadians(90.f), 0.f);
    m_pBox[1]->Get_Transform()->Rotation(0.f, XMConvertToRadians(90.f), 0.f);
    m_pBox[2]->Get_Transform()->Rotation(0.f, XMConvertToRadians(90.f), 0.f);

    m_pMirrorball = CMinigame_Mirrorball::Create();
    return S_OK;
}

void CMiniGame_Gacha::Setting_Suffle()
{
    --m_iSuffleCount;
    ++m_iSpeedCount;
    if (m_iSpeedCount >= m_fSpeedCount)
    {
        m_fSpeed += m_fAddSpeed;
        m_iSpeedCount = 0.f;
    }

    if (m_iSuffleCount <= 0)
    {
        m_pGameInstance->Set_LightEnable(TEXT("GachaSelect"), ENUM_CLASS(LEVEL::EMBARS), true);
        m_eState = SELETE_NUM;
        m_iSeleteNum = 0;
        m_fGuidePosY = 1.5f;
        CClientInstance::GetInstance()->BGM_Embars_B1();
        _float4 vLightPos{};
        XMStoreFloat4(&vLightPos, m_pBox[m_iSeleteNum]->Get_Position());
        m_pGameInstance->Set_LightPosition(TEXT("GachaSelect"), ENUM_CLASS(LEVEL::EMBARS), vLightPos);

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
        m_pGameInstance->PlaySoundOnce(TEXT("MiniGame_Shuffle.mp3"));

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
                m_vSuffleVector2.push_back(XMVectorSetX((vposA + vposB) * 0.5f, XMVectorGetX(vposA) + 2.f));
                m_vSuffleVector2.push_back(vposA);
                break;
            default:
                m_vSuffleVector2.push_back(vposB);
                m_vSuffleVector2.push_back(XMVectorSetX((vposA + vposB) * 0.5f, XMVectorGetX(vposA) - 2.f));
                m_vSuffleVector2.push_back(vposA);
                break;
            }
        }
        else
        {
            if (m_pGameInstance->Rand(0, 1) == 0)
            {
                m_vSuffleVector2.push_back(vposB);
                m_vSuffleVector2.push_back(XMVectorSetX((vposA + vposB) * 0.5f, XMVectorGetX(vposA) + 2.f));
                m_vSuffleVector2.push_back(vposA);
            }
            else
            {
                m_vSuffleVector2.push_back(vposB);
                m_vSuffleVector2.push_back(XMVectorSetX((vposA + vposB) * 0.5f, XMVectorGetX(vposA) - 2.f));
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
    m_fGuidePosY -= fTimeDelta * 0.5f;

    if (m_fGuidePosY <= 0.6f)
    {
        m_eState = SHUFFLE_SET;
        m_pGameInstance->Set_LightEnable(TEXT("GachaSelect"), ENUM_CLASS(LEVEL::EMBARS), false);
        m_pGameInstance->Stop_Effect_Force(ENUM_CLASS(LEVEL::EMBARS), TEXT("Item_normal_Gacha"));
    }
}

void CMiniGame_Gacha::Update_Selete_End0(_float fTimeDelta)
{
    if (m_eEndAnime == ANIM_STATE::START)
    {
        if (m_pBox[m_iSeleteNum]->isAnimFinish())
        {
            _vector vPos = m_pBox[m_iSeleteNum]->Get_Position();
            //vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + m_fGuidePosY);
            if (m_isSucces)
            {
                m_fEffectTime = -5.f;
                m_pBox[m_iSeleteNum]->Get_Model()->Set_Animation(13);
                
                m_pGameInstance->PlaySoundOnce(TEXT("Minigame_MP_Ta Da.mp3"));

                _vector vPos = m_pBox[m_iSeleteNum]->Get_Position();
                vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + m_fGuidePosY);
                m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Item_normal_Gacha"), vPos);

                m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Gacha_Suceess"), vPos);
                m_eEndAnime = ANIM_STATE::END;

                if (m_eMiniGameLevel == MINIGAME_LEVEL::HARD)
                {
                    CClientInstance::GetInstance()->BGM_Embars_Club_Game();
                    m_pMirrorball->Start_Mirrorball();
                }
            }
            else
            {
                m_pBox[m_iSeleteNum]->Get_Model()->Set_Animation(5);
                m_pGameInstance->PlaySoundOnce(TEXT("MiniGame_Fail.mp3"));
                m_eEndAnime = ANIM_STATE::END;
                m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Gacha_Fail"), vPos); 
            }
        }
    }
    else  if (m_eEndAnime == ANIM_STATE::DANCE)
    {
        m_pBox[m_iSeleteNum]->Get_Transform()->LookAt_Lerp(XMLoadFloat4(m_pGameInstance->Get_CamPosition()), fTimeDelta, 1.5f);
        if (*m_pBox[m_iSeleteNum]->Get_Model()->Get_CurTrackPosition() >= 260.f)
        {
            m_eState = SELECT_END1;
            CClientInstance::GetInstance()->Camera_Release_FOVHoldZoom(L"DanginZoom", 0.5f);
            m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("item_blust"));
        }
    }
    else
    {
        if (m_isSucces)
        {
            if(m_fGuidePosY <= 1.2f)
                m_fGuidePosY += fTimeDelta * 0.5f;

            if (m_fGuidePosY > 1.2f && m_fEffectTime <= -4.f)
            {
                m_fEffectTime = 1.f;

                _vector vPos = m_pBox[m_iSeleteNum]->Get_Position();
                vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + m_fGuidePosY);
               m_pGameInstance->Stop_Effect_Force(ENUM_CLASS(LEVEL::EMBARS), TEXT("Item_normal_Gacha"));
               m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("item_blust"), vPos);
            }
            if (m_fEffectTime >= 0.f)
            {
                m_fEffectTime -= fTimeDelta;

                if (m_fEffectTime <= 0.f)
                {
                    m_fEffectTime = 0.f;
                    if (m_eMiniGameLevel == MINIGAME_LEVEL::HARD)
                    {
                        m_eEndAnime = ANIM_STATE::DANCE;
                    }
                    else
                    {
                        m_eState = SELECT_END1;
                        CClientInstance::GetInstance()->Camera_Release_FOVHoldZoom(L"DanginZoom", 0.5f);
                        m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("item_blust"));
                    }
              
                }

            }
            
        }
        else
        {
            m_fEffectTime = 0.f;
            m_eState = SELECT_END1;
            CClientInstance::GetInstance()->Camera_Release_FOVHoldZoom(L"DanginZoom", 0.5f);
        }
    }
}

void CMiniGame_Gacha::Update_Selete_End1(_float fTimeDelta)
{
    m_fEffectTime += fTimeDelta;
    if (m_fEffectTime > 1.f && m_isSucces)
    {
        m_eState = SELECT_END2;
        CClientInstance::GetInstance()->Camera_ReturnToPreviousPose(0.5f);
    }
    else if (m_fEffectTime > 1.5f && m_isSucces)
    {
        m_eState = SELECT_END2;
        CClientInstance::GetInstance()->Camera_ReturnToPreviousPose(0.5f);
    }
    else if (m_fEffectTime > 1.f && !m_isSucces)
    {
        m_eState = SELECT_END2;
        CClientInstance::GetInstance()->Camera_ReturnToPreviousPose(0.5f);
    }

}

void CMiniGame_Gacha::Update_Selete_End2(_float fTimeDelta)
{
    if (m_isSucces)
    {
        if (m_eMiniGameLevel == MINIGAME_LEVEL::EASY)
        {
            static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(6001);
            static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(6011);
        }
        else if (m_eMiniGameLevel == MINIGAME_LEVEL::NORMAL)
        {
            static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(5011);
        }
        else if (!m_isGet_Pet && m_eMiniGameLevel == MINIGAME_LEVEL::HARD)
        {
            m_isGet_Pet = true;
            m_pGameInstance->Emit_Event<EVENT_PET_STATE>(ENUM_CLASS(EVENT_TYPE::PET), EVENT_PET_STATE{ true });
        }
    }
    m_eState = END;

    m_pGameInstance->Set_LightEnable(TEXT("GachaSelect"), ENUM_CLASS(LEVEL::EMBARS), false);
    m_pBox[0]->Get_Transform()->Rotation(0.f, XMConvertToRadians(90.f), 0.f);
    m_pBox[1]->Get_Transform()->Rotation(0.f, XMConvertToRadians(90.f), 0.f);
    m_pBox[2]->Get_Transform()->Rotation(0.f, XMConvertToRadians(90.f), 0.f);
    for (auto pBox : m_pBox)
        pBox->Get_Model()->Set_Animation(1);
}

void CMiniGame_Gacha::Update_Selete(_float fTimeDelta)
{
    if (m_eMiniGameLevel == MINIGAME_LEVEL::HARD && m_isHardCount >= 2 && m_pBox[m_iSeleteNum]->Get_Index() == m_iSuccesNum)
    {
        m_fLightTime += fTimeDelta;
        if (m_fLightTime >= 0.3f)
        {
            m_fLightTime = 0.f;
            m_pGameInstance->Set_LightEnable(TEXT("GachaSelect"), ENUM_CLASS(LEVEL::EMBARS), m_isHardLight);
            m_isHardLight ? m_isHardLight = false : m_isHardLight = true;
        }
    }
    else
    {
        m_pGameInstance->Set_LightEnable(TEXT("GachaSelect"), ENUM_CLASS(LEVEL::EMBARS), true);
    }

    if (m_pGameInstance->Key_Down(DIK_A, INPUT_TYPE::WORLD_UI))
    {
        --m_iSeleteNum;
        m_iSeleteNum < 0 ? m_iSeleteNum = 2 : m_iSeleteNum;

        _float4 vLightPos{};
        XMStoreFloat4(&vLightPos, m_pBox[m_iSeleteNum]->Get_Position());
        m_pGameInstance->Set_LightPosition(TEXT("GachaSelect"), ENUM_CLASS(LEVEL::EMBARS), vLightPos);

        _wstring wstrSound = TEXT("UI_common_mouse_over_0") + to_wstring(m_pGameInstance->Rand(1, 4)) + TEXT(" (SFX).wav");
        m_pGameInstance->PlaySoundOnce(wstrSound.c_str());
    }
    else if (m_pGameInstance->Key_Down(DIK_D, INPUT_TYPE::WORLD_UI))
    {
        ++m_iSeleteNum;
        m_iSeleteNum > 2 ? m_iSeleteNum = 0 : m_iSeleteNum;

        _float4 vLightPos{};
        XMStoreFloat4(&vLightPos, m_pBox[m_iSeleteNum]->Get_Position());
        m_pGameInstance->Set_LightPosition(TEXT("GachaSelect"), ENUM_CLASS(LEVEL::EMBARS), vLightPos);

        _wstring wstrSound = TEXT("UI_common_mouse_over_0") + to_wstring(m_pGameInstance->Rand(1, 4)) + TEXT(" (SFX).wav");
        m_pGameInstance->PlaySoundOnce(wstrSound.c_str());
    }
    else if (m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::WORLD_UI))
    {
        m_eState = SELECT_END0;
        if (m_iSuccesNum == m_pBox[m_iSeleteNum]->Get_Index())
            m_isSucces = true;
        else
            m_isSucces = false;

        CAMERA_POSE tCameraPose = {};
        if(m_iSeleteNum == 0)
            tCameraPose = CClientInstance::GetInstance()->Camera_MakePose(_float3(-62.444f, -90.443f, -41.798f), _float3(-0.830f, -0.241f, -0.503f)); // 왼쪽 바라보기
        else if (m_iSeleteNum == 1)
            tCameraPose = CClientInstance::GetInstance()->Camera_MakePose(_float3(-62.444f, -90.443f, -41.798f), _float3(-0.952f, -0.307f, -0.008f)); // 가운데 바라보기
        else
            tCameraPose = CClientInstance::GetInstance()->Camera_MakePose(_float3(-62.444f, -90.443f, -41.798f), _float3(-0.827f, -0.224f, 0.515f)); // 오른쪽 바라보기
 
        CClientInstance::GetInstance()->Camera_SubShot(tCameraPose, 0.2f, 0.2f);
        _float zoomFov = XMConvertToRadians(20.f);

        if(m_iSeleteNum == 1)
            zoomFov = XMConvertToRadians(28.f);
        CClientInstance::GetInstance()->Camera_Start_FOVHoldZoom(L"DanginZoom", zoomFov, 1.5f, 0);

        if(m_isSucces)
            m_pBox[m_iSeleteNum]->Get_Model()->Set_Animation(12);
        else
            m_pBox[m_iSeleteNum]->Get_Model()->Set_Animation(4);
        m_eEndAnime = ANIM_STATE::START;
        if (m_isSucces)
        {
            if (m_eMiniGameLevel == MINIGAME_LEVEL::HARD)
                CClientInstance::GetInstance()->BGM_Embars_Club_Game();

            m_fGuidePosY = 0.5f;
        }
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
    Safe_Release(m_pMirrorball);
    for (auto pbox : m_pBox)
        Safe_Release(pbox);
    m_pBox.clear();
}
