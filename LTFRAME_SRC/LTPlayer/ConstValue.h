typedef struct _MusicBasicInfo
{
	char title[1000];
	char author[100];
	double dtimes;
	char times[100];
	char path[MAX_PATH];
}MusicBasicInfo;

typedef struct _FileInfo
{
	char* fielname;
	char* fileext;
}FileInfo;

enum MinWindowType {
	Hide = 1,
	Min
};


typedef struct _AppConfigInfo
{
	bool alwaytray;
	int volumerange;
}AppConfigInfo;

//从列表删除记录
#define WM_REMOVEMUSICRECORD (WM_USER+100)
//打开文件所在文件夹
#define WM_OPENFILEFOLDER (WM_USER+102)

#define WM_MYCLOSEWINDOW_MESSAGE (WM_USER+103)

#define WM_SETCONVER_MESSAGE (WM_USER+104)


//#define WM_USERWINDOW_ACTION_MESSAGE (WM_USER+105)

#define WM_CLOSEMAINWINDOW_MESSAGE (WM_USER+106)
#define WM_SETVOLUMEVALUE_MESSAGE (WM_USER+107)
#define WM_SETCONTEXTMENUCLOSE_MESSAGE  (WM_USER+108)


#define WM_SETWINDOWENABLE_MESSAGE (WM_USER+109)


#define WM_CONTEXTPLAY_MESSAGE (WM_USER+110) 
#define WM_SETTRAYCONFIG_MESSAGE (WM_USER+111) 

#define WM_SETTRAYMENUVALUE_MESSAGE (WM_USER+112) 


#define WM_SETPREVIOUSMUSIC_MESSAGE (WM_USER+113) 
#define WM_SETPLAYMUISC_MESSAGE (WM_USER+114) 
#define WM_SETNEXTMUSIC_MESSAGE (WM_USER+115)



#define MUSICLIST_DATA "UserData\\config\\fileslist.dat"
#define PLAYCONFIG_DATA "UserData\\config\\config.dat"
#define TEMPLATEPATH "UserData\\template\\"








