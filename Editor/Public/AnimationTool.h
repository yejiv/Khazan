#pragma once
#include "Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Editor)

class CAnimationTool final : public CBase
{
private:
	CAnimationTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CAnimationTool() = default;

public:
	HRESULT Initialize_Prototype();
	void Update(_float fTimeDelta);

private:
	// === 엔진 관련 ===
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	// === 모델 관련 ===
	vector<class CJOH_EditorModelTest*>		m_GameObjects;
	vector<_wstring>						m_ObjectNames;
	_float3									m_vPreScale = { 0.0001f, 0.0001f, 0.0001f };
	_bool									m_isAnim = { false };
	_int									m_iCurrentRenderGroup = {3};
	_wstring								m_strModelName;
	string									m_strModelPath;
	_int									m_iSelectedIndex = { -1 }; //현재 선택된 게임 오브젝트

	// === 위젯 표시 플래그 ===
	_bool									m_isShowOpenModel = { false };
	_bool									m_isShowTool = { false };
	_bool									m_isShowTool_Control = { false };
	_bool									m_isShowTool_ExportUpdate = { false };
	_bool									m_isLevelBtnPress = { false };

	// === 애니메이션 정보 표시 플래그 ===
	_bool									m_isEnble_AnimList = { true };
	_bool									m_isEnble_AnimInfo = { true };
	_bool									m_isEnble_AnimSet = { true };
	_bool									m_isEnble_AnimRootMotion = { true };
	_bool									m_isEnble_AnimEvent = { true };
	_bool									m_isEnble_MakeAnimEvent = { false };
	_bool									m_isEnble_MakeAnimSet = { false };
	_bool									m_isEnble_AnimSlider = { false };

	// === 애니메이션 리스트 ===
	_int									m_iSelectedAnimIndex = { -1 }; //현재 선택된 애니메이션 인덱스 
	_char									m_szAnimSearchBuffer[256] = "";

	// === 애니메이션 프레임 제어 ===
	class CEditor_Animation*				m_pCurAnimaion = { nullptr };
	_bool									m_isPlaying = { false };
	_float*									m_fCurrentFrame = { nullptr };
	_float									m_fCurrentDuration = { 0.f };

	// === 애니메이션 이벤트 제작 (임시 저장용) ===
	_int									m_iAnimSliderListSelectedIndex = { -1 };
	vector<FLOAT2_DATA>						m_vecEventFrames;
	FLOAT2_DATA								m_vTempFrames = { 0.f, 0.f };
	vector<string>							m_vecEventKeys;
	_char									m_szEventKeyInputText[256] = "";
	vector<_uint>							m_vecTriggers;
	_bool									m_isTriggerOnce = { false };
	_bool									m_isTriggerOnEnter = { true };
	_bool									m_isTriggerOnExit = { false };
	_bool									m_isTriggerContinuous = { false };
	_bool									m_isTriggerOnce2 = { false };
	_bool									m_isTriggerOnEnter2 = { true };
	_bool									m_isTriggerOnExit2 = { false };
	_bool									m_isTriggerContinuous2 = { false };

	// === 애니메이션 세트 제작 (임시 저장용) ===
	_char									m_szAnimSetNameInputText[256] = "";
	_char									m_szAnimSetSearchBuffer[256] = "";
	vector<pair<_int, string>>				m_AnimSet;  // <애니메이션 인덱스, 애니메이션 이름>

	// === UI 레이아웃 ===
	_float									m_pannelLeftWidth = { 400.0f };
	_float									m_pannelMiddleWidth = { 600.0f };

private:
	// === 위젯 함수 ===
	void	Widget();
	void	OpenModel_Widget();
	void	Tool_Export_Update_Widget();
	void	Tool_Widget();
	void	Tool_AnimationControl_Widget();
	void	Tool_AnimationList_Widget();
	void	Tool_AnimationInfo_Widget();
	void	Tool_MakeAnimEvent_Widget();
	void	Tool_MakeAnimSet_Widget();

private:
	// === 유틸리티 함수 ===
	void	Add_Model(_uint iLevelIndex);
	void	Remove_Model();
	void	Update_DataModel(const string& strPath);
	string	ConvertToRelativePath(const string& absolutePath);
	string	ConvertToClientRelativePath(const string& absolutePath);

	string	DirectionToString(_uint iDir);
	string	RotationToString(_uint iDir);

public:
	static CAnimationTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END