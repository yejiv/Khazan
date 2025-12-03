#pragma once

#include "Editor_Defines.h"
#include "Camera.h"

NS_BEGIN(Engine)
class CCharacterVirtual;
NS_END

NS_BEGIN(Editor)

class CCamera_Compre final : public CCamera
{
public:
	typedef struct tagCameraCompreDesc : public CCamera::CAMERA_DESC
	{

	}CAMERA_COMPRE_DESC;


private:
	CCamera_Compre(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Compre(const CCamera_Compre& Prototype);
	virtual ~CCamera_Compre() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Update_Free(_float fTimeDelta);
	void Update_Spring(_float fTimeDelta);

public:
	HRESULT Ready_Camera(void* pArg);
	HRESULT Ready_Body();

public:
	CAMERA_COMPRE_DESC Get_Desc();

    _float				m_fYaw = 0.f;
    _float				m_fPitch = 0.6f;
    _float				m_fRadius = 4.f;

    _float				m_fPitchMin = -1.2f;
    _float				m_fPitchMax = 0.7f;
    _float				m_fRadiusMin = 2.f;
    _float				m_fRadiusMax = 12.f;
    _float				m_fSkin = 0.02f;

    _float				m_fFollowValue = 2.f;
    _vector				m_vLerpMove = { 0.f, 0.f, 0.f, 1.f };

    const _float4x4* m_pObjMatrix = { nullptr };
    const _float4x4* m_pSocketMatrix = { nullptr };

	/*_vector m_fMoveLerp;
	_vector m_fMoveLerp;*/

public:
	static CCamera_Compre* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;

};

NS_END