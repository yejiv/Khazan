//#pragma once
//#include "Engine_Defines.h"
//#include <wrl/client.h>
//// ===== Jolt =====
//#include <Jolt/Core/Color.h>
//#include <Jolt/Math/Vec3.h>
//#include <Jolt/Renderer/DebugRenderer.h>
//
//
//// 선택: 안전하게 new 매크로/CRT 디버그 상호작용 회피용
//#ifdef new
//#pragma push_macro("new")
//#undef new
//#endif
//#ifdef delete
//#pragma push_macro("delete")
//#undef delete
//#endif
//
//NS_BEGIN(Engine)
//
//class CPhysicsDebugRenderer final : public JPH::DebugRenderer {
//public:
//	using ComPtr = Microsoft::WRL::ComPtr<ID3D11Buffer>;
//
//
//	struct DebugVertex {
//		DirectX::XMFLOAT3 pos; // float3 position (world)
//		unsigned int rgba; // ABGR(8:8:8:8) packed color
//	};
//
//
//	// ==== 배치 구현 (Jolt의 Batch를 상속) ====
//	class TriangleBatchDX11 final : public JPH::RefTargetVirtual {
//	public:
//		TriangleBatchDX11(ID3D11Device* dev,
//			const JPH::DebugRenderer::Vertex* v, int vcount,
//			const uint32_t* i, int icount);
//
//
//		ID3D11Buffer* GetVB() const { return mVB.Get(); }
//		ID3D11Buffer* GetIB() const { return mIB.Get(); }
//		UINT GetIndexCount() const { return mIndexCount; }
//		UINT GetVertexStride() const { return sizeof(DebugVertex); }
//
//
//	private:
//		ComPtr mVB;
//		ComPtr mIB;
//		UINT mIndexCount = 0U;
//	};
//
//
//	using Batch = JPH::Ref<JPH::RefTargetVirtual>;
//	using GeometryRef = JPH::Ref<Geometry>;
//
//
//public:
//	CPhysicsDebugRenderer(ID3D11Device* dev, ID3D11DeviceContext* ctx);
//	~CPhysicsDebugRenderer();
//
//
//	// ---- 필수: DebugRenderer 인터페이스 구현 ----
//	// * 대량 메시 배치 생성 (가장 일반적인 오버로드)
//	virtual Batch CreateTriangleBatch(const JPH::DebugRenderer::Vertex* inVertices,
//		int inVertexCount,
//		const uint32_t* inIndices,
//		int inIndexCount) override;
//
//
//	// * 배치 그리기
//	virtual void DrawGeometry(JPH::RMat44Arg inModelMatrix,
//		const JPH::AABox& inWorldSpaceBounds,
//		float inLODScaleSq,
//		JPH::ColorArg inModelColor,
//		const GeometryRef& inGeometry,
//		ECullMode inCullMode,
//		ECastShadow inCastShadow,
//		EDrawMode inDrawMode) override;
//
//
//	// * 라인/삼각형 (fallback & 보조)
//	virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
//	virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3,
//		JPH::ColorArg inColor, ECastShadow inCastShadow) override;
//
//
//	// (선택) 텍스트 2D/3D, Bounding Box 등은 기본 구현 사용 or 필요 시 override
//
//
//	// ---- 프레임 관리 ----
//	void NextFrame(); // Jolt가 권장하는 per-frame 호출 (임시 배치 수명 정리)
//	void FlushAndDraw(bool drawTriangles = true, bool drawLines = true);
//
//
//	// 구성
//	void SetWireframeRS(ID3D11RasterizerState* rs) { mRSWireframe = rs; }
//	void SetSolidRS(ID3D11RasterizerState* rs) { mRSSolid = rs; }
//
//
//	// 호출 측에서 디버그 전용 셰이더/IL을 바인딩한 뒤 FlushAndDraw를 호출하는 것을 권장
//	// (정점 포맷: POSITION float3, COLOR uint)
//
//
//private:
//	// ---- 내부 유틸 ----
//	static inline unsigned int JoltColorToRGBA8(JPH::ColorArg c);
//	static inline DirectX::XMFLOAT3 ToXF3(const JPH::RVec3& v);
//
//
//	void EnsureDynamicBuffers(size_t lineVertCapacity, size_t triVertCapacity);
//
//
//private:
//	ID3D11Device* mDevice = nullptr;
//	ID3D11DeviceContext* mContext = nullptr;
//
//
//	// 동적 라인/트라이 정점 버퍼 (프레임마다 갱신)
//	ComPtr mLineVB;
//	ComPtr mTriVB;
//	UINT mLineVBSize = 0; // vertices count capacity
//	UINT mTriVBSize = 0;
//
//
//	std::vector<DebugVertex> mLineVerts;
//	std::vector<DebugVertex> mTriVerts;
//	std::mutex mMutex; // 멀티스레드 방어(선택)
//
//
//	// 배치에서 사용할 RS (외부 주입)
//	ID3D11RasterizerState* mRSWireframe = nullptr; // not owned
//	ID3D11RasterizerState* mRSSolid = nullptr; // not owned
//};
//
//NS_END
//
//#ifdef new
//#pragma pop_macro("new")
//#endif
//#ifdef delete
//#pragma pop_macro("delete")
//#endif