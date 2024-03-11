#pragma once
#include <string>
#include <afxstr.h>
#include <vector>
#include <algorithm>
#include <regex>

using namespace std;

CString string2CString(string s);
CString UTF82UNICODE(const char* szU8);
char* CStringToUTF8(CString strUnicode);
char* StringToUTF8(string strUnicode);
float CalculateSimilarity(const string& str1, const string& str2);
void Char2SplitStr(const string& input, char tag, vector<string>& output);
void Str2SplitStr(const string& input, const string& pattern, vector<string>& output);
void chineseEnglishConversion(string& cnstr);
string deldigits(const string& input);