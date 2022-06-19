#include "../include/FileSystem.h"
#include "../include/MessageHandler.h"
#include "../include/INIHandler.h"

#include <stdint.h>

namespace OrbisINIHandler
{
	static const char* MiraPath    = "/_mira/CSEL_Loader.ini";
	static const char* DataPath    = "/data/CSEL/CSEL_Loader.ini";

#if defined (__ORBIS__) || defined(__OPENORBIS__)
	static const char* AppDataPath = "/app0/data/CSEL/CSEL_Loader.ini";
#elif defined(PLATFORM_NX) || defined(__SWITCH__)
	static const char* AppDataPath = "/data/CSEL/CSEL_Loader.ini";
#endif
	OrbisINIHandler::OrbisINIHandler()
		:INIPath(nullptr)
	{
	}

	OrbisINIHandler::~OrbisINIHandler()
	{
		// 
	}

	bool OrbisINIHandler::ParseCSELINI()
	{
		INIPath = AppDataPath;

		if (OrbisFileSystem::PathExists(OrbisFileSystem::Full, AppDataPath, false))
		{
			INIPath = AppDataPath;
		}

		if (OrbisFileSystem::PathExists(OrbisFileSystem::Full, DataPath, false))
		{
			INIPath = DataPath;
		}

		if (OrbisFileSystem::PathExists(OrbisFileSystem::Full, MiraPath, false))
		{
			INIPath = MiraPath;
		}

		MessageHandler::KernelPrintOut("Reading INI from %s", INIPath);
		this->Reader = INIReader(INIPath);
		if (this->Reader.ParseError() < 0)
		{
			MessageHandler::KernelPrintOut("Failed to parse [%s]", INIPath);
			this->Parsed = false;

			this->INIOptions.PathType = 1;
			this->INIOptions.Path = "OSEL/log.txt";

			this->INIOptions.FlipRate = 0;
			this->INIOptions.AspectRatio = -1;
			this->INIOptions.Width = -1;
			this->INIOptions.Height = -1;
			this->INIOptions.pitchInPixel = -1;
			this->INIOptions.TilingMode = -1;
			this->INIOptions.PixelFormat = -1;
			this->INIOptions.SpawnCCE = false;

			this->INIOptions.EnableGFxLogger = true;
			this->INIOptions.EnableDebugLogs = true;

			this->INIOptions.BypassPSN = false;
			this->INIOptions.HideWarning = false;
			this->INIOptions.UserID	   = "NULL";
			
			this->INIOptions.SaveUnmountSleep = 1;
			this->INIOptions.AutoDumpSaveData = false;
			this->INIOptions.SaveDataDumpDirType = OrbisFileSystem::USB1;
			this->INIOptions.SaveDataDumpdir = "CSEL/dump/SaveData/";
			return false;
		}

		INIOptions.PathType				= this->Reader.GetInteger("General", "iLocalPathType", 1);
		INIOptions.Path					= strdup(this->Reader.Get("General", "sLocalPath", "OSEL/log.txt").c_str());
		INIOptions.FlipRate				= this->Reader.GetInteger("Patches", "iFlipRate", 0);
		INIOptions.AspectRatio			= this->Reader.GetInteger("Patches", "iAspectRatio", -1);
		INIOptions.Width				= this->Reader.GetInteger("Patches", "iWidth", -1);
		INIOptions.Height				= this->Reader.GetInteger("Patches", "iHeight", -1);
		INIOptions.pitchInPixel			= this->Reader.GetInteger("Patches", "iPitch", -1);
		INIOptions.TilingMode			= this->Reader.GetInteger("Patches", "iTilingMode", -1);
		INIOptions.PixelFormat			= this->Reader.GetInteger("Patches", "iPixelFormat", -1);
		INIOptions.SpawnCCE				= this->Reader.GetBoolean("Extras",  "bSpawnCCE", false);
		INIOptions.Icon					= strdup(this->Reader.Get("CSEL", "sIcon", "https://www.akcpetinsurance.com/res/akc/images/icons/home/home_dog.png").c_str());
		INIOptions.EnableGFxLogger		= this->Reader.GetBoolean("CSEL", "bEnableGFxLogger", true);
		INIOptions.EnableDebugLogs		= this->Reader.GetBoolean("CSEL", "bEnableDebugLogs", true);
		INIOptions.BypassPSN			= this->Reader.GetBoolean("PSN", "bBypassPSN", false);
		INIOptions.HideWarning			= this->Reader.GetBoolean("PSN", "bHideWarning", false);
		INIOptions.UserID				= strdup(this->Reader.Get("PSN", "sUserID", "NULL").c_str());
		INIOptions.SaveUnmountSleep		= this->Reader.GetInteger("SaveData", "iSaveUnmountSleep", 0);
		INIOptions.AutoDumpSaveData		= this->Reader.GetBoolean("SaveData", "bAutoDumpSaveData", true);
		INIOptions.SaveDataDumpDirType  = this->Reader.GetInteger("SaveData", "iSaveDataDumpDirType", OrbisFileSystem::USB1);
		INIOptions.SaveDataDumpdir		= strdup(this->Reader.Get("SaveData", "sSaveDataDumpDir", "CSEL/dump/SaveData/").c_str());

		if (this->INIOptions.BypassPSN && !this->INIOptions.HideWarning)
		{
			MessageHandler::Notify("Skyrim Mods/Creation Club PSN bypasser is active, I take no responsabilty for anything that occours while this is active.");
		}

		this->Parsed = true;
		return true;
	}

	INIReader* OrbisINIHandler::GetReader()
	{
		return &Reader;
	}

	ConsoleOptions* OrbisINIHandler::GetINIOptions()
	{
		return &INIOptions;
	}

	bool OrbisINIHandler::IsVailid()
	{
		return this->Parsed;
	}
}