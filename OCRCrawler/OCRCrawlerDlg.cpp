
// OCRCrawlerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "OCRCrawler.h"
#include "OCRCrawlerDlg.h"
#include "afxdialogex.h"
#include <windows.h>
#include <stdio.h>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// COCRCrawlerDlg 对话框


COCRCrawlerDlg::COCRCrawlerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OCRCrawler_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COCRCrawlerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COCRCrawlerDlg, CDialogEx)
	//ls add
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_META, &COCRCrawlerDlg::OnBnClickedButtonMeta)
	ON_BN_CLICKED(IDC_BUTTON_WORK, &COCRCrawlerDlg::OnBnClickedButtonWork)
	ON_BN_CLICKED(IDC_CHECK_WELCOME, &COCRCrawlerDlg::OnBnClickedCheckWelcome)
	ON_BN_CLICKED(IDC_CHECK_BARRAGE, &COCRCrawlerDlg::OnBnClickedCheckBarrage)
	ON_BN_CLICKED(IDC_CHECK_LIKE, &COCRCrawlerDlg::OnBnClickedCheckLike)
	ON_BN_CLICKED(IDC_CHECK_GIFT, &COCRCrawlerDlg::OnBnClickedCheckGift)
	ON_BN_CLICKED(IDC_CHECK_FOLLOW, &COCRCrawlerDlg::OnBnClickedCheckFollow)
	ON_MESSAGE(WM_DYBARRAGE, &COCRCrawlerDlg::OnDYBarrageMessage)
	ON_MESSAGE(WM_KSBARRAGE, &COCRCrawlerDlg::OnKSBarrageMessage)
	ON_MESSAGE(WM_WXBARRAGE, &COCRCrawlerDlg::OnWXBarrageMessage)
	ON_BN_CLICKED(IDC_RADIO_DOUYIN, &COCRCrawlerDlg::OnBnClickedRadioDouyin)
	ON_BN_CLICKED(IDC_RADIO_KUAISHOU, &COCRCrawlerDlg::OnBnClickedRadioKuaishou)
	ON_BN_CLICKED(IDC_RADIO_WECHAT, &COCRCrawlerDlg::OnBnClickedRadioWechat)
	ON_STN_CLICKED(IDC_STATIC_FRAME, &COCRCrawlerDlg::OnStnClickedStaticFrame)
END_MESSAGE_MAP()


// COCRCrawlerDlg 消息处理程序

BOOL COCRCrawlerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	((CButton*)GetDlgItem(IDC_CHECK_BARRAGE))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_BUTTON_META))->EnableWindow(false);
	((CButton*)GetDlgItem(IDC_RADIO_DOUYIN))->SetCheck(BST_CHECKED);
	keep_running_ = true;
	m_MessageBox = (CEdit*)GetDlgItem(IDC_EDIT_MESSAGE);
	m_pipeThread = CreateThread(NULL, 0, COCRCrawlerDlg::pipeThreadProc, this, 0, NULL);
	m_ocrThread   = CreateThread(NULL, 0, COCRCrawlerDlg::ocrThreadProc, this, 0, NULL);

	COLORREF maskColor = RGB(1, 255, 0);   //掩码颜色，也就是窗口背景颜色
	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) | 0x80000);  //设定窗体使用扩展模式 
	HINSTANCE hInst = LoadLibrary(_T("User32.DLL"));
	if (hInst)
	{
		typedef BOOL(WINAPI* MYFUNC)(HWND, COLORREF, BYTE, DWORD);
		MYFUNC AlphaFunc = NULL;
		AlphaFunc = (MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if (AlphaFunc)AlphaFunc(this->GetSafeHwnd(), maskColor, 255, 1); //将与掩码颜色相同的窗体部分置为透明
		FreeLibrary(hInst);
	}
	SetTopMost(TRUE);
	BringWindowToTop();
	int cx = GetSystemMetrics(SM_CXSCREEN);   
    int cy = GetSystemMetrics(SM_CYSCREEN);
	//SetWindowPos(NULL, round(cx*0.72), round(cy*1/3), round(cx*0.28), round(cy*1/5), SWP_SHOWWINDOW);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void COCRCrawlerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CRect rect;
		GetClientRect(&rect);
		dc.FillSolidRect(rect,RGB(1,255,0));
		CDialogEx::OnPaint();
	}
}
void COCRCrawlerDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	//lpMMI->ptMaxTrackSize.x = round(cx * 0.4);   //x宽度
	lpMMI->ptMaxTrackSize.x = round(cx * 0.9);   //x宽度
	lpMMI->ptMinTrackSize.x = round(cx * 0.2);
	lpMMI->ptMinTrackSize.y = round(cy * 0.2);   //y高度    

	CDialog::OnGetMinMaxInfo(lpMMI);
}
void COCRCrawlerDlg::OnSize(UINT t, int x,int y) {
	CDialog::OnSize(t,x,y);
	CWnd* pWnd;
	int id = 0;
	HWND  hwndChild = ::GetWindow(m_hWnd, GW_CHILD);
	if (!x || !y){
		return;
	}else{
		int check_rect_heigth=0;
		int button_rect_heigth=0;
		if (hwndChild) {
			CRect check_rect, button_rect;
			GetDlgItem(IDC_CHECK_BARRAGE)->GetWindowRect(&check_rect);
			ScreenToClient(&check_rect);
			GetDlgItem(IDC_BUTTON_WORK)->GetWindowRect(&button_rect);
			check_rect_heigth = check_rect.Height();
			button_rect_heigth = button_rect.Height();
		}
		while (hwndChild){
			id = ::GetDlgCtrlID(hwndChild);//获得控件的ID
			pWnd = GetDlgItem(id); //获取ID为woc的空间的句柄			
			if (pWnd)
			{
				CRect rect;
				//获取当前控件大小
				pWnd->GetWindowRect(&rect);
				//将控件大小转换为在对话框中的区域坐标
				ScreenToClient(&rect);
				int height = rect.Height();
				key = tar.find(id);
				end = tar.end();
				if (key == end)
				{
					vector<double> scale;
					scale.push_back((double)rect.left / m_rect.Width());//注意类型转换，不然保存成long型就直接为0了
					scale.push_back((double)rect.right / m_rect.Width());
					scale.push_back((double)rect.top / m_rect.Height());
					scale.push_back((double)rect.bottom / m_rect.Height());
					tar[id] = scale;
				}
				if (id == IDC_STATIC_FRAME) {
					rect.left = tar[id][0] * x;
					//rect.right = tar[id][1] * x;
					rect.top = tar[id][2] * y;
					rect.bottom = tar[id][3] * y;
					//设置控件大小
					pWnd->MoveWindow(rect);
				}
				if (id == IDC_EDIT_MESSAGE) {
					//rect.left = tar[id][0] * x;
					//rect.right = tar[id][1] * x;
					//rect.top = tar[id][2] * y;
					rect.bottom = tar[id][3] + y- (check_rect_heigth+button_rect_heigth*2) *1.5;
					pWnd->MoveWindow(rect);
				}
				//if (id == IDC_CHECK_BARRAGE || id == IDC_CHECK_LIKE ) {
				//	rect.bottom = tar[id][3] + y - (button_rect_heigth*3);
				//	rect.top = rect.bottom - height;
				//	pWnd->MoveWindow(rect);
				//	InvalidateRect(&rect, TRUE);
				//}
				if (id == IDC_CHECK_GIFT  || id == IDC_CHECK_WELCOME
					|| id == IDC_CHECK_BARRAGE || id == IDC_CHECK_LIKE) {
					rect.bottom = tar[id][3] + y - (button_rect_heigth * 2.5);
					rect.top = rect.bottom - height;
					pWnd->MoveWindow(rect);
					InvalidateRect(&rect, TRUE);
				}
				if (id == IDC_BUTTON_WORK || id == IDC_BUTTON_META) {
					rect.bottom = tar[id][3]+y-10;
					rect.top = rect.bottom - height;
					//设置控件大小
					pWnd->MoveWindow(rect);
					InvalidateRect(&rect, TRUE);
				}
			}
			hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
		 }
		//将变化后的对话框大小设为旧大小
		GetClientRect(&m_rect);
	}
}
void COCRCrawlerDlg::OnClose()
{
	working_ = false;
	connected_ = false;
	keep_running_ = false;
	//SetEvent(m_hExitEvent);
	if (WAIT_TIMEOUT == WaitForSingleObject(m_pipeThread, 5000)) {
		TerminateThread(m_pipeThread, 0);
		m_pipeThread = NULL;
	}
	if (WAIT_TIMEOUT == WaitForSingleObject(m_ocrThread, 5000)) {
		TerminateThread(m_ocrThread, 0);
		m_ocrThread = NULL;
	 }

	struct Barrage
	{
		char  mode[16];
		char  nickname[64];
		char  content[256];
	}barrage;
	DWORD bytes_write = 0;
	strncpy_s(barrage.mode, "QUIT", 16);
	
	WriteFile(ocr_pipe_, &barrage, sizeof(barrage), &bytes_write, NULL);
	
	CloseHandle(ocr_pipe_);
	printf("close app");

	CDialogEx::OnClose();
}
HCURSOR COCRCrawlerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void COCRCrawlerDlg::SetTopMost(BOOL topMost)
{
	HWND insertAfter = topMost ? HWND_TOPMOST : HWND_NOTOPMOST;
	::SetWindowPos(CWnd::GetSafeHwnd(), insertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

cv::Mat  COCRCrawlerDlg::CaptureScreenToMat(CRect dst)
{
	//抓取整个屏幕
	HDC hdcSrc = ::GetDC(NULL);
	int nBitPerPixel = GetDeviceCaps(hdcSrc, BITSPIXEL);
	int nWidth_ = GetDeviceCaps(hdcSrc, HORZRES);
	int nHeight_ = GetDeviceCaps(hdcSrc, VERTRES);
	CImage image_;
	image_.Create(nWidth_, nHeight_, nBitPerPixel,0);
	BitBlt(image_.GetDC(), 0, 0, nWidth_, nHeight_, hdcSrc, 0, 0, SRCCOPY);
	//image_.Save(L"D:/test.png", Gdiplus::ImageFormatPNG);

	//截取区域坐标图像
	CRect rect;
	rect.left = 0;
	rect.right = (dst.right - dst.left);
	rect.top = 0;
	rect.bottom = (dst.bottom - dst.top);

	CImage simage_;
	simage_.Create(rect.right, rect.bottom, nBitPerPixel,0);
	image_.StretchBlt(simage_.GetDC(), rect, dst);
	//simage_.Save(L"D:/test1.png", Gdiplus::ImageFormatPNG);
	::ReleaseDC(NULL, hdcSrc);
	image_.ReleaseDC();
	simage_.ReleaseDC();

	int nWidth = simage_.GetWidth();               //截取图像的宽
	int nHeight = simage_.GetHeight();            //截取图像的高

    //CImage 从4通道转成3通道
	CImage fcimage;
	fcimage.Create(nWidth,nHeight, 24);
	if (simage_.GetBPP() / 8 == 4)
	{
		for (int i = 0; i < nWidth; i++)
		{
			for (int j = 0; j < nHeight; j++)
			{
				byte* pByte = (byte*)simage_.GetPixelAddress(i, j);
				byte* ppngByte = (byte*)fcimage.GetPixelAddress(i, j);
				ppngByte[0] = pByte[0] * pByte[3] / 255; ppngByte[1] = pByte[1] * pByte[3] / 255;
				ppngByte[2] = pByte[2] * pByte[3] / 255;
			}
		}
	}
	simage_.Destroy();
	//CImage 图像转成 Mat格式

	cv::Mat mat_;
    mat_.create(nHeight, nWidth, CV_8UC3);

	uchar* pucRow;                                                 //指向数据区的行指针
	uchar* pucImage = (uchar*)fcimage.GetBits(); //指向数据区的指针
	int nStep = fcimage.GetPitch();                         //每行的字节数,注意这个返回值有正有负
	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat_.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			for (int nCha = 0; nCha < 3; nCha++)
			{
				pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * 3 + nCha);
			}
		}
	}
	fcimage.Destroy();
	return mat_;
}
void COCRCrawlerDlg::expandMessage(CString text) {
	int LineNum = m_MessageBox->GetLineCount(); //获取行号
	if (LineNum > 600) {
		CString content;
		m_MessageBox->GetWindowText(content);
		int len = m_MessageBox->GetWindowTextLength();
		int position = content.Find(_T("\r\n"), 580);
		content = content.Right(len - position);
		m_MessageBox->SetWindowText(content);
	}
	CTime mtime = CTime::GetCurrentTime();					//获取系统日期
	CString year, month, day, hour, minute, second;
	year.Format(_T("%d"), mtime.GetYear());												//获取年份
	month.Format(_T("%d"), mtime.GetMonth());											//获取当前月份
	day.Format(_T("%d"), mtime.GetDay());												    //获得几号
	hour.Format(_T("%d"), mtime.GetHour());												//获取当前为几时
	minute.Format(_T("%d"), mtime.GetMinute());									    //获取分钟
	second.Format(_T("%d"), mtime.GetSecond());										//获取秒
	CString message = year + "." + month + "." + day + "-" + hour + ":" + minute + ":" + second + " " + text + "\r\n";
	int iLen = m_MessageBox->GetWindowTextLength();
	m_MessageBox->SetSel(iLen, iLen, TRUE);
	m_MessageBox->ReplaceSel(message, FALSE);
}

DWORD COCRCrawlerDlg::pipeThreadProc(LPVOID param)
{
	COCRCrawlerDlg* self = (COCRCrawlerDlg*)param;
	self->pipeThreadMain();
	return 0;
}
void COCRCrawlerDlg::pipeThreadMain()
{
	while (keep_running_) {
		if (ocr_pipe_ == NULL) {

			if (WaitNamedPipe(L"\\\\.\\pipe\\barrage_msg_pipe", NMPWAIT_NOWAIT) == FALSE) {
				printf("Failed to connect to pipe!\n");
			}

			ocr_pipe_ = CreateFile(L"\\\\.\\pipe\\barrage_msg_pipe", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			printf("creat ocr_pipe");
			if (ocr_pipe_ == NULL || ocr_pipe_ == INVALID_HANDLE_VALUE) {
				ocr_pipe_ = NULL;
				Sleep(1000);
				continue;
			}
		}
		Sleep(5000);
	}
	PostMessage(WM_QUIT, 0, 0);//最常用
}
DWORD COCRCrawlerDlg::ocrThreadProc(LPVOID param)
{
	COCRCrawlerDlg* self = (COCRCrawlerDlg*)param;
	self->ocrThreadMain();
	return 0;
}
void COCRCrawlerDlg::ocrThreadMain()
{
	hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
	DWORD bytes_write = 0;
	std::vector<std::string>  last_results;
	char send_text[256] = {0};
	ocrEngine  engine = ocrEngine::getInstance();        //初始化ocr引擎
	ocrEngine::OCRConfig ocrConfig;
	ocrConfig.use_gpu = true;
	TCHAR exepath[MAX_PATH];
	ZeroMemory(exepath, MAX_PATH);
	GetModuleFileName(NULL, exepath, MAX_PATH);
	CString strPath = exepath;
	int position = strPath.ReverseFind('\\'); //反向查找"\"所在的位置
	strPath = strPath.Left(position + 1); //或者strpath=strPath.Left(position)+_T("\\"); 
	ocrConfig.det_model_path = strPath+"ch_PP-OCRv4_det_infer";
	ocrConfig.rec_model_path = strPath+"ch_PP-OCRv4_rec_infer";
	ocrConfig.rec_dict_path = strPath +"ppocr_keys_v1.txt";
	ocrConfig.score_threshold = 0.8;
	engine.initialize(ocrConfig); 

	while (keep_running_) {
		if (working_) {
			CRect  rect;
			GetClientRect(rect);
			CRect frame_rect;
			GetDlgItem(IDC_STATIC_FRAME)->GetWindowRect(&frame_rect);
			rect.left = frame_rect.Width();
			ClientToScreen(rect);
			cv::Mat mat = CaptureScreenToMat(rect);
			//cv::Mat mat = cv::imread("D:/stream.png");
			std::vector<std::string>  results;
			std::vector <ocrEngine::OCRResult>  ocresults = engine.detectdata(mat);
			if (!ocresults.empty()) {
				for (int i = 0; i < ocresults.size(); i++) {
					if (!ocresults[i].used) {
						ocresults[i].used = true;
						std::string result = ocresults[i].text;
						
						int boxiy1 = ocresults[i].box[3][1] - ocresults[i].box[0][1];       
						int obxiy2 = ocresults[i].box[2][1] - ocresults[i].box[1][1];
						int boxiy = std::round((boxiy1 + obxiy2) / 4)+ std::round((ocresults[i].box[0][1]+ ocresults[i].box[1][1])/2);

						for (int j = i + 1; j < ocresults.size(); j++) {
							int boxjy1 = ocresults[j].box[3][1] - ocresults[j].box[0][1];
							int obxjy2 = ocresults[j].box[2][1] - ocresults[j].box[1][1];
							int boxjy = std::round((boxjy1 + obxjy2) / 4)+ std::round((ocresults[j].box[0][1] + ocresults[j].box[1][1]) / 2);
							int maxdst = std::min(std::max(boxjy1, obxjy2), std::max(boxiy1, obxiy2));
							if (std::abs(boxjy - boxiy) < maxdst) {             //切割识别的图像，如果y坐标相差2以内，认为是同一行，合并字符串
								ocresults[j].used = true;
								result = result + "  " + ocresults[j].text;
							}
						}
						results.push_back(result);
					}
				}
			}
			if (!results.empty()) {
				if (m_RadioDouYin) {
					::SendMessage(hWnd, WM_DYBARRAGE, 0, (LPARAM)&results);   //发送消息
				}
				if (m_RadioKuaiShou) {
					::SendMessage(hWnd, WM_KSBARRAGE, 0, (LPARAM)&results);   //发送消息
				}
				if (m_RadioWeChat) {
					::SendMessage(hWnd, WM_WXBARRAGE, 0, (LPARAM)&results);   //发送消息
				}
			}
		}
		Sleep(500);
	}
	PostMessage(WM_QUIT, 0, 0);//最常用
}

void COCRCrawlerDlg::OnBnClickedButtonMeta()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!connected_) {
		//开始抓取
		GetDlgItem(IDC_BUTTON_META)->SetWindowText(L"断开慧播");
		expandMessage(L"开始转发弹幕到慧播数字人!");
		connected_ = true;
	}
	else {
		//停止抓取
		expandMessage(L"停止转发弹幕到慧播数字人!");
		GetDlgItem(IDC_BUTTON_META)->SetWindowText(L"连接慧播");
		connected_ = false;
	}
}
void COCRCrawlerDlg::OnBnClickedButtonWork()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!working_) {
		//开始抓取
		GetDlgItem(IDC_BUTTON_WORK)->SetWindowText(L"停止捕获");
		expandMessage(L"开始捕获弹幕!");
		GetDlgItem(IDC_BUTTON_META)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_META)->SetWindowText(L"连接慧播");
		working_ = true;
	}
	else {
		//停止抓取
		expandMessage(L"停止捕获弹幕!");
		GetDlgItem(IDC_BUTTON_WORK)->SetWindowText(L"开始捕获");
		GetDlgItem(IDC_BUTTON_META)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_META)->SetWindowText(L"连接慧播");

		/*CloseHandle(ocr_pipe_);
		ocr_pipe_ = NULL;*/

		connected_ = false;
		working_ = false;
	}
}
void COCRCrawlerDlg::OnBnClickedCheckWelcome()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_CheckWelcome) {
		m_CheckWelcome = true;
		expandMessage(L"打开进入房间捕获设置!");
	}
	else {
		m_CheckWelcome = false;
		expandMessage(L"关闭进入房间捕获设置!");
	}
}
void COCRCrawlerDlg::OnBnClickedCheckBarrage()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_CheckBarrage) {
		m_CheckBarrage = true;
		expandMessage(L"打开问答弹幕捕获设置!");
	}
	else {
		m_CheckBarrage = false;
		expandMessage(L"关闭问答弹幕捕获设置!");
	}
}
void COCRCrawlerDlg::OnBnClickedCheckLike()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_CheckLike) {
		m_CheckLike = true;
		expandMessage(L"打开点赞弹幕捕获设置!");
	}
	else {
		m_CheckLike = false;
		expandMessage(L"关闭点赞弹幕捕获设置!");
	}
}
void COCRCrawlerDlg::OnBnClickedCheckGift()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_CheckGift) {
		m_CheckGift = true;
		expandMessage(L"打开礼物弹幕捕获设置!");
	}
	else {
		m_CheckGift = false;
		expandMessage(L"关闭礼物弹幕捕获设置!");
	}
}
void COCRCrawlerDlg::OnBnClickedCheckFollow()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_CheckFollow) {
		m_CheckFollow = true;
		expandMessage(L"打开关注弹幕捕获设置!");
	}
	else {
		m_CheckFollow = false;
		expandMessage(L"关闭关注弹幕捕获设置!");
	}
}

LRESULT  COCRCrawlerDlg::OnDYBarrageMessage(WPARAM wParam, LPARAM lParam) {
	vector<string> *list =  (vector<string>* )lParam;
	DWORD bytes_write = 0;
	vector<string>  ocrpreList;
	vector<string>  ocrList;
	vector<string>  barrageList;
	//第一次筛选
	if (list->empty()) {
		return TRUE;
	}
	for (int i = 0; i < list->size(); i++) {
		string ocrstring = (*list)[i];
		ocrstring = regex_replace(ocrstring, regex("："), ":");
		if (ocrstring.find("来了") != string::npos) {
			vector<string>  spaceSplitlist;
			Char2SplitStr(ocrstring, ' ', spaceSplitlist);
			if (!spaceSplitlist.empty()) {
				ocrpreList.push_back(spaceSplitlist.back());
			}
			continue;
		}
		if (ocrstring.find("点赞了") != string::npos) {
			vector<string>  spaceSplitlist;
			Char2SplitStr(ocrstring, ' ', spaceSplitlist);
			if (!spaceSplitlist.empty()) {
				ocrpreList.push_back(spaceSplitlist.back());
			}
			continue;
		}
		if (ocrstring.find("送出") != string::npos) {
			vector<string>  spaceSplitlist;
			Char2SplitStr(ocrstring, ' ', spaceSplitlist);
			if (!spaceSplitlist.empty()) {
				for (int j = 0; j < spaceSplitlist.size(); j++) {
					if (spaceSplitlist[j].find("送出") != string::npos) {
						ocrpreList.push_back(spaceSplitlist[j]);
					}
				}
			}
			continue;
		}
		if (ocrstring.find(":") != string::npos) {
			vector<string>  spaceSplitlist;
			Char2SplitStr(ocrstring, ' ', spaceSplitlist);
			if (!spaceSplitlist.empty()) {
				for (int j = 0; j < spaceSplitlist.size(); j++) {
					if (spaceSplitlist[j].find(":") != string::npos) {
						ocrpreList.push_back(spaceSplitlist[j]);
					}
				}
			}	
			continue;
		}
	}
	//第二次筛选
	if (ocrpreList.empty()) {
		return TRUE;
	}
	for (int i = 0; i < ocrpreList.size(); i++) {
		string ocrstring = ocrpreList[i];
		if (ocrstring.find("来了") != string::npos) {
			ocrList.push_back(ocrstring);
			continue;
		}
		if (ocrstring.find("点赞了") != string::npos) {
			ocrList.push_back(ocrstring);
			continue;
		}
		if (ocrstring.find("送出") != string::npos) {
			ocrList.push_back(ocrstring);
			continue;
		}
		if (ocrstring.find(":") != string::npos) {
			ocrList.push_back(ocrstring);
			continue;
		}
	}
	//去重
	if (ocrList.empty()) {
		return TRUE;
	}
	if (ocrlastList.empty()) {
		for (int i = 0; i < ocrList.size(); i++) {
			barrageList.push_back(ocrList[i]);
			ocrlastList.push_back(ocrList[i]);
		}
	}else {	
		for (int i = 0; i < ocrList.size(); i++) {
			bool exist = false;
			for (int j = 0; j < ocrlastList.size(); j++) {
				if (CalculateSimilarity(ocrList[i], ocrlastList[j]) > 0.8) {
					exist = true;
					break;
				}
			}
			if (!exist) {
				barrageList.push_back(ocrList[i]);
			}
		}
		ocrlastList.clear();
		for (int i = 0; i < ocrList.size(); i++) {
			ocrlastList.push_back(ocrList[i]);
		}
	}
	//显示及转发
	if (barrageList.empty()) {
		return TRUE;
	}
	for (int i = 0; i < barrageList.size(); i++) {
		if (barrageList[i].find("来了") != string::npos) {
			if (m_CheckWelcome) {
				CString name = string2CString(deldigits(barrageList[i]));
				int index = name.Find(L"来了");
				CString nickname = name.Left(index);
				if (!nickname.IsEmpty()) {
					expandMessage(L"进入： " + nickname);
					if (connected_) {
						if (ocr_pipe_ != NULL) {
							struct Barrage
							{
								char  mode[16];
								char  nickname[64];
								char  content[256];
							}barrage;
							strncpy_s(barrage.mode, "MEMBER", 16);
							//strncpy_s(barrage.nickname, CT2A(nickname.GetString()), 64);
							strncpy_s(barrage.nickname, CStringToUTF8(nickname), 64);
							if (!WriteFile(ocr_pipe_, &barrage, sizeof(barrage), &bytes_write, NULL)) {
								expandMessage(L"进入信息转发失败！");
								ocr_pipe_ = NULL;
							}
						}
					}
				}
			}
			continue;
		}
		if (barrageList[i].find("点赞了") != string::npos) {
			if (m_CheckLike) {
				if (barrageList[i].find(":") != string::npos) {
					vector<string>  colonSplitlist;
					Char2SplitStr(barrageList[i], ':', colonSplitlist);
					if (!colonSplitlist.empty()) {
						vector<string>  spaceSplitlist;
						Char2SplitStr(colonSplitlist[0], ' ', spaceSplitlist);
						if (!spaceSplitlist.empty()) {
							expandMessage(L"点赞： " + string2CString(deldigits(spaceSplitlist.back())));
							if (connected_) {
								if (ocr_pipe_ != NULL) {
									struct Barrage
									{
										char  mode[16];
										char  nickname[64];
										char  content[256];
									}barrage;
									strncpy_s(barrage.mode, "LIKE", 16);
									//strncpy_s(barrage.nickname, deldigits(spaceSplitlist.back()).c_str(), 64);
									strncpy_s(barrage.nickname, StringToUTF8(deldigits(spaceSplitlist.back())), 64);
									if (!WriteFile(ocr_pipe_, &barrage, sizeof(barrage), &bytes_write, NULL)) {
										expandMessage(L"点赞信息转发失败！");
										ocr_pipe_ = NULL;
									}
								}
							}
						}
					}
				}
			}
			continue;
		}
		if (barrageList[i].find("送出") != string::npos) {
			if (m_CheckGift) {
				if (barrageList[i].find(":") != string::npos) {
					vector<string>  colonSplitlist;
					Char2SplitStr(barrageList[i], ':', colonSplitlist);
					if (!colonSplitlist.empty()) {
						vector<string>  spaceSplitlist;
						Char2SplitStr(colonSplitlist[0], ' ', spaceSplitlist);
						if (!spaceSplitlist.empty()) {
							expandMessage(L"礼物：" + string2CString(deldigits(spaceSplitlist.back())));
							if (connected_) {
								if (ocr_pipe_ != NULL) {
									struct Barrage
									{
										char  mode[16];
										char  nickname[64];
										char  content[256];
									}barrage;
									strncpy_s(barrage.mode, "GIFT", 16);
									strncpy_s(barrage.nickname, StringToUTF8(deldigits(spaceSplitlist.back())), 64);
									if (!WriteFile(ocr_pipe_, &barrage, sizeof(barrage), &bytes_write, NULL)) {
										expandMessage(L"礼物信息转发失败！");
										ocr_pipe_ = NULL;
									}
								}
							}
						}
					}
				}
			}
			continue;
		}
		if (barrageList[i].find(":") != string::npos) {
			//expandMessage(string2CString(barrageList[i]));
			vector<string>  colonSplitlist;
			Char2SplitStr(barrageList[i], ':', colonSplitlist);
			if (!colonSplitlist.empty() && colonSplitlist.size() > 1) {
				vector<string>  spaceSplitlist;
				Char2SplitStr(colonSplitlist[0], ' ', spaceSplitlist);
				if (!spaceSplitlist.empty()) {
					if (m_CheckBarrage) {
						expandMessage(L"问答：" + string2CString(deldigits(spaceSplitlist.back())) + L" ：" + string2CString(colonSplitlist.back()));
						if (connected_) {
							if (ocr_pipe_ != NULL) {
								struct Barrage
								{
									char  mode[16];
									char  nickname[64];
									char  content[256];
								}barrage;
								strncpy_s(barrage.mode, "CHAT", 16);
								strncpy_s(barrage.nickname, StringToUTF8(deldigits(spaceSplitlist.back())), 64);
								strncpy_s(barrage.content, StringToUTF8(colonSplitlist.back()), 256);
								if (!WriteFile(ocr_pipe_, &barrage, sizeof(barrage), &bytes_write, NULL)) {
									expandMessage(L"问答信息转发失败！");
									ocr_pipe_ = NULL;
								}
							}
						}
					}
				}
			}
			continue;
		}
	}
	
	return TRUE;
}
LRESULT  COCRCrawlerDlg::OnKSBarrageMessage(WPARAM wParam, LPARAM lParam) {
	vector<string>* list = (vector<string>*)lParam;
	DWORD bytes_write = 0;
	vector<string>  ocrfList;
	vector<string>  ocrList;
	vector<string>  barrageList;
	//第一次筛选
	if (list->empty()) {
		return TRUE;
	}
	for (int i = 0; i < list->size(); i++) {
		string ocrstring = (*list)[i];
		ocrstring = regex_replace(ocrstring, regex("："), ":");
		if (ocrstring.find("加入直播间") != string::npos) {
			//vector<string>  spaceSplitlist;
			//Char2SplitStr(ocrstring, ' ', spaceSplitlist);
			//if (!spaceSplitlist.empty()) {
				ocrfList.push_back(ocrstring);
			//}
			continue;
		}
		if (ocrstring.find("点亮") != string::npos) {
			vector<string>  spaceSplitlist;
			Char2SplitStr(ocrstring, ' ', spaceSplitlist);
			if (!spaceSplitlist.empty()) {
				ocrfList.push_back(spaceSplitlist.back());
				//expandMessage(string2CString(spaceSplitlist.back()));
			}
			continue;
		}
		if (ocrstring.find("送") != string::npos) {
			vector<string>  spaceSplitlist;
			Char2SplitStr(ocrstring, ' ', spaceSplitlist);
			if (!spaceSplitlist.empty()) {
				for (int j = 0; j < spaceSplitlist.size(); j++) {
					if (spaceSplitlist[j].find("送") != string::npos) {
						ocrfList.push_back(spaceSplitlist[j]);
						//expandMessage(string2CString(spaceSplitlist[j]));
					}
				}
			}
			continue;
		}
		if (ocrstring.find(":") != string::npos) {
			vector<string>  spaceSplitlist;
			Char2SplitStr(ocrstring, ' ', spaceSplitlist);
			if (!spaceSplitlist.empty()) {
				for (int j = 0; j < spaceSplitlist.size(); j++) {
					if (spaceSplitlist[j].find(":") != string::npos) {
						ocrfList.push_back(spaceSplitlist[j]);
						//expandMessage(string2CString(spaceSplitlist[j]));
					}
				}
			}
			continue;
		}
	}

	//第二次筛选
	if (ocrfList.empty()) {
		return TRUE;
	}
	for (int i = 0; i < ocrfList.size(); i++) {
		string ocrstring = ocrfList[i];
		if (ocrstring.find("加入直播间") != string::npos) {
			ocrList.push_back(ocrstring);
			continue;
		}
		if (ocrstring.find("点亮") != string::npos) {
			ocrList.push_back(ocrstring);
			continue;
		}
		if (ocrstring.find("送") != string::npos) {
			ocrList.push_back(ocrstring);
			continue;
		}
		if (ocrstring.find(":") != string::npos) {
			ocrList.push_back(ocrstring);
			continue;
		}
	}
	//去重
	if (ocrList.empty()) {
		return TRUE;
	}
	if (ocrlastList.empty()) {
		for (int i = 0; i < ocrList.size(); i++) {
			barrageList.push_back(ocrList[i]);
			ocrlastList.push_back(ocrList[i]);
		}
	}else {
		for (int i = 0; i < ocrList.size(); i++) {
			bool exist = false;
			for (int j = 0; j < ocrlastList.size(); j++) {
				if (CalculateSimilarity(ocrList[i], ocrlastList[j]) > 0.9) {
					exist = true;
					break;
				}
			}
			if (!exist) {
				barrageList.push_back(ocrList[i]);
			}
		}
		ocrlastList.clear();
		for (int i = 0; i < ocrList.size(); i++) {
			ocrlastList.push_back(ocrList[i]);
		}
	}
	//显示及转发
	if (barrageList.empty()) {
		return TRUE;
	}
	for (int i = 0; i < barrageList.size(); i++) {
		if (barrageList[i].find("加入直播间") != string::npos) {
			if (m_CheckWelcome) {
				CString name = string2CString(barrageList[i]);
				int index = name.Find(L"加入直播间");
				CString nickname = name.Left(index);
				//tring content = name.Right(index);
				if (!nickname.IsEmpty() ) {
					expandMessage(L"进入："+ nickname);
					if (connected_) {
						if (ocr_pipe_ != NULL) {
							struct Barrage
							{
								char  mode[16];
								char  nickname[64];
								char  content[256];
							}barrage;
							strncpy_s(barrage.mode, "MEMBER", 16);
							strncpy_s(barrage.nickname, CStringToUTF8(nickname), 64);
							//strncpy_s(barrage.content, CStringToUTF8(content), 256);
							if (!WriteFile(ocr_pipe_, &barrage, sizeof(barrage), &bytes_write, NULL)) {
								expandMessage(L"进入信息转发失败！");
								ocr_pipe_ = NULL;
							}
						}
					}
				}
			}
			continue;
		}			
		if (barrageList[i].find("点亮") != string::npos) {
			if (m_CheckLike) {
				vector<string>  spaceSplitlist;
				Char2SplitStr(barrageList[i], ' ', spaceSplitlist);
				if (!spaceSplitlist.empty()){
					for (int i = 0; i < spaceSplitlist.size(); i++) {
						if (spaceSplitlist[i].find("点亮") != string::npos) {
							CString name = string2CString(deldigits(spaceSplitlist[i]));
							int index = name.Find(L"点亮");
							CString nickname = name.Left(index);
							if (!nickname.IsEmpty()) {
								expandMessage(L"点赞：" + nickname);
								if (connected_) {
									if (ocr_pipe_ != NULL) {
										struct Barrage
										{
											char  mode[16];
											char  nickname[64];
											char  content[256];
										}barrage;
										strncpy_s(barrage.mode, "LIKE", 16);
										strncpy_s(barrage.nickname, CStringToUTF8(nickname), 64);
										//strncpy_s(barrage.nickname, CT2A(nickname.GetString()), 64);
										if (!WriteFile(ocr_pipe_, &barrage, sizeof(barrage), &bytes_write, NULL)) {
											expandMessage(L"点赞信息转发失败！");
											ocr_pipe_ = NULL;
										}
									}
								}
							}
						}
					}
				}
			}
			continue;
		}
		if (barrageList[i].find("送") != string::npos) {
			if (m_CheckGift) {
				CString name = string2CString(deldigits(barrageList[i]));
				int index = name.Find(L"送");
				CString nickname = name.Left(index);
				if (!nickname.IsEmpty()) {
					expandMessage(L"礼物：" + nickname);
					if (connected_) {
						if (ocr_pipe_ != NULL) {
							struct Barrage
							{
								char  mode[16];
								char  nickname[64];
								char  content[256];
							}barrage;
							strncpy_s(barrage.mode, "GIFT", 16);
							strncpy_s(barrage.nickname, CStringToUTF8(nickname), 64);
							//strncpy_s(barrage.nickname, CT2A(nickname.GetString()), 64);
							if (!WriteFile(ocr_pipe_, &barrage, sizeof(barrage), &bytes_write, NULL)) {
								expandMessage(L"礼物信息转发失败！");
								ocr_pipe_ = NULL;
							}
						}
					}
				}	
			}
			continue;
		}
		if (barrageList[i].find(":") != string::npos) {
			//expandMessage(string2CString(barrageList[i]));
			vector<string>  colonSplitlist;
			Char2SplitStr(barrageList[i], ':', colonSplitlist);
			if (!colonSplitlist.empty()) {
				vector<string>  spaceSplitlist;
				Char2SplitStr(colonSplitlist[0], ' ', spaceSplitlist);
				if (!spaceSplitlist.empty()) {
					if (m_CheckBarrage) {
						expandMessage(L"问答：" + string2CString(deldigits(spaceSplitlist.back())) + L" ：" + string2CString(colonSplitlist.back()));
						if (connected_) {
							if (ocr_pipe_ != NULL) {
								struct Barrage
								{
									char  mode[16];
									char  nickname[64];
									char  content[256];
								}barrage;
								strncpy_s(barrage.mode, "CHAT", 16);
								strncpy_s(barrage.nickname, StringToUTF8(deldigits(spaceSplitlist.back())), 64);
								strncpy_s(barrage.content, StringToUTF8(colonSplitlist.back()), 256);
								if (!WriteFile(ocr_pipe_, &barrage, sizeof(barrage), &bytes_write, NULL)) {
									expandMessage(L"问答信息转发失败！");
									ocr_pipe_ = NULL;
								}
							}
						}
					}	
				}
			}
			continue;
		}
	}
	return TRUE;
}
LRESULT  COCRCrawlerDlg::OnWXBarrageMessage(WPARAM wParam, LPARAM lParam) {
	vector<string>* list = (vector<string>*)lParam;
	DWORD bytes_write = 0;
	vector<string>  ocrpreList;
	vector<string>  ocrList;
	vector<string>  barrageList;
	//第一次筛选
	if (list->empty()) {
		return TRUE;
	}
	for (int i = 0; i < list->size(); i++) {
		string ocrstring = (*list)[i];
		ocrstring = regex_replace(ocrstring, regex("："), ":");
		if (ocrstring.find("送出") != string::npos) {
			vector<string>  spaceSplitlist;
			Char2SplitStr(ocrstring, ' ', spaceSplitlist);
			if (!spaceSplitlist.empty()) {
				for (int j = 0; j < spaceSplitlist.size(); j++) {
					if (spaceSplitlist[j].find("送出") != string::npos) {
						ocrpreList.push_back(spaceSplitlist[j]);
					}
				}
			}
			continue;
		}
		if (ocrstring.find(":") != string::npos) {
			vector<string>  spaceSplitlist;
			Char2SplitStr(ocrstring, ' ', spaceSplitlist);
			if (!spaceSplitlist.empty()) {
				for (int j = 0; j < spaceSplitlist.size(); j++) {
					if (spaceSplitlist[j].find(":") != string::npos) {
						ocrpreList.push_back(spaceSplitlist[j]);
					}
				}
			}
			continue;
		}
	}
	//第二次筛选
	if (ocrpreList.empty()) {
		return TRUE;
	}
	for (int i = 0; i < ocrpreList.size(); i++) {
		string ocrstring = ocrpreList[i];
		if (ocrstring.find("送出") != string::npos) {
			ocrList.push_back(ocrstring);
			continue;
		}
		if (ocrstring.find(":") != string::npos) {
			ocrList.push_back(ocrstring);
			continue;
		}
	}
	//去重
	if (ocrList.empty()) {
		return TRUE;
	}
	if (ocrlastList.empty()) {
		for (int i = 0; i < ocrList.size(); i++) {
			barrageList.push_back(ocrList[i]);
			ocrlastList.push_back(ocrList[i]);
		}
	}
	else {
		for (int i = 0; i < ocrList.size(); i++) {
			bool exist = false;
			for (int j = 0; j < ocrlastList.size(); j++) {
				if (CalculateSimilarity(ocrList[i], ocrlastList[j]) > 0.9) {
					exist = true;
					break;
				}
			}
			if (!exist) {
				barrageList.push_back(ocrList[i]);
			}
		}
		ocrlastList.clear();
		for (int i = 0; i < ocrList.size(); i++) {
			ocrlastList.push_back(ocrList[i]);
		}
	}
	//显示及转发
	if (barrageList.empty()) {
		return TRUE;
	}
	for (int i = 0; i < barrageList.size(); i++) {
		if (barrageList[i].find("送出") != string::npos) {
			if (m_CheckGift) {
				CString name = string2CString(barrageList[i]);
				int index = name.Find(L"送出");
				CString nickname = name.Left(index);
				string snickname = CT2A(nickname.GetString());
				if (!nickname.IsEmpty()) {
					expandMessage(L"礼物：" +string2CString(deldigits(snickname)));
					if (connected_) {
						if (ocr_pipe_ != NULL) {
							struct Barrage
							{
								char  mode[16];
								char  nickname[64];
								char  content[256];
							}barrage;
							strncpy_s(barrage.mode, "GIFT", 16);
							strncpy_s(barrage.nickname, StringToUTF8(snickname), 64);
							if (!WriteFile(ocr_pipe_, &barrage, sizeof(barrage), &bytes_write, NULL)) {
								expandMessage(L"礼物信息转发失败！");
								ocr_pipe_ = NULL;
							}
						}
					}

				}
			}
			continue;
		}
		if (barrageList[i].find(":") != string::npos) {
			//expandMessage(string2CString(barrageList[i]));
			vector<string>  colonSplitlist;
			Char2SplitStr(barrageList[i], ':', colonSplitlist);
			if (!colonSplitlist.empty()) {
				vector<string>  spaceSplitlist;
				Char2SplitStr(colonSplitlist[0], ' ', spaceSplitlist);
				if (!spaceSplitlist.empty()) {
					if (m_CheckBarrage) {
						expandMessage(L"问答：" + string2CString(deldigits(spaceSplitlist.back())) + L" ：" + string2CString(colonSplitlist.back()));
						if (connected_) {
							if (ocr_pipe_ != NULL) {
								struct Barrage
								{
									char  mode[16];
									char  nickname[64];
									char  content[256];
								}barrage;
								strncpy_s(barrage.mode, "CHAT", 16);
								strncpy_s(barrage.nickname, StringToUTF8(spaceSplitlist.back()), 64);
								strncpy_s(barrage.content, StringToUTF8(colonSplitlist.back()), 256);
								if (!WriteFile(ocr_pipe_, &barrage, sizeof(barrage), &bytes_write, NULL)) {
									expandMessage(L"问答信息转发失败！");
									ocr_pipe_ = NULL;
									printf("wx -- ocr_pipe == null");
								}
							}
						}
					}
				}	
			}
			continue;
		}
	}

	return TRUE;
}

void COCRCrawlerDlg::OnBnClickedRadioDouyin()
{
	// TODO: 在此添加控件通知处理程序代码
	m_RadioWeChat = false;
	m_RadioDouYin = true;
	m_RadioKuaiShou = false;
	m_CheckWelcome = false;           //欢迎
	m_CheckBarrage = true;              //弹幕
	m_CheckLike = false;                   //点赞
	m_CheckGift = false;                   //礼物
	((CButton*)GetDlgItem(IDC_CHECK_WELCOME))->EnableWindow(1);
	((CButton*)GetDlgItem(IDC_CHECK_LIKE))->EnableWindow(1); 
	((CButton*)GetDlgItem(IDC_CHECK_BARRAGE))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_GIFT))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_LIKE))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_WELCOME))->SetCheck(BST_UNCHECKED);
	m_MessageBox->SetWindowText(L"");
}
void COCRCrawlerDlg::OnBnClickedRadioKuaishou()
{
	// TODO: 在此添加控件通知处理程序代码
	m_RadioWeChat = false;
	m_RadioDouYin = false;
	m_RadioKuaiShou = true;
	m_CheckWelcome = false;           //欢迎
	m_CheckBarrage = true;              //弹幕
	m_CheckLike = false;                   //点赞
	m_CheckGift = false;                   //礼物
	((CButton*)GetDlgItem(IDC_CHECK_BARRAGE))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_GIFT))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_LIKE))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_WELCOME))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_WELCOME))->EnableWindow(1);
	((CButton*)GetDlgItem(IDC_CHECK_LIKE))->EnableWindow(1);
	m_MessageBox->SetWindowText(L"");
}
void COCRCrawlerDlg::OnBnClickedRadioWechat()
{
	// TODO: 在此添加控件通知处理程序代码
	m_RadioWeChat = true;
	m_RadioDouYin = false;
	m_RadioKuaiShou = false;
	m_CheckWelcome = false;           //欢迎
	m_CheckBarrage = true;              //弹幕
	m_CheckLike = false;                   //点赞
	m_CheckGift = false;                   //礼物
	((CButton*)GetDlgItem(IDC_CHECK_BARRAGE))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_GIFT))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_LIKE))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_WELCOME))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_WELCOME))->EnableWindow(0);
	((CButton*)GetDlgItem(IDC_CHECK_LIKE))->EnableWindow(0);
	m_MessageBox->SetWindowText(L"");
}


void COCRCrawlerDlg::OnStnClickedStaticFrame()
{
	// TODO: 在此添加控件通知处理程序代码
}
