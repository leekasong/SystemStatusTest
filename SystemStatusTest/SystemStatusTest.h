
// SystemStatusTest.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CSystemStatusTestApp:
// �� Ŭ������ ������ ���ؼ��� SystemStatusTest.cpp�� �����Ͻʽÿ�.
//

class CSystemStatusTestApp : public CWinApp
{
public:
	CSystemStatusTestApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CSystemStatusTestApp theApp;