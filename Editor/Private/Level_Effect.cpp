#include "Level_Effect.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Effect.h"

CLevel_Effect::CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Effect::Initialize()
{
    if (FAILED(Ready_Layer_BackGround()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera()))
        return E_FAIL;

    if (FAILED(Ready_Layer_GameObejct()))
        return E_FAIL;

    m_pGameInstance->AddWidget(TEXT("Effect"), [&]()
        {
            ImGui::Begin("Particle System Editor");

            ImGui::End();
        });

    m_pGameInstance->AddWidget(TEXT("Effect"), [&]()
        {
            ImGui::Begin("EmitterSettings");


            ImGui::End();
        });

	return S_OK;
}

void CLevel_Effect::Update(_float fTimeDelta)
{
    m_PrefabPrototype->Priority_Update(fTimeDelta);
    m_PrefabPrototype->Update(fTimeDelta);
    m_PrefabPrototype->Late_Update(fTimeDelta);

	return;
}

HRESULT CLevel_Effect::Render()
{
	SetWindowText(g_hWnd, TEXT("ÀÌÆåÆ®Åø"));

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_BackGround()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_BackGround"),
		ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Terrain_Grid"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Camera()
{
    CCamera_Effect::CAMERA_EFFECT_DESC Desc{};

    Desc.vEye = _float4(0.f, 5.f, -5.f, 1.f);
    Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    Desc.fFovy = XMConvertToRadians(60.0f);
    Desc.fNear = 0.1f;
    Desc.fFar = 1000.f;
    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);
    Desc.fMouseSensor = 0.1f;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Camera"),
        ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Camera_Effect"), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_GameObejct()
{
    m_PrefabPrototype = CEffect_Prefab::Create(m_pDevice, m_pContext);
    return S_OK;
}

CLevel_Effect* CLevel_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Effect* pInstance = new CLevel_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Effect::Free()
{
	__super::Free();
}
