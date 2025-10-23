#pragma once

#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Engine)
//class CCharacterVirtual;
NS_END

NS_BEGIN(Client)

class CCamera_Compre final : public CCamera
{
public:
	typedef struct tagCameraCompreDesc : public CCamera::CAMERA_DESC
	{

	}CAMERA_COMPRE_DESC;

	typedef struct tagCameraContactDesc {
		_float3 vNormal;
		_float3 vPoint;
		_bool isValid = false;
		_float fTtl = 0.0f;
	}CAMERA_CONTACT_DESC;

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
	void Collision_Enter(CGameObject* pObject, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	void Collision_Stay(CGameObject* pObject, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;

public:
	HRESULT Ready_Camera(void* pArg);
	HRESULT Ready_Body();

public:
	HRESULT Spring(_float fTimeDelta);
	HRESULT RayCast(_float fTimeDelta);

public:
	CAMERA_COMPRE_DESC Get_Desc();
private:
	//CCharacterVirtual* m_pCharVirCom = { nullptr };

	CAMERA_CONTACT_DESC m_CCDesc;

	/*_vector m_fMoveLerp;
	_vector m_fMoveLerp;*/

public:
	static CCamera_Compre* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;

};

NS_END