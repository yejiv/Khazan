#pragma once
#include "Editor_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CMotionTrail;
NS_END

NS_BEGIN(Editor)

class CE_Body_Khazan_Spear final : public CPartObject
{
public:
    typedef struct tagBodyKhazanSpearDesc : public CPartObject::PARTOBJECT_DESC
    {
        _uint* pState = { nullptr };
        _uint* pStatus = { nullptr };

        class CTransform* pParentTransform = { nullptr };

    }BODY_KHAZAN_SPEAR_DESC;

private:
    CE_Body_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CE_Body_Khazan_Spear(const CE_Body_Khazan_Spear& Prototype);
    virtual ~CE_Body_Khazan_Spear() = default;

public:
    _float4x4*          Get_BoneMatrix(const _char* pBoneName);
    _bool*              Get_FinishedAnimation() { return &m_isFinishedAnimation; }

public:
	virtual HRESULT     Initialize_Prototype();
	virtual HRESULT     Initialize_Clone(void* pArg);
	virtual void        Priority_Update(_float fTimeDelta);
	virtual void        Update(_float fTimeDelta);
	virtual void        Late_Update(_float fTimeDelta);
	virtual HRESULT     Render();
	virtual HRESULT     Render_Shadow() override;
    virtual HRESULT     Render_MotionTrail() override;
	void			    Render_Part(CModel* pModel);
	void			    Render_Part_Shadow(CModel* pModel);
    void                Render_Part_MotionTrail(CModel* pModel, class CShader* pShader);

public:
	CModel*             Get_Model() { return m_pModelCom; }

public:
    // Shader
    void                Set_EnableEdge(_bool isEnable) { m_isEnableEdge = isEnable; }
    void                Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }

private:
	class CTransform*   m_pParentTransform = { nullptr };   

    CShader*            m_pShaderCom = { nullptr };

    CModel*             m_pModelCom = { nullptr };
    CModel*             m_pModelCom_Arm = { nullptr };
    CModel*             m_pModelCom_Face = { nullptr };
    CModel*             m_pModelCom_Hair = { nullptr };
    CModel*             m_pModelCom_Leg = { nullptr };
    CModel*             m_pModelCom_Shoes = { nullptr };
    CModel*             m_pModelCom_Torso = { nullptr };

    _uint*              m_pParentState = { nullptr };
    _uint*              m_pParentStatus = { nullptr };
    _uint				m_iCurState = {  };

    _bool				m_isFinishedAnimation = { false };
    _uint				m_iCurSetAnimIndex = { 0 };

    OUTLINE_CONFIG      m_OutlineConfig = { _float3(1.f, 0.f, 1.f), 0.001f, 0.f, 0.f };

    // Shader
    _bool               m_isEnableEdge = { true };
    _float              m_fAlpha = { 1.f };

    // Motion Trail Test
    CMotionTrail*       m_pMotionTrailCom = { nullptr };

private:
    HRESULT				Ready_Components();
    HRESULT				Bind_ShaderResources();

public:
    static CE_Body_Khazan_Spear*    Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void                    Free() override;
};

NS_END
