#include "stdafx.h"
#include "add_fav_dialog.h"

#include "ios_explorer.h"

IMPLEMENT_DYNAMIC(CAddFavDialog, CDialog)
CAddFavDialog::CAddFavDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAddFavDialog::IDD, pParent)
{

}

CAddFavDialog::~CAddFavDialog()
{
}

void CAddFavDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);;
}


BEGIN_MESSAGE_MAP(CAddFavDialog, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CAddFavDialog::SetFavName(const CString &name)
{
	m_name = name;
}

CString CAddFavDialog::GetFavName() const
{
	return m_name;
}

void CAddFavDialog::SetFavAddress(const CString &address)
{
	m_address = address;
}

CString CAddFavDialog::GetFavAddress() const
{
	return m_address;
}

BOOL CAddFavDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_ADDFAV_NAME)->SetWindowText(m_name);
	GetDlgItem(IDC_ADDFAV_ADDRESS)->SetWindowText(m_address);
	GetDlgItem(IDC_ADDFAV_ADDRESS)->SetFocus();
	return TRUE;
}

void CAddFavDialog::OnDestroy()
{
	GetDlgItem(IDC_ADDFAV_NAME)->GetWindowText(m_name);
	GetDlgItem(IDC_ADDFAV_ADDRESS)->GetWindowText(m_address);
	CDialog::OnDestroy();
}
// CHAddFavDialog 消息处理程序
