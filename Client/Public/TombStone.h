#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Engine)
class CBody;
NS_END

NS_BEGIN(Client)

class CTombStone final : public CProp_Interactive
{
private:
	enum ANIM_STATE
	{
		AFTER_END, AFTER_IDLE, AFTER_LOOP, AFTER_START, BEFORE_IDLE, BEFORE_START,
		END
	};

public:
	typedef struct tagTombStoneDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
	{


	}TOMBSTONE_DESC;

private:
	CTombStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTombStone(const CTombStone& Prototype);
	virtual ~CTombStone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CBody* m_pStaticCom = { nullptr };
	CBody* m_pTriggerCom = { nullptr };

	class CInteraction_Guide* m_pGuide = { nullptr };

private:
	ANIM_STATE m_eAnimState = { ANIM_STATE::BEFORE_IDLE };

private:
	_tchar m_szPlaceName[MAX_PATH] = {};

	_float3 m_vPlayerPosition = {};

	EventObject m_Event = {};

	_float m_fTimeAcc = { 0.f };

	_float m_fEmissiveIntensity = { 0.f };
	_bool m_isEnableEmissive = { true };
	_bool m_isEnableBloom = { true };

private:
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT Ready_Collision(void* pArg);
	HRESULT Ready_Interaction_Guide(void* pArg);
	HRESULT Ready_PlaceName(void* pArg);
	virtual HRESULT Bind_Materials(_uint iMeshIndex) override;

	void Input_Interact_Event(_float fTimeDelta);
	void Animation_Update(_float fTimeDelta);
	void Animation_Change(_float fTimeDelta);

private:
	virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;

public:
	static CTombStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END