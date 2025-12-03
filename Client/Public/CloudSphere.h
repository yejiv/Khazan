#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CCloudSphere final : public CGameObject
{
public:
	typedef struct tagCloudSphereDesc : public CGameObject::GAMEOBJECT_DESC
	{
		LEVEL eLevel = LEVEL::END;

		CLOUD_DESC CloudDesc{};

	}CLOUD_SPHERE_DESC;

private:
	enum TEX_TYPE { DISTANCE_GRADATION, LOOKUP, NORMAL, DISTORTION, TEX_TYPE_END };

private:
	CCloudSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCloudSphere(const CCloudSphere& Prototype);
	virtual ~CCloudSphere() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	CLOUD_DESC Get_CloudDesc() { return m_CloudDesc; }
	void Set_CloudDesc(CLOUD_DESC CloudDesc) { m_CloudDesc = CloudDesc; }
    void Start_LerpCloud(CLOUD_DESC LerpCloudDesc, _float fDuration);

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[TEX_TYPE_END] = { nullptr };

	CLOUD_DESC m_CloudDesc = {};
    CLOUD_DESC m_StartCloudDesc = {};
    CLOUD_DESC m_LerpCloudDesc = {};

	_float m_fTimeAcc = { 0.f };

    _bool m_isTransition = { false };
    _float m_fTransTimeAcc = { 0.f };
    _float m_fDuration = { 0.f };

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

	HRESULT Bind_Cloud_ShaderResources();

#ifdef _DEBUG
private:
    _bool m_isCloudWindow = { false };

    CLOUD_DESC m_FixDesc = {};

    _char m_szFilePath[MAX_PATH] = { "../../Client/Bin/Data/Map/MapData/" };
    KHAZAN_MAP m_eMapType = { KHAZAN_MAP::END };

    string m_strFolderName = {};

    _char m_szFileName[MAX_PATH] = {};

    string m_strFileName = {};

private:
    void Debug_CloudEdit();
#endif // _DEBUG

public:
	static CCloudSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END