#pragma once
#include "AI_Controller.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END


NS_BEGIN(Client)

class CAI_Controller_Yetuga final : public CAI_Controller
{
private:
	CAI_Controller_Yetuga();
	virtual ~CAI_Controller_Yetuga() = default;

public:
	virtual HRESULT					Initialize(void* pArg);
	virtual void					Update(CGameObject* pOwner, _float fTimeDelta) override;

	HRESULT							Ready_BlackBoard();
	HRESULT							Ready_Perception();
	HRESULT							Ready_BehaviorTree();

public:
	static CAI_Controller_Yetuga*	Create(void* pArg);
	virtual void					Free() override;
};

NS_END