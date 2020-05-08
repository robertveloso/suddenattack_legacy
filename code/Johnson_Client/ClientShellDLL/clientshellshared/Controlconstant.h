#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define USABLE_WIDTH_I 800
#define USABLE_WIDTH_F 800.0f
#define USABLE_HEIGHT_I 600
#define USABLE_HEIGHT_F 600.0f
#define USABLE_BPP_I	16
#define USABLE_BPP_F	16.0f

// sudden attack font style
#define SAFS_NORMAL		0x0
#define SAFS_BOLD		0x1
#define SAFS_ITALIC		0x2
#define SAFS_UNDERLINE	0x4
#define SAFS_STRIKE		0x8

const uint32	argbSelected	= 0xFFFFFFFF;
const uint32	argbNormal		= 0xFFB0B0B0;
const uint32	argbDisabled	= 0XFF505050;
const uint32	argbChecked		= 0XFFF00000;
const uint32	argbSAWhite		= 0XB0FFFFFF;
const uint32	argbSAGray		= 0XB0B0B0B0;
const uint32	argbSADarkGray	= 0XB0808080;
const uint32	argbSABlack		= 0XB0000000;
const uint32	argbSARed		= 0XB0FF0000;
const uint32	argbSAGreen		= 0XB000FF00;
const uint32	argbSABlue		= 0XB00000FF;
const uint32	argbSAYellow	= 0XB0FFFF00;
const uint32	argbSACyan		= 0XB000FFFF;
const uint32	argbSAViolet	= 0XB0FF00FF;


#define LO_BASIC_TAG					"BasicLayout"
#define LO_SELECT_TAG					"SelectionLayout"
#define LO_DEFAULT_TAG					"GenericScreen"
#define LO_HUD_TAG						"HUDLayout"
#define LO_MISC_TAG						"Miscellaneous"
#define LO_MASK_TAG						"Overlay"
#define LO_DIALOGUE_TAG					"DialogueWindow"
#define LO_DECISION_TAG					"DecisionWindow"
#define LO_MENU_TAG						"Menu"
#define LO_CHAR_TAG						"Character"
#define LO_LIGHT_TAG					"Light"
#define LO_FX_TAG						"FX"
#define LO_CREDITS_TAG					"Credits"
//--credit tag,button texture file path 지정--
//--Source Code Add START-[Date:07.01.2003,작성자:Cyon]
#define LO_BASIC_CREDIT_T               "CreditTex"
#define LO_BASIC_CREDIT_TH              "CreditTexH"
#define LO_BASIC_CREDIT_POS             "CreditPos"
//--Source Code Add E N D-[Date:07.01.2003,작성자:Cyon]
#define LO_TRANSITION_TAG				"Transition"
//---------------------------------------------------------------------------------------------
//Source code comment:"ok" button texture.
//---------------------------------------------------------------------------------------------
#define LO_BASIC_OK_T                   "OKButtonTex"   // quit messagebox ok button normal. 
#define LO_BASIC_OK_TH                  "OKButtonTexH"  // quit messagebox ok button select. 
#define LO_BASIC_OK_THH                 "OKButtonTexHH" // quit messagebox ok button noselect. 
#define LO_BASIC_OK_POS                 "OKButtonPos"   // quit messagebox ok button position.
//---------------------------------------------------------------------------------------------
//Source code comment:"cancel" button texture.
//---------------------------------------------------------------------------------------------
#define LO_BASIC_CANCEL_T               "CancelButtonTex"   // quit messagebox cancel normal.
#define LO_BASIC_CANCEL_TH              "CancelButtonTexH"  // quit messagebox cancel select.
#define LO_BASIC_CANCEL_THH             "CancelButtonTexHH" // quit messagebox cancel noselect.
#define LO_BASIC_CANCEL_POS             "CancelButtonPos"   // quit messagebox cancel position.
//---------------------------------------------------------------------------------------------
//Source code comment:"YES" button texture.
//---------------------------------------------------------------------------------------------
#define LO_BASIC_YES_T                  "YesButtonTex"   // quit messagebox Yes button normal. 
#define LO_BASIC_YES_TH                 "YesButtonTexH"  // quit messagebox Yes button select. 
#define LO_BASIC_YES_THH                "YesButtonTexHH" // quit messagebox Yes button noselect. 
#define LO_BASIC_YES_POS                "YesButtonPos"   // quit messagebox Yes button position.
//---------------------------------------------------------------------------------------------
//Source code comment:"No" button texture.
//---------------------------------------------------------------------------------------------
#define LO_BASIC_NO_T					"NoButtonTex"   // quit messagebox NO normal.
#define LO_BASIC_NO_TH					"NoButtonTexH"  // quit messagebox NO select.
#define LO_BASIC_NO_THH					"NoButtonTexHH" // quit messagebox NO noselect.
#define LO_BASIC_NO_POS					"NoButtonPos"   // quit messagebox NO position.
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에서 의 basic layout 에 해당.
//---------------------------------------------------------------------------------------------
#define LO_BASIC_HELP_RECT				"HelpRect"
#define LO_BASIC_HELP_FONT				"HelpFont"
#define LO_BASIC_HELP_SIZE				"HelpSize"
#define LO_BASIC_HUD_FONT				"HUDFont"
#define LO_BASIC_BACK_FONT				"BackFont"
#define LO_BASIC_BACK_SIZE				"BackSize"
#define LO_BASIC_BACK_POS				"BackPos"
#define LO_BASIC_NEXT_POS				"NextPos"
#define LO_BASIC_BACK_COLOR				"BackColor"
#define LO_BASIC_BACK_SPRITE			"BackSprite"
#define LO_BASIC_BACK_SCALE				"BackSpriteScale"
#define LO_BASIC_SLIDER					"SliderTex"
//->Source 추가 Start.[추가되어진 요일:19.10.2002]
#define LO_BASIC_SLIDER_BUTTON          "SliderButton"
//->Source 추가 End.  [추가       사람:Oh Se Jang]  [#|#] slider 와 연결 되어 있는 button. 
#define LO_BASIC_ARROW_BACK				"ArrowBackTex"
#define LO_BASIC_ARROW_BACK_H			"ArrowBackTexH"
#define LO_BASIC_ARROW_BACK_POS			"ArrowBackPos"
#define LO_BASIC_ARROW_NEXT				"ArrowNextTex"
#define LO_BASIC_ARROW_NEXT_H			"ArrowNextTexH"
#define LO_BASIC_ARROW_NEXT_POS			"ArrowNextPos"
//->Source 추가 Start.[추가되어진 요일:04.10.2002]
//---------------------------------------------------------------------------------------------
//Source code comment:mainmenu 에서 의 check button 구현.
//---------------------------------------------------------------------------------------------
#define LO_BASIC_CHECK_T                "CheckButtonTex"
#define LO_BASIC_CHECK_TH               "CheckButtonTexH"
#define LO_BASIC_CHECK_POS              "CheckButtonPos"
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에 정의 되어지는 single player texture name.  
//---------------------------------------------------------------------------------------------
#define LO_BASIC_SINGLE_T               "SingleTex" 
#define LO_BASIC_SINGLE_TH              "SingleTexH"
#define LO_BASIC_SINGLE_POS             "SinglePos"
//added by teseus << New Button Constant.
#define LO_CHRSLT_LEFTARROW_POS			"LeftArrowPos"
#define LO_CHRSLT_RIGHTARROW_POS		"RightArrowPos"
//added by teseus >>

//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에 정의 되어지는 single player texture name.  
//---------------------------------------------------------------------------------------------
#define LO_BASIC_SINGLE_TT               "SingleTexx" 
#define LO_BASIC_SINGLE_THH              "SingleTexHH"
#define LO_BASIC_SINGLE_POSS             "SinglePoss"
//--Source Code Add START-[Date:14.01.2003,작성자:Cyon]
//--[internet title button tag name]
#define LO_BASIC_INTERNETTITLE_T         "InternetTitleTex"
#define LO_BASIC_INTERNETTITLE_TH        "InternetTitleTexH"
#define LO_BASIC_INTERNETTITLE_POS       "InternetTitlePos"
//--[internet button tag name]
#define LO_BASIC_INTERNET_T              "InternetTex"
#define LO_BASIC_INTERNET_TH             "InternetTexH"
#define LO_BASIC_INTERNET_POS            "InternetPos"
//--Source Code Add E N D-[Date:14.01.2003,작성자:Cyon]

//--Source Code Add START-[Date:10.03.2002,Create:SPY]
//--[Original]
//--[Retouch ]  screenmulti page button.
#define LO_BASIC_MULTIGAME_T			 "GamePageButtonTex"
#define LO_BASIC_MULTIGAME_TH			 "GamePageButtonTexH"
#define LO_BASIC_MULTIGAME_POS			 "GamePageButtonPos"

#define LO_BASIC_MULTISETUP_T			 "SetupPageButtonTex"
#define LO_BASIC_MULTISETUP_TH			 "SetupPageButtonTexH"
#define LO_BASIC_MULTISETUP_POS			 "SetupPageButtonPos"

#define LO_BASIC_MULTIMAP_T				 "MapPageButtonTex"
#define LO_BASIC_MULTIMAP_TH			 "MapPageButtonTexH"
#define LO_BASIC_MULTIMAP_POS			 "MapPageButtonPos"

#define LO_BASIC_MULTI_ADVANCE_T		 "AdvancedPageButtonTex"
#define LO_BASIC_MULTI_ADVANCE_TH		 "AdvancedPageButtonTexH"
#define LO_BASIC_MULTI_ADVANCE_POS		 "AdvancedPageButtonPos"

#define LO_BASIC_MULTI_PLAYER_T		     "PlayerPageButtonTex"
#define LO_BASIC_MULTI_PLAYER_TH		 "PlayerPageButtonTexH"
#define LO_BASIC_MULTI_PLAYER_POS		 "PlayerPageButtonPos"
//--Source Code Add E N D-[Date:10.03.2002,Create:SPY]
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에서 정의 되어지는 multi player texture name.
//---------------------------------------------------------------------------------------------
#define LO_BASIC_MULTI_T                "MultiTex"
#define LO_BASIC_MULTI_TH               "MultiTexH"
#define LO_BASIC_MULTI_POS              "MultiPos"
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에서 정의 되어지는 multi player texture name.
//---------------------------------------------------------------------------------------------
#define LO_BASIC_MULTI_TT                "MultiTexx"
#define LO_BASIC_MULTI_THH               "MultiTexHH"
#define LO_BASIC_MULTI_POSS              "MultiPoss"
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에서 정의 되어지는 optins texture name.       
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_OPTION_T               "OptionTex"
#define LO_BAISIC_OPTION_TH              "OptionTexH"
#define LO_BAISIC_OPTION_POS             "OptionPos"
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에서 정의 되어지는 optins texture title name.       
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_OPTION_TT               "OptionTexx"
#define LO_BAISIC_OPTION_THH              "OptionTexHH"
#define LO_BAISIC_OPTION_POSS             "OptionPoss"
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에서 정의 되어지는 optins texture title name.       
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_QUIT_T                  "QuitTex"
#define LO_BAISIC_QUIT_TH                 "QuitTexH"
#define LO_BAISIC_QUIT_POS                "QuitPos"
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에서 정의 되어지는 Delete Button 속성 added by teseus       
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_DELETE_T                "DeleteTex"
#define LO_BAISIC_DELETE_TH               "DeleteTexH"
#define LO_BAISIC_DELETE_POS              "DeletePos"
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에서 정의 되어지는 OK Button 속성 added by teseus       
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_OK_T					"OKButtonTex"
#define LO_BAISIC_OK_TH					"OKButtonTexH"
#define LO_BAISIC_OK_POS				"OKButtonPos"
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에서 정의 되어지는 new game texture name.       
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_NEWGAME_T				 "NewGameTex"
#define LO_BAISIC_NEWGAME_TH			 "NewGameTexH"
#define LO_BAISIC_NEWGAME_POS			 "NewGamePos"
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에서 정의 되어지는 load game texture name.       
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_LOADGAME_T			 "LoadGameTex"
#define LO_BAISIC_LOADGAME_TH			 "LoadGameTexH"
#define LO_BAISIC_LOADGAME_POS			 "LoadGamePos"
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에서 정의 되어지는 new game texture name.       
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_NEWGAME_TT			 "NewGameTexx"
#define LO_BAISIC_NEWGAME_THH			 "NewGameTexHH"
#define LO_BAISIC_NEWGAME_POSS			 "NewGamePoss"
//---------------------------------------------------------------------------------------------
//Source code comment:layout.txt 에서 정의 되어지는 load game texture name.       
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_LOADGAME_TT			 "LoadGameTexx"
#define LO_BAISIC_LOADGAME_THH			 "LoadGameTexHH"
#define LO_BAISIC_LOADGAME_POSS			 "LoadGamePoss"
//---------------------------------------------------------------------------------------------
//Source code comment:display 에서 title texture button. 
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_VIDEOSETUP_TT			 "VideosetupTexx" 
#define LO_BAISIC_VIDEOSETUP_THH		 "VideosetupTexHH"
#define LO_BAISIC_VIDEOSETUP_POSS        "VideosetupPoss"

//---------------------------------------------------------------------------------------------
//Source code comment:display 에서 sub texture button.
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_VIDEOSETUP_T			 "VideosetupTex" 
#define LO_BAISIC_VIDEOSETUP_TH			 "VideosetupTexH"
#define LO_BAISIC_VIDEOSETUP_POS         "VideosetupPos"

//---------------------------------------------------------------------------------------------
//Source code comment:audiosetup.
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_AUDIOSETUP_T           "AudioSetupTex"
#define LO_BAISIC_AUDIOSETUP_TH          "AudioSetupTexH"
#define LO_BAISIC_AUDIOSETUP_POS         "AudioSetupPos" 

//---------------------------------------------------------------------------------------------
//Source code comment:audiosetupmain title
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_AUDIOSETUP_TT          "AudioSetupTexx"
#define LO_BAISIC_AUDIOSETUP_THH         "AudioSetupTexHH"
#define LO_BAISIC_AUDIOSETUP_POSS        "AudioSetupPoss" 

//---------------------------------------------------------------------------------------------
//Source code comment:control
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_CONTROLS_T             "ControlsTex"
#define LO_BAISIC_CONTROLS_TH            "ControlsTexH" 
#define LO_BAISIC_CONTROLS_POS           "ControlsPos" 

//---------------------------------------------------------------------------------------------
//Source code comment:control title.
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_CONTROLS_TT            "ControlsTexx"
#define LO_BAISIC_CONTROLS_THH           "ControlsTexHH" 
#define LO_BAISIC_CONTROLS_POSS          "ControlsPoss" 

#define LO_BAISIC_RESOLUTION_T           "ResolutionTex"
#define LO_BAISIC_RESOLUTION_TH          "ResolutionTexH"
#define LO_BAISIC_RESOLUTION_POS         "ResolutionPos"

#define LO_BAISIC_GAMMA_T                "GammaTex" 
#define LO_BAISIC_GAMMA_TH               "GammaTexH"
#define LO_BAISIC_GAMMA_POS              "GammaPos"

#define LO_BAISIC_MUSICVOLUME_T          "musicvolumeTex"        
#define LO_BAISIC_MUSICVOLUME_TH         "musicvolumeTexH"
#define LO_BAISIC_MUSICVOLUME_POS        "musicvolumePos"

#define LO_BAISIC_SFXVOIUME_T            "sfxvolumeTex"
#define LO_BAISIC_SFXVOIUME_TH           "sfxvolumeTexH"
#define LO_BAISIC_SFXVOIUME_POS          "sfxvolumePos"

#define LO_BAISIC_SOUNDQUALITY_T         "soundqualityTex"
#define LO_BAISIC_SOUNDQUALITY_TH        "soundqualityTexH"
#define LO_BAISIC_SOUNDQUALITY_POS       "soundqualityPos"

#define LO_BAISIC_CONFIGURE_T            "configureTex"
#define LO_BAISIC_CONFIGURE_TH           "configureTexH"
#define LO_BAISIC_CONFIGURE_POS          "configurePos"
//---------------------------------------------------------------------------------------------
//Source code comment:
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_CONFIGURE_TT            "configureTexx"
#define LO_BAISIC_CONFIGURE_THH           "configureTexHH"
#define LO_BAISIC_CONFIGURE_POSS          "configurePoss"

#define LO_BAISIC_MOUSE_T                "mouseTex"
#define LO_BAISIC_MOUSE_TH               "mouseTexH"
#define LO_BAISIC_MOUSE_POS              "mousePos"
//---------------------------------------------------------------------------------------------
//Source code comment:
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_MOUSE_TT                "mouseTexx"
#define LO_BAISIC_MOUSE_THH               "mouseTexHH"
#define LO_BAISIC_MOUSE_POSS              "mousePoss"

#define LO_BAISIC_KEYBOARD_T             "keyboadTex"
#define LO_BAISIC_KEYBOARD_TH            "keyboadTexH"
#define LO_BAISIC_KEYBOARD_POS           "keyboadPos"
//---------------------------------------------------------------------------------------------
//Source code comment:
//---------------------------------------------------------------------------------------------
#define LO_BAISIC_KEYBOARD_TT             "keyboadTexx"
#define LO_BAISIC_KEYBOARD_THH            "keyboadTexHH"
#define LO_BAISIC_KEYBOARD_POSS           "keyboadPoss"

#define LO_BAISIC_RESTOREDEFAULTS_T      "restoredefaultsTex"
#define LO_BAISIC_RESTOREDEFAULTS_TH     "restoredefaultsTexH"
#define LO_BAISIC_RESTOREDEFAULTS_POS    "restoredefaultsPos"

#define LO_BAISIC_ACTION_T               "ActionTex"
#define LO_BAISIC_ACTION_TH              "ActionTexH"
#define LO_BAISIC_ACTION_POS             "ActionPos"

#define LO_BAISIC_VIEW_T                 "ViewTex"
#define LO_BAISIC_VIEW_TH                "ViewTexH"
#define LO_BAISIC_VIEW_POS               "ViewPos"

#define LO_BAISIC_STATUS_T               "StatusTex"
#define LO_BAISIC_STATUS_TH              "StatusTexH"
#define LO_BAISIC_STATUS_POS             "StatusPos"

#define LO_BAISIC_MOVEMENT_T             "movementTex"
#define LO_BAISIC_MOVEMENT_TH            "movementTexH"
#define LO_BAISIC_MOVEMENT_POS           "movementPos"

#define LO_BAISIC_ALWAYSMOUSELOOK_T      "alwaysmouselookTex"
#define LO_BAISIC_ALWAYSMOUSELOOK_TH     "alwaysmouselookTexH"
#define LO_BAISIC_ALWAYSMOUSELOOK_POS    "alwaysmouselookPos"

#define LO_BAISIC_MOUSESENSITIVITY_T     "mousesensitivityTex"
#define LO_BAISIC_MOUSESENSITIVITY_TH    "mousesensitivityTexH"
#define LO_BAISIC_MOUSESENSITIVITY_POS   "mousesensitivityPos"

#define LO_BAISIC_SMOOTHMOUSE_T          "smoothmouseTex"
#define LO_BAISIC_SMOOTHMOUSE_TH         "smoothmouseTexH"
#define LO_BAISIC_SMOOTHMOUSE_POS        "smoothmousePos"

#define LO_BAISIC_INVERTMOUSE_T          "invertmouseTex"
#define LO_BAISIC_INVERTMOUSE_TH         "invertmouseTexH"
#define LO_BAISIC_INVERTMOUSE_POS        "invertmousePos"

#define LO_BAISIC_YES_T                  "YesTex"
#define LO_BAISIC_YES_TH                 "YesTexH"
#define LO_BAISIC_YES_POS                "YesPos"

#define LO_BAISIC_NO_T                   "NoTex"
#define LO_BAISIC_NO_TH                  "NoTexH"
#define LO_BAISIC_NO_POS                 "NoPos"

#define LO_BAISIC_UP_DOWNLOOK_T          "Up_downlookTex"
#define LO_BAISIC_UP_DOWNLOOK_TH         "Up_downlookTexH"
#define LO_BAISIC_UP_DOWNLOOK_POS        "Up_downlookPos"

#define LO_BAISIC_UP_TURNSPEED_T         "turnspeedTex"
#define LO_BAISIC_UP_TURNSPEED_TH        "turnspeedTexH"
#define LO_BAISIC_UP_TURNSPEED_POS       "turnspeedPos"

//--[SCREEN PROFILE BUTTON]
#define LO_BASIC_PROFILE_T				"ProfileTex"
#define LO_BASIC_PROFILE_TH				"ProfileTexH"
#define LO_BASIC_PROFILE_POS            "ProfilePos"
//--[SCREEN PROFILE TITLE BAR]
#define LO_BASIC_PROFILETITLE_T         "ProfiletitelTex"
#define LO_BASIC_PROFILETITLE_TH        "ProfiletitleTexH"
#define LO_BASIC_PROFILETITLE_POS       "ProfiletitlePos"
//--[SCREEN MULTIPLAYER BUTTON]
#define LO_BASIC_PLAYERSETUP_T			"PlayerSetupTex"
#define LO_BASIC_PLAYERSETUP_TH         "PlayerSetupTexH"
#define LO_BASIC_PLAYERSETUP_POS		"PlayerSetupPos"
//--[SCREEN MULTIPLAYER JOIN BUTTON]
#define LO_BASIC_JOIN_T					"JoinTex"
#define LO_BASIC_JOIN_TH				"JoinTexH"
#define LO_BASIC_JOIN_POS				"JoinPos"
//--[SCREEN MULTIPLAYER HOST BUTTON]
#define LO_BASIC_HOST_T					"HostTex"
#define LO_BASIC_HOST_TH				"HostTexH"
#define LO_BASIC_HOST_POS				"HostPos"
//--[SCREEN MULTIPLAYER FINDSERVER BUTTON]
#define LO_BASIC_FINDSERVER_T			"FindServerTex"
#define LO_BASIC_FINDSERVER_TH			"FindServerTexH"
#define LO_BASIC_FINDSERVER_POS			"FindServerPos"
//--[SCREEN MULTIPLAYER NETWORKPORT BUTTON]
#define LO_BASIC_NETWORKPORT_T			"NetworkportTex"
#define LO_BASIC_NETWORKPORT_TH			"NetworkportTexH"
#define LO_BASIC_NETWORKPORT_POS		"NetworkportPos"
//--[SCREEN MULTIPLAYER TITLE BAR]
#define LO_BASIC_PLAYERSETUP_TITLE_T		"PlayerSetupTitleTex"
#define LO_BASIC_PLAYERSETUP_TITLE_TH        "PlayerSetupTitleTexH"
#define LO_BASIC_PLAYERSETUP_TITLE_POS		"PlayerSetupTitlePos"
//--[SCREEN MULTIPLAYER JOIN TITLE BAR]
#define LO_BASIC_JOIN_TITLE_T				"JoinTitleTex"
#define LO_BASIC_JOIN_TITLE_TH				"JoinTitleTexH"
#define LO_BASIC_JOIN_TITLE_POS				"JoinTitlePos"
//--[SCREEN MULTIPLAYER HOST TITLE BAR]
#define LO_BASIC_HOST_TITLE_T				"HostTitleTex"
#define LO_BASIC_HOST_TITLE_TH				"HostTitleTexH"
#define LO_BASIC_HOST_TITLE_POS				"HostTitlePos"
//--[SCREEN MULTIPLAYER HOST LAUNCHGAME BUTTON]
#define LO_BASIC_LAUNCHGAME_T				"LaunchGameTex"
#define LO_BASIC_LAUNCHGAME_TH				"LaunchGameTexH"
#define LO_BASIC_LAUNCHGAME_POS				"LaunchGamePos"

//--Source Code Add START-[Date:25.02.2003,작성자:Cyon]
//--[Original]
//--[Retouch ] 
// select rect frame. 
#define LO_BASIC_SELECT_RECT				"SelectRectTex" 
//--Source Code Add E N D-[Date:25.02.2003,작성자:Cyon]

//->Source 추가 End.  [추가       사람:Oh Se Jang]  [#|#] 
#define LO_BASIC_BOSS_RECT				"BossRect"

#define LO_SCREEN_TITLE_POS				"TitlePos"
#define LO_SCREEN_TITLE_FONT			"TitleFont"
#define LO_SCREEN_TITLE_SIZE			"TitleSize"
#define LO_SCREEN_BACKGROUND			"Background"
#define LO_SCREEN_PAGE_RECT				"PageRect"
#define LO_SCREEN_ITEM_FONT				"FontFace"
#define LO_SCREEN_ITEM_SIZE				"FontSize"
#define LO_SCREEN_ITEM_SPACE			"ItemSpace"
#define LO_SCREEN_ITEM_ALIGN			"ItemAlign"
#define LO_SCREEN_MUSIC_INTENSITY		"MusicIntensity"
#define LO_SCREEN_CHARACTER				"Character"
#define LO_SCREEN_ATTACH				"Attachment"

#define LO_HUD_LAYOUT_NAME				"LayoutName"

#define	LO_HUD_AMMO_BASEPOS				"AmmoBasePos"
#define	LO_HUD_AMMO_USEBAR				"UseAmmoBar"
#define	LO_HUD_AMMO_BAROFF				"AmmoClipOffset"
#define	LO_HUD_AMMO_CLIPSZ				"AmmoClipUnitSize"
#define	LO_HUD_AMMO_OFFSET				"AmmoBarOffset"
#define	LO_HUD_AMMO_USETEXT				"UseAmmoText"
#define	LO_HUD_AMMO_TEXTOFF				"AmmoTextOffset"
#define	LO_HUD_AMMO_ICONOFF				"AmmoIconOffset"
#define	LO_HUD_AMMO_ICONSZ				"AmmoIconSize"
#define	LO_HUD_AMMO_ALPHA				"AmmoTextAlpha"
#define	LO_HUD_AMMO_COLOR				"AmmoTextColor"

#define	LO_HUD_HEALTH_BASEPOS			"HealthBasePos"
#define	LO_HUD_HEALTH_USEBAR			"UseHealthBar"
#define	LO_HUD_HEALTH_BAROFF			"HealthBarOffset"
#define	LO_HUD_ARMOR_OFFSET				"ArmorBarOffset"
#define	LO_HUD_HEALTH_USETEXT			"UseHealthText"
#define	LO_HUD_HEALTH_TEXTOFF			"HealthTextOffset"
#define	LO_HUD_ARMOR_TEXTOFF			"ArmorTextOffset"
#define	LO_HUD_HEALTH_USEICON			"UseHealthIcon"
#define	LO_HUD_HEALTH_ICONOFF			"HealthIconOffset"
#define	LO_HUD_ARMOR_ICONOFF			"ArmorIconOffset"
#define	LO_HUD_HEALTH_ICONSZ			"HealthIconSize"
#define	LO_HUD_HEALTH_ALPHA				"HealthAlpha"
#define	LO_HUD_HEALTH_COLOR				"HealthColor"
#define	LO_HUD_ARMOR_COLOR				"ArmorColor"

//->Source 추가 Start.[추가되어진 요일:01.12.2003]
#define	LO_HUD_GUAGE_USETEXT			"UseGuageText"
#define	LO_HUD_GUAGE_USETEXTSIZE		"UseGuageTextSizeHeight"
#define LO_HUD_GUAGE_USETEXTSIZEWIDTH   "UseGuageTextSizeWidth" 
//->Source 추가 End.  [추가       사람:monk77]  [#|#] 


#define	LO_HUD_AIR_BASEPOS				"AirBasePos"
#define	LO_HUD_AIR_USEICON				"UseAirIcon"
#define	LO_HUD_AIR_ICONOFF				"AirIconOffset"
#define	LO_HUD_AIR_ICONSZ				"AirIconSize"
#define	LO_HUD_AIR_USETEXT				"UseAirText"
#define	LO_HUD_AIR_TEXTOFF				"AirTextOffset"
#define	LO_HUD_AIR_USEBAR				"UseAirBar"
#define	LO_HUD_AIR_BAROFF				"AirBarOffset"
#define	LO_HUD_AIR_ALPHA				"AirAlpha"
#define	LO_HUD_AIR_COLOR				"AirColor"

#define	LO_HUD_BAR_HEIGHT				"BarHeight"
#define	LO_HUD_BAR_SCALE				"BarScale"
#define	LO_HUD_TEXT_HEIGHT				"TextHeight"

#define LO_HUD_DAMAGE_BASEPOS			"DamageBasePos" 
#define LO_HUD_DAMAGE_ICON_SZ			"DamageIconSize"

#define LO_HUD_COMPASS_POS				"CompassPos"
#define LO_HUD_COMPASS_SZ				"CompassSize"

#define LO_HUD_RADAR_OBJSZ				"RadarObjectSize"
#define LO_HUD_RADAR_MAXDIST			"RadarMaxDistance"
#define LO_HUD_RADAR_LIVE				"RadarLivePlayerColor"
#define LO_HUD_RADAR_DEAD				"RadarDeadPlayerColor"
#define LO_HUD_RADAR_TALK				"RadarTalkPlayerColor"
#define LO_HUD_RADAR_FLASH				"RadarFlashTime"

#define LO_HUD_DAMAGE_SZ				"DamageSize"

#define LO_HUD_CARRY_POS				"CarryPos"
#define LO_HUD_CARRY_SZ					"CarrySize"

//  [12/19/2003]GameHi
#define LO_HUD_ARMOR_POS				"ArmorPos"
#define LO_HUD_ARMOR_SZ					"ArmorSize"

#define LO_HUD_HEALTH_POS				"HealthPos"
#define LO_HUD_HEALTH_SZ				"HealthSize"

#define LO_HUD_BULLET_BAR_POS			"BulletBarPos"
#define LO_HUD_BULLET_BAR_SZ			"BulletBarSize"

#define LO_HUD_TIME_BAR_POS				"TimeBarPos"
#define LO_HUD_TIME_BAR_SZ				"TimeBarSize"
//  [12/19/2003]GameHi

#define LO_HUD_OBJ_POS					"ObjectivePos"
#define LO_HUD_OBJ_SZ					"ObjectiveSize"
#define LO_HUD_OBJ_BLINKDUR				"ObjectiveBlinkDuration"
#define LO_HUD_OBJ_BLINKSPD				"ObjectiveBlinkSpeed"

#define LO_HUD_HIDE_POS					"HideIconPos"
#define LO_HUD_HIDE_SZ					"HideIconSize"
#define LO_HUD_HIDE_SPEED				"HideIconBlinkSpeed"
#define LO_HUD_HIDE_ALPHA				"HideIconAlpha"
#define	LO_HUD_HIDE_BASEPOSY			"HidingBarBasePosY"
#define LO_HUD_HIDE_OFFSET				"HidingBarOffset"
#define LO_HUD_HIDE_HEIGHT				"HidingBarHeight"
#define LO_HUD_HIDE_SCALE				"HidingBarScale"
#define LO_HUD_HIDE_TEX					"HidingBarTexture"

#define LO_HUD_DIST_POS					"DistanceIconPos"
#define LO_HUD_DIST_SPEED				"DistanceIconBlinkSpeed"
#define LO_HUD_DIST_ALPHA				"DistanceIconAlpha"
#define LO_HUD_DIST_FADESPEED			"DistanceIconFadeOutSpeed"

#define LO_HUD_ACT_POS					"ActivationTextPos"
#define LO_HUD_ACT_SZ					"ActivationTextSize"
#define LO_HUD_ACT_JUST					"ActivationTextJustify"
#define LO_HUD_ACT_ALPHA				"ActivationTextAlpha"
#define LO_HUD_ACT_COLOR				"ActivationTextColor"
// #burumal #enemy flag
// {
#define LO_HUD_ACT_COLOR2				"ActivationTextColor2"
// }
#define LO_HUD_ACT_DIS_ALPHA			"ActivationTextDisAlpha"
#define LO_HUD_ACT_DIS_COLOR			"ActivationTextDisColor"

#define LO_HUD_DBG_POS					"DebugTextPos"
#define LO_HUD_DBG_SZ					"DebugTextSize"
#define LO_HUD_DBG_WD					"DebugTextWidth"
#define LO_HUD_DBG_JUST					"DebugTextJustify"
#define LO_HUD_DBG_ALPHA				"DebugTextAlpha"
#define LO_HUD_DBG_COLOR				"DebugTextColor"

#define LO_HUD_CHS_SZ					"ChooserTextSize"
#define LO_HUD_CHS_TEX					"ChooserTextureScale"
#define LO_HUD_CHS_HT					"ChooserIconHeight"
#define LO_HUD_CHS_ALPHA				"ChooserTextAlpha"
#define LO_HUD_CHS_COLOR				"ChooserTextColor"

#define	LO_HUD_PROG_BASEPOSY			"ProgressBarBasePosY"
#define LO_HUD_PROG_OFFSET				"ProgressBarOffset"
#define LO_HUD_PROG_HEIGHT				"ProgressBarHeight"
#define LO_HUD_PROG_SCALE				"ProgressBarScale"
#define LO_HUD_PROG_TEX					"ProgressBarTexture"

#define	LO_HUD_DISPLAYMETER_BASEPOSY	"DisplayMeterBasePosY"
#define LO_HUD_DISPLAYMETER_OFFSET		"DisplayMeterOffset"
#define LO_HUD_DISPLAYMETER_HEIGHT		"DisplayMeterHeight"
#define LO_HUD_DISPLAYMETER_SCALE		"DisplayMeterScale"
#define LO_HUD_DISPLAYMETER_TEX			"DisplayMeterTexture"

#define	LO_HUD_MODE_TEXTPOS				"ModeTextPos"

// 광식
#define LO_HUD_CHAT_POS					"ChatBackImagePos"


#define	LO_MISC_SV_MODEL				"SpyVisionModelColor"
#define	LO_MISC_SV_LIGHT				"SpyVisionLightScale"
#define	LO_MISC_WPN_COLOR				"WeaponPickupColor"
#define	LO_MISC_AMMO_COLOR				"AmmoPickupColor"
#define	LO_MISC_TINTTIME				"TintTime"
#define	LO_MISC_GAPMIN					"CrosshairGapMin"
#define	LO_MISC_GAPMAX					"CrosshairGapMax"
#define	LO_MISC_BARMIN					"CrosshairBarMin"
#define	LO_MISC_BARMAX					"CrosshairBarMax"
#define	LO_MISC_ROTEFFECT				"PerturbRotationEffect"
#define	LO_MISC_PERTURBINC				"PerturbIncreaseSpeed"
#define	LO_MISC_PERTURBDEC				"PerturbDecreaseSpeed"
#define	LO_MISC_WALKPER					"PerturbWalkPercent"
#define LO_MISC_DLG_FRAME				"DialogFrame"
#define LO_MISC_DLG_FONT_FACE			"DialogFontFace"
#define LO_MISC_DLG_FONT_SIZE			"DialogFontSize"
#define LO_MISC_DEATHDELAY				"DeathDelay"
#define LO_MISC_MSG_FADE				"MessageMinimumFade"
#define LO_MISC_MSG_TIME				"MessageMinimumTime"
#define LO_MISC_OBJ_RECT				"ObjectiveRect"
#define LO_MISC_POPUP_RECT				"PopupTextRect"
#define LO_MISC_SUB_TINT				"SubtitleTint"
#define LO_MISC_HEALTH_TINT				"HealthTint"
#define LO_MISC_ARMOR_TINT				"ArmorTint"
#define LO_MISC_AMMO_TINT				"AmmoTint"
#define LO_MISC_POPUP_TINT				"PopupTint"

#define	LO_MASK_SPRITE					"Sprite"
#define	LO_MASK_MODEL					"Model"
#define	LO_MASK_SKIN					"Skin"
#define	LO_MASK_SCALE					"Scale"
#define	LO_MASK_ALPHA					"Alpha"

#define	LO_CHAR_NAME					"Name"
#define	LO_CHAR_MOD						"Model"
#define	LO_CHAR_SKIN					"Skin"
#define	LO_CHAR_STYLE					"RenderStyle"
#define	LO_CHAR_POS						"Pos"
#define	LO_CHAR_SCALE					"Scale"
#define	LO_CHAR_ROT						"Rotation"

#define	LO_LIGHT_NAME					"Name"
#define	LO_LIGHT_POS					"Pos"
#define	LO_LIGHT_COLOR					"Color"
#define	LO_LIGHT_RADIUS					"Radius"

// credit setting (layout.txt 의 Credits )value 
#define	LO_CREDITS_FADEIN				"FadeInTime"
#define	LO_CREDITS_HOLD					"HoldTime"
#define	LO_CREDITS_FADEOUT				"FadeOutTime"
#define	LO_CREDITS_DELAY				"DelayTime"
#define	LO_CREDITS_POS_UL				"PositionUL"
#define	LO_CREDITS_POS_UR				"PositionUR"
#define	LO_CREDITS_POS_LR				"PositionLR"
#define	LO_CREDITS_POS_LL				"PositionLL"

#define LO_DEFAULT_MENU_TAG				"GenericMenu"
#define LO_MENU_FONT					"Font"
#define LO_MENU_FONT_SIZE				"FontSize"
#define LO_MENU_TITLE_FONT				"TitleFont"
#define LO_MENU_TITLE_FONT_SIZE			"TitleSize"
#define LO_MENU_SIZE					"Size"
#define LO_MENU_FRAME					"Frame"
#define LO_MENU_FRAME_TIP				"FrameTip"

#define LO_MENU_ARROW_U					"UpArrow"
#define LO_MENU_ARROW_U_H				"UpArrowH"
#define LO_MENU_ARROW_D					"DownArrow"
#define LO_MENU_ARROW_D_H				"DownArrowH"
#define LO_MENU_ARROW_L					"LeftArrow"
#define LO_MENU_ARROW_L_H				"LeftArrowH"
#define LO_MENU_ARROW_R					"RightArrow"
#define LO_MENU_ARROW_R_H				"RightArrowH"

#define LO_MENU_ARROW_S_U				"UpArrowS"
#define LO_MENU_ARROW_S_U_H				"UpArrowSH"
#define LO_MENU_ARROW_S_D				"DownArrowS"
#define LO_MENU_ARROW_S_D_H				"DownArrowSH"
#define LO_MENU_ARROW_S_L				"LeftArrowS"
#define LO_MENU_ARROW_S_L_H				"LeftArrowSH"
#define LO_MENU_ARROW_S_R				"RightArrowS"
#define LO_MENU_ARROW_S_R_H				"RightArrowSH"

/*
#define LO_MENU_CLOSE					"Close"
#define LO_MENU_CLOSE_H					"CloseH"
*/
#define LO_MENU_INDENT					"Indent"
#define LO_MENU_POS						"Position"
#define LO_MENU_IN_TIME					"SlideInTime"
#define LO_MENU_OUT_TIME				"SlideOutTime"

// screen menu 에서의 button color.
#define LO_SELECTED_COLOR				"SelectedColor" 
#define LO_NONSELECTED_COLOR			"NonSelectedColor" 
#define LO_DISABLED_COLOR				"DisabledColor"

// Added by IN'T ===>
// 기본 화살버튼(ex. 스크롤바에서의 화살버튼)
// D : Default , H = Hover
#define TX_ARROWUP_D				"sa_interface\\menu\\Textures\\Common\\arrowup.dtx"
#define TX_ARROWUP_H				"sa_interface\\menu\\Textures\\Common\\arrowup_h.dtx"
#define TX_ARROWDOWN_D				"sa_interface\\menu\\Textures\\Common\\arrowdn.dtx"
#define TX_ARROWDOWN_H				"sa_interface\\menu\\Textures\\Common\\arrowdn_h.dtx"
#define TX_ARROWLEFT_D				"sa_interface\\menu\\Textures\\Common\\arrowlt.dtx"
#define TX_ARROWLEFT_H				"sa_interface\\menu\\Textures\\Common\\arrowlt_h.dtx"
#define TX_ARROWRIGHT_D				"sa_interface\\menu\\Textures\\Common\\arrowrt.dtx"
#define TX_ARROWRIGHT_H				"sa_interface\\menu\\Textures\\Common\\arrowrt_h.dtx"

#define TX_ARROWUP2_D				"sa_interface\\menu\\Textures\\Common\\arrowup2.dtx"
#define TX_ARROWUP2_H				"sa_interface\\menu\\Textures\\Common\\arrowup2_h.dtx"
#define TX_ARROWDOWN2_D				"sa_interface\\menu\\Textures\\Common\\arrowdn2.dtx"
#define TX_ARROWDOWN2_H				"sa_interface\\menu\\Textures\\Common\\arrowdn2_h.dtx"
#define TX_ARROWLEFT2_D				"sa_interface\\menu\\Textures\\Common\\arrowlt2.dtx"
#define TX_ARROWLEFT2_H				"sa_interface\\menu\\Textures\\Common\\arrowlt2_h.dtx"
#define TX_ARROWRIGHT2_D			"sa_interface\\menu\\Textures\\Common\\arrowrt2.dtx"
#define TX_ARROWRIGHT2_H			"sa_interface\\menu\\Textures\\Common\\arrowrt2_h.dtx"

#define TX_COUNT					"sa_interface\\menu\\Textures\\Common\\txCount.dtx"
#define TX_DIALOG_FRAME				"sa_interface\\menu\\Textures\\Common\\DialogFrame.dtx"
#define TX_DEFAULT_FRAME			"sa_interface\\menu\\Textures\\Common\\DefaultFrame.dtx"

#define TX_SCORE_FRAME				"sa_interface\\menu\\Textures\\Common\\ScoreFrame.dtx"

//광식
// Main Weapon
#define TX_WEAPON_AKS74_LARGE		"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\AKs74u128Tx.dtx"
#define TX_WEAPON_AKS74_SMALL		"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\AKs74u64Tx.dtx"
#define TX_WEAPON_M4A1_LARGE		"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\M4A1128Tx.dtx"
#define TX_WEAPON_M4A1_SMALL		"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\M4A164Tx.dtx"
#define TX_WEAPON_MP5_LARGE			"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\MP5128Tx.dtx"
#define TX_WEAPON_MP5_SMALL			"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\MP564Tx.dtx"
#define TX_WEAPON_MSG90_LARGE		"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\MSG90A128Tx.dtx"
#define TX_WEAPON_MSG90_SMALL		"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\MSG90A64Tx.dtx"
#define TX_WEAPON_DRAGUNOV_LARGE	"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\Dragunov128Tx.dtx"
#define TX_WEAPON_DRAGUNOV_SMALL	"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\Dragunov64Tx.dtx"
// Sub Weapon
#define TX_WEAPON_ANACONDA_LARGE	"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\Anaconda128Tx.dtx"
#define TX_WEAPON_ANACONDA_SMALL	"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\Anaconda64Tx.dtx"
#define TX_WEAPON_DESERTEGLE_LARGE	"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\DesertEgle128Tx.dtx"
#define TX_WEAPON_DESERTEGLE_SMALL	"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\DesertEgle64Tx.dtx"
// Etc Weapon
#define TX_WEAPON_GRENADE_LARGE		"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\Grenade128Tx.dtx"		
#define TX_WEAPON_GRENADE_SMALL		"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\Grenade64Tx.dtx"
#define TX_WEAPON_KNIFE_LARGE		"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\Knife128Tx.dtx"
#define TX_WEAPON_KNIFE_SMALL		"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\Knife64Tx.dtx"
// Default
#define TX_WEAPON_DEFAULT_LARGE		"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\EmptyLargeTx.dtx"
#define	TX_WEAPON_DEFAULT_SMALL		"sa_interface\\menu\\Textures\\weapon\\WeaponTx\\EmptySmallTx.dtx"
// Title
#define	TX_WEAPON_AKS74_TITLE		"sa_interface\\menu\\Textures\\weapon\\Title\\Aks74TitleTx.dtx"
#define TX_WEAPON_M4A1_TITLE		"sa_interface\\menu\\Textures\\weapon\\Title\\M4A3TitleTx.dtx"
#define TX_WEAPON_MP5_TITLE			"sa_interface\\menu\\Textures\\weapon\\Title\\MP5TitleTx.dtx"
#define TX_WEAPON_MSG90_TITLE		"sa_interface\\menu\\Textures\\weapon\\Title\\Msg90TitleTx.dtx"
#define TX_WEAPON_DRAGUNOV_TITLE	"sa_interface\\menu\\Textures\\weapon\\Title\\DragunovTitleTx.dtx"
#define TX_WEAPON_ANACONDA_TITLE	"sa_interface\\menu\\Textures\\weapon\\Title\\AnacondaTitleTx.dtx"
#define	TX_WEAPON_DESERTEGLE_TITLE	"sa_interface\\menu\\Textures\\weapon\\Title\\DesertEagleTitleTx.dtx"
#define	TX_WEAPON_GRENADE_TITLE		"sa_interface\\menu\\Textures\\weapon\\Title\\GrenadeTitleTx.dtx"
#define TX_WEAPON_KNIFE_TITLE		"sa_interface\\menu\\Textures\\weapon\\Title\\KnifeTitleTx.dtx"
#define	TX_WEAPON_DEFAULT_TITLE		"sa_interface\\menu\\Textures\\weapon\\Title\\EmptyTitleTx.dtx"
// Team
#define TX_TEAM0_PICTURE0			"sa_interface\\menu\\Textures\\Team\\BlueFirst.dtx"
#define TX_TEAM0_PICTURE1			"sa_interface\\menu\\Textures\\Team\\BlueSecond.dtx"
#define TX_TEAM1_PICTURE0			"sa_interface\\menu\\Textures\\Team\\RedFirst.dtx"
#define TX_TEAM1_PICTURE1			"sa_interface\\menu\\Textures\\Team\\RedSecond.dtx"
