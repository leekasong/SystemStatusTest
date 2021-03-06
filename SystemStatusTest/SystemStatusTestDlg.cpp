
// SystemStatusTestDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "SystemStatusTest.h"
#include "SystemStatusTestDlg.h"
#include "afxdialogex.h"

#include "SystemStatus.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//----------------------------------------------
//전역변수
CSystemStatus sStatus;

size_t	cpuCount = 0;
LONG	*pArrayCore = nullptr;

size_t	netAdaptorCount = 0;
LONG	*pArrayNetValue = nullptr;

#ifdef UNICODE
std::wstring *pNetAdaptorName=nullptr;
#else
std::string *pNetAdaptorName = nullptr;
#endif

// CSystemStatusTestDlg 대화 상자

CSystemStatusTestDlg::CSystemStatusTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYSTEMSTATUSTEST_DIALOG, pParent)
	, m_strEditOutput(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSystemStatusTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_OUTPUT, m_strEditOutput);
}

BEGIN_MESSAGE_MAP(CSystemStatusTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CSystemStatusTestDlg 메시지 처리기

BOOL CSystemStatusTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	sStatus.Init();
	cpuCount = sStatus.getCPUCount();
	pArrayCore = new LONG[cpuCount];

	netAdaptorCount = sStatus.getNetAdaptorCount();

#ifdef UNICODE
	pNetAdaptorName = new std::wstring[netAdaptorCount];
#else
	pNetAdaptorName = new std::string[netAdaptorCount];
#endif
	TCHAR* pList = sStatus.getNetAdatorList();
	for(int cnt=0; cnt<netAdaptorCount; cnt++){
		pNetAdaptorName[cnt] = pList;
		pList += (1 + _tcslen(pList));
	}
	pArrayNetValue = new LONG[netAdaptorCount];

	SetTimer(0, 1000, NULL);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CSystemStatusTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CSystemStatusTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSystemStatusTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	sStatus.Update();
	CString tmpLog;

	//-----------------------------------------------------------------------
	LONG cpu;
	sStatus.getCPUStatus(cpu, pArrayCore, cpuCount);
	tmpLog.Format(TEXT("CPU: %d%% \r\n"), cpu);
	m_strEditOutput = tmpLog;

	for (int cnt = 0; cnt < cpuCount; cnt++) {
		tmpLog.Format(TEXT("core(%d): %d%% \r\n"), cnt, pArrayCore[cnt]);
		m_strEditOutput += tmpLog;
	}
	//-----------------------------------------------------------------------
	int aMem = 0;
	int pMem = 0;
	sStatus.getRAMStatus(aMem, pMem);
	tmpLog.Format(TEXT("RAM: %dMB / %dMB\r\n"), aMem, pMem);
	m_strEditOutput += tmpLog;
	//-----------------------------------------------------------------------
	sStatus.getNETStatus(pArrayNetValue, netAdaptorCount);
	for (int cnt = 0; cnt < netAdaptorCount; cnt++) {
		tmpLog.Format(TEXT("%s: %dbyte\r\n"), pNetAdaptorName[cnt].c_str(), pArrayNetValue[cnt]);
		m_strEditOutput += tmpLog;
	}
	//-----------------------------------------------------------------------
	//배터리 관련 정보
	//https://msdn.microsoft.com/en-us/library/windows/desktop/aa373232(v=vs.85).aspx
	SYSTEM_POWER_STATUS power_status = { 0 };
	if (GetSystemPowerStatus(&power_status)) {

		if (power_status.BatteryFlag != 128) { //128 == No system battery
			if (power_status.ACLineStatus == 1)
				tmpLog.Format(TEXT("전원 연결중\r\n"));
			else
				tmpLog.Format(TEXT("배터리 사용중\r\n"));
			m_strEditOutput += tmpLog;

			tmpLog.Format(TEXT("배터리 충전률: %d%%\r\n"), power_status.BatteryLifePercent);
			m_strEditOutput += tmpLog;

			if (power_status.BatteryLifeTime != -1){
				int min = power_status.BatteryLifeTime / 60;
				if (min > 60)
					tmpLog.Format(TEXT("남은시간: %d시간 %d분\r\n"), min / 60, min % 60);
				else
					tmpLog.Format(TEXT("남은시간: %d분\r\n"), min);
			}
			else
				tmpLog.Format(TEXT("남은시간 계산중...\r\n"));
			m_strEditOutput += tmpLog;
		}
		else {
			tmpLog.Format(TEXT("No Battery\r\n"));
			m_strEditOutput += tmpLog;
		}
	}
	//-----------------------------------------------------------------------
	//하드디스크 
	DWORD dwDrive = GetLogicalDrives();
	DWORD dwDriveCh = 0x0001;
	TCHAR DriveText[3] = { 0 };

	for (TCHAR Drive = 'A'; Drive <= 'Z'; Drive++){
		wsprintf(DriveText, TEXT("%C:"), Drive);
		UINT type = GetDriveType(DriveText);
		if ((dwDrive & dwDriveCh) && (type == DRIVE_REMOVABLE || type == DRIVE_FIXED || type == DRIVE_RAMDISK)){
			// 하드용량			
			ULARGE_INTEGER i64FreeBytesToCaller = { 0 }, i64TotalBytes = { 0 }, i64FreeBytes = { 0 };
			BOOL bRsult = GetDiskFreeSpaceEx(DriveText, (PULARGE_INTEGER)&i64FreeBytesToCaller, (PULARGE_INTEGER)&i64TotalBytes, (PULARGE_INTEGER)&i64FreeBytes);
			if (bRsult){
				float ra = 1.0f - (float)(int)(i64FreeBytes.QuadPart >> 20) / (float)(int)(i64TotalBytes.QuadPart >> 20);
				tmpLog.Format(TEXT("%c: %I64d MB / %I64d MB(%d%%)\r\n"), Drive, i64FreeBytes.QuadPart >> 20, i64TotalBytes.QuadPart >> 20, (int)(ra * 100));
				m_strEditOutput += tmpLog;
			}
		}
		dwDriveCh <<= 1;
	}
	//-----------------------------------------------------------------------

		
	


	UpdateData(FALSE);
	CDialogEx::OnTimer(nIDEvent);
}


void CSystemStatusTestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	sStatus.Terminate();
	delete[] pArrayCore;
	delete[] pArrayNetValue;
	delete[] pNetAdaptorName;
}
