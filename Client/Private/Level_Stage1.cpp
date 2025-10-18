#include "Level_Stage1.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Event_Defines.h"
//#include "Camera_Free.h"
#include "Player.h"
#include "Camera_Compre.h"
#include "Dummy.h"

#pragma region MAP OBJECT
#include "MapObject_Header.h"
#pragma endregion


CLevel_Stage1::CLevel_Stage1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Stage1::Initialize()
{

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;*/

	/*if (FAILED(Ready_Layer_Test(TEXT("Layer_Test"))))
		return E_FAIL;*/

	CHECK_FAILED(Ready_Layer_MapObject_Test(TEXT("Layer_Test")), E_FAIL);

	m_pGameInstance->Jolt_Test();

	return S_OK;
}

void CLevel_Stage1::Update(_float fTimeDelta)
{
	//if (GetKeyState(VK_RETURN) & 0x8000)
	//{
	//	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
	//		return;
	//}

	if (m_pGameInstance->Key_Down(DIK_F1))
	{
		m_pGameInstance->Change_Camera(ENUM_CLASS(LEVEL::STAGE1), ENUM_CLASS(CAMERATYPE::FREE));
	}
	else if (m_pGameInstance->Key_Down(DIK_F2))
	{
		m_pGameInstance->Change_Camera(ENUM_CLASS(LEVEL::STAGE1), ENUM_CLASS(CAMERATYPE::SPRING));
	}

	return;
}

HRESULT CLevel_Stage1::Render()
{
	SetWindowText(g_hWnd, TEXT("스테이지1 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	//(LightDesc.Diffuse * MtrlDesc.Diffuse) * (fShade(0 ~ 1) + (LightDesc.Ambient * MtrlDesc.Ambient))
	LightDesc.eType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(TEXT("Directional_Stage1"), ENUM_CLASS(LEVEL::STAGE1), LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Compre::CAMERA_FREE_DESC	CameraFreeDesc{};

	CameraFreeDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraFreeDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraFreeDesc.fFovy = XMConvertToRadians(60.0f);
	CameraFreeDesc.fNear = 0.1f;
	CameraFreeDesc.fFar = 500.f;
	CameraFreeDesc.fSpeedPerSec = 10.f;
	CameraFreeDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraFreeDesc.fMouseSensor = 0.2f;
	CameraFreeDesc.iCameraType = ENUM_CLASS(CAMERATYPE::FREE);

	CCamera_Compre* pCamera_Free = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Camera_Compre"), &CameraFreeDesc));
	pCamera_Free->Set_IsActive(false);

	m_pGameInstance->Add_Camera(ENUM_CLASS(LEVEL::STAGE1), pCamera_Free);

	m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag, pCamera_Free);

	CCamera_Compre::CAMERA_SPRING_DESC	CameraSpringDesc{};

	CameraSpringDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraSpringDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraSpringDesc.fFovy = XMConvertToRadians(60.0f);
	CameraSpringDesc.fNear = 0.1f;
	CameraSpringDesc.fFar = 500.f;
	CameraSpringDesc.fSpeedPerSec = 10.f;
	CameraSpringDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraSpringDesc.fMouseSensor = 0.2f;
	CameraSpringDesc.iCameraType = ENUM_CLASS(CAMERATYPE::SPRING);

	CGameObject* pPlayer = m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::STAGE1), TEXT("Layer_Player"));
	CameraSpringDesc.pObjMatrix = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr();

	CCamera_Compre* pCamera_Spring = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Camera_Compre"), &CameraSpringDesc));
	pCamera_Spring->Set_IsActive(false);

	m_pGameInstance->Add_Camera(ENUM_CLASS(LEVEL::STAGE1), pCamera_Spring);

	m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag, pCamera_Spring);

	m_pGameInstance->Change_Camera(ENUM_CLASS(LEVEL::STAGE1), ENUM_CLASS(CAMERATYPE::FREE));

	

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag,
		ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Dummy"), TEXT("Dummy"), nullptr, 10);

	for (size_t i = 0; i < 10; i++)
	{
		//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag,
		//	ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Monster"))))
		//	return E_FAIL;

		CDummy* pDummy = dynamic_cast<CDummy*>(m_pGameInstance->Pop_PoolObject(TEXT("Dummy")));
		m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag, pDummy);
	}
	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Test(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag,
		ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_JOH_Test1"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_MapObject_Test(const _wstring& strLayerTag)
{
	CProp_Test::PROP_OBJECT_DESC ObjectDesc = {};

	memcpy(ObjectDesc.szModelName, TEXT("Prototype_Component_Model_WP_WOD_Ground_Base_004"), sizeof(ObjectDesc.szModelName));
	ObjectDesc.eLevel = LEVEL::STAGE1;

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag,
		ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Prop_Test"), &ObjectDesc), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag,
	//	ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag,
		ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	return S_OK;
}


CLevel_Stage1* CLevel_Stage1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage1* pInstance = new CLevel_Stage1(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Stage1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Stage1::Free()
{
	__super::Free();



}
