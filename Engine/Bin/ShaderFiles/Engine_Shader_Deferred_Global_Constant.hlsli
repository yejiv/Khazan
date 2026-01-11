// ===== Time =====
float g_fTimeDelta;

// ===== Matrix =====
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// ===== Camera =====
matrix g_CamViewMatrix, g_CamProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
vector g_vCamPosition;

// ===== Textures =====
Texture2D g_DiffuseTexture, g_NormalTexture, g_DepthTexture, g_ShadeTexture, g_SpecularTexture, g_EmissiveTexture;
Texture2D g_ShadowTexture;
Texture2D g_PostSceneTexture, g_CombinedTexture;
Texture2D g_BrightTexture, g_BlurXTexture, g_BloomTexture;
Texture2D g_FogTexture, g_OutlineTexture, g_NoiseTexture, g_SSAOTexture, g_LUTTexture, g_VelocityTexture;
Texture2D g_Texture;
Texture2D g_AccumColorTexture, g_AccumAlphaTexture;
Texture2DArray<float> g_TextureArray;
int g_iTextureArrayIndex;

// ===== Light =====
vector g_vLightDir, g_vLightPos;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;
float g_fToonShadeLevel;
float g_fLightRange;
float2 g_vSpecularPower;
float g_fRimPower, g_fRimToonThreshold, g_fRimIntensity;
float g_fSpecularAttuenation;

// ===== Material =====
vector g_vMtrlAmbient = { 1.f, 1.f, 1.f, 1.f }, g_vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

// ===== Cascade Shadow =====
//  uint g_iNumCascades;
//  float g_Splits[4];
//  matrix g_LightViewMatrices[4], g_LightProjMatrices[4];
float2 g_vShadowMapSize;
float g_fShadowBias;
float g_fShadowIntensity;
matrix g_LightViewMatrix, g_LightProjMatrix;
float g_fSplitFar;

// ===== SSAO =====
float2 g_vScreenSize;
StructuredBuffer<float3> g_Kernels;
uint g_iNumKernels;
float g_fSSAORadius, g_fSSAOIntensity, g_fSSAOContrast;

// ===== Gaussian Blur =====
float g_fViewportWidth, g_fViewportHeight;
StructuredBuffer<float> g_Weights;
float g_fBlurNormalization;
int g_iWeightRadius;

// ===== Fog =====
uint g_iFogMode;
float g_fFogNear, g_fFogFar, g_fFogDensity, g_fFogBias;
float4 g_vFogColor;
float2 g_vNoiseSpeed, g_vNoiseScale;
float g_fNoiseStrength, g_fNoiseContrast;
float g_fFogBaseHeight, g_fFogHeightDensity;
float g_fSubColorStartHeight;
float4 g_vFogSubColor;
float g_fFogLightBleedStrength;

// ===== Outline =====
float g_fOutlineAlpha, g_fOutlineBias;

// ===== Vignette =====
float g_fVignettePower, g_fVignetteIntensity, g_fVignetteContrast;
float3 g_vVignetteColor;

// ===== Distortion =====
float g_fDistortionPower, g_fDistortionSpeed, g_fAspect, g_fDistortionRange;
float3 g_vWorldCenterPos;

// ===== LUT =====
float g_fLUTIntensity;
uint g_iLUTSliceSize;
float2 g_vLUTTextureSize;

// ===== Radial Blur =====
float2 g_vRadialBlurCenterUV, g_vRadialBlurMaskRadius;
float g_fRadialBlurRadius, g_fRadialBlurAtt, g_fRadialBlurExp, g_fRadialBlurStrength;
uint g_iNumRadialBlurSamples;

// ===== Motion Blur =====
matrix g_PrevViewMatrix, g_PrevProjMatrix;
uint g_iNumMotionBlurSamples;
float g_fMotionBlurBias, g_fMotionBlurStrength;

// ===== Render Flags =====
bool g_isEnableToonShade = { true };
bool g_isEnableShadow = { true };
bool g_isEnableSSAO = { true };
bool g_isEnableFog = { true };
bool g_isEnableFogNoise;
bool g_isUseSubColor;
bool g_isUseHeightFog;
bool g_isEnableOutline;
bool g_isEnableVignette;
bool g_isUseVignetteNoise;
bool g_isEnableRadialBlur;
bool g_isEnableMotionBlur;
bool g_isEnableLUT;
bool g_isEnableDistortion;
bool g_isEnableRimLight;
bool g_isToonLight;
bool g_isUnlitMode;
bool g_isLitMode;
bool g_isEnableSpecular;
bool g_isEnableBloom;
