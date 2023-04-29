#pragma once

// INIReader
#include "../Third-Party/benhoyt/1.0/INIReader.h"

// xxxxxx_t
#include <stdint.h>

// used to make sure the INI paths are changed when to whatever is needed in the event it's used outside of skyrim, refer to OF4EL for using.
extern void SetPath(const char** a_log, const char** a_mira, const char** a_data, const char** a_appdata);

namespace xUtilty
{
	namespace INIHandler
	{
		struct Settings
		{
			// [General]
			long		PathType{ 1 };																												// iLocalPathType
			const char*	Path{ "OSEL/log.txt" };																										// sLocalPath

			// [VideoOut]
			int			FlipRate{ 0 };																												// iFlipRate
			int32_t		AspectRatio{ -1 };																											// iAspectRatio
			int32_t		Width{ -1 };																												// iWidth
			int32_t		Height{ -1 };																												// iHeight
			int32_t		pitchInPixel{ -1 };																											// iPitch
			int32_t		TilingMode{ -1 };																											// iTilingMode
			int32_t		PixelFormat{ -1 };																											// iPixelFormat

			// [Extras]
			bool		SpawnCCE{ false };																											// bSpawnCCE

			// [CSEL]
			bool		UseCustomIconURL{ false };																									// bUseCustomIcon
			const char* Icon{ "https://www.akcpetinsurance.com/res/akc/images/icons/home/home_dog.png" }; 											// sIcon


			// [Game]
			int32_t		RenderTargetTextureWidth{ 1920 };																							// iRenderTargetTextureWidth
			int32_t		RenderTargetTextureHeight{ 1080 };																							// iRenderTargetTextureHeight
			bool        EnableContainerCategorization{ 0 };																							// bEnableContainerCategorization
			bool		ClearInvalidRegistrations{ 0 };																								// bClearInvalidRegistrations

			// [PSN]
			bool		BypassPSN{ false };																											// bHideWarning
			bool		HideWarning{ false };																										// bBypassPSN
			const char* UserID{ "NULL" };																											// sUserID
			int8_t		Age{ 18 };																													// iUserAge
			const char* CountryCode{ "US" };																										// iUserContryCode

			// [SaveData]
			bool		CompressSaveData{ false };																									// bCompressSaveData
			long		SaveUnmountSleep{ 0 };																										// iSaveUnmountSleep
			bool		AutoDumpSaveData{ false };																									// bAutoDumpSaveData
			long		SaveDataDumpDirType{ 3 };																									// iSaveDataDumpDirType
			const char* SaveDataDumpdir{ "CSEL/dump/SaveData/" };																					// sSaveDataDumpDir


			// [Debugger]
			bool		EnableGFxLogger{ true };																									// bEnableGFxLogger
			bool		EnableVirtualMachineLog{ false };																							// bEnableVirtualMachineLog
			bool		IsDebugMode{ false };																										// bIsDebugMode

			// [Executable]
			bool		UseModuleStartAsEntryPoint{ false };																						// bUseModuleStartAsEntryPoint
		};

		class INIHandler
		{
		public:
			INIHandler() = default;
			~INIHandler() = default;

			//
			static INIHandler* GetSingleton()
			{
				static INIHandler _INIHandler;
				return &_INIHandler;
			}

			bool				Parse();
			inline const char*	GetPath()		{ return m_path; }
			INIReader&			GetReader()		{ return m_reader; }
			Settings*			GetINIOptions()	{ return &m_ConsoleToptions;  }
		public:
			const char*	m_path;
			INIReader	m_reader;
			Settings	m_ConsoleToptions;
		};
	}
}