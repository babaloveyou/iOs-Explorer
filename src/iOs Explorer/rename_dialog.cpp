/*
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# The Original Code is Copyright (C) 2009 by exherb (htpp://4leaf.me).
# All rights reserved.
#
# The Original Code is: all of this file.
#
# Contributor(s): none yet.
#
*/
#include "stdafx.h"
#include "rename_dialog.h"

#include "ios_explorer.h"

IMPLEMENT_DYNAMIC(CRenameDialog, CDialog)
CRenameDialog::CRenameDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRenameDialog::IDD, pParent)
{

}

CRenameDialog::~CRenameDialog()
{
}

void CRenameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CRenameDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_EDIT)->SetWindowText(m_name);
	GetDlgItem(IDC_EDIT)->SetFocus();
	return TRUE;
}

void CRenameDialog::SetName(const CString &content)
{
	m_name = content;
}

CString CRenameDialog::GetName() const
{
	return m_name;
}

BEGIN_MESSAGE_MAP(CRenameDialog, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CRenameDialog::OnDestroy()
{
	GetDlgItem(IDC_EDIT)->GetWindowText(m_name);
	CDialog::OnDestroy();
}
