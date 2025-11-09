#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "Player.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CBody_Khazan_Spear final : public CPartObject
{
public:
    typedef struct tagBodyKhazanSpearDesc : public CPartObject::PARTOBJECT_DESC
    {
        _uint* pState = { nullptr };
        class CTransform* pParentTransform = { nullptr };

    }BODY_KHAZAN_SPEAR_DESC;

private:
    CBody_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBody_Khazan_Spear(const CBody_Khazan_Spear& Prototype);
    virtual ~CBody_Khazan_Spear() = default;

public:
    _float4x4* Get_BoneMatrix(const _char* pBoneName);
    _bool* Get_FinishedAnimation() { return &m_isFinishedAnimation; }
    void		Set_matSpearFX(_float4x4* mat) { m_pSpearFX_Matrix = mat; }
    void		Set_matSpearOffset(_matrix mat) { m_SpearOffset_Matrix = mat; }


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void    Priority_Update(_float fTimeDelta);
	virtual void    Update(_float fTimeDelta);
	virtual void    Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() override;
    virtual HRESULT Render_Outline() override;
	void			Render_Part(CModel* pModel);
	void			Render_Part_Shadow(CModel* pModel);
    void            Render_Part_Outline(CModel* pModel);

public:
	CModel* Get_Model() { return m_pModelCom; }

public:
    OUTLINE_CONFIG Get_OutlineConfig() { return m_OutlineConfig; }
    void Set_OutlineConfig(OUTLINE_CONFIG Config)
    {
        m_OutlineConfig.vColor = Config.vColor;
        m_OutlineConfig.fSize = Config.fSize;
    }

private:
	class CTransform* m_pParentTransform = { nullptr };
    CShader* m_pShaderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };
    CModel* m_pModelCom_Arm = { nullptr };
    CModel* m_pModelCom_Face = { nullptr };
    CModel* m_pModelCom_Hair = { nullptr };
    CModel* m_pModelCom_Leg = { nullptr };
    CModel* m_pModelCom_Shoes = { nullptr };
    CModel* m_pModelCom_Torso = { nullptr };

    CBody* m_pBodyCom_SpearTip1 = { nullptr };

    _float4x4* m_pSpearFX_Matrix = { nullptr };
    _matrix				m_SpearOffset_Matrix = {};

    _uint* m_pParentState = { nullptr };
    _uint				m_iCurState = {  };


    _bool				m_isFinishedAnimation = { false };
    _bool				m_isSetAnimation = { false };
    _uint				m_iCurSetAnimIndex = { 0 };

    const _uint			m_iSetAnimation[3] = { 3,2,1 };

    /* 뼈 위치 */
    _float4x4* m_pSpearTip1_Matrix = { nullptr };
    _float4x4			m_pSpearTip1_MatrixW;

    OUTLINE_CONFIG      m_OutlineConfig = { _float3(1.f, 0.f, 1.f), 0.001f, 0.f, 0.f };

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;


private:
    void				Update_Collider(_float fTimeDelta);

private:
    HRESULT				Ready_Components();
    HRESULT				Ready_AnimationEvent();
    HRESULT				Ready_Collider();
    HRESULT				Bind_ShaderResources();


    /*
    공격 진입~ 공격 끝날 때까지 ( 트리거 진입 , 범위, 탈출 이벤트 다 있음 )
    필요 시 함수 만들어서 사용하셔도 됩니다.

    - 단일 찌르기 공격 -
    Effect1 : Fast

    - 연속 세트 찌르기 공격 -
    Effect2	: Set   1 번째 공격
    Effect3 : Set	2 번째 공격
    Effect4 : Set	3 번째 공격
    Effect5 : Set	4 번째 공격
    Effect6 : Set	5 번째 공격
    Effect7 : Set	6 번째 공격
    */
private:
    void	Effect1_Enter();
    void	Effect1_Exit();
    void	Effect1_Continue();
    void	Effect2();
    void	Effect3();
    void	Effect4();
    void	Effect5();
    void	Effect6();
    void	Effect7_Enter();
    void	Effect7_Exit();
    void	Effect7_Continue();


private:
    inline void		Add_State(_uint i) { *m_pParentState |= i; }
    inline void		Toggle_State(_uint i) { *m_pParentState ^= i; }
    inline void		Remove_State(_uint i) { *m_pParentState &= ~i; }
    inline _bool	Has_State(_uint i) { return (*m_pParentState & i) != 0; }
    inline void		Clear_State() { *m_pParentState = 0; }
    inline _bool	Has_States();

public:
    static CBody_Khazan_Spear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END
