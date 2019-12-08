/**********************************************************************************************
	* Author:	Oso, Oluwafemi Ebenezer
	* Date:		09th of September, 2013
	* Filename:	tagHelper.h

	A wrapper for the id3lib library
***********************************************************************************************/
#include "stdafx.h"
#include "windows.h"

#define ID3LIB_LINKOPTION 3
#include "id3/tag.h"
#pragma comment(lib, "id3lib.lib")

class tagHelper
{
private:
	bool isLinked;
	ID3_Tag myTag;
	const Mp3_Headerinfo* mp3header;
	char* getGenre();

public:
	tagHelper(char* path);
	~tagHelper();
	bool linked();
	
	bool hasLyrics();
	bool hasV1Tag();
	bool hasV2Tag();
	int fileSize();
	Mpeg_Layers getMPEGLayer();
	Mpeg_Version getMPEGVersion();
	Mp3_ChannelMode getMP3ChannelMode();

	MP3_BitRates getCbrBitRate();
	uint32 getVbrBitRate();
	uint32 getSampleRate();
	uint32 getTrackLength();
	const Mp3_Headerinfo* getMP3Header();
	
	char* getValue(ID3_FrameID fid);
	void setValue(ID3_FrameID fid, char* value);
	void addAlbumart(char* imgpath);
	bool getAlbumart(char* path);

	bool removeTag(ID3_FrameID fid);
	bool removeAllTags();
};