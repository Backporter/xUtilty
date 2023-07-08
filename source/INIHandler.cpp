#include "../include/INIHandler.h"

//
#include "../include/FileSystem.h"
#include "../include/MessageHandler.h"

namespace xUtilty
{
	namespace INIHandler
	{
		static const char* LogPath     = "OSEL/log.txt";
		static const char* MiraPath    = "/_mira/CSEL_Loader.ini";
		static const char* DataPath    = "/data/CSEL/CSEL_Loader.ini";
		static const char* AppDataPath = "/app0/data/CSEL/CSEL_Loader.ini";
	
		bool INIHandler::Parse()
		{
			SetPath(&LogPath, &MiraPath, &DataPath, &AppDataPath);
		
			m_path = AppDataPath;
	
			if (FileSystem::PathExists(FileSystem::Full, AppDataPath, false))
			{
				m_path = AppDataPath;
			}
	
			if (FileSystem::PathExists(FileSystem::Full, DataPath, false))
			{
				m_path = DataPath;
			}
	
			if (FileSystem::PathExists(FileSystem::Full, MiraPath, false))
			{
				m_path = MiraPath;
			}
	
			KernelPrintOut("Reading INI from %s", m_path);
	
			m_reader = INIReader(m_path);
			if (!m_reader.ParseError())
			{
				// General
				m_ConsoleToptions.PathType						= m_reader.GetInteger("General", "iLocalPathType", 1);
				m_ConsoleToptions.Path							= strdup(m_reader.Get("General", "sLocalPath", "OSEL/log.txt").c_str());
	
				// VideoOut
				m_ConsoleToptions.FlipRate						= m_reader.GetInteger("VideoOut", "iFlipRate", 0);
				m_ConsoleToptions.AspectRatio					= m_reader.GetInteger("VideoOut", "iAspectRatio", -1);
				m_ConsoleToptions.Width							= m_reader.GetInteger("VideoOut", "iWidth", -1);
				m_ConsoleToptions.Height						= m_reader.GetInteger("VideoOut", "iHeight", -1);
				m_ConsoleToptions.pitchInPixel					= m_reader.GetInteger("VideoOut", "iPitch", -1);
				m_ConsoleToptions.TilingMode					= m_reader.GetInteger("VideoOut", "iTilingMode", -1);
				m_ConsoleToptions.PixelFormat					= m_reader.GetInteger("VideoOut", "iPixelFormat", -1);
	
				// Extras
				m_ConsoleToptions.SpawnCCE						= m_reader.GetBoolean("Extras", "bSpawnCCE", false);
	
				// CSEL
				m_ConsoleToptions.UseExperimentalLogic			= m_reader.GetBoolean("CSEL", "bUseExperimentalLogic", false);
				m_ConsoleToptions.UseCustomIconURL				= m_reader.GetBoolean("CSEL", "bUseCustomIcon", false);
				m_ConsoleToptions.Icon							= strdup(m_reader.Get("CSEL", "sIcon", "https://www.akcpetinsurance.com/res/akc/images/icons/home/home_dog.png").c_str());
				m_ConsoleToptions.GAME_BUILD_MINOR				= m_reader.GetInteger("CSEL", "iOffsetDatabaseMinor", 1);
				m_ConsoleToptions.GAME_BUILD_MAJOR				= m_reader.GetInteger("CSEL", "iOffsetDatabaseMajor", 5);
				m_ConsoleToptions.GAME_BUILD_BUILD				= m_reader.GetInteger("CSEL", "iOffsetDatabaseBuild", 23);
				m_ConsoleToptions.GAME_BUILD_SUB				= m_reader.GetInteger("CSEL", "iOffsetDatabaseSub", 0);

				// Game
				m_ConsoleToptions.RenderTargetTextureWidth      = m_reader.GetInteger("Game", "iRenderTargetTextureWidth", 1920);
				m_ConsoleToptions.RenderTargetTextureHeight     = m_reader.GetInteger("Game", "iRenderTargetTextureHeight", 1080);
				m_ConsoleToptions.EnableContainerCategorization = m_reader.GetBoolean("Game", "bEnableContainerCategorization", 0);
				m_ConsoleToptions.ClearInvalidRegistrations		= m_reader.GetBoolean("Game", "bClearInvalidRegistrations", 0);
	
				// PSN
				m_ConsoleToptions.BypassPSN						= m_reader.GetBoolean("PSN", "bBypassPSN", false);
				m_ConsoleToptions.HideWarning					= m_reader.GetBoolean("PSN", "bHideWarning", false);
				m_ConsoleToptions.UserID						= strdup(m_reader.Get("PSN", "sUserID", "NULL").c_str());
				m_ConsoleToptions.Age							= m_reader.GetInteger("PSN", "iAge", 18);
				m_ConsoleToptions.UserID						= strdup(m_reader.Get("PSN", "sCountryCode", "US").c_str());
	
				// SaveData
				m_ConsoleToptions.CompressSaveData				= m_reader.GetBoolean("SaveData", "bCompressSaveData", false);
				m_ConsoleToptions.SaveUnmountSleep				= m_reader.GetInteger("SaveData", "iSaveUnmountSleep", 0);
				m_ConsoleToptions.AutoDumpSaveData				= m_reader.GetBoolean("SaveData", "bAutoDumpSaveData", true);
				m_ConsoleToptions.SaveDataDumpDirType			= m_reader.GetInteger("SaveData", "iSaveDataDumpDirType", FileSystem::Data);
				m_ConsoleToptions.SaveDataDumpdir				= strdup(m_reader.Get("SaveData", "sSaveDataDumpDir", "CSEL/dump/SaveData/").c_str());
	
				// Debugger
				m_ConsoleToptions.EnableGFxLogger				= m_reader.GetBoolean("Debugger",	"bEnableGFxLogger",			true);
				m_ConsoleToptions.EnableVirtualMachineLog		= m_reader.GetBoolean("Debugger",	"bEnableVirtualMachineLog", false);
				m_ConsoleToptions.IsDebugMode					= m_reader.GetBoolean("Debugger",	"bIsDebugMode",				false);
	
				// Executible
				m_ConsoleToptions.UseModuleStartAsEntryPoint	= m_reader.GetBoolean("Executable", "bUseModuleStartAsEntryPoint", false);

				// AFR
				m_ConsoleToptions.UseAFR						= m_reader.GetBoolean("AFR", "UseAFR", false);
				m_ConsoleToptions.FileSeperator					= m_reader.GetInteger("AFR", "FileSeperator", '\0');
				m_ConsoleToptions.FilesToRedirect				= strdup(m_reader.Get("AFR", "FilesToRedirect", "").c_str());

				if (m_ConsoleToptions.BypassPSN && !m_ConsoleToptions.HideWarning)
				{
					Notify("Skyrim Mods/Creation Club PSN bypasser is active, I take no responsabilty for anything that occours while this is active.");
				}

				return true;
			}

			return false;
		}
	}
}