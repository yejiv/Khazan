#pragma once

#include "Editor_Defines.h"
#include "Perception_Defines.h"
#include "Level.h"

NS_BEGIN(Editor)

class CLevel_AI final : public CLevel
{
private:
	CLevel_AI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_AI() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT			Ready_Edit_Objects();


#pragma region Perception
private:
	void				Show_Peception_Menu(const char* szDefaultFileName);
	AIPERCEPTION_DATA	Convert_AIPerceptionData(const SIGHT_DESC& Desc, const string& MonsterType, const string& FileName, const vector<string>& Callbacks);
	void				Save_Perception(const vector<SIGHT_DESC>& SightList, const string& MonsterType, const string& FileName, const vector<string>& Callbacks);
	void				Load_Perception(const vector<SIGHT_DESC>& SightList, const string& FileName);
#pragma endregion

#pragma region BlackBoard
	void				Show_BlackBoard_Menu(const char* szDefaultFileName);
	void				Save_BlackBoard(const vector<AIBLACKBOARD_DATA>& BBList, const string& FileName);
	void				Load_BlackBoard(vector<AIBLACKBOARD_DATA>& BBList, const string& FileName);
#pragma endregion 

#pragma region BehaviorTree
	void				Show_BehaviorTree_Menu(const char* szDefaultFileName);
	void				Show_BT_Editor(AI_BTDATA& TreeData);
	void				Show_BTNode_Hierarchy(AIBTNODE_DATA& Node);

	void				Save_BehaviorTree(const AI_BTDATA& Data, const string& FileName);

	void				SaveNode(JSON& j, const AIBTNODE_DATA& Node);
	void				LoadNode(const JSON& j, AIBTNODE_DATA& OutNode);

#pragma endregion 




public:
	static CLevel_AI*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free() override;
};

NS_END