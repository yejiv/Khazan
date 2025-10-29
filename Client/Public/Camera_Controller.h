#pragma once

#include "Base.h"
#include "Camera_Compre.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CCamera_Controller final : public CBase
{
private:
	CCamera_Controller();
	virtual ~CCamera_Controller() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	HRESULT Ready_Camera(const _wstring& strLayerTag);
public:
	void Create_Camera();

public:
	HRESULT Ready_ImGui();

	void Ready_ImGui_Create();
	void Ready_ImGui_List();
	void Ready_ImGui_Active_Camera_Info();
	void Ready_ImGui_Active_Camera_Animation();
	void Ready_ImGui_Active_Camera_Animation_Item();
	void Ready_ImGui_Active_Camera_Event_Item();
	void Ready_Guizmo();

private:
	_char m_szCreate_CameraName[MAX_PATH];
	CCamera_Compre::CAMERA_COMPRE_DESC m_tCreateCameraDesc{};

	_char m_szEdit_CameraName[MAX_PATH];
	CCamera_Compre::CAMERA_COMPRE_DESC m_tEditCameraDesc{};

	_char m_szCreate_AnimationName[MAX_PATH];
	_char m_szCreate_EventName[MAX_PATH];
	map<_wstring, vector<CAMERA_KEYFRAME>> m_CreateAnimations;
	map<_wstring, vector<CAMERA_EVENT_DATA>> m_CreateEvents;

	_int m_iListSelectCamera = {};
	_wstring m_strListSelectAnimation = {};

	_char m_szLoadFilePath[MAX_PATH] = "../../Client/Bin/Data/Camera";
	_char m_szSaveFilePath[MAX_PATH] = "../../Client/Bin/Data/Camera";

	LEVEL m_eCurrentLevel = {};

public:
	static CCamera_Controller* Create();
	virtual void Free() override;
};

NS_END