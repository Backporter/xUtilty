#pragma once

#include <stdint.h>
#include "../Third-Party/benhoyt/1.0/INIReader.h"

// used to make sure the INI paths are changed when to whatever is needed in the event it's used outside of skyrim, refer to OF4EL for using.
extern void SetPath(const char** a_log, const char** a_mira, const char** a_data, const char** a_appdata);

namespace OrbisINIHandler
{	
	struct ConsoleOptions 
	{
		// [General]
		long		PathType;		// iLocalPathType
		const char*	Path;			// sLocalPath

		// [VideoOut]
		int		 FlipRate;			// iFlipRate
		uint32_t AspectRatio;		// iAspectRatio
		uint32_t Width;				// iWidth
		uint32_t Height;			// iHeight
		uint32_t pitchInPixel;		// iPitch
		uint32_t TilingMode;		// iTilingMode
		uint32_t PixelFormat;		// iPixelFormat

		// [Extras]
		bool	SpawnCCE;

		// [CSEL]
		bool		UseCustomIconURL;
		const char* Icon;
		bool		EnableGFxLogger;
		bool		EnableDebugLogs;
		bool		EnableVirtualMachineLog;

		// [Game]
		uint32_t	RenderTargetTextureWidth;
		uint32_t	RenderTargetTextureHeight;

		// [PSN]
		bool		BypassPSN;
		bool		HideWarning;
		const char* UserID;

		// [SaveData]
		long		SaveUnmountSleep;		// iSaveUnmountSleep
		bool		AutoDumpSaveData;		//
		long		SaveDataDumpDirType;	// iSaveDataDumpDirType
		const char* SaveDataDumpdir;		//
	};

	class OrbisINIHandler
	{
	public:
		OrbisINIHandler();
		virtual ~OrbisINIHandler();

		virtual bool			ParseINI();
		virtual ConsoleOptions* GetINIOptions();
		virtual INIReader*		GetReader();
		virtual bool			IsVailid();

		static OrbisINIHandler* GetSingleton()
		{
			static OrbisINIHandler _OrbisINIHandler;
			return &_OrbisINIHandler;
		}
	public:
		bool		   Parsed;
		const char*	   INIPath;
		INIReader	   Reader;
		ConsoleOptions INIOptions;
	};
}