#include "Level_Effect.h"
#include "GameInstance.h"
#include "Level_Loading.h"

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

    /*m_pGameInstance->AddWidget(TEXT("Effect"), [&]()
        {
            ImGui::Begin("Particle System Editor");

            ImGui::End();
        });

    m_pGameInstance->AddWidget(TEXT("Effect"), [&]()
        {
            ImGui::Begin("EmitterSettings");


            ImGui::End();
        });*/

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

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Camera()
{
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
