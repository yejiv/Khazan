#pragma once
#include "Editor_Defines.h"
#include "Base.h"



NS_BEGIN(Editor)

class CAnimationTool final :  public CBase
{
private:
	CAnimationTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CAnimationTool() = default;

public:
	HRESULT Initialize_Prototype();
	//virtual HRESULT Initialize_Clone(void* pArg);
	void Update(_float fTimeDelta);

public:
	void	Add_Model();
	void	Remove_Model();

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	vector<class CJOH_EditorModelTest*> m_GameObjects;
	vector<_wstring>	m_ObjectNames; 
	_float3				m_vPreScale = { 0.01f ,0.01f ,0.01f };
	_bool				m_isAnim = {};
	_wstring			m_strModelName = {};
	string				m_strModelPath = {};

	// 선택된 게임 오브젝트 인덱스
	_int m_iSelectedIndex = -1;


	// 위젯 표시 플래그
	_bool m_bShowOpenModel = { false };
	_bool m_bShowTool = { false };

private:
	void	Widget();
	void	OpenModel_Widget();
	void	Tool_Widget();
	void	Tool_Export_Update_Widget();
public:
	static CAnimationTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END