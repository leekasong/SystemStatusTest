
// SystemStatusTestDlg.cpp : ���� ����
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
//��������
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

// CSystemStatusTestDlg ��ȭ ����

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


// CSystemStatusTestDlg �޽��� ó����

BOOL CSystemStatusTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

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
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CSystemStatusTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
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
	//���͸� ���� ����
	//https://msdn.microsoft.com/en-us/library/windows/desktop/aa373232(v=vs.85).aspx
	SYSTEM_POWER_STATUS power_status = { 0 };
	if (GetSystemPowerStatus(&power_status)) {

		if (power_status.BatteryFlag != 128) { //128 == No system battery
			if (power_status.ACLineStatus == 1)
				tmpLog.Format(TEXT("���� ������\r\n"));
			else
				tmpLog.Format(TEXT("���͸� �����\r\n"));
			m_strEditOutput += tmpLog;

			tmpLog.Format(TEXT("���͸� ������: %d%%\r\n"), power_status.BatteryLifePercent);
			m_strEditOutput += tmpLog;

			if (power_status.BatteryLifeTime != -1){
				int min = power_status.BatteryLifeTime / 60;
				if (min > 60)
					tmpLog.Format(TEXT("�����ð�: %d�ð� %d��\r\n"), min / 60, min % 60);
				else
					tmpLog.Format(TEXT("�����ð�: %d��\r\n"), min);
			}
			else
				tmpLog.Format(TEXT("�����ð� �����...\r\n"));
			m_strEditOutput += tmpLog;
		}
		else {
			tmpLog.Format(TEXT("No Battery\r\n"));
			m_strEditOutput += tmpLog;
		}
	}
	//-----------------------------------------------------------------------
	//�ϵ��ũ 
	DWORD dwDrive = GetLogicalDrives();
	DWORD dwDriveCh = 0x0001;
	TCHAR DriveText[3] = { 0 };

	for (TCHAR Drive = 'A'; Drive <= 'Z'; Drive++){
		wsprintf(DriveText, TEXT("%C:"), Drive);
		UINT type = GetDriveType(DriveText);
		if ((dwDrive & dwDriveCh) && (type == DRIVE_REMOVABLE || type == DRIVE_FIXED || type == DRIVE_RAMDISK)){
			// �ϵ�뷮			
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
