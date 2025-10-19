#pragma once

#include "Editor_Defines.h"
#include "Base.h"
#include "Camera_Compre.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)

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

	HRESULT Ready_ImGui_Create();
	HRESULT Ready_ImGui_List();
	HRESULT Ready_ImGui_Active_Camera_Info();

private:
	_char m_szCreate_CameraName[MAX_PATH];
	CCamera_Compre::CAMERA_COMPRE_DESC m_tCreateCameraDesc{};

	_int m_iListSelectCamera = {};

public:
	static CCamera_Controller* Create();
	virtual void Free() override;
};

NS_END