#include "stdafx.h"
#include "ie_wrapper.h"

#include <mshtml.h>

#include "resource.h"


IMPLEMENT_DYNCREATE(CIEWrapper, CWnd)
BOOL CIEWrapper::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_RBUTTONDOWN)||(pMsg->message == WM_RBUTTONDBLCLK))
	{
		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_PREVIEW_MENU));
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);

		POINT clickPoint;
		::GetCursorPos(&clickPoint);

		pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, clickPoint.x,  clickPoint.y, this);
		return TRUE;
	}
	return CWnd::PreTranslateMessage(pMsg);
}
BEGIN_MESSAGE_MAP(CIEWrapper, CWnd)
	ON_COMMAND(ID_UTF8, &CIEWrapper::OnUtf8)
	ON_COMMAND(ID_UTF32, &CIEWrapper::OnUtf32)
	ON_COMMAND(ID_UTF16, &CIEWrapper::OnUtf16)
	ON_COMMAND(ID_GB2312, &CIEWrapper::OnGb2312)
	ON_COMMAND(ID_GB18030, &CIEWrapper::OnGb18030)
	ON_COMMAND(ID_GBK, &CIEWrapper::OnGbk)
	ON_COMMAND(ID_HZ, &CIEWrapper::OnHz)
	ON_COMMAND(ID_BIG5, &CIEWrapper::OnBig5)
	ON_COMMAND(ID_BIG5HK, &CIEWrapper::OnBig5hk)
	ON_COMMAND(ID_EUCTW, &CIEWrapper::OnEuctw)
	ON_COMMAND(ID_ISO2022CN, &CIEWrapper::OnIso2022cn)
	ON_COMMAND(ID_ISO88591, &CIEWrapper::OnIso88591)
	ON_COMMAND(ID_MACROMAN, &CIEWrapper::OnMacroman)
END_MESSAGE_MAP()

void CIEWrapper::SetCharset(LPTSTR charset)
{
	IDispatch *pDisp = get_Document();
	CComQIPtr<IDispatch> disp(pDisp);  
	if(pDisp)
	{
		CComQIPtr<IHTMLDocument2> doc(disp);  
		if(doc)
		{
			doc->put_charset(charset);
			this->Refresh();
		}
	}
}

void CIEWrapper::OnUtf8()
{
	SetCharset(L"utf-8");
}

void CIEWrapper::OnUtf32()
{
	SetCharset(L"utf-32");
}

void CIEWrapper::OnUtf16()
{
	SetCharset(L"utf-16");
}

void CIEWrapper::OnGb2312()
{
	SetCharset(L"gb2312");
}

void CIEWrapper::OnGb18030()
{
	SetCharset(L"gb18030");
}

void CIEWrapper::OnGbk()
{
	SetCharset(L"gbk");
}

void CIEWrapper::OnHz()
{
	SetCharset(L"hz");
}

void CIEWrapper::OnBig5()
{
	SetCharset(L"big5");
}

void CIEWrapper::OnBig5hk()
{
	SetCharset(L"big5-hkscs");
}

void CIEWrapper::OnEuctw()
{
	SetCharset(L"euc-tw");
}

void CIEWrapper::OnIso2022cn()
{
	SetCharset(L"iso-2022-cn");
}

void CIEWrapper::OnIso88591()
{
	SetCharset(L"iso-8859-1");
}

void CIEWrapper::OnMacroman()
{
	SetCharset(L"MacRoman");
}

