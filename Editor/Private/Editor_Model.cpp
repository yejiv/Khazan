#include "GameInstance.h"
#include "Editor_Model.h"
#include "Editor_Mesh.h"
#include "Editor_MeshMaterial.h"
#include "Editor_Bone.h"
#include "Editor_Animation.h"

#include "Shader.h"

#include <regex>
#include <fstream>
#include <filesystem>
#include "../../Engine/Public/assimp/postprocess.h"


CEditor_Model::CEditor_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice ,pContext }
{
}


CEditor_Model::CEditor_Model(const CEditor_Model& Prototype)
    : CComponent{ Prototype }
    , m_eModelType{ Prototype.m_eModelType }
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
    , m_pModelFilePath{ Prototype.m_pModelFilePath }
    , m_Model_Data(Prototype.m_Model_Data)

    , m_iNumMeshes{ Prototype.m_iNumMeshes }
    , m_Meshes{ Prototype.m_Meshes }

    , m_iNumMaterials{ Prototype.m_iNumMaterials }
    , m_Materials{ Prototype.m_Materials }

   // , m_Bones{ Prototype.m_Bones }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
    , m_iRootBoneIndex{Prototype.m_iRootBoneIndex }
    , m_fRootMotionBlendTime{Prototype.m_fRootMotionBlendTime }

{
    for (auto& pPrototypeAnimation : Prototype.m_Animations) {
        CEditor_Animation* pAnimation = pPrototypeAnimation->Clone();
        m_Animations.push_back(pAnimation);
        pAnimation->Set_TrackPositionPtr(&m_fCurrentTrackPosition);
    }

    for (auto& pPrototypeBone : Prototype.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());

    for (auto& pAinmation : m_Animations)
        Safe_AddRef(pAinmation);

    for (auto& pBone : m_Bones)
        Safe_AddRef(pBone);

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    for (auto& pMaterial : m_Materials)
        Safe_AddRef(pMaterial);


}

HRESULT CEditor_Model::Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    m_eModelType = eModelType;
    m_pModelFilePath = pModelFilePath;
    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    string filePath(pModelFilePath);
    _bool isGLTF = (filePath.substr(filePath.find_last_of(".") + 1) == "gltf");

    _uint iFlag = 0;

    if (isGLTF)
        iFlag = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded;
    else
        iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;


    if (m_eModelType == MODELTYPE::NONANIM)
        iFlag |= aiProcess_PreTransformVertices;

    if (isGLTF)
    {
        // GLTF의 빈 본 제거 방지 (GLTF는 본 구조가 중요함)
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_REMOVE_EMPTY_BONES, false);
        // GLTF의 피벗 포인트 보존
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    }

    _char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    OutputDebugStringA(("[Current Working Directory] " + string(currentDir) + "\n").c_str());
    OutputDebugStringA(("[Model Path] " + filePath + "\n").c_str());

	if (filesystem::exists(filePath))
	{
		string message = "Model file exists: " + filePath + "\n";
		OutputDebugStringA(message.c_str());
	}
	else
	{
		string error = "Model file does not exist: " + filePath + "\n";
		OutputDebugStringA(error.c_str());
		return E_FAIL;
	}

    m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
    if (nullptr == m_pAIScene)
    {
        string error = "Failed to load model: " + string(m_Importer.GetErrorString())+ "\n";
        OutputDebugStringA(error.c_str());
        return E_FAIL;
    }

    //string message = " ------------------------------------------------------------------\n";
    //OutputDebugStringA(message.c_str());

    //message = "Model loaded: " + std::string(pModelFilePath) + "\n";
    //OutputDebugStringA(message.c_str());

    //message = +"Root node: " + std::string(m_pAIScene->mRootNode->mName.data) + "\n";
    //OutputDebugStringA(message.c_str());

    //message = "Meshes: " + std::to_string(m_pAIScene->mNumMeshes) + "\n";
    //OutputDebugStringA(message.c_str());

    //message = "Materials: " + std::to_string(m_pAIScene->mNumMaterials) + "\n";
    //OutputDebugStringA(message.c_str());

    //message = "Animations: " + std::to_string(m_pAIScene->mNumAnimations) + "\n";
    //OutputDebugStringA(message.c_str());


    if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
        return E_FAIL;

    if (FAILED(Ready_Meshes()))
        return E_FAIL;

    if (FAILED(Ready_Materials()))
        return E_FAIL;

    if (FAILED(Ready_Animation()))
        return E_FAIL;


    _char   szName[MAX_PATH];
    _splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);
    m_Model_Data.strModelName = static_cast<string>(szName);
    m_Model_Data.strModelFilePath = pModelFilePath;
    m_Model_Data.iModelType = static_cast<_uint>(eModelType);
    m_Model_Data.iNumMeshes = m_iNumMeshes;
    m_Model_Data.iNumMaterials = m_iNumMaterials;
    m_Model_Data.iNumAnimations = m_iNumAnimations;

    memcpy(&m_Model_Data.vPreTransformMatrix, &m_PreTransformMatrix, sizeof(_float4x4));

    if (m_eModelType == MODELTYPE::ANIM)
    {
        auto it = find_if(m_Bones.begin(), m_Bones.end(),
            [&](CEditor_Bone* pBone) {
                if (pBone->Compare_Name("Root")|| pBone->Compare_Name("Bip001"))
                    return true;
                return false;
            });

        if (it == m_Bones.end())
        {
            OutputDebugStringA(("!!!!!!!!1!!!!!!! 루트본 못 찾음!!!!!!!!!!!!!!!!!!!!!!!!!"));
        }

        m_iRootBoneIndex = static_cast<_uint>(distance(m_Bones.begin(), it));
        OutputDebugStringA(("[Root Boon Index] : " + to_string(m_iRootBoneIndex) + "\n").c_str());

    }

    return S_OK;
}

HRESULT CEditor_Model::Initialize_Clone(void* pArg)
{

    return S_OK;
}

HRESULT CEditor_Model::Render(_uint iMeshIndex)
{
    if (FAILED(m_Meshes[iMeshIndex]->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEditor_Model::Bind_Materials(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint iTextureType, _uint iTextureIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (m_iNumMaterials <= iMaterialIndex)
		return E_FAIL;
	m_Materials[iMaterialIndex]->Bind_Resources(pShader, pConstantName, iTextureType, iTextureIndex);

	return   S_OK;
}

HRESULT CEditor_Model::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;
	m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);

	return S_OK;
}

_bool CEditor_Model::Play_Animation(_float fTimeDelta)
{
    m_isFinished = false;

    if (m_isSetAnimNextPlay/* && m_isSetAnimPlaying && !m_isSetAnimFinished*/)
    {
        _uint iIndex = static_cast<_uint>(m_Model_Data.vecAnimationSets[m_iCurSelectSetAnimIndex].vecAnimIndices[m_iCurSetAnimIndex]);
        
        Set_Animation(iIndex, m_Model_Data.vecAnimation[m_iCurSelectSetAnimIndex].animSetup.isLoop);

        m_isSetAnimNextPlay = false;
    }


    if (m_isChangedAnimation)
    {
        if (m_iPrevAnimIndex != m_iCurrentAnimIndex)
        {
            //루트 모션 적용된 것만 실행 
            if(m_Model_Data.vecAnimation[m_iCurrentAnimIndex].animSetup.isRootMotion)
                OnRootMotion();

            m_Animations[m_iCurrentAnimIndex]->OnAnimationBlend(move(m_Animations[m_iPrevAnimIndex]->Get_ChannelMatrices()));

        }
        m_isChangedAnimation = false;
    }

    m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, &m_isFinished, fTimeDelta);

    for (auto& pBone : m_Bones)
        pBone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);

    if (m_isRootMotion)
        Update_RootMotion(fTimeDelta);

    if (m_isSetAnimPlaying && m_isFinished)
    {
        ++m_iCurSetAnimIndex;
        m_isSetAnimNextPlay = true;
        if (m_iCurSetAnimIndex == m_iCurSetAnimMaxIndex)
        {
            m_isSetAnimPlaying = false;
            m_isSetAnimFinished = true;
            m_isSetAnimNextPlay = false;
        }
    }
    return m_isFinished;
}

void CEditor_Model::Set_Animation(_uint iIndex, _bool isLoop)
{
    if (iIndex >= m_iNumAnimations)
        return;

    m_isLoop = isLoop;

    m_iPrevAnimIndex = m_iCurrentAnimIndex;
    m_iCurrentAnimIndex = iIndex;

    if (m_iPrevAnimIndex >= 0 &&  m_iCurrentAnimIndex != m_iPrevAnimIndex)
        m_isChangedAnimation = true;
}

void CEditor_Model::Set_SetAnimation(const string& strKey)
{
    m_iCurSetAnimIndex = 0;

    // vecAnimationSets에서 strKey와 일치하는 세트 찾기
    auto iter = find_if(m_Model_Data.vecAnimationSets.begin(),
        m_Model_Data.vecAnimationSets.end(),
        [&strKey](const ANIMATION_SET_DATA& set) {
            return set.strAnimSetName == strKey;
        });

    if (iter != m_Model_Data.vecAnimationSets.end())
    {
        m_isSetAnimPlaying = true;
        m_isSetAnimNextPlay = true;
        m_isSetAnimFinished = false;
        m_iCurSelectSetAnimIndex = static_cast<_uint>(distance(m_Model_Data.vecAnimationSets.begin(), iter));
        m_iCurSetAnimMaxIndex = static_cast<_uint>(iter->vecAnimIndices.size());

        OutputDebugStringA(("[Animation Set Selected] " + strKey +
            "\n - Set Index: " + to_string(m_iCurSelectSetAnimIndex) +
            "\n - Total Animations: " + to_string(m_iCurSetAnimMaxIndex) + "\n").c_str());

    }
    else
    {
        m_isSetAnimPlaying = false;
        m_isSetAnimNextPlay = false;
        m_isSetAnimFinished = false;
        m_iCurSelectSetAnimIndex = 0;
        m_iCurSetAnimMaxIndex = 0;
        OutputDebugStringA(("[Error] Invalid Animation Set Key: " + strKey + "\n").c_str());
    }
}

CEditor_Animation* CEditor_Model::Get_CurAnimtion()
{
    if (m_iPrevAnimIndex > 0) m_Animations[m_iPrevAnimIndex]->EnbleTrackPosition(true);
    m_Animations[m_iCurrentAnimIndex]->EnbleTrackPosition(true);
    return m_Animations[m_iCurrentAnimIndex];
}

void CEditor_Model::ExportModel(string& strPath)
{
    ///* 파일시스템에서 실행파일 위치를 .exe로 고정 */
    //_char exePath[MAX_PATH];
    //GetModuleFileNameA(NULL, exePath, MAX_PATH);
    //string exeDir = exePath;
    //size_t lastSlash = exeDir.find_last_of("\\/");
    //if (lastSlash != string::npos) exeDir = exeDir.substr(0, lastSlash);
    //SetCurrentDirectoryA(exeDir.c_str());

    /* 현재 실행파일 저장 */
    _char savedDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, savedDir);

    /* 실행파일 위치 Client/default 로 고정  */
    _char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    filesystem::path exeDir = filesystem::path(exePath).parent_path();
    //OutputDebugStringA(("[Editor.exe Dir] " + exeDir.string() + "\n").c_str());

    filesystem::path editorDefaultDir = exeDir.parent_path().parent_path() / "Default";
    //OutputDebugStringA(("[Editor Default Dir] " + editorDefaultDir.string() + "\n").c_str());

    filesystem::path clientDefaultDir = editorDefaultDir.parent_path().parent_path() / "Client" / "Default";
   // OutputDebugStringA(("[Client Default] " + clientDefaultDir.string() + "\n").c_str());

    SetCurrentDirectoryA(clientDefaultDir.string().c_str());

    filesystem::path fullPath(strPath);
    string strDirectory = fullPath.parent_path().string();
    string strFileName = fullPath.stem().string();  // 확장자 제외

    // 모델 이름으로 폴더 경로 생성
    string strModelFolder = strDirectory + "/" + strFileName + "/";

    // 폴더가 없으면 생성
    if (!filesystem::exists(strModelFolder))
    {
        try
        {
            filesystem::create_directories(strModelFolder);
        }
        catch (const filesystem::filesystem_error& e)
        {
            string errorMsg = "폴더 생성 실패: " + string(e.what());
            MessageBoxA(nullptr, errorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
            return;
        }
    }

    // 파일 경로 생성 (strFileName 사용)
    string strDatPath = strModelFolder + strFileName + ".dat";
    string strAnimJsonPath = strModelFolder + strFileName + "_Anim.json";
    string strSummayAnimJsonPath = strModelFolder + strFileName + "_Summary_Anim.json";
    string strMaterialJsonPath = strModelFolder + strFileName + "_Material.json";

    // 덮어쓰기 확인
    _bool bDatExists = filesystem::exists(strDatPath);
    _bool bAnimExists = filesystem::exists(strAnimJsonPath);
    _bool bMaterialExists = filesystem::exists(strMaterialJsonPath);

    if (bDatExists || bAnimExists || bMaterialExists)
    {
        wstring msg = TEXT("다음 파일이 이미 존재합니다:\n\n");
        if (bDatExists)
            msg += AnsiToWString(strFileName) + TEXT(".dat\n");
        if (bAnimExists)
            msg += AnsiToWString(strFileName) + TEXT("_Anim.json\n");
        if (bMaterialExists)
            msg += AnsiToWString(strFileName) + TEXT("_Material.json\n");

        msg += L"\n덮어쓰시겠습니까?";

        _int result = MessageBox(
            nullptr,
            msg.c_str(),
            TEXT("파일 덮어쓰기 확인"),
            MB_YESNO | MB_ICONQUESTION
        );

        if (result == IDNO)
        {
            MSG_BOX(TEXT("저장을 취소했습니다."));
            return;
        }
    }

    // 1. Binary 저장 (.dat) - 전체 데이터
    Export_Binary(strDatPath);

    // 2. Animation JSON 저장
    if (m_eModelType == MODELTYPE::ANIM)
    {
        if (!Export_AnimationJson(strAnimJsonPath, strSummayAnimJsonPath))
        {
            MSG_BOX(TEXT("Animation JSON 저장 실패"));
            return;
        }
    }

    // 3. Material JSON 저장
    if (!Export_MaterialJson(strMaterialJsonPath))
    {
        MSG_BOX(TEXT("Material JSON 저장 실패"));
        return;
    }

    // 성공 메시지
    _wstring successMsg = TEXT("Export 완료!\n\n");
    successMsg += TEXT("폴더: ") + AnsiToWString(strModelFolder) + TEXT("\n");
    successMsg += AnsiToWString(strFileName) + TEXT(".dat (전체)\n");
    if (m_eModelType == MODELTYPE::ANIM)
        successMsg += AnsiToWString(strFileName) + TEXT("_Anim.json\n");
    successMsg += AnsiToWString(strFileName) + TEXT("_Material.json");

    MessageBox(nullptr, successMsg.c_str(), TEXT("Success"), MB_OK | MB_ICONINFORMATION);

    //이전 실행파일로 복귀
    SetCurrentDirectoryA(savedDir);
}

void CEditor_Model::ExportModel_NoMsg(string& strPath)
{
    filesystem::path fullPath(strPath);
    string strDirectory = fullPath.parent_path().string();
    string strFileName = fullPath.stem().string();  // 확장자 제외

    // 모델 이름으로 폴더 경로 생성
    string strModelFolder = strDirectory + "/" + strFileName + "/";

    // 폴더가 없으면 생성
    if (!filesystem::exists(strModelFolder))
    {
        try
        {
            filesystem::create_directories(strModelFolder);
        }
        catch (const filesystem::filesystem_error& e)
        {
            string errorMsg = "폴더 생성 실패: " + string(e.what());
            OutputDebugStringA(errorMsg.c_str());
            return;
        }
    }

    // 파일 경로 생성 (strFileName 사용)
    string strDatPath = strModelFolder + strFileName + ".dat";
    string strAnimJsonPath = strModelFolder + strFileName + "_Anim.json";
    string strSummayAnimJsonPath = strModelFolder + strFileName + "_Summary_Anim.json";
    string strMaterialJsonPath = strModelFolder + strFileName + "_Material.json";

    // 덮어쓰기 확인
    _bool bDatExists = filesystem::exists(strDatPath);
    _bool bAnimExists = filesystem::exists(strAnimJsonPath);
    _bool bMaterialExists = filesystem::exists(strMaterialJsonPath);

    if (bDatExists || bAnimExists || bMaterialExists)
    {
        wstring msg = TEXT("다음 파일이 이미 존재합니다:\n\n");
        if (bDatExists)
            msg += AnsiToWString(strFileName) + TEXT(".dat\n");
        if (bAnimExists)
            msg += AnsiToWString(strFileName) + TEXT("_Anim.json\n");
        if (bMaterialExists)
            msg += AnsiToWString(strFileName) + TEXT("_Material.json\n");

        msg += L"\n덮어쓰시겠습니까?";

        _int result = MessageBox(
            nullptr,
            msg.c_str(),
            TEXT("파일 덮어쓰기 확인"),
            MB_YESNO | MB_ICONQUESTION
        );

        if (result == IDNO)
        {
            MSG_BOX(TEXT("저장을 취소했습니다."));
            return;
        }
    }

    // 1. Binary 저장 (.dat) - 전체 데이터
    Export_Binary_NoMsg(strDatPath);

    // 2. Animation JSON 저장
    if (m_eModelType == MODELTYPE::ANIM)
    {
        if (!Export_AnimationJson(strAnimJsonPath, strSummayAnimJsonPath))
        {
            OutputDebugStringA("Animation JSON 저장 실패");
            OutputDebugStringA("Animation JSON 저장 실패");
            OutputDebugStringA("Animation JSON 저장 실패");
            OutputDebugStringA("Animation JSON 저장 실패");
            OutputDebugStringA("Animation JSON 저장 실패");
            return;
        }
    }

    // 3. Material JSON 저장
    if (!Export_MaterialJson_ForDDS(strMaterialJsonPath))
    {
        OutputDebugStringA("Material JSON 저장 실패");
        OutputDebugStringA("Material JSON 저장 실패");
        OutputDebugStringA("Material JSON 저장 실패");
        OutputDebugStringA("Material JSON 저장 실패");
        OutputDebugStringA("Material JSON 저장 실패");
        return;
    }

    // 성공 메시지
    _wstring successMsg = TEXT("Export 완료!\n\n");
    successMsg += TEXT("폴더: ") + AnsiToWString(strModelFolder) + TEXT("\n");
    successMsg += AnsiToWString(strFileName) + TEXT(".dat (전체)\n");
    if (m_eModelType == MODELTYPE::ANIM)
        successMsg += AnsiToWString(strFileName) + TEXT("_Anim.json\n");
    successMsg += AnsiToWString(strFileName) + TEXT("_Material.json");

    successMsg += TEXT("\n!!! Success !!!");

    OutputDebugStringA(WStringToAnsi(successMsg).c_str());
}

void CEditor_Model::LoadModel(_wstring strModelName)
{
    //string strBasePath = "../Data/";
    //string strDatPath = "../Data/" + WStringToAnsi(strModelName) + "/" + WStringToAnsi(strModelName) + ".dat";;

    //if (!filesystem::exists(strDatPath))
    //{
    //    MSG_BOX(TEXT(".dat 파일이 존재하지 않습니다."));
    //    return;
    //}

    //std::ifstream ifs(strDatPath, std::ios::binary);
    //if (!ifs.is_open())
    //{
    //    MSG_BOX(TEXT("binary 파일 열기 실패"));
    //    return;
    //}

    //m_Model_Data.LoadBinary(ifs);
    //ifs.close();
}
void CEditor_Model::Update_DAT_From_JSON(string& strPath)
{
    ///* 파일시스템에서 실행파일 위치를 .exe로 고정 */
    //_char exePath[MAX_PATH];
    //GetModuleFileNameA(NULL, exePath, MAX_PATH);
    //string exeDir = exePath;
    //size_t lastSlash = exeDir.find_last_of("\\/");
    //if (lastSlash != string::npos) exeDir = exeDir.substr(0, lastSlash);
    //SetCurrentDirectoryA(exeDir.c_str());

       /* 현재 실행파일 저장 */
    _char savedDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, savedDir);

    /* 실행파일 위치 Client/default 로 고정  */
    _char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    filesystem::path exeDir = filesystem::path(exePath).parent_path();
    //OutputDebugStringA(("[Editor.exe Dir] " + exeDir.string() + "\n").c_str());

    filesystem::path editorDefaultDir = exeDir.parent_path().parent_path() / "Default";
    //OutputDebugStringA(("[Editor Default Dir] " + editorDefaultDir.string() + "\n").c_str());

    filesystem::path clientDefaultDir = editorDefaultDir.parent_path().parent_path() / "Client" / "Default";
    // OutputDebugStringA(("[Client Default] " + clientDefaultDir.string() + "\n").c_str());

    SetCurrentDirectoryA(clientDefaultDir.string().c_str());

    filesystem::path fullPath(strPath);
    string strDirectory = fullPath.parent_path().string()+ "/";
    string strFileName = fullPath.stem().string();  // 확장자 제외

    // 모델 폴더 경로
    //string strModelFolder = strDirectory + "/" + strFileName + "/";

    // 파일 경로 생성
    string strDatPath = strDirectory + strFileName + ".dat";
    string strAnimJsonPath = strDirectory + strFileName + "_Anim.json";
    string strMaterialJsonPath = strDirectory + strFileName + "_Material.json";

    // .dat 파일 존재 확인
    if (!filesystem::exists(strDirectory))
    {
        _tchar szMessage[MAX_PATH] = {};
        swprintf_s(szMessage, TEXT(".dat 파일이 존재하지 않습니다!\n경로: %S"),
            strDatPath.c_str());
        MessageBox(nullptr, szMessage, TEXT("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // 1. 기존 .dat 파일 로드
    {
        ifstream ifs(strDatPath, ios::binary);
        if (!ifs.is_open())
        {
            MSG_BOX(TEXT(".dat 파일 열기 실패"));
            return;
        }
        m_Model_Data.LoadBinary(ifs);
        ifs.close();
    }

    // 2. Animation JSON 로드 (파일이 있으면)
    if (filesystem::exists(strAnimJsonPath))
    {
        ifstream ifs(strAnimJsonPath);
        if (!ifs.is_open())
        {
            MSG_BOX(TEXT("Animation JSON 파일 열기 실패"));
            return;
        }

        JSON j;
        ifs >> j;
        ifs.close();

        // 애니메이션 교체
        m_Model_Data.vecAnimation = j.get<vector<ANIMATION_DATA>>();
        m_Model_Data.iNumAnimations = static_cast<_uint>(m_Model_Data.vecAnimation.size());
    }

    // 3. Material JSON 로드 (파일이 있으면)
    if (filesystem::exists(strMaterialJsonPath))
    {
        ifstream ifs(strMaterialJsonPath);
        if (!ifs.is_open())
        {
            MSG_BOX(TEXT("Material JSON 파일 열기 실패"));
            return;
        }

        JSON j;
        ifs >> j;
        ifs.close();

        // 머티리얼 교체
        m_Model_Data.vecMaterials = j.get<vector<MATERIAL_DATA>>();
        m_Model_Data.iNumMaterials = static_cast<_uint>(m_Model_Data.vecMaterials.size());
    }

    // 4. 업데이트된 데이터를 .dat에 다시 저장
    {
        ofstream ofs(strDatPath, ios::binary);
        if (!ofs.is_open())
        {
            MSG_BOX(TEXT(".dat 파일 쓰기 실패"));
            return;
        }
        m_Model_Data.SaveBinary(ofs);
        ofs.close();
    }

    // 성공 메시지
    _tchar szMessage[MAX_PATH] = {};
    swprintf_s(szMessage, TEXT(".dat 파일 업데이트 완료!\n\n폴더: %S\n파일: %S.dat"),
        strDirectory.c_str(), strFileName.c_str());
    MessageBox(nullptr, szMessage, TEXT("Success"), MB_OK | MB_ICONINFORMATION);

    //실행파일 위치 복귀
    SetCurrentDirectoryA(savedDir);

}

HRESULT CEditor_Model::Ready_Meshes()
{
    /* 매쉬 몇 개인지 저장하기 */
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        CEditor_Mesh* pMesh = CEditor_Mesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
        if (pMesh == nullptr)
        {
            MSG_BOX(TEXT("진짜 비상 CEditor_Mesh::Create() 실패!!!!!!"));
            return E_FAIL;
        }
        m_Meshes.push_back(pMesh);

        MESH_DATA meshData;
        pMesh->Get_Data(meshData);
        m_Model_Data.vecMeshes.push_back(meshData);
    }

    return S_OK;
}

HRESULT CEditor_Model::Ready_Materials()
{
    m_iNumMaterials = m_pAIScene->mNumMaterials;

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        CEditor_MeshMaterial* pMeshMaterial = CEditor_MeshMaterial::Create(m_pDevice, m_pContext, m_pModelFilePath, m_pAIScene->mMaterials[i]);
        if (nullptr == pMeshMaterial)
            return E_FAIL;

        m_Materials.push_back(pMeshMaterial);

        MATERIAL_DATA materialData;
        pMeshMaterial->Get_Data(materialData);
        m_Model_Data.vecMaterials.push_back(materialData);
    }

    return S_OK;
}

HRESULT CEditor_Model::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
    CEditor_Bone* pBone = CEditor_Bone::Create(pAINode, iParentIndex);
    if (pBone == nullptr)
        return E_FAIL;

    m_Bones.push_back(pBone);

    BONE_DATA boneData;
    pBone->Get_Data(boneData);
    m_Model_Data.vecBones.push_back(boneData);

    _uint  iIndex = static_cast<_uint>(m_Bones.size() - 1);

    for (size_t i = 0; i < pAINode->mNumChildren; i++)
        Ready_Bones(pAINode->mChildren[i], iIndex);

    return S_OK;
}

HRESULT CEditor_Model::Ready_Animation()
{
    m_iNumAnimations = m_pAIScene->mNumAnimations;

    for (size_t i = 0; i < m_iNumAnimations; i++)
    {
        CEditor_Animation* pAnimation = CEditor_Animation::Create(m_pAIScene->mAnimations[i], m_Bones);
        if (nullptr == pAnimation)
            return E_FAIL;
        m_Animations.push_back(pAnimation);

        ANIMATION_DATA animationData;
        pAnimation->Get_Data(animationData);
        m_Model_Data.vecAnimation.push_back(animationData);
    }

    return S_OK;
}


void CEditor_Model::OnRootMotion()
{
    m_isRootMotion = true;
    m_fCurRootMotionBlendTime = 0.f;
    m_PreRootMatrix = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();
}

void CEditor_Model::Update_RootMotion(_float fTimeDelta)
{
    _matrix CurrentRootMatrix = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();

    m_fCurRootMotionBlendTime += fTimeDelta;

    if(m_fCurRootMotionBlendTime >= m_fRootMotionBlendTime)
        m_isRootMotion = false;
    else
    {
        _float fRatio = m_fCurRootMotionBlendTime / m_fRootMotionBlendTime;

        CurrentRootMatrix.r[3] = XMVectorLerp(m_PreRootMatrix.r[3], CurrentRootMatrix.r[3], fRatio);

        m_Bones[m_iRootBoneIndex]->Set_TransformationMatrix(CurrentRootMatrix);
    }
}

_bool CEditor_Model::Export_AnimationJson(const string& strFilePath, const string& strFilePath2)
{
	/* 전체 애니메이션 JSON 저장*/

	JSON j = m_Model_Data.vecAnimation;
	ofstream ofs(strFilePath);
	if (!ofs.is_open())
		return false;

	ofs << j.dump(4);
	ofs.close();

	if (!ofs.good())
		return false;


	/* 요약본 저장 */

	ANIMATION_SUMMARIES_DATA AnimSummaries;

	for (const auto& anim : m_Model_Data.vecAnimation)
	{
		ANIMATION_SUMMARY_DATA summary;
		summary.strName = anim.strName;
		summary.fDuration = anim.fDuration;
		summary.fTickPerSecond = anim.fTickPerSecond;
		summary.iNumChannels = anim.iNumChannels;

		AnimSummaries.vecSummaries.push_back(summary);
	}
    AnimSummaries.vecAnimationSets = m_Model_Data.vecAnimationSets;

	JSON j2 = AnimSummaries;
	ofstream ofs2(strFilePath2);
	if (!ofs2.is_open())
		return false;

	ofs2 << j2.dump(4);
	ofs2.close();

	if (!ofs2.good())
		return false;


    return true;
}

_bool CEditor_Model::Export_MaterialJson(const string& strFilePath)
{
    _bool    isSuccess = true;
    JSON j = m_Model_Data.vecMaterials;

    ofstream ofs(strFilePath);
    if (ofs.is_open())
    {
        //string jsonStr = j.dump(4);
        //jsonStr = PostProcessJSON(jsonStr);
        //ofs << jsonStr;
		ofs << j.dump(4);
        if (!ofs.good())
            isSuccess = false;

        ofs.close();
    }
    else
    {
        isSuccess = false;
    }

    return isSuccess;
}

_bool CEditor_Model::Export_MaterialJson_ForDDS(const string& strFilePath)
{
    _bool    isSuccess = true;

    for (auto& pMaterial : m_Model_Data.vecMaterials)
    {
        for (auto& Exts : pMaterial.vecExts)
        {
            for (auto& pExt : Exts)
                pExt = ".dds";
        }
        for (auto& FullPaths : pMaterial.vecFullPaths)
        {
            for (auto& pFullPath : FullPaths)
            {
                string strFixFullPath = {};

                _char szDrive[MAX_PATH] = {};
                _char szDirectory[MAX_PATH] = {};
                _char szFileName[MAX_PATH] = {};
                _char szExtern[MAX_PATH] = {};

                _splitpath_s(pFullPath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, szFileName, MAX_PATH, szExtern, MAX_PATH);

                pFullPath = szDrive;
                pFullPath += szDirectory;
                pFullPath += szFileName;
                pFullPath += ".dds";
            }
        }
    }

    JSON j = m_Model_Data.vecMaterials;

    ofstream ofs(strFilePath);
    if (ofs.is_open())
    {
        //string jsonStr = j.dump(4);
        //jsonStr = PostProcessJSON(jsonStr);
        //ofs << jsonStr;
        ofs << j.dump(4);
        if (!ofs.good())
            isSuccess = false;

        ofs.close();
    }
    else
    {
        isSuccess = false;
    }

    return isSuccess;
}

void CEditor_Model::Export_Binary(const string& strFilePath)
{
    // 현재 작업 디렉터리 확인
    filesystem::path currentDir = filesystem::current_path();
    OutputDebugStringA(("[Current Directory] " + currentDir.string() + "\n").c_str());

    // 실제로 열리는 절대 경로 계산
    filesystem::path absolutePath = filesystem::absolute(strFilePath);
    OutputDebugStringA(("[Full Binary Path] " + absolutePath.string() + "\n").c_str());

    ofstream ofs(strFilePath, ios::binary);
    if (!ofs.is_open())
    {
        MSG_BOX(TEXT("binary 파일 열기 실패"));
        return;
    }

    m_Model_Data.SaveBinary(ofs);

    ofs.close();

    MSG_BOX(TEXT("Binary 파일 저장 성공"));
}

void CEditor_Model::Export_Binary_NoMsg(const string& strFilePath)
{
    ofstream ofs(strFilePath, ios::binary);
    if (!ofs.is_open())
    {
        OutputDebugStringA("Binary 파일 열기 실패");
        return;
    }

    m_Model_Data.SaveBinary(ofs);

    ofs.close();

    OutputDebugStringA("Binary 파일 저장 성공");
}

string CEditor_Model::PostProcessJSON(const string& jsonStr)
{
    istringstream iss(jsonStr);
    ostringstream oss;
    string line;
    string arrayBuffer;
    bool inShortArray = false;

    while (getline(iss, line))
    {
        // 배열 시작 감지
        if (line.find('[') != string::npos &&
            line.find(']') == string::npos)
        {
            // 다음 몇 줄을 확인해서 짧은 배열인지 판단
            arrayBuffer = line;
            inShortArray = true;
            continue;
        }

        if (inShortArray)
        {
            arrayBuffer += line;

            // 배열 끝 감지
            if (line.find(']') != string::npos)
            {
                // 한 줄로 압축
                string compressed = CompressArray(arrayBuffer);
                oss << compressed << "\n";
                inShortArray = false;
                arrayBuffer.clear();
                continue;
            }
        }
        else
        {
            oss << line << "\n";
        }
    }

    return oss.str();
}

string CEditor_Model::CompressArray(const string& arrayStr)
{
    string result = arrayStr;

    // 줄바꿈 제거
    result.erase(remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(remove(result.begin(), result.end(), '\r'), result.end());

    // 연속된 공백을 하나로
    result = regex_replace(result, regex(R"(\s+)"), " ");

    return result;
}


CEditor_Model* CEditor_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    CEditor_Model* pInstance = new CEditor_Model(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PreTransformMatrix)))
    {
        MSG_BOX(TEXT("Failed to Created : CEditor_Model"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CEditor_Model::Clone(void* pArg)
{
    CEditor_Model* pInstance = new CEditor_Model(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEditor_Model"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEditor_Model::Free()
{
    __super::Free();

    for (auto& pAinmation : m_Animations)
        Safe_Release(pAinmation);
    m_Animations.clear();

    for (auto& pBone : m_Bones)
        Safe_Release(pBone);
    m_Bones.clear();

    for (auto& pMesh : m_Meshes)
        Safe_Release(pMesh);
    m_Meshes.clear();

    for (auto& pMaterial : m_Materials)
        Safe_Release(pMaterial);
    m_Meshes.clear();

    m_Importer.FreeScene();
}
