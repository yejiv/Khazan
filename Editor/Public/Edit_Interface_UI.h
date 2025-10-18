#pragma once
#include "Base.h"
#include "Editor_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)
class CEdit_Interface_UI final : public CBase
{
private:
	CEdit_Interface_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEdit_Interface_UI() = default;
public:
	void						Update_UIInterface(_float fTimeDelta);

public:
	HRESULT						Initialize(LEVEL eLevel);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	class CEdit_UIBackGround* m_pBackGround = { nullptr };
	_bool						m_RenderBackGround = { false };
	LEVEL						m_eLevel;

	vector<class CEdit_UIBase*> m_pRootUIs;
	_int						m_iSeletRootUI = { -1 };
	string						m_szSeleteUIName = {};

	//UI 셋팅 관련
	_char						m_szUIName[MAX_PATH] = {};
	_char						m_szClassName[MAX_PATH] = {};
	_char						m_szUIReName[MAX_PATH] = {};
	_int						m_iUIType = {};
	_int						m_iUISize[2] = {};

	_int						m_iMovePos[2] = {};
	_int						m_iScalingSize[2] = {};

	_char						m_szFilePath[MAX_PATH] = {};

	//Tex 관련
	_char						m_szPrototypePath[MAX_PATH] = {};
	_char						m_szFrameName[MAX_PATH] = {};
	_float						m_fTexSize = { 1.f };
	_int						m_iTexType = {};

	_float						m_fAccTime = {};
	_bool						m_isAnime = { false };
private:
	HRESULT						Ready_Object(LEVEL eLevel);
	void						Update_BackColor(_float fTimeDelta);
	void						SaveLoad_UI();
	void						Create_UI();
	void						Selete_UI(_float fTimeDelta);
	void						Animation_UI(_float fTimeDelta);

	void						SetName_UI();
	void						SizePos_UI(_float fTimeDelta);
	void						SetTexture_UI();
	void						Anime_Option(_float fTimeDelta, _bool bAnimCehck);
public:
	static CEdit_Interface_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);
	virtual void				Free();
};
NS_END
