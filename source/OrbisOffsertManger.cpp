#include "../include/OrbisOffsertManger.h"

#include "../include/FileSystem.h"
#include "../include/OrbisSFOHandler.h"
#include "../include/MessageHandler.h"
#include "../include/SystemWrapper.h"
#include "../include/WIP/Streams/IFIleStream.h"

#if defined(__ORBIS__)
#include <kernel.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

#include <unistd.h>

OffsetManger::OffsetManger()
{
	Initialize();
}

bool OffsetManger::Initialize()
{
	return true;
}

OffsetManger::~OffsetManger()
{
}