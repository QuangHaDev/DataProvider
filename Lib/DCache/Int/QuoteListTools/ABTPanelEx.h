#if !defined(AFX_ABTPANELEX_H__3A12E674_0AE0_4C7C_9D2F_465C05D28801__INCLUDED_)
#define AFX_ABTPANELEX_H__3A12E674_0AE0_4C7C_9D2F_465C05D28801__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ABTPanelEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CABTPanelEx window

//=========================================================================
// CCellItem class
//=========================================================================
class CCellItem
{
public:
	CCellItem()
	{
		m_nRow = 0;
		m_nCol = 0;
		m_nTick = 0;
		m_clrRestore = 0;
	}
	CCellItem( int nRow, int nCol, int nTick=0, COLORREF clrRestore=0 )
	{
		m_nRow = nRow;
		m_nCol = nCol;
		m_nTick = nTick;
		m_clrRestore = clrRestore;
	}

public:
	int m_nRow;
	int m_nCol;
	int m_nTick;
	COLORREF m_clrRestore;
};


//=========================================================================
// CCellFormat class
//=========================================================================
class CCellFormat  
{
public:
	CCellFormat()
	{
		m_clrForeColor        = 0;
		m_clrBackColor        = RGB(255,255,255);
		m_nAlternateFontIndex = 0;
		m_blnFontBold         = FALSE;
		m_blnFontItalic       = FALSE;
		m_nAlign              = TA_LEFT;
		m_blnDrawArrow        = FALSE;
		m_blnDrawBorder       = FALSE;
		m_clrBorder           = RGB(255,0,0);
		m_nDrawEdge           = 0;
		m_clrEdge             = m_clrBorder;
		m_clrUpArrow		  = RGB(0,255,0);
		m_clrDnArrow		  = RGB(255,0,0);
	}

	CCellFormat( COLORREF clrForeColor, COLORREF clrBackColor, int nAlign=TA_LEFT )
	{
		m_clrForeColor         = clrForeColor;
		m_clrBackColor         = clrBackColor;
		m_nAlternateFontIndex  = 0;
		m_blnFontBold          = FALSE;
		m_blnFontItalic        = FALSE;
		m_nAlign               = nAlign;
		m_blnDrawArrow         = FALSE;
		m_blnDrawBorder        = FALSE;
		m_clrBorder            = RGB(255,0,0);
		m_nDrawEdge            = 0;
		m_clrEdge              = m_clrBorder;
		m_clrUpArrow		   = RGB(0,255,0);
		m_clrDnArrow		   = RGB(255,0,0);
	}
	virtual ~CCellFormat(){};


public:
	COLORREF m_clrForeColor;	// Cell foreground color.
	COLORREF m_clrBackColor;	// Cell background color.
	int      m_nAlternateFontIndex;
	BOOL     m_blnFontBold;
	BOOL     m_blnFontItalic;
	int	     m_nAlign;
	BOOL     m_blnDrawArrow;
	BOOL     m_blnDrawBorder;
	int      m_nDrawEdge;
	COLORREF m_clrBorder;
	COLORREF m_clrEdge;
	COLORREF m_clrUpArrow;
	COLORREF m_clrDnArrow;
};


//=========================================================================
// CABTPanelEx class
//=========================================================================
class CABTPanelEx : public CListCtrl
{
// Construction
public:
	CABTPanelEx();

// Attributes
public:
	COLORREF m_clrForeColor;    // Current foreground color.
	COLORREF m_clrBackColor;    // Current background color.
	COLORREF m_clrHighLight;    // Current HighLight Text Color
	CMapStringToPtr  m_map;     // Map use to store update position.
	CMapStringToPtr* m_pmapColorUpdate; // A list of color updating cells.
	BOOL  m_blnSelectMode;      // Highlight selection.
	BOOL  m_blnEraseBkg;        // Erase background flag.
	CFont m_fontAlternate[3];   // Alternate font.
	BOOL  m_blnSlideMode;       // For column resizing.
	int   m_nCodePage;          // Current code page.

	enum { ABTEXLABEL=0, ABTEXDATA };
	enum { ABTEXVERT, ABTEXHORZ };
	enum { MAX_TICK=5 };
	enum { TOP_EDGE=1, LEFT_EDGE=2, RIGHT_EDGE=4, BOTTOM_EDGE=8 };

private:
	BOOL ArchiveSetting( LPCTSTR lpszFilename, LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, CString& strText, BOOL bSave );
	void (*m_fnLButtonDown)(UINT,CPoint,void*);
	void (*m_fnLButtonUp)(UINT,CPoint,void*);
	void (*m_fnRButtonDown)(UINT,CPoint,void*);
	void (*m_fnRButtonUp)(UINT,CPoint,void*);
	BOOL m_bRestoreCapture;
	bool  m_use_owner_draw;          // if use owner draw.

// Operations
public:
	void DrawColorText( int nRow, int nCol, LPCTSTR lpszText, COLORREF clrColor );
	CPoint HitTestEx( const CPoint& pt );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CABTPanelEx)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetCodePage();
	void SetCodePage(int nCodePage);
	int GetCellDrawEdge(int nRow, int nCol, COLORREF& clrEdge);
	void SetCellDrawEdge(int nRow, int nCol, int nEdge, COLORREF clrEdge);
	BOOL GetCellDrawBorder(int nRow, int nCol, COLORREF& clrBorder);
	void SetCellDrawBorder(int nRow, int nCol, BOOL blnEnabled, COLORREF clrBorder=0);
	BOOL GetCellDrawArrow(int nRow, int nCol);
	void SetCellDrawArrow(int nRow, int nCol, BOOL blnEnabled, COLORREF clrUpArrow=0, COLORREF clrDnArrow=0);
	void ResetColOrder( BOOLEAN blnRefresh=FALSE );
	BOOL LoadColSettings( LPCTSTR lpszFilename, LPCTSTR lpszSectionName, int nMode=0 );
	BOOL SaveColSettings( LPCTSTR lpszFilename, LPCTSTR lpszSectionName, int nMode=0 );
	BOOL GetColWidth( CString& strColWidth );
	BOOL SetColWidth( LPCTSTR lpszColWidth );
	BOOL GetColOrder( CString& strOrder );
	BOOL SetColOrder( LPCTSTR lpszOrder );
	void SetRButtonUpFn(void (*fnRButtonUp)(UINT,CPoint,void *));
	void SetRButtonDownFn(void (*fnRButtonDown)(UINT,CPoint,void *));
	void SetLButtonUpFn(void (*fnLButtonUp)(UINT,CPoint,void *));
	void SetLButtonDownFn( void fnLButtonDown(UINT,CPoint,void*) );
	UINT GetCellAlign( int nRow, int nCol );
	void SetCellAlign( int nRow, int nCol, int nAlign );
	void SetColAlign(int nCol, int nAlign);
	void CreateChineseAlternateFont(LPCTSTR lpszFontName, int nSize);
	CString GetItemTextEx( int nRow, int nCol );
	void SetItemTextEx( int nRow, int nCol, LPCTSTR lpszText, BOOL blnSkipConversion=FALSE );
	BOOL DeleteItemWithStyle( int nItem );
	void ClearAllStyle( BOOL blnRemovePtrArray=TRUE, int nFromRow=0, int nToRow=-1, int nFromCol=0, int nToCol=-1 );
	void Clear( BOOL blnClearStyle=FALSE );
	void SetFont( CFont* pFont, BOOL bRedraw=TRUE );
	int GetCellFont( int nRow, int nCol, int nIndex );
	void SetCellFont( int nRow, int nCol, int nIndex, BOOL blnUpdate=TRUE );
	void CreateAlternateFont( LPCTSTR lpszFontName, int nSize, int nIndex, BOOL blnBold=FALSE, BYTE bCharSet=ANSI_CHARSET );
	CString GetHeaderText( int nCol );
	void SetHeaderText( int nCol, LPSTR lpszText );
	BOOL GetFixedRows();
	void SetFixedRows( BOOL blnFixedRow );
	void ClearMap( BOOL blnDestroyColorUpdateMap=TRUE, BOOL blnClearMap=TRUE, BOOL blnRestoreColor=FALSE );
	void SetUniformColWidthEx( int nCols, int narExcludeCols[], int nArrayElements, int nOffset=0 );
	void SetUniformColWidth( int nCols, int nOffset=0 );
	void SetGridStyleEx( int nStart, int nEnd, int nLinesPerSection, int nOrientation, COLORREF clrColor );
	BOOL GetHighLight();
	void SetHighLight( BOOL blnHighLight=TRUE );
	COLORREF GetHighLightColor();
	BOOL SetHighLightColor( COLORREF clrColor, BOOL blnUpdate );
	BOOL ReadTemplate( LPCTSTR lpszFilename );
	int GetColWidth( int nCol );
	void SetColWidth( int nCol, int nWidth );
	void SetColorUpdate( BOOL blnColorUpdate );
	void SetGridLines( BOOL blnGridLines );
	CString GetFID( LPCTSTR lpszFid );
	void SetFID( LPCTSTR lpszFid, LPCTSTR lpszValue, BOOL blnColorUpdate=TRUE );
	void SetFID(LPCTSTR lpszFid, LPCTSTR lpszValue, COLORREF clrColor);
	CString GetGridData( int nRow, int nCol, int& nDataType );
	void SetGridData( int nRow, int nCol, int nDataType, LPCTSTR lpszText, BOOL blnSkipConversion=FALSE );
	int GetScrollBars();
	void SetScrollBars( int nMode );
	int GetCols();
	void SetCols( int nCols, BOOL blnRedraw=TRUE, int nFormat=LVCFMT_LEFT, int nWidth=100 );
	int GetRows();
	void SetRows( int nRows, BOOL blnRedraw=TRUE );
	int GetColAlignment( int nCol );
	void SetColAlignment( int nCol, int nMode );
	CString GetTextMatrix( int nRow, int nCol );
	void SetTextMatrix( int nRow, int nCol, LPCTSTR lpszText );
	COLORREF GetBackColor();
	BOOL SetBackColor( COLORREF clrColor, BOOL blnUpdate=TRUE );
	COLORREF GetForeColor();
	BOOL SetForeColor( COLORREF clrColor, BOOL blnUpdate=TRUE );
	COLORREF GetCellForeColor( int nRow, int nCol );
	BOOL SetCellForeColor( int nRow, int nCol, COLORREF clrColor, BOOL blnUpdate=TRUE );
	COLORREF GetCellBackColor( int nRow, int nCol );
	BOOL SetCellBackColor( int nRow, int nCol, COLORREF clrColor, BOOL blnUpdate=TRUE );
	CCellFormat* GetCell( int nRow, int nCol, BOOL blnInsertMode=FALSE );
	virtual ~CABTPanelEx();
	void CABTPanelEx::SetUseOwnerDraw( bool value );

	//static void TimerProc( HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime );

	// Generated message map functions
protected:
	//{{AFX_MSG(CABTPanelEx)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	virtual afx_msg void OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult );

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABTPANELEX_H__3A12E674_0AE0_4C7C_9D2F_465C05D28801__INCLUDED_)
