#include"pch.h"
#include"util.h"

CString string2CString(std::string s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r.c_str();
}
char* CStringToUTF8(CString strUnicode) {
    int len = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, NULL, 0, NULL, NULL);
    char* strUTF8 = new char[len];
    WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, strUTF8, len, NULL, NULL);
    return strUTF8;
}
char* StringToUTF8(string strUnicode) {
    int len = MultiByteToWideChar(CP_ACP, 0, strUnicode.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, strUnicode.c_str(), -1, wstr, len);

    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* strUTF8 = new char[len];
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, strUTF8, len, NULL, NULL);

    delete[] wstr;
    return strUTF8;
}
CString UTF82UNICODE(const char* szU8)
{
    int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
    wchar_t* wszString = new wchar_t[wcsLen + 1];
    ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
    wszString[wcsLen] = '\0';
    CString unicodeString(wszString);
    delete[] wszString;
    wszString = NULL;
    return unicodeString;
}
int CalculateLevenshteinDistance(const std::string& str1, const std::string& str2)
{
    int m = str1.length();
    int n = str2.length();
std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    // ��ʼ���߽�
    for (int i = 0; i <= m; i++)
    {
        dp[i][0] = i;
    }
    for (int j = 0; j <= n; j++)
    {
        dp[0][j] = j;
    }

    // ��̬�滮����Levenshtein����
    for (int i = 1; i <= m; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            if (str1[i - 1] == str2[j - 1])
            {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else
            {
                dp[i][j] = min(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]) + 1;
            }
        }
    }

    return dp[m][n];
}
float CalculateSimilarity(const std::string& str1, const std::string& str2)
{
    int distance = CalculateLevenshteinDistance(str1, str2);
    float similarity = 1.0 - (distance / (float)(str1.length() + str2.length()));
    return similarity;
}
void Char2SplitStr(const string& input, char tag, vector<string>& output) {
    std::string subStr;
    for (size_t i = 0; i < input.length(); i++)
    {
        if (tag == input[i]) //���һ���и�
        {
            if (!subStr.empty())
            {
                output.push_back(subStr);
                subStr.clear();
            }
        }
        else //��iλ�õ��ַ������Ӵ�
        {
            subStr.push_back(input[i]);
        }
    }
    if (!subStr.empty()) //ʣ����Ӵ���Ϊ�������ַ���
    {
        output.push_back(subStr);
    }
}
void Str2SplitStr(const string& input, const string& pattern, vector<string>& output) {
    string subStr;
    string tPattern;
    size_t patternLen = pattern.length();
    //�����ַ�������iλ�õ��ַ������Ӵ��У�������pattern�Ӵ�ʱ���һ���и�
    //����֮��õ��и����Ӵ��б�
    for (size_t i = 0; i < input.length(); i++)
    {
        if (pattern[0] == input[i])//������Ҫ���pattern�����
        {
            tPattern = input.substr(i, patternLen);
            if (tPattern == pattern)//�ҵ�һ��ƥ���pattern������и�
            {
                i += patternLen - 1;
                if (!subStr.empty())
                {
                    output.push_back(subStr);
                    subStr.clear();
                }
            }
            else//����ƥ���pattern����iλ�õ��ַ������Ӵ�
            {
                subStr.push_back(input[i]);
            }
        }
        else//δ����pattern����iλ�õ��ַ������Ӵ�
        {
            subStr.push_back(input[i]);
        }
    }
    if (!subStr.empty())//���Ӵ��е�ʣ���ַ��������ַ�������
    {
        output.push_back(subStr);
    }
}
void chineseEnglishConversion(string& cnstr) {
    cnstr = regex_replace(cnstr, regex("��"), ":");
   // cnstr = regex_replace(cnstr, regex("��"), ",");
   // cnstr = regex_replace(cnstr, regex("��"), ";");
   // cnstr = regex_replace(cnstr, regex("��"), "?");
   // cnstr = regex_replace(cnstr, regex("��"), "!");
   // cnstr = regex_replace(cnstr, regex("��"), ".");
}
/*string FullStrToHalfstr(string strFullWidth)
{
    string output;
    unsigned char tmp; unsigned char tmp1;
    for (unsigned int i = 0; i < strFullWidth.length(); i++)
    {
        tmp = strFullWidth[i];
        tmp1 = strFullWidth[i + 1];
        if (tmp == 163)
        {
            output += (unsigned char)strFullWidth[i + 1] - 128;
            i++;
            continue;
        }
        else if (tmp > 163)
        {
            output += strFullWidth.substr(i, 2);
            i++;
            continue;
        }
        else if (tmp == 161 && tmp1 == 161)
        {
            output += "";
            i++;
        }
        else
        {
            output += strFullWidth.substr(i, 1);
        }
    }
    string reslut = chineseEnglishCharacterConversion(output);
    return reslut;
}*/

string deldigits(const string& input) {
    if (input.empty()) {
        return NULL;
    }
    int i = 0;
    for (char c : input) {
        if (isdigit(c)) {
            i = i + 1;
        }else {
            break;
        }
    }
   return  input.substr(i);
}


