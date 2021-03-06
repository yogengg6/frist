// OpenEBTSEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OpenEBTSEditor.h"
#include "MainFrm.h"
#include "StatusDialog.h"
#include "OpenEBTSEditorDoc.h"
#include "OpenEBTSEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenEBTSEditorApp

BEGIN_MESSAGE_MAP(COpenEBTSEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &COpenEBTSEditorApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// COpenEBTSEditorApp construction

COpenEBTSEditorApp::COpenEBTSEditorApp()
{
	m_pStatusWnd = NULL;
}


// The one and only COpenEBTSEditorApp object

COpenEBTSEditorApp theApp;


// COpenEBTSEditorApp initialization

BOOL COpenEBTSEditorApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(COpenEBTSEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(COpenEBTSEditorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	CStatusDialog dlg;
	BOOL bShowStatus = (!cmdInfo.m_strFileName.IsEmpty());

	if(bShowStatus)
	{
		m_pStatusWnd = &dlg;
		dlg.Create(CStatusDialog::IDD);
		dlg.ShowWindow(SW_SHOW);
	}

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	BOOL bProcessed = ProcessShellCommand(cmdInfo);

	if(bShowStatus)
	{
		dlg.ShowWindow(SW_HIDE);
		dlg.DestroyWindow();
	}

	if (!bProcessed)
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	m_pStatusWnd = m_pMainWnd;

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void COpenEBTSEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// COpenEBTSEditorApp message handlers


int COpenEBTSEditorApp::ExitInstance()
{
	GdiplusShutdown(gdiplusToken);

	return CWinApp::ExitInstance();
}
