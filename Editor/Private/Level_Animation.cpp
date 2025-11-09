#include "Level_Animation.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "Editor_TempCamera.h"
#include "AnimationTool.h"

CLevel_Animation::CLevel_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
	, m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLevel_Animation::Initialize()
{
	//_char currentDir[MAX_PATH];
	//GetCurrentDirectoryA(MAX_PATH, currentDir);
	//OutputDebugStringA(("[Current Working Directory] " + string(currentDir) + "\n").c_str());

	//char exePath[MAX_PATH];
	//GetModuleFileNameA(NULL, exePath, MAX_PATH);
	//std::filesystem::path editorExeDir = std::filesystem::path(exePath).parent_path();

	//OutputDebugStringA(("[Editor.exe Dir] " + editorExeDir.string() + "\n").c_str());

	CHECK_FAILED(Ready_Lights(), E_FAIL);

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Test(TEXT("Layer_Test"))))
	//	return E_FAIL;

	tool = CAnimationTool::Create(m_pDevice, m_pContext);

	return S_OK;
}

void CLevel_Animation::Update(_float fTimeDelta)
{

	return;
}

HRESULT CLevel_Animation::Render()
{
	SetWindowText(g_hWnd, TEXT("애니메이션툴"));

	return S_OK;
}

HRESULT CLevel_Animation::Ready_Lights()
{
	LIGHT_DESC LightDesc = {};

	LightDesc.eType = LIGHT_DESC::DIRECTIONAL;

	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(TEXT("Directional_Animation"), ENUM_CLASS(LEVEL::ANIMATION), LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Animation::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CEditor_TempCamera::TEMPCAMERA_DESC		CameraDesc{};


	CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = 0.2f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::ANIMATION), strLayerTag,
		ENUM_CLASS(LEVEL::ANIMATION), TEXT("Prototype_GameObject_JOH_TempCamera"), TIME_CHANNEL::WORLD, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_Animation::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::ANIMATION), strLayerTag,
	//	ENUM_CLASS(LEVEL::ANIMATION), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Animation::Ready_Layer_Test(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::ANIMATION), strLayerTag,
		ENUM_CLASS(LEVEL::ANIMATION), TEXT("Prototype_GameObject_Editor_Animation_TestModel"))))
		return E_FAIL;

	return S_OK;
}



CLevel_Animation* CLevel_Animation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Animation* pInstance = new CLevel_Animation(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Animation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Animation::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

}
