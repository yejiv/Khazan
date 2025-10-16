#include "Level_Shader.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CLevel_Shader::CLevel_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Shader::Initialize()
{
	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Shader::Update(_float fTimeDelta)
{
	return;
}

HRESULT CLevel_Shader::Render()
{
	SetWindowText(g_hWnd, TEXT("쉐이더툴"));

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), strLayerTag,
	//	ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}


CLevel_Shader* CLevel_Shader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Shader* pInstance = new CLevel_Shader(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Shader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Shader::Free()
{
	__super::Free();



}
