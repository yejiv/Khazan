#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Engine)
class CBody;
NS_END

NS_BEGIN(Client)

class CBladeNexus final : public CProp_Interactive
{
private:
	enum ANIM_STATE
	{
		AFTER_END, AFTER_IDLE, AFTER_LOOP, AFTER_START,
		BEFORE_END, BEFORE_IDLE, BEFORE_LOOP, BEFORE_START,
		END
	};

	enum class BLADENEXUS_ID
	{
		HEINMACH_ENTER = 0, HEINMACH_CAVE, HEINMACH_CLIFF, HEINMACH_YETUGA,
        EMBARS_UNDER, EMBARS_DEEP, EMBARS_CORE,
		ID_END
	};

public:
	typedef struct tagBladeNexusDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
	{


	}BLADENEXUS_DESC;

private:
	CBladeNexus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBladeNexus(const CBladeNexus& Prototype);
	virtual ~CBladeNexus() = default;

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
	_int m_iBladeNexus_ID = {};
	_tchar m_szPlaceName[MAX_PATH] = {};
    _bool m_isUnLock = { false };                   // 귀검 해금 여부

	EventObject m_Event = {};

	_float m_fEmissiveIntensity = { 0.f };
	_bool m_isEnableEmissive = { true };
	_bool m_isEnableBloom = { true };

    _bool m_isFindTarget = { false };
    CTransform* m_pTargetCom = { nullptr };

    _bool m_isFinishedRadialBlur = {};
    _float m_fRadialBlurTimeAcc = {};

    _uint m_iEventID = { 0 };

private:
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT Ready_Collision(void* pArg);
	HRESULT Ready_Interaction_Guide(void* pArg);
	HRESULT Ready_DefaultSetting(void* pArg);
    HRESULT	Ready_AnimationEvent();
    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;

	void Input_Interact_Event(_float fTimeDelta);
	void Animation_Update(_float fTimeDelta);
	void Animation_Change(_float fTimeDelta);

    void Find_Target();

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

public:
	static CBladeNexus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END