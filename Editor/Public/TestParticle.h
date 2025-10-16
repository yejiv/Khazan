#pragma once

#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
class CVIBuffer_Mesh_Instance;
NS_END

NS_BEGIN(Editor)

class CTestParticle final : public CGameObject
{
private:
	CTestParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestParticle(const CTestParticle& Prototype);
	virtual ~CTestParticle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	//	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	CVIBuffer_Mesh_Instance* m_pVIBufferCom = { nullptr };

#ifdef _DEBUG
	LARGE_INTEGER	m_Freq = {};				// CPU 타이머 주파수 (초당 카운트 수)
	LARGE_INTEGER	m_StartGpu{}, m_EndGpu{};	// GPU 구간 측정용
	LARGE_INTEGER	m_StartCpu{}, m_EndCpu{};	// CPU 구간 측정용

	_float			m_TotalGpuTime = {};		// GPU 누적 시간(ms)
	_float			m_TotalCpuTime = {};		// CPU 누적 시간(ms)
	_int			m_FrameCount = {};			// 누적 프레임 수

	_int			m_iPrintInterval = { 60 };		// 성능 출력 간격 (ex. 60프레임마다)
#endif

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTestParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END