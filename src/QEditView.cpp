#include "pch.h"
#include "framework.h"
#include "QEdit.h"

#include "QEditDoc.h"
#include "QEditView.h"
#include "MainFrm.h"

#include "usermsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CQEditView, CView)

BEGIN_MESSAGE_MAP(CQEditView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_DROPFILES()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CQEditView::CQEditView() noexcept
{

}

CQEditView::~CQEditView()
{

}

BOOL CQEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	//Modify the Window class or styles here by modifying CREATESTRUCT cs
	return CView::PreCreateWindow(cs);
}

void CQEditView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	DragAcceptFiles(TRUE);
	GetParent()->SendMessage(WM_CUSTOM_VIEW_SIZE_CHANGED);
}

void CQEditView::OnDraw(CDC* cDC)
{
	CQEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(!pDoc)
	{
		return;
	}
	CRect rect;
	this->GetClientRect(&rect);
	int width = rect.Width();
	int height = rect.Height();

	if(pDoc->FilePath() == NULL)
	{
		CFont font;
		font.CreatePointFont(200, _T("Calibri"));
		CFont* oldFont = cDC->SelectObject(&font);

		CString str = _T("Drop video file here...");
		CSize strSize = cDC->GetTextExtent(str);
		cDC->SetTextColor(RGB(128, 128, 128));
		cDC->TextOutW((width - strSize.cx) / 2, (height - strSize.cy) / 2, str);

		cDC->SelectObject(oldFont);
	}
	else
	{
		//Double buffering
		CMemDC dbMemDC = CMemDC(*cDC, this);
		CDC& dbDC = dbMemDC.GetDC();
		CDC* pDC = &dbDC;

		//Render black bars as background
		CBrush brush = CBrush(RGB(0, 0, 0));
		pDC->FillRect(&rect, &brush);

		//Calculate target size
		auto [vidWidth, vidHeight] = pDoc->VideoResolution();
		double vidRatio = vidWidth / static_cast<double>(vidHeight);
		double wndRatio = width / static_cast<double>(height);
		int targetWidth = vidRatio < wndRatio ? static_cast<int>(height * vidRatio) : width;
		int targetHeight = vidRatio > wndRatio ? static_cast<int>(width / vidRatio) : height;

		//Get video params
		double time = 0.0;
		if(CMediaCtrlWnd* mediaCtrl = GetMediaCtrl())
		{
			time = mediaCtrl->CurrentTime() * pDoc->Duration();
		}

		//Render video frame
		CBitmap bitmap = CBitmap();
		if(pDoc->GetVideoFrame(time, targetWidth, targetHeight, &bitmap))
		{
			CDC memDC;
			memDC.CreateCompatibleDC(pDC);
			CBitmap* old = memDC.SelectObject(&bitmap);
			BITMAP bm;
			bitmap.GetBitmap(&bm);
			pDC->StretchBlt((width - targetWidth) / 2, (height - targetHeight) / 2, targetWidth, targetHeight, &memDC, 0, bm.bmHeight, bm.bmWidth, -bm.bmHeight, SRCCOPY);
			memDC.SelectObject(old);
		}
	}
}

CMediaCtrlWnd* CQEditView::GetMediaCtrl() const
{
	if(CMainFrame* mainFrm = dynamic_cast<CMainFrame*>(GetParentFrame()))
	{
		return mainFrm->GetMediaCtrl();
	}
	return nullptr;
}

BOOL CQEditView::OnEraseBkgnd(CDC* pDC)
{
	//Required to prevent flickering (in combination with CMemDC double buffering in OnDraw)
	return TRUE;
}

void CQEditView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}

void CQEditView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CQEditView::OnDropFiles(HDROP hDropInfo)
{
	CView::OnDropFiles(hDropInfo);

	UINT nFiles = DragQueryFileA(hDropInfo, (UINT)-1, NULL, 0);
	if(nFiles == 1)
	{
		char file[MAX_PATH];
		UINT len = DragQueryFileA(hDropInfo, 0, file, MAX_PATH);

		CQEditDoc* doc = GetDocument();
		if(doc != nullptr)
		{
			if(!doc->SetVideoFile(file))
			{
				AfxMessageBox(_T("Input file is not a valid video file!"));
			}
			GetParent()->SendMessage(WM_COMMAND, ID_CUSTOM_VIDEO_LOADED);
			if(CMediaCtrlWnd* mediaCtrl = GetMediaCtrl())
			{
				mediaCtrl->InitVideoInfo(doc->Duration(), doc->FrameCount(), doc->AvgFps());
			}
		}
	}
	else
	{
		AfxMessageBox(_T("Expected a single file!"), MB_ICONEXCLAMATION | MB_OK);
	}
	DragFinish(hDropInfo);
}

void CQEditView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	GetParent()->SendMessage(WM_CUSTOM_VIEW_SIZE_CHANGED);
}

#ifdef _DEBUG
void CQEditView::AssertValid() const
{
	CView::AssertValid();
}

void CQEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CQEditDoc* CQEditView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CQEditDoc)));
	return (CQEditDoc*)m_pDocument;
}
#endif //_DEBUG
