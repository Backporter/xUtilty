#include "../include/FileSystem.h"
#include "../include/MessageHandler.h"
#include "../include/INIHandler.h"

#include <stdint.h>

namespace OrbisINIHandler
{
	static const char* LogPath     = "OSEL/log.txt";
	static const char* MiraPath    = "/_mira/CSEL_Loader.ini";
	static const char* DataPath    = "/data/CSEL/CSEL_Loader.ini";
	static const char* AppDataPath = "/app0/data/CSEL/CSEL_Loader.ini";

	bool OrbisINIHandler::Parse()
	{
		SetPath(&LogPath, &MiraPath, &DataPath, &AppDataPath);
		m_path = AppDataPath;

		if (OrbisFileSystem::PathExists(OrbisFileSystem::Full, AppDataPath, false))
		{
			m_path = AppDataPath;
		}

		if (OrbisFileSystem::PathExists(OrbisFileSystem::Full, DataPath, false))
		{
			m_path = DataPath;
		}

		if (OrbisFileSystem::PathExists(OrbisFileSystem::Full, MiraPath, false))
		{
			m_path = MiraPath;
		}

		MessageHandler::KernelPrintOut("Reading INI from %s", m_path);

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
			m_ConsoleToptions.UseCustomIconURL				= m_reader.GetBoolean("CSEL", "bUseCustomIcon", false);
			m_ConsoleToptions.Icon							= strdup(m_reader.Get("CSEL", "sIcon", "https://www.akcpetinsurance.com/res/akc/images/icons/home/home_dog.png").c_str());

			// Game
			m_ConsoleToptions.RenderTargetTextureWidth      = m_reader.GetInteger("Game", "iRenderTargetTextureWidth", 1920);
			m_ConsoleToptions.RenderTargetTextureHeight     = m_reader.GetInteger("Game", "iRenderTargetTextureHeight", 1080);
			m_ConsoleToptions.EnableContainerCategorization = m_reader.GetInteger("Game", "bEnableContainerCategorization", 0);
			
			// PSN
			m_ConsoleToptions.BypassPSN						= m_reader.GetBoolean("PSN", "bBypassPSN", false);
			m_ConsoleToptions.HideWarning					= m_reader.GetBoolean("PSN", "bHideWarning", false);
			m_ConsoleToptions.UserID						= strdup(m_reader.Get("PSN", "sUserID", "NULL").c_str());

			// SaveData
			m_ConsoleToptions.CompressSaveData				= m_reader.GetBoolean("SaveData", "bCompressSaveData", false);
			m_ConsoleToptions.SaveUnmountSleep				= m_reader.GetInteger("SaveData", "iSaveUnmountSleep", 0);
			m_ConsoleToptions.AutoDumpSaveData				= m_reader.GetBoolean("SaveData", "bAutoDumpSaveData", true);
			m_ConsoleToptions.SaveDataDumpDirType			= m_reader.GetInteger("SaveData", "iSaveDataDumpDirType", OrbisFileSystem::USB1);
			m_ConsoleToptions.SaveDataDumpdir				= strdup(m_reader.Get("SaveData", "sSaveDataDumpDir", "CSEL/dump/SaveData/").c_str());

			// Debugger
			m_ConsoleToptions.EnableGFxLogger				= m_reader.GetBoolean("Debugger",	"bEnableGFxLogger",			true);
			m_ConsoleToptions.EnableVirtualMachineLog		= m_reader.GetBoolean("Debugger",	"bEnableVirtualMachineLog", false);
			m_ConsoleToptions.IsDebugMode					= m_reader.GetBoolean("Debugger",	"bIsDebugMode",				false);

			if (m_ConsoleToptions.BypassPSN && !m_ConsoleToptions.HideWarning)
			{
				MessageHandler::Notify("Skyrim Mods/Creation Club PSN bypasser is active, I take no responsabilty for anything that occours while this is active.");
			}

			return true;
		}

		return false;
	}
}