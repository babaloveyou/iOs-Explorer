#ifndef SPLITTER_WINDOW_H_
#define SPLITTER_WINDOW_H_

#define WM_SPLITTER_MOVED   (WM_USER+1000)
class CSplitterWindow : public CWnd
{
public:
	CSplitterWindow();
	virtual ~CSplitterWindow();

    BOOL BindWithControl(CWnd *parent, DWORD ctrlId);
    void Unbind(void);

    void SetMinWidth(int left, int right);

    BOOL AttachAsLeftPane(DWORD ctrlId);
    BOOL AttachAsRightPane(DWORD ctrlId);
    BOOL DetachAllPanes(void);
    void RecalcLayout(void);

protected:
    BOOL GetMouseClipRect(LPRECT rectClip, CPoint point);

    CWnd        *m_Parent;
    CDWordArray m_leftIds, m_rightIds;
    int         m_minLeft, m_minRight;

protected:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

class CySplitterWnd : public CWnd
{
public:
	CySplitterWnd();
	virtual ~CySplitterWnd();

    BOOL BindWithControl(CWnd *parent, DWORD ctrlId);
    void Unbind(void);

    void SetMinHeight(int above, int below);

    BOOL AttachAsAbovePane(DWORD ctrlId);
    BOOL AttachAsBelowPane(DWORD ctrlId);
    BOOL DetachAllPanes(void);
    void RecalcLayout(void);

protected:
    BOOL GetMouseClipRect(LPRECT rectClip, CPoint point);

    CWnd        *m_Parent;
    CDWordArray m_aboveIds, m_belowIds;
    int         m_minAbove, m_minBelow;

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()

};


#endif // SPLITTER_WINDOW_H_
