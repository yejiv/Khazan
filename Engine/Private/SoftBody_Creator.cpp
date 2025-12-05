#include "SoftBody_Creator.h"

Ref<SoftBodySharedSettings> CSoftBody_Creator::CreateCloth(uint inGridSizeX, uint inGridSizeZ, float inGridSpacing, const function<float(uint, uint)>& inVertexGetInvMass, const function<Vec3(uint, uint)>& inVertexPerturbation, SoftBodySharedSettings::EBendType inBendType, const SoftBodySharedSettings::VertexAttributes& inVertexAttributes)
{
    const float cOffsetX = -0.5f * inGridSpacing * (inGridSizeX - 1);
    const float cOffsetZ = -0.5f * inGridSpacing * (inGridSizeZ - 1);

    // Create settings
    SoftBodySharedSettings* settings = new SoftBodySharedSettings;
    for (uint z = 0; z < inGridSizeZ; ++z)
        for (uint x = 0; x < inGridSizeX; ++x)
        {
            SoftBodySharedSettings::Vertex v;
            Vec3 position = inVertexPerturbation(x, z) + Vec3(cOffsetX + x * inGridSpacing, 0.0f, cOffsetZ + z * inGridSpacing);
            position.StoreFloat3(&v.mPosition);
            v.mInvMass = inVertexGetInvMass(x, z);
            settings->mVertices.push_back(v);
        }

    // Function to get the vertex index of a point on the cloth
    auto vertex_index = [inGridSizeX](uint inX, uint inY) -> uint
        {
            return inX + inY * inGridSizeX;
        };

    // Create faces
    for (uint z = 0; z < inGridSizeZ - 1; ++z)
        for (uint x = 0; x < inGridSizeX - 1; ++x)
        {
            SoftBodySharedSettings::Face f;
            f.mVertex[0] = vertex_index(x, z);
            f.mVertex[1] = vertex_index(x, z + 1);
            f.mVertex[2] = vertex_index(x + 1, z + 1);
            settings->AddFace(f);

            f.mVertex[1] = vertex_index(x + 1, z + 1);
            f.mVertex[2] = vertex_index(x + 1, z);
            settings->AddFace(f);
        }

    // Create constraints
    settings->CreateConstraints(&inVertexAttributes, 1, inBendType);

    // Optimize the settings
    settings->Optimize();

    return settings;
}

Ref<SoftBodySharedSettings> CSoftBody_Creator::CreateClothWithFixatedCorners(uint inGridSizeX, uint inGridSizeZ, float inGridSpacing)
{
    auto inv_mass = [inGridSizeX, inGridSizeZ](uint inX, uint inZ) {
        return (inX == 0 && inZ == 0)
            || (inX == inGridSizeX - 1 && inZ == 0)
            || (inX == 0 && inZ == inGridSizeZ - 1)
            || (inX == inGridSizeX - 1 && inZ == inGridSizeZ - 1) ? 0.0f : 1.0f;
        };

    return CreateCloth(inGridSizeX, inGridSizeZ, inGridSpacing, inv_mass);
}

Ref<SoftBodySharedSettings> CSoftBody_Creator::CreateClothSoftBodySettings(const CLOTH_MESH_DATA& tCloth, const SKELETON_DATA& tSkeleton)
{
    SoftBodySharedSettings* pSettings = new SoftBodySharedSettings;

    // 버텍스 / 페이스 채우기

    // 1. Vertices
    pSettings->mVertices.resize((_uint)tCloth.vVertices.size());
    for (_uint i = 0; i < tCloth.vVertices.size(); i++)
    {
        const CLOTH_VERTEX& tCV = tCloth.vVertices[i];

        SoftBodySharedSettings::Vertex vertex;
        vertex.mPosition = StoreFloat3(tCV.vBindPos);
        vertex.mInvMass = 1.f;

        pSettings->mVertices[i] = vertex;
    }

    // 2. Face
    for (_uint i = 0; i < tCloth.vIndices.size(); i += 3)
    {
        SoftBodySharedSettings::Face face;
        face.mVertex[0] = tCloth.vIndices[i + 0];
        face.mVertex[1] = tCloth.vIndices[i + 1];
        face.mVertex[2] = tCloth.vIndices[i + 2];
        pSettings->AddFace(face);
    }

    // 3. InvBindMatrices 세팅

    // InvBindMatrices :  이 소프트바디에 실제로 쓰이는 본만 모아도 됨
    vector<int> BoneToInvBindIndex(tSkeleton.vJoints.size(), -1);

    for (_uint j = 0; j < tSkeleton.vJoints.size(); j++)
    {
        const JOINT_INFO& joint = tSkeleton.vJoints[j];

        Mat44 Bind = LoadMat44(joint.matrixBindPoseModel);
        Mat44 invBind = Bind.Inversed();

        SoftBodySharedSettings::InvBind inv(joint.iJointIndex, invBind);
        BoneToInvBindIndex[j] = (int)pSettings->mInvBindMatrices.size();
        pSettings->mInvBindMatrices.push_back(inv);
    }

    // 4. Skinned Constraints
    pSettings->mSkinnedConstraints.clear();
    pSettings->mSkinnedConstraints.reserve(tCloth.vVertices.size());

    for (_uint i = 0; i < tCloth.vVertices.size(); i++)
    {
        const CLOTH_VERTEX& tCV = tCloth.vVertices[i];

        SoftBodySharedSettings::Skinned Skin;
        Skin.mVertex = i;

        _uint write = 0;
        for (_uint k = 0; k < 4; k++)
        {
            _float fWeight = tCV.fBoneWeight[k];
            _uint iBoneIndex = tCV.iBoneIndex[k];

            if (fWeight <= 0.0f)
                continue;

            _int iInvBindIdx = BoneToInvBindIndex[iBoneIndex];
            if (iInvBindIdx < 0)
                continue;

            Skin.mWeights[write++] = SoftBodySharedSettings::SkinWeight((_uint)iInvBindIdx, fWeight);

            if (write >= 4) 
                break;
        }

        for (; write < 4; write++)
            Skin.mWeights[write] = SoftBodySharedSettings::SkinWeight();

        Skin.NormalizeWeights();

        _float fY = tCloth.vVertices[i].vBindPos.y;        
        _float fT = clamp((fY - 1) / (2 - 1), 0.f, 1.f);

        Skin.mMaxDistance = Lerp(0.01f, 0.3f, 1.f - fT);

        Skin.mBackStopDistance = 0.01f;
        Skin.mBackStopRadius = 0.02f;

        pSettings->mSkinnedConstraints.push_back(Skin);
    }

    // 5. VertexAttributes

    SoftBodySharedSettings::VertexAttributes Attr;
    Attr.mCompliance = 0.0f;      // 에지(거리 유지) 꽉 잡기
    Attr.mShearCompliance = 0.0f;      // shear 에지도 꽉
    Attr.mBendCompliance = 0.001f;    // 살짝 구부러질 수 있게 (값 키우면 더 휨)

    // LRA는 일단 안 씀 (스커트는 스키닝+Skinned constraints로 충분)
    Attr.mLRAType = SoftBodySharedSettings::ELRAType::None;
    Attr.mLRAMaxDistanceMultiplier = 1.0f;

    // 전체 버텍스에 공통 attr 하나 쓰는 경우:
    pSettings->CreateConstraints(&Attr, 1, SoftBodySharedSettings::EBendType::Dihedral);

    // 스키닝 노멀 계산
    pSettings->CalculateSkinnedConstraintNormals();

    pSettings->Optimize();

    return pSettings;
}

Ref<SoftBodySharedSettings> CSoftBody_Creator::CreateClothSoftBody_Settings_independent(const CLOTH_MESH_DATA& tCloth)
{
    SoftBodySharedSettings* pSettings = new SoftBodySharedSettings;

    const _uint iNumVerts = (_uint)tCloth.vVertices.size();

    // --- 0. 치마 높이 범위 계산 (허리 vs 아래 구분용) ---
    _float fMinY = FLT_MAX;
    _float fMaxY = -FLT_MAX;
    for (_uint i = 0; i < iNumVerts; ++i)
    {
        const CLOTH_VERTEX& tCV = tCloth.vVertices[i];
        _float y = tCV.vBindPos.y; // 모델 로컬 기준 Y
        fMinY = min(fMinY, y);
        fMaxY = max(fMaxY, y);
    }
    _float fHeight = max(fMaxY - fMinY, 1e-3f);

    // --- 1. Vertices ---
    pSettings->mVertices.resize(iNumVerts);
    for (_uint i = 0; i < iNumVerts; ++i)
    {
        const CLOTH_VERTEX& tCV = tCloth.vVertices[i];

        SoftBodySharedSettings::Vertex vertex;
        vertex.mPosition = StoreFloat3(tCV.vBindPos); // 바인드 포즈 위치

        _float y = tCV.vBindPos.y;
        _float t = (y - fMinY) / fHeight;   // 0 = 치마 맨 아래, 1 = 치마 맨 위(허리)

        // ----- 핵심 변경: 허리 쪽만 고정 -----
        // 예시) 상단 20% 구간은 고정점으로 사용
        if (t > 0.8f)
            vertex.mInvMass = 0.0f;    // 허리 링 -> 고정
        else
            vertex.mInvMass = 1.0f;    // 나머지 -> 소프트바디 자유

        pSettings->mVertices[i] = vertex;
    }

    // --- 2. Faces (삼각형 인덱스) ---
    for (_uint i = 0; i < tCloth.vIndices.size(); i += 3)
    {
        SoftBodySharedSettings::Face face;
        face.mVertex[0] = tCloth.vIndices[i + 0];
        face.mVertex[1] = tCloth.vIndices[i + 1];
        face.mVertex[2] = tCloth.vIndices[i + 2];
        pSettings->AddFace(face);
    }

    // --- 4. VertexAttributes (일반 거리/굽힘 제약만 사용) ---
    SoftBodySharedSettings::VertexAttributes Attr;
    Attr.mCompliance = 0.0f;      // 거리 스프링 딱딱
    Attr.mShearCompliance = 0.0f;      // 대각선 edge도 딱딱
    Attr.mBendCompliance = 0.001f;    // 조금 휘어질 수 있게

    Attr.mLRAType = SoftBodySharedSettings::ELRAType::None;
    Attr.mLRAMaxDistanceMultiplier = 1.0f;

    pSettings->CreateConstraints(&Attr, 1, SoftBodySharedSettings::EBendType::Dihedral);

    pSettings->Optimize();

    return pSettings;
}

Ref<SoftBodySharedSettings> CSoftBody_Creator::CreateClothSoftBody_Settings_independentV2(vector<_float3> vVertexPos, vector<_uint> Indices)
{
    SoftBodySharedSettings* pSettings = new SoftBodySharedSettings;

    const _uint iNumVerts = (_uint)vVertexPos.size();

    // --- 0. 치마 높이 범위 계산 (허리 vs 아래 구분용) ---
    _float fMinY = FLT_MAX;
    _float fMaxY = -FLT_MAX;
    for (_uint i = 0; i < iNumVerts; ++i)
    {
        _float y = vVertexPos[i].y; // 모델 로컬 기준 Y
        fMinY = min(fMinY, y);
        fMaxY = max(fMaxY, y);
    }
    _float fHeight = max(fMaxY - fMinY, 1e-3f);

    // --- 1. Vertices ---
    pSettings->mVertices.resize(iNumVerts);
    for (_uint i = 0; i < iNumVerts; ++i)
    {
        SoftBodySharedSettings::Vertex vertex;
        vertex.mPosition = StoreFloat3(vVertexPos[i]); // 바인드 포즈 위치

        _float y = vVertexPos[i].y;
        _float t = (y - fMinY) / fHeight;   // 0 = 치마 맨 아래, 1 = 치마 맨 위(허리)

        // ----- 핵심 변경: 허리 쪽만 고정 -----
        // 예시) 상단 20% 구간은 고정점으로 사용
        if (t > 0.8f)
            vertex.mInvMass = 0.0f;    // 허리 링 -> 고정
        else
            vertex.mInvMass = 1.0f;    // 나머지 -> 소프트바디 자유

        pSettings->mVertices[i] = vertex;
    }

    // --- 2. Faces (삼각형 인덱스) ---
    for (_uint i = 0; i < Indices.size(); i += 3)
    {
        SoftBodySharedSettings::Face face;
        face.mVertex[0] = Indices[i + 0];
        face.mVertex[1] = Indices[i + 1];
        face.mVertex[2] = Indices[i + 2];
        pSettings->AddFace(face);
    }

    // --- 4. VertexAttributes (일반 거리/굽힘 제약만 사용) ---
    SoftBodySharedSettings::VertexAttributes Attr;
    Attr.mCompliance = 0.0f;      // 거리 스프링 딱딱
    Attr.mShearCompliance = 0.0f;      // 대각선 edge도 딱딱
    Attr.mBendCompliance = 0.001f;    // 조금 휘어질 수 있게

    Attr.mLRAType = SoftBodySharedSettings::ELRAType::None;
    Attr.mLRAMaxDistanceMultiplier = 1.0f;

    pSettings->CreateConstraints(&Attr, 1, SoftBodySharedSettings::EBendType::Dihedral);

    pSettings->Optimize();

    return pSettings;
}

Ref<SoftBodySharedSettings> CSoftBody_Creator::CreateSphere(float inRadius, uint inNumTheta, uint inNumPhi, SoftBodySharedSettings::EBendType inBendType, const SoftBodySharedSettings::VertexAttributes& inVertexAttributes)
{
    // Create settings
    SoftBodySharedSettings* settings = new SoftBodySharedSettings;

    // Create vertices
    // NOTE: This is not how you should create a soft body sphere, we explicitly use polar coordinates to make the vertices unevenly distributed.
    // Doing it this way tests the pressure algorithm as it receives non-uniform triangles. Better is to use uniform triangles,
    // see the use of DebugRenderer::Create8thSphere for an example.
    SoftBodySharedSettings::Vertex v;
    (inRadius * Vec3::sUnitSpherical(0, 0)).StoreFloat3(&v.mPosition);
    settings->mVertices.push_back(v);
    (inRadius * Vec3::sUnitSpherical(JPH_PI, 0)).StoreFloat3(&v.mPosition);
    settings->mVertices.push_back(v);
    for (uint theta = 1; theta < inNumTheta - 1; ++theta)
        for (uint phi = 0; phi < inNumPhi; ++phi)
        {
            (inRadius * Vec3::sUnitSpherical(JPH_PI * theta / (inNumTheta - 1), 2.0f * JPH_PI * phi / inNumPhi)).StoreFloat3(&v.mPosition);
            settings->mVertices.push_back(v);
        }

    // Function to get the vertex index of a point on the sphere
    auto vertex_index = [inNumTheta, inNumPhi](uint inTheta, uint inPhi) -> uint
        {
            if (inTheta == 0)
                return 0;
            else if (inTheta == inNumTheta - 1)
                return 1;
            else
                return 2 + (inTheta - 1) * inNumPhi + inPhi % inNumPhi;
        };

    // Create faces
    SoftBodySharedSettings::Face f;
    for (uint phi = 0; phi < inNumPhi; ++phi)
    {
        for (uint theta = 0; theta < inNumTheta - 2; ++theta)
        {
            f.mVertex[0] = vertex_index(theta, phi);
            f.mVertex[1] = vertex_index(theta + 1, phi);
            f.mVertex[2] = vertex_index(theta + 1, phi + 1);
            settings->AddFace(f);

            if (theta > 0)
            {
                f.mVertex[1] = vertex_index(theta + 1, phi + 1);
                f.mVertex[2] = vertex_index(theta, phi + 1);
                settings->AddFace(f);
            }
        }

        f.mVertex[0] = vertex_index(inNumTheta - 2, phi + 1);
        f.mVertex[1] = vertex_index(inNumTheta - 2, phi);
        f.mVertex[2] = vertex_index(inNumTheta - 1, 0);
        settings->AddFace(f);
    }

    // Create constraints
    settings->CreateConstraints(&inVertexAttributes, 1, inBendType);

    // Optimize the settings
    settings->Optimize();

    return settings;
}
