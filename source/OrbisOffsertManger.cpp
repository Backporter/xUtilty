#include "../include/OrbisOffsertManger.h"
#include "../include/FileSystem.h"
#include "../include/OrbisSFOHandler.h"
#include "../include/MessageHandler.h"
#include "../include/SystemWrapper.h"

#if defined(__ORBIS__)
#include <kernel.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

#include <unistd.h>

namespace OrbisOffsetManger
{
	void	  OffsetManger::Initialize()
	{
		char path[260] { 0 };
		const char* TITILE = NULL;
		const char* APP_VER = NULL;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		TITILE = OrbisSFOHandler::sfo_parser::GetSingleton()->GetKeySTR("TITLE");
		APP_VER = OrbisSFOHandler::sfo_parser::GetSingleton()->GetKeySTR("APP_VER");
		
		if (!TITILE)
			TITILE = "Skyrim";
		
		if (!APP_VER)
			APP_VER = "01.09";
#endif

		OrbisFileSystem::CreateFullPath(path, OrbisFileSystem::App, "data/DB/%s-%s.offset", TITILE, APP_VER);
		m_path = strdup(path);
	}

	bool	  OffsetManger::Parse()
	{
		int64_t s_ret = 0;
		V1		s_entryv1{ 0 };
		V2		s_entryv2{ 0 };
		
		if (m_path)
		{
			if ((m_fd = open(m_path, 0, 0)) >= 0)
			{
				if ((s_ret = read(m_fd, &m_header, sizeof(m_header))) > 0)
				{
					if (m_header.Magic == GetPlatformID())
					{
						if (!strcasecmp(m_header.Platform, GetPlatformStringID()))
						{
							if (m_header.Count > 0)
							{
								if (m_header.Type == DBType::V1)
								{
									for (int i = 0; i < m_header.Count; i++)
									{
										if (read(m_fd, &s_entryv1, sizeof(s_entryv1)) > 0)
											m_entiresv1.push_back(s_entryv1);
									}

									m_flags |= Flags::kV1;
									close(m_fd);
									g_useDB = true;
									return true;
								}
								else if (m_header.Type == DBType::V2)
								{
									for (int i = 0; i < m_header.Count; i++)
									{
										if (read(m_fd, &s_entryv2, sizeof(s_entryv2)) > 0)
											m_entiresv2.push_back(s_entryv2);
									}

									m_flags |= Flags::kV2;
									close(m_fd);
									g_useDB = true;
									return true;
								}
							}
							else
							{
								g_useDB = false;
								MessageHandler::KernelPrintOut("Entry count is empty");
							}
						}
						else
						{
							g_useDB = false;
							MessageHandler::KernelPrintOut("Header Platform-Mismatch");
						}
					}
					else
					{
						g_useDB = false;
						MessageHandler::KernelPrintOut("Header Magic-Mismatch");
					}
				}
				else
				{
					g_useDB = false;
					MessageHandler::KernelPrintOut("Failed to read header for 0x%lx", s_ret);
				}
			}
			else
			{
				g_useDB = false;
				MessageHandler::KernelPrintOut("Failed to open db(%s/%d)", m_path, m_fd);
			}
		}

		close(m_fd);
		return false;
	}

	uintptr_t OffsetManger::GetOffset(uint32_t a_crc32)
	{
		/* is V1?Get V1 */
		if ((m_flags & Flags::kV1) == 0)
		{
			for (auto& item : m_entiresv1)
			{
				if (CryptoHandler::GetCRC32(item.ID, 0) == a_crc32)
					return item.offset;
			}
		}

		else if ((m_flags & Flags::kV2) == 0)
		{
			for (auto& item : m_entiresv2)
			{
				if (item.ID == a_crc32)
					return item.offset;
			}
		}

		return -1;
	}
}