#include "EditorPch.h"
#include "Level_Effect.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CLevel_Effect::CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Effect::Initialize()
{

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;


	return S_OK;
}

void CLevel_Effect::Update(_float fTimeDelta)
{

	return;
}

HRESULT CLevel_Effect::Render()
{
	SetWindowText(g_hWnd, TEXT("이펙트툴"));

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), strLayerTag,
	//	ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

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
