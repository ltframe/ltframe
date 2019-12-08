/**********************************************************************************************
	* Author:	Oso, Oluwafemi Ebenezer
	* Date:		09th of September, 2013
	* Filename:	tagHelper.cpp

	A wrapper for the id3lib library
***********************************************************************************************/
#include "stdafx.h"
#include "tagHelper.h"

tagHelper::tagHelper(char* path)
{
	isLinked = false;
	if(myTag.Link(path) > 0 && (mp3header = myTag.GetMp3HeaderInfo()) != 0)
	{
		isLinked = true;
	}
}

bool tagHelper::linked()
{
	return isLinked;
}

bool tagHelper::hasLyrics()
{
	return myTag.HasLyrics();
}

bool tagHelper::hasV1Tag()
{
	return myTag.HasV1Tag();
}

bool tagHelper::hasV2Tag()
{
	return myTag.HasV2Tag();
}

int tagHelper::fileSize()
{
	return (int)myTag.GetFileSize();
}

tagHelper::~tagHelper()
{
}

MP3_BitRates tagHelper::getCbrBitRate()
{
	if(isLinked)
		return mp3header->bitrate;
	else
		return MP3BITRATE_FALSE;
}


uint32 tagHelper::getVbrBitRate()
{
	if(isLinked)
		return mp3header->vbr_bitrate;
	else
		return NULL;
} 

uint32 tagHelper::getSampleRate()
{
	if(isLinked)
		return mp3header->frequency;
	else
		return NULL;
}

uint32 tagHelper::getTrackLength()
{
	if(isLinked)
		return mp3header->time;
	else
		return NULL;
}

Mpeg_Layers tagHelper::getMPEGLayer()
{
	if(isLinked)
		return mp3header->layer;
	else
		return MPEGLAYER_FALSE;
}

Mpeg_Version tagHelper::getMPEGVersion()
{
	if(isLinked)
		return mp3header->version;
	else
		return MPEGVERSION_FALSE;
}

Mp3_ChannelMode tagHelper::getMP3ChannelMode()
{
	if(isLinked)
		return mp3header->channelmode;
	else
		return MP3CHANNELMODE_FALSE;
}

const Mp3_Headerinfo* tagHelper::getMP3Header()
{
	if(isLinked)
		return mp3header;
	else
		return NULL;
}

char* tagHelper::getGenre()
{
	char* buf;
	ID3_Frame* frame = myTag.Find(ID3FID_CONTENTTYPE);
	if(frame == 0 || !frame->Contains(ID3FN_TEXTENC))
	{
		delete frame;
		return NULL;
	}

	ID3_Field* field = 0;
	if(frame->Contains(ID3FN_TEXT))
	{
		field = frame->GetField(ID3FN_TEXT);
	}
	if(field==0)
	{
		delete frame;
		return NULL;
	}

	field->SetEncoding(ID3TE_ISO8859_1); // use Latin-1 charset
	const char* res = field->GetRawText();

	if(myTag.HasV2Tag())
	{	
		buf = new char[strlen(res) + 1];
		sprintf(buf, res);
		return buf;
	}
	else
	{
		//Try using V1
		int gid;
		sscanf(res, "(%i)", &gid); // filter Genre ID

		if(gid >= ID3_NR_OF_V1_GENRES) // genre ID too large
		{
			buf = new char[strlen(res) + 1];
			sprintf(buf, res);
			return buf;
		}
		buf = new char[strlen(ID3_V1GENRE2DESCRIPTION(gid)) + 1];
		sprintf(buf, ID3_V1GENRE2DESCRIPTION(gid));
		return buf;
	}
}

char* tagHelper::getValue(ID3_FrameID fid)
{
	char* buf;

	if(fid == ID3FID_CONTENTTYPE)
		return getGenre();

	ID3_Frame* frame = myTag.Find(fid);
	
	if(frame==0)
	{
		return NULL;
	}

    if(!frame->Contains(ID3FN_TEXTENC))
	{
		delete frame;
		return NULL;
	}

	ID3_Field* field = 0;
    if(frame->Contains(ID3FN_TEXT))
	{
		field = frame->GetField(ID3FN_TEXT);
	}

    if(frame->Contains(ID3FN_URL) && (field==0))
	{
		field = frame->GetField(ID3FN_URL);
	}

    if(field==0)
	{
		delete frame;
		return NULL;
	}

    field->SetEncoding(ID3TE_ISO8859_1); // use Latin-1 charset

    const char* res = field->GetRawText();
    buf = new char[strlen(res) + 1];
	sprintf(buf, res);
    return buf;
}

void tagHelper::setValue(ID3_FrameID fid, char* value)
{
	ID3_Frame* frame = myTag.Find(fid);
	ID3_Frame* xframe = new ID3_Frame(fid);
	if (frame == 0)
	{
		myTag.AddFrame(xframe);
		frame = myTag.Find(fid);
	}

	ID3_Field* field = frame->GetField(ID3FN_TEXT);
    field->SetEncoding(ID3TE_ISO8859_1); // use Latin-1 charset

    field->Set(value);
	myTag.Update();
	delete xframe;
}

void tagHelper::addAlbumart(char* imgpath)
{
	ID3_Frame* frame = myTag.Find(ID3FID_PICTURE);
	ID3_Frame* xframe = new ID3_Frame(ID3FID_PICTURE);
	if (frame == 0)
	{
		myTag.AddFrame(xframe);
		frame = myTag.Find(ID3FID_PICTURE);
	}
	frame->GetField(ID3FN_MIMETYPE)->Set("image/jpeg");
	frame->GetField(ID3FN_PICTURETYPE)->Set(ID3PT_COVERFRONT);
	frame->GetField(ID3FN_DATA)->FromFile(imgpath);

	myTag.Update();
	delete xframe;
}

bool tagHelper::getAlbumart(char* path)
{
	ID3_Frame* frame = myTag.Find(ID3FID_PICTURE);
	if (frame && frame->Contains(ID3FN_DATA))
	{
		frame->Field(ID3FN_DATA).ToFile(path);
		return true;
	}
	return false;
}

bool tagHelper::removeTag(ID3_FrameID fid)
{
	ID3_Frame* frame = myTag.Find(fid);
	myTag.RemoveFrame(frame);
	return myTag.Update() == 1 ? true : false;
}

bool tagHelper::removeAllTags()
{
	return myTag.Strip();
}