#ifndef ADD_FAV_DIALOG_H_
#define ADD_FAV_DIALOG_H_

#include <afxwin.h>

#include "resource.h"

class CAddFavDialog : public CDialog
{
	DECLARE_DYNAMIC(CAddFavDialog)

public:
	CAddFavDialog(CWnd* pParent = NULL);
	virtual ~CAddFavDialog();

	void SetFavName(const CString &name);
	CString GetFavName() const;
	void SetFavAddress(const CString &address);
	CString GetFavAddress() const;

	enum { IDD = IDD_ADDFAV_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	virtual BOOL OnInitDialog();
	CString m_name;
	CString m_address;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();

};

#endif // ADD_FAV_DIALOG_H_

