#include "../O/OrbisDialogueManger.h"
#include "../O/OrbisMessageHandler.h"

namespace OrbisDialogueManger
{
	void SetDefaultExtendedSettings(SceImeParamExtended* ExtendedSettings)
	{
		// ExtendedSettings->option = 1;
	}

	OrbisDialogueManger::OrbisDialogueManger()
		:UseImeExtendedSettings(false)
	{
	}

	OrbisDialogueManger::~OrbisDialogueManger()
	{

	}

	void OrbisDialogueManger::InitializeMsgDialog(char* message, int mode, int ButtonType)
	{
		sceMsgDialogParamInitialize(&this->MsgDialogParam);
		memset(&this->MsgDialogUserMessageParam, 0, sizeof(this->MsgDialogUserMessageParam));

		this->MsgDialogUserMessageParam.msg		   = message; // L"EXAMPLE"
#if defined(__ORBIS__)
		this->MsgDialogParam.mode = mode;
		this->MsgDialogUserMessageParam.buttonType = ButtonType;
#elif defined(__OPENORBIS__)
		this->MsgDialogParam.mode				   = (OrbisMsgDialogMode)mode;
		this->MsgDialogUserMessageParam.buttonType = (OrbisMsgDialogButtonType)ButtonType;
#endif
		this->MsgDialogParam.userMsgParam		   = &this->MsgDialogUserMessageParam;
	}

	void OrbisDialogueManger::InitializeMsgDialog2(char* message, char* btn1, char* btn2, int mode, int ButtonType)
	{
		sceMsgDialogParamInitialize(&this->MsgDialogParam);
		memset(&this->MsgDialogUserMessageParam, 0, sizeof(this->MsgDialogUserMessageParam));
		memset(&this->MsgDialogButtonsParam, 0, sizeof(this->MsgDialogButtonsParam));
		memset(&this->MsgDialogResult, 0, sizeof(this->MsgDialogResult));

		this->MsgDialogButtonsParam.msg1		     = btn1;
		this->MsgDialogButtonsParam.msg2		     = btn2;			
#if defined(__ORBIS__)
		this->MsgDialogParam.mode				     = mode;
		this->MsgDialogUserMessageParam.buttonType	 = ButtonType;
#elif defined(__OPENORBIS__)
		this->MsgDialogParam.mode					 = (OrbisMsgDialogMode)mode;
		this->MsgDialogUserMessageParam.buttonType   = (OrbisMsgDialogButtonType)ButtonType;
#endif
		this->MsgDialogUserMessageParam.msg		     = message;
		this->MsgDialogParam.userMsgParam		     = &this->MsgDialogUserMessageParam;
		this->MsgDialogUserMessageParam.buttonsParam = &this->MsgDialogButtonsParam;
	}

	int32_t OrbisDialogueManger::ShowMsgDialog()
	{
		if ((ret = sceSysmoduleLoadModule(0x00a4)) != 0)
		{
			OrbisMessageHandler::Notify("sceSysmoduleLoadModule(0x00a4) failed with 0x%lx", ret);
			return 0;
		}

		if ((ret = sceCommonDialogInitialize()) < 0 && ret != 0x80B80002)
		{
			OrbisMessageHandler::Notify("sceCommonDialogInitialize() failed with 0x%lx", ret);
			return 0;
		}

		if ((ret = sceMsgDialogInitialize()) < 0 && ret != 0x80B80004)
		{
			OrbisMessageHandler::Notify("sceMsgDialogInitialize() failed with 0x%lx", ret);
			return 0;
		}

		if ((ret = sceMsgDialogOpen(&MsgDialogParam)) < 0)
		{
			OrbisMessageHandler::Notify("sceMsgDialogOpen failed with 0x%lx", ret);
			sceMsgDialogClose();
			return 0;
		}

		while (sceMsgDialogUpdateStatus() != 3);
		sceMsgDialogClose();

		sceMsgDialogGetResult(&MsgDialogResult);

		sceMsgDialogTerminate();
		return 0;
	}

	SceMsgDialogResult* OrbisDialogueManger::GetMsgDialogResult()
	{
		return &MsgDialogResult;
	}
	
	void OrbisDialogueManger::InitializeErrorDialog(int ErrorCode)
	{
		sceErrorDialogParamInitialize(&this->ErrorDialogParam);
#if defined(__ORBIS__)
		this->ErrorDialogParam.errorCode = ErrorCode;
#elif defined(__OPENORBIS__)
		this->ErrorDialogParam.errorcode = ErrorCode;
#endif
	}

	int32_t OrbisDialogueManger::ShowErrorDialog()
	{
		if ((ret = sceSysmoduleLoadModule(0x00ac)) != 0)
		{
			OrbisMessageHandler::Notify("sceSysmoduleLoadModule(0x00ac) failed with 0x%lx", ret);
			return 0;
		}

		if ((ret = sceCommonDialogInitialize()) < 0 && ret != 0x80B80002)
		{
			OrbisMessageHandler::Notify("sceCommonDialogInitialize() failed with 0x%lx", ret);
			return 0;
		}

		if ((ret = sceErrorDialogInitialize()) < 0 && ret != 0x80B80004)
		{
			OrbisMessageHandler::Notify("sceErrorDialogInitialize() failed with 0x%lx", ret);
			return 0;
		}

		if ((ret = sceErrorDialogOpen(&ErrorDialogParam)) < 0)
		{
			OrbisMessageHandler::Notify("sceErrorDialogOpen failed with 0x%lx", ret);
			sceMsgDialogClose();
			return 0;
		}

		while (sceErrorDialogUpdateStatus() != 3);
		sceErrorDialogClose();
		sceErrorDialogTerminate();
		return 0;
	}

	void OrbisDialogueManger::InitializeImeDialog(wchar_t* title, wchar_t* exampletext, float x, float y, int type, int enterlabeltype, int option, int max_length)
	{
		sceImeDialogParamInit(&this->ImeDialogParam);
#if defined(__ORBIS__)
		sceUserServiceGetInitialUser(&this->ImeDialogParam.userId);
		this->ImeDialogParam.type				 = SceImeType::SCE_IME_TYPE_DEFAULT;
		this->ImeDialogParam.enterLabel			 = SceImeEnterLabel::SCE_IME_ENTER_LABEL_DEFAULT;
		this->ImeDialogParam.inputMethod		 = SceImeInputMethod::SCE_IME_INPUT_METHOD_DEFAULT;
#elif defined(__OPENORBIS__)
		int UserID = 0;
		sceUserServiceGetInitialUser(&UserID);
		this->ImeDialogParam.userId = UserID;
		this->ImeDialogParam.type				 = OrbisImeType::ORBIS_TYPE_DEFAULT;
		this->ImeDialogParam.enterLabel			 = OrbisButtonLabel::ORBIS_BUTTON_LABEL_DEFAULT;
		this->ImeDialogParam.inputMethod		 = OrbisInput::ORBIS__DEFAULT;
#endif

		this->ImeDialogParam.supportedLanguages = 0;
		this->ImeDialogParam.filter				 = NULL;
		this->ImeDialogParam.option				 = option;
		this->ImeDialogParam.maxTextLength		 = max_length;
		this->ImeDialogParam.inputTextBuffer	 = this->Buffer = new wchar_t[max_length];
		memset(this->Buffer, 0, max_length);

		this->ImeDialogParam.posx				 = x;
		this->ImeDialogParam.posy				 = y;
#if defined(__ORBIS__)
		this->ImeDialogParam.horizontalAlignment = SceImeHorizontalAlignment::SCE_IME_HALIGN_LEFT;
		this->ImeDialogParam.verticalAlignment	 = SceImeVerticalAlignment::SCE_IME_VALIGN_TOP;
#elif defined(__OPENORBIS__)
		this->ImeDialogParam.horizontalAlignment = OrbisHAlignment::ORBIS_H_LEFT;
		this->ImeDialogParam.verticalAlignment	 = OrbisVAlignment::ORBIS_V_TOP;
#endif
		this->ImeDialogParam.placeholder		 = exampletext;
		this->ImeDialogParam.title				 = title;

	}

	SceImeParamExtended* OrbisDialogueManger::GetImeDialogExtendedSettings()
	{
		UseImeExtendedSettings = true;
		return &ImeParamExtended;
	}

	int32_t OrbisDialogueManger::ShowImeDialog(char* buf)
	{
		int ret = 0;
		// char buf[2048] { 0 };

		if ((ret = sceSysmoduleLoadModule(0x0096)) != 0)
		{
			OrbisMessageHandler::Notify("sceSysmoduleLoadModule(0x0096) failed with 0x%lx", ret);
			return ret;
		}

		if ((ret = sceCommonDialogInitialize()) < 0 && ret != 0x80B80002)
		{
			OrbisMessageHandler::Notify("sceCommonDialogInitialize() failed with 0x%lx", ret);
			return ret;
		}

		SceImeParamExtended* ptr = NULL;
		if (this->UseImeExtendedSettings)
		{
			ptr = &this->ImeParamExtended;
		}

		if ((ret = sceImeDialogInit(&this->ImeDialogParam, ptr)) < 0 && ret != 0x80BC0001)
		{
			OrbisMessageHandler::Notify("sceImeDialogInit() failed with 0x%lx", ret);
			return ret;
		}

		while (sceImeDialogGetStatus() != 2);

		if ((ret = sceImeDialogGetResult(&ImeDialogResult)) != 0)
		{
			OrbisMessageHandler::Notify("sceImeDialogGetResult(&ImeDialogResult) failed with 0x%lx", ret);
			return ret;
		}

		wcstombs(buf, this->ImeDialogParam.inputTextBuffer, 2048);
		
		free(this->ImeDialogParam.inputTextBuffer);
		this->ImeDialogParam.inputTextBuffer = nullptr;

		if ((ret = sceImeDialogTerm()) != 0)
		{
			OrbisMessageHandler::Notify("sceImeDialogTerm() failed with 0x%lx", ret);
			return ret;
		}

		return 0;
	}
}