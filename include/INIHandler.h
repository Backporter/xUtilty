#pragma once

#include <stdint.h>
#include "../Third-Party/benhoyt/1.0/INIReader.h"

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
		const char* Icon;
		bool		EnableGFxLogger;
		bool		EnableDebugLogs;

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
		virtual bool ParseCSELINI();

		virtual ConsoleOptions* GetINIOptions();
		virtual INIReader* GetReader();
		virtual bool IsVailid();

		static OrbisINIHandler* GetSingleton()
		{
			static OrbisINIHandler _OrbisINIHandler;
			return &_OrbisINIHandler;
		}
	private:
		bool Parsed;
		const char*	   INIPath;
		INIReader	   Reader;
		ConsoleOptions INIOptions;
	};
}