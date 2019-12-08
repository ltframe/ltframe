#include "StdAfx.h"
#include "LTUtils.h"




LTMusic::LTMusic(HWND m_hWnd):m_stream(0)
{
	BASS_Init(-1,44100,0,m_hWnd,NULL);
	BASS_PluginLoad("basswma.dll", 0);
	BASS_PluginLoad("bassflac.dll", 0);
	BASS_PluginLoad("bass_aac.dll",0);
	BASS_PluginLoad("bass_ape.dll",0);
}

LTMusic::~LTMusic(void)
{
	BASS_Free();
}

bool LTMusic::MusicPlay(wchar_t* filename)
{
    m_stream =BASS_StreamCreateFile(FALSE,filename,0,0,BASS_UNICODE);
	if(!m_stream)
	{
		return false;
	}
	BASS_ChannelPlay(m_stream,FALSE);
	return true;
}
void LTMusic::MusicStop()
{
	BASS_ChannelStop(m_stream);
	BASS_StreamFree(m_stream);
	m_stream = 0;
}


double LTMusic::CurrentMusicTime()
{
	QWORD  qword = BASS_ChannelGetPosition(m_stream,BASS_POS_BYTE);
	double s = BASS_ChannelBytes2Seconds(m_stream,qword);
	return s;
}
char* LTMusic::Seconds2Time(int sec)
{
	int mm=sec/60;
	int se=sec%60;
	char* _pmm = "";
	char* _pse = "";
	if(mm<10)
	{
		_pmm = "0";
	}else{_pmm="";}
	if(se<10)
	{
		_pse = "0";
	}else{_pse="";}
	char cs[MAX_PATH];
	sprintf(cs,"%s%d:%s%d",_pmm,mm,_pse,se);
	return cs;
}

double LTMusic::GetCurrentMusicSeconds()
{
	return GetMusicSeconds(m_stream);
}
double LTMusic::GetMusicSeconds(HSTREAM m_stream)
{
	QWORD len=BASS_ChannelGetLength(m_stream, BASS_POS_BYTE); 
	double time=BASS_ChannelBytes2Seconds(m_stream, len); // the length in seconds
	return time;
}

MusicBasicInfo LTMusic::GetMusicBasicInformation(char* filepath)
{

	MusicBasicInfo info={};
	strcpy(info.path,filepath);
	char* const g_fmt = "%IFV2(%ITRM(%TITL),%ITRM(%TITL),)|%IFV2(%ITRM(%ARTI),%ITRM(%ARTI),)";

	HSTREAM handle =BASS_StreamCreateFile(FALSE,filepath,0,0,0);
    char* str =(char*) TAGS_Read(handle, g_fmt);
	info.dtimes=GetMusicSeconds(handle);
	strcpy(info.times,Seconds2Time(info.dtimes));
	//char* str ="";
	char _str[MAX_PATH] = {0};
	strcpy(_str,str);
	BASS_StreamFree(handle);
	char * token = strtok(_str,"|");
	int i=0;
	
	while( token != NULL)
     {
		if(i==0){
			if(token!=""){
				strcpy(info.title,token);
			}else
			{
				char * fpath = CLTCommon::GetFileInfo(filepath);
				strcpy(info.title,fpath);
				free(fpath);
			}
		}else{
			if(token!="")
				strcpy(info.author,token);
		}
        token = strtok(NULL,"|"); 
		i++;
     }
	if(strcmp(info.title,"")==0)
	{
		char * fpath = CLTCommon::GetFileInfo(filepath);
		strcpy(info.title,fpath);
		strcpy(info.author,"");
		free(fpath);
	}
	return info;
}


HSTREAM LTMusic::GetMusicHand()
{
	return m_stream;
}
int LTMusic::GetMusicStatus()
{
	return BASS_ChannelIsActive(m_stream);
}

bool LTMusic::IsPlayMusic()
{
	return m_stream ? 1:0;
}

void LTMusic::SetMusicPosition(double seconds)
{
	QWORD  p = BASS_ChannelSeconds2Bytes(m_stream,seconds);
	BASS_ChannelSetPosition(m_stream,p,BASS_POS_BYTE);
}

void LTMusic::SetMusicVolume(float volume)
{
	BASS_ChannelSetAttribute(m_stream,BASS_ATTRIB_VOL,volume);
}


