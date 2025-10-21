#ifndef Engine_Json_h__
#define Engine_Json_h__

#include "Json/json.hpp"
using JSON = nlohmann::json;

#include <fstream>

namespace Engine
{

#pragma region BASE

    typedef struct tagInt2
    {
        int x;
        int y;
    }INT2_DATA;

    typedef struct tagInt3
    {
        int x;
        int y;
        int z;
    }INT3_DATA;

    typedef struct tagInt4
    {
        int x;
        int y;
        int z;
        int w;
    }INT4_DATA;

    typedef struct tagUInt2
    {
        unsigned int x;
        unsigned int y;
    }UINT2_DATA;

    typedef struct tagUInt3
    {
        unsigned int x;
        unsigned int y;
        unsigned int z;
    }UINT3_DATA;

    typedef struct tagUInt4
    {
        unsigned int x;
        unsigned int y;
        unsigned int z;
        unsigned int w;
    }UINT4_DATA;

    typedef struct tagFloat2
    {
        float x;
        float y;

        tagFloat2() : x(0.f), y(0.f){}
        tagFloat2(float _x, float _y) : x(_x), y(_y) {}
    }FLOAT2_DATA;

    typedef struct tagFloat3
    {
        float x;
        float y;
        float z;

        tagFloat3() : x(0.f), y(0.f), z(0.f) {}
        tagFloat3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    }FLOAT3_DATA;

    typedef struct tagFloat4
    {
        float x;
        float y;
        float z;
        float w;

        tagFloat4() : x(0.f), y(0.f), z(0.f) {}
        tagFloat4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    }FLOAT4_DATA;

    typedef struct tagFloat4x4
    {
        FLOAT4_DATA m1;
        FLOAT4_DATA m2;
        FLOAT4_DATA m3;
        FLOAT4_DATA m4;

        FLOAT4_DATA& operator[](int index) { return *(&m1 + index); }
        const FLOAT4_DATA& operator[](int index) const { return *(&m1 + index); }

        void SaveBinary(std::ofstream& ofs) const { ofs.write((char*)this, sizeof(*this)); }
        void LoadBinary(std::ifstream& ifs) { ifs.read((char*)this, sizeof(*this)); }

    }FLOAT4X4_DATA;

#pragma endregion

#pragma region Animation

    typedef struct tagVertexMeshSet
    {
        FLOAT3_DATA					position;
        FLOAT3_DATA					normal;
        FLOAT3_DATA					tangent;
        FLOAT3_DATA					binormal;
        FLOAT2_DATA					texcoord;

        //ANIM
        UINT4_DATA					blendIndex;
        FLOAT4_DATA					blendWeight;

        void SaveBinary(std::ofstream& ofs) const { ofs.write((char*)this, sizeof(*this)); }
        void LoadBinary(std::ifstream& ifs) { ifs.read((char*)this, sizeof(*this)); }

    }MESH_VERTEX_DATA;


    typedef struct tagKeyFrameDataSet
    {
        FLOAT3_DATA  scale;
        FLOAT4_DATA  rotation;
        FLOAT3_DATA  translation;
        float		 trackPostion;

        void SaveBinary(std::ofstream& ofs) const { ofs.write((char*)this, sizeof(*this)); }
        void LoadBinary(std::ifstream& ifs) { ifs.read((char*)this, sizeof(*this)); }

    }KEYFRAME_DATA;

    typedef struct tagMeshDataSet
    {
        unsigned int				iMaterialIndex;
        unsigned int				iNumVertices;
        unsigned int				iVertexStride;
        unsigned int				iNumIndices;
        unsigned int				iIndexStride;
        unsigned int				iNumVertexBuffers;
        unsigned int				iIndexFormat;
        unsigned int				iPrimitiveType;
        unsigned int				iNumFace;

        //ANIM
        std::string					strName;
        unsigned int				iNumBones;
        std::vector<int>			vecBoneIndices;
        std::vector<FLOAT4X4_DATA>  vecOffsetMatrices;

        std::vector<Engine::MESH_VERTEX_DATA>		vecVertices;
        std::vector<Engine::UINT3_DATA>		vecIndices;

        void SaveBinary(std::ofstream& ofs) const
        {
            auto write_string = [&](const std::string& s) {
                uint32_t len = static_cast<uint32_t>(s.size());
                ofs.write((char*)&len, sizeof(len));
                ofs.write(s.data(), len);
                };
            auto write_vec = [&](auto& vec) {
                uint32_t count = static_cast<uint32_t>(vec.size());
                ofs.write((char*)&count, sizeof(count));
                for (auto& e : vec) e.SaveBinary(ofs);
                };

            ofs.write((char*)&iMaterialIndex, sizeof(iMaterialIndex));
            ofs.write((char*)&iNumVertices, sizeof(iNumVertices));
            ofs.write((char*)&iVertexStride, sizeof(iVertexStride));
            ofs.write((char*)&iNumIndices, sizeof(iNumIndices));
            ofs.write((char*)&iIndexStride, sizeof(iIndexStride));
            ofs.write((char*)&iNumVertexBuffers, sizeof(iNumVertexBuffers));
            ofs.write((char*)&iIndexFormat, sizeof(iIndexFormat));
            ofs.write((char*)&iPrimitiveType, sizeof(iPrimitiveType));
            ofs.write((char*)&iNumFace, sizeof(iNumFace));
            write_string(strName);
            ofs.write((char*)&iNumBones, sizeof(iNumBones));

            uint32_t count = static_cast<uint32_t>(vecBoneIndices.size());
            ofs.write((char*)&count, sizeof(count));
            for (int idx : vecBoneIndices)
                ofs.write((char*)&idx, sizeof(int));

            count = static_cast<uint32_t>(vecOffsetMatrices.size());
            ofs.write((char*)&count, sizeof(count));
            for (auto& m : vecOffsetMatrices)
                m.SaveBinary(ofs);

            count = static_cast<uint32_t>(vecVertices.size());
            ofs.write((char*)&count, sizeof(count));
            for (auto& v : vecVertices)
                v.SaveBinary(ofs);

            count = static_cast<uint32_t>(vecIndices.size());
            ofs.write((char*)&count, sizeof(count));
            for (auto& i : vecIndices)
                ofs.write((char*)&i, sizeof(UINT3_DATA));
        }
        void LoadBinary(std::ifstream& ifs)
        {
            auto read_string = [&]() -> std::string {
                uint32_t len;
                ifs.read((char*)&len, sizeof(len));
                std::string s(static_cast<size_t>(len), '\0');
                ifs.read(&s[0], len);
                return s;
                };

            ifs.read((char*)&iMaterialIndex, sizeof(iMaterialIndex));
            ifs.read((char*)&iNumVertices, sizeof(iNumVertices));
            ifs.read((char*)&iVertexStride, sizeof(iVertexStride));
            ifs.read((char*)&iNumIndices, sizeof(iNumIndices));
            ifs.read((char*)&iIndexStride, sizeof(iIndexStride));
            ifs.read((char*)&iNumVertexBuffers, sizeof(iNumVertexBuffers));
            ifs.read((char*)&iIndexFormat, sizeof(iIndexFormat));
            ifs.read((char*)&iPrimitiveType, sizeof(iPrimitiveType));
            ifs.read((char*)&iNumFace, sizeof(iNumFace));

            strName = read_string();
            ifs.read((char*)&iNumBones, sizeof(iNumBones));

            uint32_t count;
            ifs.read((char*)&count, sizeof(count));
            vecBoneIndices.resize(static_cast<size_t>(count));
            for (auto& idx : vecBoneIndices)
                ifs.read((char*)&idx, sizeof(int));

            ifs.read((char*)&count, sizeof(count));
            vecOffsetMatrices.resize(static_cast<size_t>(count));
            for (auto& m : vecOffsetMatrices)
                m.LoadBinary(ifs);

            ifs.read((char*)&count, sizeof(count));
            vecVertices.resize(static_cast<size_t>(count));
            for (auto& v : vecVertices)
                v.LoadBinary(ifs);

            ifs.read((char*)&count, sizeof(count));
            vecIndices.resize(static_cast<size_t>(count));
            for (auto& i : vecIndices)
                ifs.read((char*)&i, sizeof(UINT3_DATA));
        }

    }MESH_DATA;

    typedef struct tagMaterialDataSet
    {
        std::vector<unsigned int>					iNumTextures;

        std::vector<std::vector<std::string>>		vecExts;
        std::vector<std::vector<std::string>>		vecFullPaths;

        void SaveBinary(std::ofstream& ofs) const
        {
            auto write_string = [&](const std::string& s) {
                uint32_t len = static_cast<uint32_t>(s.size());
                ofs.write((char*)&len, sizeof(len));
                ofs.write(s.data(), len);
                };
            auto write_vec = [&](auto& vec, auto&& write_elem) {
                uint32_t count = static_cast<uint32_t>(vec.size());
                ofs.write((char*)&count, sizeof(count));
                for (auto& e : vec) write_elem(e);
                };

            write_vec(iNumTextures, [&](const unsigned int& v) {
                ofs.write((char*)&v, sizeof(unsigned int));
                });
            write_vec(vecExts, [&](auto& extList) {
                write_vec(extList, write_string);
                });
            write_vec(vecFullPaths, [&](auto& pathList) {
                write_vec(pathList, write_string);
                });
        }
        void LoadBinary(std::ifstream& ifs)
        {
            auto read_string = [&]() -> std::string {
                uint32_t len;
                ifs.read((char*)&len, sizeof(len));
                std::string s(static_cast<size_t>(len), '\0');
                ifs.read(&s[0], len);
                return s;
                };

            uint32_t count;
            ifs.read((char*)&count, sizeof(count));
            iNumTextures.resize(static_cast<size_t>(count));
            for (auto& v : iNumTextures)
                ifs.read((char*)&v, sizeof(unsigned int));

            ifs.read((char*)&count, sizeof(count));
            vecExts.resize(static_cast<size_t>(count));
            for (auto& extList : vecExts)
            {
                uint32_t subCount;
                ifs.read((char*)&subCount, sizeof(subCount));
                extList.resize(static_cast<size_t>(subCount));
                for (auto& ext : extList)
                    ext = read_string();
            }

            ifs.read((char*)&count, sizeof(count));
            vecFullPaths.resize(static_cast<size_t>(count));
            for (auto& pathList : vecFullPaths)
            {
                uint32_t subCount;
                ifs.read((char*)&subCount, sizeof(subCount));
                pathList.resize(static_cast<size_t>(subCount));
                for (auto& path : pathList)
                    path = read_string();
            }
        }

    }MATERIAL_DATA;

    typedef struct tagBoneDataSet
    {
        int				iParentBoneIndex;
        std::string			strName;
        FLOAT4X4_DATA	transformationMatrix;

        void SaveBinary(std::ofstream& ofs) const
        {
            uint32_t len = static_cast<uint32_t>(strName.size());
            ofs.write((char*)&iParentBoneIndex, sizeof(iParentBoneIndex));
            ofs.write((char*)&len, sizeof(len));
            ofs.write(strName.data(), len);
            transformationMatrix.SaveBinary(ofs);
        }
        void LoadBinary(std::ifstream& ifs)
        {
            uint32_t len;
            ifs.read((char*)&iParentBoneIndex, sizeof(iParentBoneIndex));
            ifs.read((char*)&len, sizeof(len));
            strName.resize(static_cast<size_t>(len));
            ifs.read(&strName[0], len);
            transformationMatrix.LoadBinary(ifs);
        }

    }BONE_DATA;

    typedef struct tagChannelDataSet
    {
        std::string			strName;
        unsigned int    iBoneIndex;
        unsigned int    iNumKeyFrame;

        std::vector< KEYFRAME_DATA> vecKeyFrames;

        void SaveBinary(std::ofstream& ofs) const
        {
            uint32_t len = static_cast<uint32_t>(strName.size());
            ofs.write((char*)&len, sizeof(len));
            ofs.write(strName.data(), len);
            ofs.write((char*)&iBoneIndex, sizeof(iBoneIndex));
            ofs.write((char*)&iNumKeyFrame, sizeof(iNumKeyFrame));

            uint32_t count = static_cast<uint32_t>(vecKeyFrames.size());
            ofs.write((char*)&count, sizeof(count));
            for (auto& key : vecKeyFrames)
                key.SaveBinary(ofs);
        }
        void LoadBinary(std::ifstream& ifs)
        {
            uint32_t len;
            ifs.read((char*)&len, sizeof(len));
            strName.resize(static_cast<size_t>(len));
            ifs.read(&strName[0], len);

            ifs.read((char*)&iBoneIndex, sizeof(iBoneIndex));
            ifs.read((char*)&iNumKeyFrame, sizeof(iNumKeyFrame));

            uint32_t count;
            ifs.read((char*)&count, sizeof(count));
            vecKeyFrames.resize(static_cast<size_t>(count));
            for (auto& key : vecKeyFrames)
                key.LoadBinary(ifs);
        }

    }CHANNEL_DATA;

    // 애니메이션 세트 데이터 (MODEL_DATA가 관리)
    typedef struct tagAnimationSetData
    {
        std::string             strAnimSetName = { "DefaultAnimSet" };      // 애니메이션 세트 이름
        std::vector<int>        vecAnimIndices;                             // 세트에 포함된 애니메이션 인덱스들 (실행 순서대로)

        void SaveBinary(std::ofstream& ofs) const
        {
            auto write_string = [&](const std::string& s) {
                uint32_t len = static_cast<uint32_t>(s.size());
                ofs.write((char*)&len, sizeof(len));
                ofs.write(s.data(), len);
                };

            write_string(strAnimSetName);

            uint32_t count = static_cast<uint32_t>(vecAnimIndices.size());
            ofs.write((char*)&count, sizeof(count));
            for (int idx : vecAnimIndices)
                ofs.write((char*)&idx, sizeof(int));
        }
        void LoadBinary(std::ifstream& ifs)
        {
            auto read_string = [&]() -> std::string {
                uint32_t len;
                ifs.read((char*)&len, sizeof(len));
                std::string s(static_cast<size_t>(len), '\0');
                ifs.read(&s[0], len);
                return s;
                };

            strAnimSetName = read_string();

            uint32_t count;
            ifs.read((char*)&count, sizeof(count));
            vecAnimIndices.resize(static_cast<size_t>(count));
            for (auto& idx : vecAnimIndices)
                ifs.read((char*)&idx, sizeof(int));
        }

    }ANIMATION_SET_DATA;

    // 개별 애니메이션 설정 (각 애니메이션이 관리)
    typedef struct tagAnimationSetUp
    {
        /* 기본 설정 */
        std::string             strName = { "DefaultAnim" };
        bool                    isLoop = { false };
        unsigned int            iDirection = { 0 };                         // 방향 ANIM_DIRECTION

        /* 애니메이션 전환 설정 (애니메이션 세트에 사용할 예정) */
        unsigned int            iTransitionType = { 0 };                    // 전환 조건 (0:Auto, 1:Flag, 2:Input, 3:Manual)
        bool                    isWaitForComplete = { false };               // 완료 대기 여부 true: 전환할 때 애니메이션이 다 끝나고 전환하겠다. / false : 바로 전환하겠다.
        float                   fBlendOutTime = { 0.15f };                  // 블렌드 아웃 시간
        float                   fBlendInTime = { 0.15f };                   // 블렌드 인 시간

        /* 루트 모션 */
        bool                    isRootMotion = { true };                   // 루트 모션 사용 여부
        bool                    isApplyRootRotation = { false };            // 회전 적용 여부
        bool                    isApplyRootPosition = { true };            // 위치 적용 여부
        FLOAT3_DATA             RootMitionScale = FLOAT3_DATA(1.f,1.f,1.f);  // 축별 적용 스케일

        /* 이벤트 */
        bool                    isEvent = { false };                        // 이벤트 존재 여부
        //bool                  isTriggerOnce = { false };       =1<<0      // 루프 시 한 번만 발동
        //bool					isTriggerOnEnter = { true };     =1<<1      // 범위 진입 시 발동
        //bool					isTriggerOnExit = { false };     =1<<2      // 범위 탈출 시 발동
        //bool					isTriggerContinuous = { false }; =1<<3      // 범위 내에서 계속 발동
        std::vector<unsigned int>   vecEventTriggers;                       // 이벤트 발동 조건 모음
        std::vector<FLOAT2_DATA>    vecEventFrames;                         // 이벤트 프레임 (x: start, y: end)
        std::vector<std::string>    vecEventKeys;                           // 이벤트 키

        void SaveBinary(std::ofstream& ofs) const
        {
            auto write_string = [&](const std::string& s) {
                uint32_t len = static_cast<uint32_t>(s.size());
                ofs.write((char*)&len, sizeof(len));
                ofs.write(s.data(), len);
                };

            // 기본 설정
            write_string(strName);
            ofs.write((char*)&isLoop, sizeof(isLoop));
            ofs.write((char*)&iDirection, sizeof(iDirection));

            // 애니메이션 전환 설정
            ofs.write((char*)&iTransitionType, sizeof(iTransitionType));
            ofs.write((char*)&isWaitForComplete, sizeof(isWaitForComplete));
            ofs.write((char*)&fBlendOutTime, sizeof(fBlendOutTime));
            ofs.write((char*)&fBlendInTime, sizeof(fBlendInTime));

            // 루트 모션
            ofs.write((char*)&isRootMotion, sizeof(isRootMotion));
            ofs.write((char*)&isApplyRootRotation, sizeof(isApplyRootRotation));
            ofs.write((char*)&isApplyRootPosition, sizeof(isApplyRootPosition));
            ofs.write((char*)&RootMitionScale, sizeof(FLOAT3_DATA));

            // 이벤트
            ofs.write((char*)&isEvent, sizeof(isEvent));
            //ofs.write((char*)&isTriggerOnce, sizeof(isTriggerOnce));
            //ofs.write((char*)&isTriggerOnEnter, sizeof(isTriggerOnEnter));
            //ofs.write((char*)&isTriggerOnExit, sizeof(isTriggerOnExit));
            //ofs.write((char*)&isTriggerContinuous, sizeof(isTriggerContinuous));
            uint32_t count = static_cast<uint32_t>(vecEventTriggers.size());
            ofs.write((char*)&count, sizeof(count));
            for (const auto& frame : vecEventTriggers)
            {
                ofs.write((char*)&frame, sizeof(unsigned int));
            }

            count = static_cast<uint32_t>(vecEventFrames.size());
            ofs.write((char*)&count, sizeof(count));
            for (const auto& frame : vecEventFrames)
            {
                ofs.write((char*)&frame, sizeof(FLOAT2_DATA));
            }

            count = static_cast<uint32_t>(vecEventKeys.size());
            ofs.write((char*)&count, sizeof(count));
            for (const auto& key : vecEventKeys)
            {
                write_string(key);
            }
        }
        void LoadBinary(std::ifstream& ifs)
        {
            auto read_string = [&]() -> std::string {
                uint32_t len;
                ifs.read((char*)&len, sizeof(len));
                std::string s(static_cast<size_t>(len), '\0');
                ifs.read(&s[0], len);
                return s;
                };

            // 기본 설정
            strName = read_string();
            ifs.read((char*)&isLoop, sizeof(isLoop));
            ifs.read((char*)&iDirection, sizeof(iDirection));

            // 애니메이션 전환 설정
            ifs.read((char*)&iTransitionType, sizeof(iTransitionType));
            ifs.read((char*)&isWaitForComplete, sizeof(isWaitForComplete));
            ifs.read((char*)&fBlendOutTime, sizeof(fBlendOutTime));
            ifs.read((char*)&fBlendInTime, sizeof(fBlendInTime));

            // 루트 모션
            ifs.read((char*)&isRootMotion, sizeof(isRootMotion));
            ifs.read((char*)&isApplyRootRotation, sizeof(isApplyRootRotation));
            ifs.read((char*)&isApplyRootPosition, sizeof(isApplyRootPosition));
            ifs.read((char*)&RootMitionScale, sizeof(FLOAT3_DATA));

            // 이벤트
            ifs.read((char*)&isEvent, sizeof(isEvent));
            //ifs.read((char*)&isTriggerOnce, sizeof(isTriggerOnce));
            //ifs.read((char*)&isTriggerOnEnter, sizeof(isTriggerOnEnter));
            //ifs.read((char*)&isTriggerOnExit, sizeof(isTriggerOnExit));
            //ifs.read((char*)&isTriggerContinuous, sizeof(isTriggerContinuous));

            uint32_t count;

            ifs.read((char*)&count, sizeof(count));
            vecEventTriggers.resize(static_cast<size_t>(count));
            for (auto& frame : vecEventTriggers)
            {
                ifs.read((char*)&frame, sizeof(unsigned int));
            }

            ifs.read((char*)&count, sizeof(count));
            vecEventFrames.resize(static_cast<size_t>(count));
            for (auto& frame : vecEventFrames)
            {
                ifs.read((char*)&frame, sizeof(FLOAT2_DATA));
            }

            ifs.read((char*)&count, sizeof(count));
            vecEventKeys.resize(static_cast<size_t>(count));
            for (auto& key : vecEventKeys)
            {
                key = read_string();
            }
        }
    }ANIMATION_SETUP_DATA;

    typedef struct tagAnimationDataSet
    {
        std::string		strName;
        float			fDuration;
        float			fTickPerSecond;
        float           fAnimationBlendTime = { 0.25f };
        unsigned int    iNumChannels;

        std::vector< CHANNEL_DATA> vecChannels;

        ANIMATION_SETUP_DATA    animSetup;

        void SaveBinary(std::ofstream& ofs) const
        {
            uint32_t len = static_cast<uint32_t>(strName.size());
            ofs.write((char*)&len, sizeof(len));
            ofs.write(strName.data(), len);
            ofs.write((char*)&fDuration, sizeof(fDuration));
            ofs.write((char*)&fTickPerSecond, sizeof(fTickPerSecond));
            ofs.write((char*)&fAnimationBlendTime, sizeof(fAnimationBlendTime));
            ofs.write((char*)&iNumChannels, sizeof(iNumChannels));

            uint32_t count = static_cast<uint32_t>(vecChannels.size());
            ofs.write((char*)&count, sizeof(count));
            for (auto& ch : vecChannels)
                ch.SaveBinary(ofs);
            animSetup.SaveBinary(ofs);
        }
        void LoadBinary(std::ifstream& ifs)
        {
            uint32_t len;
            ifs.read((char*)&len, sizeof(len));
            strName.resize(static_cast<size_t>(len));
            ifs.read(&strName[0], len);

            ifs.read((char*)&fDuration, sizeof(fDuration));
            ifs.read((char*)&fTickPerSecond, sizeof(fTickPerSecond));
            ifs.read((char*)&fAnimationBlendTime, sizeof(fAnimationBlendTime));
            ifs.read((char*)&iNumChannels, sizeof(iNumChannels));

            uint32_t count;
            ifs.read((char*)&count, sizeof(count));
            vecChannels.resize(static_cast<size_t>(count));
            for (auto& ch : vecChannels)
                ch.LoadBinary(ifs);
            animSetup.LoadBinary(ifs);
        }

    }ANIMATION_DATA;

    typedef struct tagAnimationSummaryDataSet
    {
        std::string		strName;
        float			fDuration;
        float			fTickPerSecond;
        unsigned int    iNumChannels;
        ANIMATION_SETUP_DATA    animSetup;
    }ANIMATION_SUMMARY_DATA;

    typedef struct tagAnimationSummariesDataSet
    {
        std::vector<ANIMATION_SUMMARY_DATA> vecSummaries;
        std::vector<Engine::ANIMATION_SET_DATA>     vecAnimationSets;  // 애니메이션 세트 (추가됨)

    }ANIMATION_SUMMARIES_DATA;

    typedef struct tagModelDataSet
    {
        std::string					strModelFilePath;
        std::string					strModelName;
        unsigned int				iModelType;
        unsigned int				iNumMeshes;
        unsigned int				iNumMaterials;

        //ANIM
        unsigned int				iNumAnimations;
        FLOAT4X4_DATA				vPreTransformMatrix;

        std::vector<Engine::MESH_DATA>			    vecMeshes;
        std::vector<Engine::MATERIAL_DATA>		    vecMaterials;
        std::vector<Engine::BONE_DATA>			    vecBones;

        //ANIM
        std::vector<Engine::ANIMATION_DATA>		    vecAnimation;
        std::vector<Engine::ANIMATION_SET_DATA>     vecAnimationSets;  // 애니메이션 세트 (추가됨)

        ~tagModelDataSet() {
            vecMeshes.clear();
            vecMaterials.clear();
            vecBones.clear();
            vecAnimation.clear();
            vecAnimationSets.clear();
        };

        void SaveBinary(std::ofstream& ofs) const
        {
            auto write_string = [&](const std::string& s) {
                uint32_t len = static_cast<uint32_t>(s.size());
                ofs.write((char*)&len, sizeof(len));
                ofs.write(s.data(), len);
                };

            write_string(strModelFilePath);
            write_string(strModelName);
            ofs.write((char*)&iModelType, sizeof(iModelType));
            ofs.write((char*)&iNumMeshes, sizeof(iNumMeshes));
            ofs.write((char*)&iNumMaterials, sizeof(iNumMaterials));
            ofs.write((char*)&iNumAnimations, sizeof(iNumAnimations));
            vPreTransformMatrix.SaveBinary(ofs);

            uint32_t count = static_cast<uint32_t>(vecMeshes.size());
            ofs.write((char*)&count, sizeof(count));
            for (auto& mesh : vecMeshes)
                mesh.SaveBinary(ofs);

            count = static_cast<uint32_t>(vecMaterials.size());
            ofs.write((char*)&count, sizeof(count));
            for (auto& mat : vecMaterials)
                mat.SaveBinary(ofs);

            count = static_cast<uint32_t>(vecBones.size());
            ofs.write((char*)&count, sizeof(count));
            for (auto& bone : vecBones)
                bone.SaveBinary(ofs);

            count = static_cast<uint32_t>(vecAnimation.size());
            ofs.write((char*)&count, sizeof(count));
            for (auto& anim : vecAnimation)
                anim.SaveBinary(ofs);

            count = static_cast<uint32_t>(vecAnimationSets.size());
            ofs.write((char*)&count, sizeof(count));
            for (auto& animSet : vecAnimationSets)
                animSet.SaveBinary(ofs);
        }
        void LoadBinary(std::ifstream& ifs)
        {
            auto read_string = [&]() -> std::string {
                uint32_t len;
                ifs.read((char*)&len, sizeof(len));
                std::string s(static_cast<size_t>(len), '\0');
                ifs.read(&s[0], len);
                return s;
                };

            strModelFilePath = read_string();
            strModelName = read_string();
            ifs.read((char*)&iModelType, sizeof(iModelType));
            ifs.read((char*)&iNumMeshes, sizeof(iNumMeshes));
            ifs.read((char*)&iNumMaterials, sizeof(iNumMaterials));
            ifs.read((char*)&iNumAnimations, sizeof(iNumAnimations));
            vPreTransformMatrix.LoadBinary(ifs);

            uint32_t count;
            ifs.read((char*)&count, sizeof(count));
            vecMeshes.resize(static_cast<size_t>(count));
            for (auto& mesh : vecMeshes)
                mesh.LoadBinary(ifs);

            ifs.read((char*)&count, sizeof(count));
            vecMaterials.resize(static_cast<size_t>(count));
            for (auto& mat : vecMaterials)
                mat.LoadBinary(ifs);

            ifs.read((char*)&count, sizeof(count));
            vecBones.resize(static_cast<size_t>(count));
            for (auto& bone : vecBones)
                bone.LoadBinary(ifs);

            ifs.read((char*)&count, sizeof(count));
            vecAnimation.resize(static_cast<size_t>(count));
            for (auto& anim : vecAnimation)
                anim.LoadBinary(ifs);

            ifs.read((char*)&count, sizeof(count));
            vecAnimationSets.resize(static_cast<size_t>(count));
            for (auto& animSet : vecAnimationSets)
                animSet.LoadBinary(ifs);
        }

    }MODEL_DATA;


#pragma endregion



#pragma region Effect

#pragma endregion



#pragma region Map

    typedef struct tagJsonMapDataSet
    {
        std::string strModelName{};

        unsigned int iNumInstances{};

        bool isInstance = { false };
        std::vector<FLOAT3_DATA> vInstancePosition;
        std::vector<FLOAT3_DATA> vInstanceScale;
        std::vector<FLOAT4_DATA> vInstanceQuaternion;

        bool isObject = { false };
        FLOAT3_DATA vPosition{};
        FLOAT3_DATA vScale{};
        FLOAT3_DATA vRotation{};

    }JSON_MAP_DATA;

    typedef struct tagJsonPrototypeDataSet
    {
        std::string PrototypeTag{};
        std::string FileName{};
        std::string FilePath{};

    }JSON_MAP_PROTOTYPE_DATA;

#pragma endregion



#pragma region UI

#pragma endregion


#pragma region Serialization / Deserialization

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(INT2_DATA, x, y);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(INT3_DATA, x, y, z);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(INT4_DATA, x, y, z, w);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UINT2_DATA, x, y);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UINT3_DATA, x, y, z);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UINT4_DATA, x, y, z, w);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FLOAT2_DATA, x, y);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FLOAT3_DATA, x, y, z);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FLOAT4_DATA, x, y, z, w);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FLOAT4X4_DATA, m1, m2, m3, m4);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MESH_VERTEX_DATA, position, normal, tangent, binormal, texcoord, blendIndex, blendWeight);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(KEYFRAME_DATA, scale, rotation, translation, trackPostion);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MESH_DATA,
        iMaterialIndex,
        iNumVertices,
        iVertexStride,
        iNumIndices,
        iIndexStride,
        iNumVertexBuffers,
        iIndexFormat,
        iPrimitiveType,
        iNumFace,
        strName,
        iNumBones,
        vecBoneIndices,
        vecOffsetMatrices,
        vecVertices,
        vecIndices
    );
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MATERIAL_DATA, iNumTextures, vecExts, vecFullPaths);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BONE_DATA, iParentBoneIndex, strName, transformationMatrix);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CHANNEL_DATA, strName, iBoneIndex, iNumKeyFrame, vecKeyFrames);

    // 애니메이션 세트 직렬화
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ANIMATION_SET_DATA,
        strAnimSetName,
        vecAnimIndices
    );

    // 애니메이션 설정 직렬화
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ANIMATION_SETUP_DATA,
        strName,
        isLoop,
        iDirection,
        iTransitionType,
        isWaitForComplete,
        fBlendOutTime,
        fBlendInTime,
        isRootMotion,
        isApplyRootRotation,
        isApplyRootPosition,
        RootMitionScale,
        isEvent,
        //isTriggerOnce,
        //isTriggerOnEnter,
        //isTriggerOnExit,
        //isTriggerContinuous,
        vecEventTriggers,
        vecEventFrames,
        vecEventKeys
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ANIMATION_DATA, strName, fDuration, fTickPerSecond, iNumChannels, vecChannels, animSetup);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ANIMATION_SUMMARY_DATA, strName, fDuration, fTickPerSecond, iNumChannels, animSetup);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ANIMATION_SUMMARIES_DATA, vecSummaries, vecAnimationSets);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MODEL_DATA,
        strModelFilePath,
        strModelName,
        iModelType,
        iNumMeshes,
        iNumMaterials,
        iNumAnimations,
        vPreTransformMatrix,
        vecMeshes,
        vecMaterials,
        vecBones,
        vecAnimation,
        vecAnimationSets
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JSON_MAP_DATA,
        strModelName,
        iNumInstances,
        isInstance,
        vInstancePosition,
        vInstanceScale,
        vInstanceQuaternion,
        isObject,
        vPosition,
        vScale,
        vRotation
    );
    // 프로토타입 관련 태그, 이름, 경로 직렬화
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JSON_MAP_PROTOTYPE_DATA, PrototypeTag, FileName, FilePath);

#pragma endregion


}

#endif // Engine_Json_h__