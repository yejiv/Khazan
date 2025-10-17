#include "Level_Shader.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Shader.h"

CLevel_Shader::CLevel_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Shader::Initialize()
{
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Shader::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif

	return;
}

HRESULT CLevel_Shader::Render()
{
	SetWindowText(g_hWnd, TEXT("½¦ÀÌ´õÅø"));

#ifdef _DEBUG
	++m_iRenderCount;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS:%d"), m_iRenderCount);
		m_fTimeAcc = 0.f;
		m_iRenderCount = 0;
	}
	m_pGameInstance->DrawText(TEXT("Font_153"), m_szFPS, _float2(100.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));
#endif

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Lights()
{
	LIGHT_DESC LightDesc = {};
	LightDesc.eType = LIGHT_DESC::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	if (FAILED(m_pGameInstance->Add_Light(TEXT("Directional_Shader"), ENUM_CLASS(LEVEL::SHADER), LightDesc)))
		return E_FAIL;

	// Point_Green
	LightDesc.eType = LIGHT_DESC::POINT;
	LightDesc.vPosition = _float4(20.f, 2.f, 10.f, 1.f);
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.6f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	if (FAILED(m_pGameInstance->Add_Light(TEXT("Point_Green"), ENUM_CLASS(LEVEL::SHADER), LightDesc)))
		return E_FAIL;

	// Point_Red
	LightDesc.eType = LIGHT_DESC::POINT;
	LightDesc.vPosition = _float4(10.f, 2.f, 10.f, 1.f);
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	if (FAILED(m_pGameInstance->Add_Light(TEXT("Point_Red"), ENUM_CLASS(LEVEL::SHADER), LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Layer_Camera()
{
	CCamera_Shader::CAMERA_EFFECT_DESC Desc{};

	Desc.vEye = _float4(0.f, 20.f, -20.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fMouseSensor = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_Camera"),
		ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Camera_Shader"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Layer_BackGround()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_BackGround"),
		ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Terrain_Shader"))))
		return E_FAIL;

	return S_OK;
}

CLevel_Shader* CLevel_Shader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Shader* pInstance = new CLevel_Shader(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Shader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Shader::Free()
{
	__super::Free();



}
