#pragma once

#include "Engine_Defines.h"
#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include "Jolt/Renderer/DebugRendererSimple.h"

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif
NS_BEGIN(Engine)

class CJolt_DebugRenderer final : public DebugRendererSimple
{
public:
	explicit CJolt_DebugRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CJolt_DebugRenderer();

public:
	void	BeginFrame();
	void	EndFrame();
	virtual		void		DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor) override;
	virtual		void		DrawText3D(RVec3Arg inPosition, const string_view& inString, ColorArg inColor, float inHeight) override;

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

	bool        m_bBatchOpen = false;
	XMMATRIX    m_matWorldIdent = XMMatrixIdentity();
	XMMATRIX    m_matView = XMMatrixIdentity();
	XMMATRIX    m_matProj = XMMatrixIdentity();
};

NS_END