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
	_int				m_iSelectedIndex = -1;


	// 위젯 표시 플래그
	_bool				m_isShowOpenModel = { false };
	_bool				m_isShowTool = { false };
	_bool				m_isShowTool_Control = { false };
	_bool				m_isShowTool_ExportUpdate = { false };
	_bool				m_isShowTool_AnimationList = { false };

	_bool				m_isEnble_AnimList = { true };

	//ANIMATION_SETUP_DATA
	_bool				m_isEnble_AnimInfo = { false };
	_bool				m_isEnble_AnimTime = { false };
	_bool				m_isEnble_AnimSet = { false };
	_bool				m_isEnble_AnimRootMotion = { false };
	_bool				m_isEnble_AnimEvent = { false };

	_bool				m_isLevelBtnPress = { false };

	/*애니메이션 */
	_int				m_iSelectedAnimIndex = { -1 };
	_char				m_szAnimSearchBuffer[256] = "";
	// 구간별 속도 제어
	_float				m_fCurrentFrame = 0.f;
	_bool				m_isPlating = { false };


	/* 위젯 */
	_float m_pannelLeftWidth = 400.0f;
	_float m_pannelMiddleWidth = 500.0f;

	_float	m_AnimInfo_Btn_Width = 120.f;
private:
	void	Widget();

	/* 모델 열기 */
	void	OpenModel_Widget();

	/* 정보 추출 및 정보 업데이트 */
	void	Tool_Export_Update_Widget();

	/* 툴 */
	void	Tool_Widget();
	void	Tool_AnimationControl_Widget();
	void	Tool_AnimationList_Widget();
	void	Tool_AnimationInfo_Widget();



private:
	void	Add_Model(_uint iLevelIndex);
	void	Remove_Model();
	string  ConvertToRelativePath(const string& absolutePath);
	string  ConvertToClientRelativePath(const string& absolutePath);

public:
	static CAnimationTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END

