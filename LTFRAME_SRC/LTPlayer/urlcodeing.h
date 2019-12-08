#pragma once


#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

class strCoding
{
public:
    strCoding(void);
    ~strCoding(void);
  
    void UTF_8ToGB2312(string &pOut, char *pText, int pLen);//utf_8转为gb2312
    void GB2312ToUTF_8(string& pOut,char *pText, int pLen); //gb2312 转utf_8
    string UrlGB2312(char * str);                           //urlgb2312编码
    string UrlUTF8(char * str);                             //urlutf8 编码
    string UrlUTF8Decode(string str);                  //urlutf8解码
    string UrlGB2312Decode(string str);                //urlgb2312解码

private:
    void Gb2312ToUnicode(WCHAR* pOut,char *gbBuffer);
    void UTF_8ToUnicode(WCHAR* pOut,char *pText);
    void UnicodeToUTF_8(char* pOut,WCHAR* pText);
    void UnicodeToGB2312(char* pOut,WCHAR uData);
    char CharToInt(char ch);
    char StrToBin(char *str);

};