
// OCRCrawlerDlg.h: 头文件
//

#pragma once
#include "opencv2/core.hpp"
#include <opencv2/highgui/highgui.hpp> 
#include "ocrEngine.h"
#include "util.h"
#include <map>
#include <vector>

using namespace std;

#define WM_DYBARRAGE  WM_USER+1
#define WM_KSBARRAGE  WM_USER+2
#define WM_WXBARRAGE  WM_USER+3

enum BarrageMode {
	CHAT,            //弹幕
	MEMBER,     //进入房间
	LIKE,             //点赞
	GIFT,            //礼物
	SOCIAL,        //关注
	INFO             //消息（传递通知或异常）
};

// COCRCrawlerDlg 对话框
class COCRCrawlerDlg : public CDialogEx
{
	HANDLE m_ocrThread;
	HANDLE m_pipeThread;
	bool keep_running_ = false;
	bool connected_ = true;
	bool working_ = false;
	HANDLE ocr_pipe_;
	CFile txtfile;
	bool firstOCR = true;
	vector<string> ocrlastList;
// 构造
public:

	COCRCrawlerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OCRCrawler_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	HWND hWnd;
	CRect m_rect;
	bool m_RadioDouYin = true;                //抖音
	bool m_RadioKuaiShou = false;           //快手
	bool m_RadioWeChat = false;             //微信

	map<int, vector<double>> tar;				    //存储所有控件比例信息    key = 控件ID  value = 控件比例
	map<int, vector<double>>::iterator key;		//迭代器查找结果
	map<int, vector<double>>::iterator end;		//存储容器的最后一个元素
	CEdit* m_MessageBox;                     //ORC消息显示框
	bool m_CheckWelcome = false;           //欢迎
	bool m_CheckBarrage = true;              //弹幕
	bool m_CheckFollow = false;               //关注
	bool m_CheckLike = false;                   //点赞
	bool m_CheckForward = false;            //转发
	bool m_CheckGift = false;                   //礼物
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	//afx_msg void OnPaint();
	afx_msg void OnPaint();
	//void OnClose();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	void OnSize(UINT t, int x, int y);
	DECLARE_MESSAGE_MAP();
	void SetTopMost(BOOL topMost);
	void expandMessage(CString text);
	static DWORD CALLBACK pipeThreadProc(LPVOID param);
	void pipeThreadMain();
	static DWORD CALLBACK ocrThreadProc(LPVOID param);
	void ocrThreadMain();
	cv::Mat CaptureScreenToMat(CRect dst);
public:
	afx_msg void OnBnClickedButtonMeta();
	afx_msg void OnBnClickedButtonWork();
	afx_msg void OnBnClickedCheckWelcome();
	afx_msg void OnBnClickedCheckBarrage();
	afx_msg void OnBnClickedCheckLike();
	afx_msg void OnBnClickedCheckGift();
	afx_msg void OnBnClickedCheckFollow();
	afx_msg LRESULT  OnDYBarrageMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnKSBarrageMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnWXBarrageMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedRadioDouyin();
	afx_msg void OnBnClickedRadioKuaishou();
	afx_msg void OnBnClickedRadioWechat();
	afx_msg void OnStnClickedStaticFrame();
};
