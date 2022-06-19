#pragma once

#include <switch.h>

namespace nn 
{
typedef uint64_t ApplicationId;
};

#ifdef __cplusplus
extern "C" {
#endif

extern int main(int argc, char** argv);
extern void nninitStartup();
extern void _init();
extern void _fini();
extern void __nnDetailNintendoSdkRuntimeObjectFileRefer();
extern void __nnDetailNintendoSdkRuntimeObjectFile();
extern void __nnDetailNintendoSdkNsoFileRefer();
extern void __nnmusl_init_dso_0();
extern void __nnmusl_fini_dso_0();
extern void __nnDetailNintendoSdkNsoFile_0();

#ifdef __cplusplus
}
#endif