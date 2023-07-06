#pragma once

#include "../include/SystemWrapper.h"
#include "../include/Types.h"

#include <stdint.h>

// Confirm platform via build-platform
#if __ORBIS__ || __OPENORBIS__
// OpenOrbis SDK?, include headers and typedef types to add to build platform cross-compatibility
#if __OPENORBIS__
#include <orbis/UserService.h>
#include <orbis/ImeDialog.h>
#include <orbis/Ime.h>
#include <orbis/ErrorDialog.h>
#include <orbis/MsgDialog.h>
#include <orbis/CommonDialog.h>
#include <orbis/Sysmodule.h>
#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>

typedef OrbisMsgDialogParam			   SceMsgDialogParam;
typedef OrbisMsgDialogUserMessageParam SceMsgDialogUserMessageParam;
typedef OrbisMsgDialogButtonsParam	   SceMsgDialogButtonsParam;
typedef OrbisMsgDialogResult		   SceMsgDialogResult;
typedef OrbisImeDialogSetting		   SceImeDialogParam;
typedef OrbisDialogResult			   SceImeDialogResult;
typedef OrbisImeSettingsExtended       SceImeParamExtended;
typedef OrbisErrorDialogParam		   SceErrorDialogParam;
typedef OrbisCommonDialogBaseParam	   SceCommonDialogBaseParam;

static inline void sceImeDialogParamInit(SceImeDialogParam *param)
{
	memset(param, 0x0, sizeof(SceImeDialogParam));
	param->userId = -1;
}

static inline void sceErrorDialogParamInitialize(SceErrorDialogParam * param)
{
	memset(param, 0x0, sizeof(SceErrorDialogParam));
	param->size = sizeof(SceErrorDialogParam);
	param->userId = 0xFE;
}

static inline void _sceCommonDialogSetMagicNumber(uint32_t* magic, const SceCommonDialogBaseParam* param)
{
	*magic = (uint32_t)(ORBIS_COMMON_DIALOG_MAGIC_NUMBER + (uint64_t)param);
}

static inline void _sceCommonDialogBaseParamInit(SceCommonDialogBaseParam *param)
{
	memset(param, 0x0, sizeof(SceCommonDialogBaseParam));
	param->size = (uint32_t)sizeof(SceCommonDialogBaseParam);
	_sceCommonDialogSetMagicNumber(&(param->magic), param);
}

static inline void sceMsgDialogParamInitialize(SceMsgDialogParam *param)
{
	memset(param, 0x0, sizeof(SceMsgDialogParam));

	_sceCommonDialogBaseParamInit(&param->baseParam);
	param->size = sizeof(SceMsgDialogParam);
}

// SCEE SDK?, include headers.
#elif __ORBIS__
#include <libsysmodule.h>
#include <libime.h>
#include <ime_dialog.h>
#include <error_dialog.h>
#include <message_dialog.h>
#include <common_dialog.h>
#endif

namespace xUtilty
{
	namespace DialogueManger
	{
		// 
		void SetDefaultExtendedSettings(SceImeParamExtended*);

		// to be implemented
		class OrbisDialogueManger
		{
		public:
			enum Mode
			{
				M_INVALID = 0,
				M_USER_MSG = 1,
				M_PROGRESS_BAR = 2,
				M_SYSTEM_MSG = 3,
			};

			enum ButtonType
			{
				BT_OK = 0,
				BT_YESNO = 1,
				BT_NONE = 2,
				BT_OK_CANCEL = 3,
				BT_WAIT = 5,
				BT_WAIT_CANCEL = 6,
				BT_YESNO_FOCUS_NO = 7,
				BT_OK_CANCEL_FOCUS_CANCEL = 8,
				BT_2BUTTONS = 9,
			};
		public:
			OrbisDialogueManger();
			virtual ~OrbisDialogueManger();

			inline wchar_t_t* CreateTitle(const char* a_title)
			{
				if (!a_title)
					return nullptr;

				size_t ret = 0;
				static wchar_t_t buffer[512];
				memset(buffer, 0, sizeof(buffer));

				if ((ret = SystemWrapper::mbstowcs(buffer, a_title, strlen(a_title)) == static_cast<size_t>(-1)))
					return nullptr;
				else
					return buffer;
			}

			inline wchar_t_t* CreateExample(const char* a_ex)
			{
				if (!a_ex)
					return nullptr;

				size_t ret = 0;
				static wchar_t_t buffer[512];
				memset(buffer, 0, sizeof(buffer));

				if ((ret = SystemWrapper::mbstowcs(buffer, a_ex, strlen(a_ex)) == static_cast<size_t>(-1)))
					return nullptr;
				else
					return buffer;
			}

			// Set params for use in MsgDialog
			virtual void InitializeMsgDialog(const char* message, int mode = 1, int ButtonType = 0);

			// two button mode with 
			virtual void InitializeMsgDialog2(const char* message, char* btn1, char* btn2, int mode = 1, int ButtonType = 9);

			// returns 0 if sucessfully used
			virtual int32_t ShowMsgDialog();

			// refer to enums for details for IDs
			virtual SceMsgDialogResult* GetMsgDialogResult();

			virtual void InitializeErrorDialog(int ErrorCode);

			virtual int32_t ShowErrorDialog();

			virtual void InitializeImeDialog(const char* title = "", const char* exampletext = "", float x = 959, float y = 959, int type = 0, int enterlabeltype = 0, int option = 0x2 | 0x8 | 0x10, int max_length = 2048);

			// retuns this->ImeParamExtended so you can set the params
			virtual SceImeParamExtended* GetImeDialogExtendedSettings();

			// BUFFER NEEDS TO BE EXACTLY "char buffer[2048];"
			virtual int32_t ShowImeDialog(char* buf);

			// Singleton implementation for a single class
			static OrbisDialogueManger* GetSingleton()
			{
				static OrbisDialogueManger Singleton;
				return &Singleton;
			}
		private:
			int32_t						ret;

			// MsgDialog
			SceMsgDialogParam			 MsgDialogParam;
			SceMsgDialogUserMessageParam MsgDialogUserMessageParam;
			SceMsgDialogButtonsParam	 MsgDialogButtonsParam;
			SceMsgDialogResult			 MsgDialogResult;

			// ErrorDialog
			SceErrorDialogParam			 ErrorDialogParam;

			// imeDialog
			SceImeDialogParam			 ImeDialogParam;
			SceImeDialogResult			 ImeDialogResult;
			SceImeParamExtended			 ImeParamExtended;
			bool						 UseImeExtendedSettings;
			wchar_t_t*					 Buffer;
		};
	}
}

#endif