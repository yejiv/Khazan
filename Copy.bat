// 명령어			옵션			원본 파일이 있는 위치			사본 파일을 저장할 위치

xcopy			/y/s		.\Engine\Public\*.*						.\EngineSDK\Debug\Inc\
xcopy			/y/s		.\Engine\Public\*.*						.\EngineSDK\Release\Inc\

xcopy			/y			.\Engine\Bin\Debug\Engine.dll			.\Client\Bin\Debug\
xcopy			/y			.\Engine\Bin\Release\Engine.dll			.\Client\Bin\Release\

xcopy			/y			.\Engine\Bin\Debug\Engine.dll			.\Editor\Bin\Debug\
xcopy			/y			.\Engine\Bin\Release\Engine.dll			.\Editor\Bin\Release\

xcopy			/y			.\Engine\Bin\Debug\Engine.lib			.\EngineSDK\Debug\Lib\
xcopy			/y			.\Engine\Bin\Release\Engine.lib			.\EngineSDK\Release\Lib\

xcopy			/y			.\Engine\ThirdPartyLib\Debug\*.lib		.\EngineSDK\Debug\Lib\
xcopy			/y			.\Engine\ThirdPartyLib\Release\*.lib	.\EngineSDK\Release\Lib\

xcopy			/y			.\Engine\Bin\ShaderFiles\*.* .\Client\Bin\ShaderFiles\
xcopy			/y			.\Engine\Bin\ShaderFiles\*.* .\Editor\Bin\ShaderFiles\