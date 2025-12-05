#pragma once
#include "Engine_Defines.h"

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

NS_BEGIN(Engine)

class CSoftBody_Creator
{
public:
    static Ref<SoftBodySharedSettings>	CreateCloth(uint inGridSizeX = 30, uint inGridSizeZ = 30, float inGridSpacing = 0.75f, const function<float(uint, uint)>& inVertexGetInvMass = [](uint, uint) { return 1.0f; }, const function<Vec3(uint, uint)>& inVertexPerturbation = [](uint, uint) { return Vec3::sZero(); }, SoftBodySharedSettings::EBendType inBendType = SoftBodySharedSettings::EBendType::None, const SoftBodySharedSettings::VertexAttributes& inVertexAttributes = { 1.0e-5f, 1.0e-5f, 1.0e-5f });

    /// Same as above but fixates the corners of the cloth
    static Ref<SoftBodySharedSettings>	CreateClothWithFixatedCorners(uint inGridSizeX = 30, uint inGridSizeZ = 30, float inGridSpacing = 0.75f);

    static Ref<SoftBodySharedSettings>  CreateClothSoftBodySettings(const CLOTH_MESH_DATA& tCloth, const SKELETON_DATA& tSkeleton);

    static Ref<SoftBodySharedSettings>  CreateClothSoftBody_Settings_independent(const CLOTH_MESH_DATA& tCloth);

    static Ref<SoftBodySharedSettings> CreateClothSoftBody_Settings_independentV2(vector<_float3> vVertexPos, vector<_uint> Indices);

    /// Create a hollow sphere
    /// @param inRadius Radius of the sphere
    /// @param inNumTheta Number of segments in the theta direction
    /// @param inNumPhi Number of segments in the phi direction
    static Ref<SoftBodySharedSettings>	CreateSphere(float inRadius = 1.0f, uint inNumTheta = 10, uint inNumPhi = 20, SoftBodySharedSettings::EBendType inBendType = SoftBodySharedSettings::EBendType::None, const SoftBodySharedSettings::VertexAttributes& inVertexAttributes = { 1.0e-4f, 1.0e-4f, 1.0e-3f });
};

NS_END