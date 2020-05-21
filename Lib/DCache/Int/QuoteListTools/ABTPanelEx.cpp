// ABTPanelEx.cpp : implementation file
//

#include "stdafx.h"
#include "ABTPanelEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CABTPanelEx

CABTPanelEx::CABTPanelEx() : m_map(101)
{
	m_clrForeColor    = 0;
	m_clrBackColor    = RGB(255,255,255);
	m_clrHighLight	  = RGB(255,0,0);
	m_pmapColorUpdate = NULL;
	m_blnSelectMode   = FALSE;
	m_blnEraseBkg     = TRUE;
	m_blnSlideMode    = TRUE;
	m_fnLButtonDown   = NULL;
	m_fnLButtonUp     = NULL;
	m_fnRButtonDown   = NULL;
	m_fnRButtonUp     = NULL;
	m_bRestoreCapture = FALSE;
	m_nCodePage       = 1252;
	m_use_owner_draw  = false;
}

CABTPanelEx::~CABTPanelEx()
{
}


BEGIN_MESSAGE_MAP(CABTPanelEx, CListCtrl)
	//{{AFX_MSG_MAP(CABTPanelEx)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT( NM_CUSTOMDRAW, OnCustomDraw )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CABTPanelEx message handlers

//=======================================================================
// Event Handler
//=======================================================================
void CABTPanelEx::OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult )
{
	int nRow;
	int nCol;
	CPtrArray* pPtrArray;
	CCellFormat* pCellFormat;
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	static CFont* pFont=NULL;


	// Restore previous font.
	if ( pFont )
	{
		SelectObject( lplvcd->nmcd.hdc, pFont );
		*pResult = CDRF_NEWFONT;
		pFont = NULL;
	}

	// Removes selection.
	if ( !m_blnSelectMode )
		SetItemState( lplvcd->nmcd.dwItemSpec, 0x0, LVIS_SELECTED | LVIS_FOCUSED );

	switch ( lplvcd->nmcd.dwDrawStage )
	{
		case CDDS_PREPAINT:
			//*pResult = CDRF_NOTIFYITEMDRAW;
			// Set this will fire a POSTPAINT message.
			//*pResult = CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			//m_blnEraseBkg = FALSE;
			return;
			break;

		/*
		case CDDS_POSTPAINT:
			TRACE( "PostPaint\n" );
			break;
		*/

		case CDDS_ITEMPREPAINT:
			//TRACE( "ItemPrepaint\n" );
			//lplvcd->clrText = RGB(255,255,255);
			//lplvcd->clrTextBk = RGB(50,50,240);
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;

		case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
			// Get current row and col value.
			nRow = lplvcd->nmcd.dwItemSpec;
			nCol = lplvcd->iSubItem;
			// Set default forecolor and backcolor;
			lplvcd->clrText = m_clrForeColor;
			lplvcd->clrTextBk = m_clrBackColor;
			// If there's a format for current cell, use it.
			pPtrArray = (CPtrArray*)GetItemData( nRow );
			if ( pPtrArray )
			{
				if ( pPtrArray->GetSize() > nCol )
				{
					pCellFormat = (CCellFormat*)(*pPtrArray)[nCol];
					if ( pCellFormat )
					{
						lplvcd->clrText = pCellFormat->m_clrForeColor;
						lplvcd->clrTextBk = pCellFormat->m_clrBackColor;

						if ( pCellFormat->m_blnDrawArrow ) {
							pCellFormat->m_nAlign = 1;
						}

						// Draw cell alignment (if other than default).
						if ( ((pCellFormat->m_nAlign > 0)&&(m_use_owner_draw)) || (pCellFormat->m_blnDrawBorder) || (pCellFormat->m_nDrawEdge) )
						{
							HDC hDC = lplvcd->nmcd.hdc;

                            CDC dc;
                            dc.Attach( hDC );

							// Get cell rectangle.
							CRect rect;
							if ( nCol == 0 )
							{
								GetItemRect( nRow, &rect, LVIR_BOUNDS );
								rect.left += 2;	// Add an offset.
							}
							else
							{
								GetSubItemRect( nRow, nCol, LVIR_BOUNDS, rect );
							}
							// Erase background.
							COLORREF clrFg = GetCellForeColor( nRow, nCol );
							COLORREF clrBg = GetCellBackColor( nRow, nCol );
							CBrush brBg( clrBg );
							COLORREF clrOldFg = ::SetTextColor( hDC, clrFg );
							::FillRect( hDC, &rect, brBg );
							// Different handling for first column.
							if ( nCol == 0 )
								rect.right = GetColWidth( 0 ) - GetScrollPos(SB_HORZ);

							// Draw the text.
							UINT nFormat = DT_VCENTER;
							switch ( pCellFormat->m_nAlign )
							{
								case LVCFMT_CENTER:
									nFormat += DT_CENTER;
									break;
								case LVCFMT_RIGHT:
									nFormat += DT_RIGHT;
									break;
								default:
									nFormat += DT_LEFT;
							}
							CString strItem = GetItemText( nRow, nCol );

							// Change Font Style.
							CFont* pOldFont = NULL;
							int nIndex = pCellFormat->m_nAlternateFontIndex;
							if ( nIndex > 0 )
								pOldFont = (CFont*)::SelectObject( hDC, m_fontAlternate[nIndex].GetSafeHandle() );

							// Clip region.
							CRgn rgn;
							rgn.CreateRectRgnIndirect( &rect );
							::SelectClipRgn( hDC, (HRGN)rgn.GetSafeHandle() );

							// Draw unicode text.
//							CUString ustrItem( strItem );
//							ustrItem.Draw( &dc, rect, nFormat );
							dc.DrawText( strItem, rect, nFormat );

							if ( pCellFormat->m_blnDrawBorder )
							{
								// Draw borders around cell.
								CDC dc;
								CBrush* pBrush, *pOldBrush;
								CPen* pOldPen;
								CRect rectBorder(rect);
								int nOldBkMode;
								CPen penThick(PS_SOLID, 2, pCellFormat->m_clrBorder);
								CPen penBG(PS_SOLID, 1, pCellFormat->m_clrBackColor);
								// Prepare DC.
								dc.Attach(hDC);
								nOldBkMode = dc.SetBkMode(TRANSPARENT);
								pBrush = CBrush::FromHandle((HBRUSH)::GetStockObject(NULL_BRUSH));
								pOldBrush = dc.SelectObject(pBrush);
								pOldPen = dc.SelectObject(&penThick);
								// Draw rectangle.
								rectBorder.InflateRect(-1, -1);
								dc.Rectangle(&rectBorder);
								dc.SelectObject(&penBG);
								// Restore DC.
								dc.SelectObject(pOldPen);
								dc.SelectObject(pOldBrush);
								dc.Detach();
							}

							if (pCellFormat->m_nDrawEdge) {
								// * Draw edge around cell
								CDC dc;
								dc.Attach(hDC);
								CPen penLine(PS_SOLID, 1, pCellFormat->m_clrBackColor);
								CPen penEdge(PS_SOLID, 2, pCellFormat->m_clrEdge);
								CPen *pOldPen = dc.SelectObject(&penEdge);
								CRect rectEdge(rect);
								rectEdge.InflateRect(-1, -2);

								int nEdge = pCellFormat->m_nDrawEdge;
								if (nEdge & TOP_EDGE) {
									dc.MoveTo(rectEdge.left, rectEdge.top);
									dc.LineTo(rectEdge.right, rectEdge.top);
								}
								if (nEdge & LEFT_EDGE) {
									dc.MoveTo(rectEdge.left, rect.top);
									dc.LineTo(rectEdge.left, rect.bottom);
								}
								if (nEdge & RIGHT_EDGE) {
									dc.MoveTo(rectEdge.right, rect.top);
									dc.LineTo(rectEdge.right, rect.bottom);
								}
								if (nEdge & BOTTOM_EDGE) {
									dc.MoveTo(rectEdge.left, rectEdge.bottom);
									dc.LineTo(rectEdge.right, rectEdge.bottom);
								}

								dc.SelectObject(penLine);
								if (nEdge & TOP_EDGE) {
									dc.MoveTo(rect.left, rect.top);
									dc.LineTo(rect.right, rect.top);
								}
								if (nEdge & BOTTOM_EDGE) {
									dc.MoveTo(rect.left, rect.bottom-1);
									dc.LineTo(rect.right, rect.bottom-1);
								}


								dc.SelectObject(pOldPen);
								dc.Detach();
							}

							if ( pCellFormat->m_blnDrawArrow )
							{
								// Draw arrows.
								int nStart=0;
								int i;
								int nArrowWidth;
								CSize sizePos;
								CSize sizeChar;
								CPoint p[3];
								int nX;
								int nY;
								int nX2;
								int nY2;
								CPen penGreen;
								CPen penGreen2;
								CBrush brGreen;
								CPen penRed;
								CPen penRed2;
								CBrush brRed;
								int nLenSearchStr;
								int nLenReplaceStr;
								TEXTMETRIC tm;
								const int nOffset = 3;

								CString sSearchUp = "+";
								CString sSearchDown = "-";
								CString sReplaceUp = "+";
								CString sReplaceDown = "-";

								// Prepare DC.
								dc.GetTextMetrics( &tm );
								int nSaveDC = dc.SaveDC();

								CSize sizeItem = dc.GetOutputTextExtent( strItem );
								switch ( pCellFormat->m_nAlign )
								{
									case LVCFMT_CENTER:
										rect.left = (rect.right+rect.left)/2 - (sizeItem.cx/2);
										break;
									case LVCFMT_RIGHT:
										rect.left = rect.right - sizeItem.cx;
										break;
								}

								CSize sizeArrow = dc.GetOutputTextExtent( "0", 1 );
								CSize sizeSpace = dc.GetOutputTextExtent( "W", 1 );
								// Up arrow.
								nLenSearchStr = sSearchUp.GetLength();
								nLenReplaceStr = sReplaceUp.GetLength();
								sizeChar = dc.GetOutputTextExtent( sReplaceUp, nLenReplaceStr );
								nArrowWidth = sizeArrow.cx / 4;
								penGreen.CreatePen( PS_SOLID, 1, pCellFormat->m_clrUpArrow );
								penGreen2.CreatePen( PS_SOLID, nArrowWidth, pCellFormat->m_clrUpArrow );
								brGreen.CreateSolidBrush( pCellFormat->m_clrUpArrow );
								sizeChar.cy -= 3;
								while ( (i=strItem.Find(sSearchUp,nStart)) > -1 )
								{
									sizePos = dc.GetOutputTextExtent( strItem, i+nLenReplaceStr );
									nX = rect.left + sizeChar.cx - sizeSpace.cx;
									nX2 = nX + sizeArrow.cx;
									nY = rect.top;
									nY2 = rect.top + (int)(sizeChar.cy*0.3);
									p[0].x = nX;
									p[0].y = nY2+2;
									p[1].x = nX + (sizeArrow.cx/2);
									p[1].y = nY+2;
									p[2].x = nX2 - 1;
									p[2].y = nY2+2;
									dc.FillSolidRect( CRect(nX,rect.top,rect.left+sizeChar.cx,rect.bottom), clrBg );
									dc.SelectObject( &penGreen );
									dc.SelectObject( &brGreen );
									dc.Polygon( p, 3 );
									dc.SelectObject( &penGreen2);
									dc.MoveTo( nX + (sizeArrow.cx/2), nY2 );
									dc.LineTo( nX + (sizeArrow.cx/2), rect.bottom-2);
									nStart = i+1;
								}
								// Down arrow.
								sizeChar = dc.GetOutputTextExtent( sReplaceDown, nLenReplaceStr );
								penRed.CreatePen( PS_SOLID, 1, pCellFormat->m_clrDnArrow );
								penRed2.CreatePen( PS_SOLID, nArrowWidth, pCellFormat->m_clrDnArrow );
								brRed.CreateSolidBrush( pCellFormat->m_clrDnArrow );
								sizeChar.cy -= 3;
								while ( (i=strItem.Find(sSearchDown,nStart)) > -1 )
								{
									sizePos = dc.GetOutputTextExtent( strItem, i+nLenReplaceStr );
									nX = rect.left + sizeChar.cx - sizeSpace.cx;
									nX2 = nX + sizeArrow.cx;
									nY = rect.top + 3;
									nY2 = rect.bottom - (int)(sizeChar.cy*0.3);
									p[0].x = nX;
									p[0].y = nY2-2;
									p[1].x = nX + (sizeArrow.cx/2);
									p[1].y = rect.bottom-2;
									p[2].x = nX2 - 1;
									p[2].y = nY2-2;
									dc.FillSolidRect( CRect(nX,rect.top,rect.left+sizeChar.cx,rect.bottom), clrBg );
									dc.SelectObject( &penRed );
									dc.SelectObject( &brRed );
									dc.Polygon( p, 3 );
									dc.SelectObject( &penRed2 );
									dc.MoveTo( nX + (sizeArrow.cx/2), nY2 );
									dc.LineTo( nX + (sizeArrow.cx/2), rect.top+2);
									nStart = i+1;
								}

								dc.RestoreDC( nSaveDC );
							}

							// Restore DC settings.
							::SelectClipRgn( hDC, NULL );
							::SetTextColor( hDC, clrOldFg );
							if ( pOldFont )
								::SelectObject( hDC, pOldFont );

							dc.Detach();

							*pResult = CDRF_SKIPDEFAULT;
							return;
						}

                        // Change Font Style.
						int nIndex = pCellFormat->m_nAlternateFontIndex;
						if ( nIndex > 0 )
						{
							if ( pFont == NULL )
								pFont = (CFont*)SelectObject( lplvcd->nmcd.hdc, m_fontAlternate[nIndex].GetSafeHandle() );
							*pResult = CDRF_NEWFONT;
							return;
						}
					}
				}
			}
			*pResult = CDRF_DODEFAULT;
			break;
	}
}

/*
void CABTPanelEx::OnPaint() 
{
	//CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	// First let the control do its default drawing.
	const MSG* pMsg = GetCurrentMessage();
	DefWindowProc( pMsg->message, pMsg->wParam, pMsg->lParam );
	
	// Do not call CListCtrl::OnPaint() for painting messages
}
*/

int CABTPanelEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	ModifyStyle( LVS_LIST|LVS_ICON|LVS_SMALLICON, LVS_REPORT|LVS_NOCOLUMNHEADER|LVS_NOSORTHEADER|LVS_SINGLESEL );
	DWORD dwStyle = GetExtendedStyle();
	dwStyle |= LVS_EX_FLATSB;
	SetExtendedStyle( dwStyle );

	// Create default alternate font (BOLD).
	CFont* pFont = GetFont();
	LOGFONT logFont;
	pFont->GetLogFont( &logFont );
	logFont.lfWeight = FW_BOLD;
	m_fontAlternate[1].CreateFontIndirect( &logFont );

	return 0;
}

void CABTPanelEx::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	CListCtrl::PreSubclassWindow();

	ModifyStyle( LVS_EDITLABELS|LVS_LIST|LVS_ICON|LVS_SMALLICON, LVS_REPORT|LVS_NOCOLUMNHEADER|LVS_NOSORTHEADER|LVS_SINGLESEL );
	DWORD dwStyle = GetExtendedStyle();
	dwStyle |= LVS_EX_FLATSB;
	SetExtendedStyle( dwStyle );

	// Create default alternate font (BOLD).
	CFont* pFont = GetFont();
	LOGFONT logFont;
	pFont->GetLogFont( &logFont );
	logFont.lfWeight = FW_BOLD;
	m_fontAlternate[1].CreateFontIndirect( &logFont );
}

void CABTPanelEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	m_blnEraseBkg = TRUE;
	
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CABTPanelEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	m_blnEraseBkg = TRUE;
	
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CABTPanelEx::OnSize(UINT nType, int cx, int cy) 
{
	CListCtrl::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	m_blnEraseBkg = TRUE;
}

void CABTPanelEx::OnDestroy() 
{
	
	// TODO: Add your message handler code here
	// Removes all cell items and col arrays.
	KillTimer( 1 );

	ClearAllStyle( TRUE );
	/*
	for ( int i=0; i<GetItemCount(); i++ )
	{
		pPtrArray = (CPtrArray*)GetItemData( i );
		if ( pPtrArray )
		{
			for ( int j=0; j<pPtrArray->GetSize(); j++ )
			{
				pCellFormat = (CCellFormat*)(*pPtrArray)[j];
				if ( pCellFormat )
					delete pCellFormat;
			}
			delete pPtrArray;
		}
	}
	*/

	// Clear both maps.
	ClearMap();

	CListCtrl::OnDestroy();
}

BOOL CABTPanelEx::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	//return FALSE;
	
	/*
	if ( m_blnEraseBkg )
		return CListCtrl::OnEraseBkgnd(pDC);
	else
		return FALSE;
	*/


	//========================================
	// Erase only the undraw part.
	//========================================
	int nTop;
	int nBottom;
	int nRight;
	int nLastRowItem = GetItemCount() - 1;
	if ( nLastRowItem < 0 )
	{
		nTop    = 0;
		nBottom = 0;
		nRight  = 0;
	}
	else
	{
		// Get the whole item rect.
		RECT ItemRect;
		GetItemRect( nLastRowItem, &ItemRect, LVIR_BOUNDS );
		nBottom = ItemRect.bottom;
		nRight = ItemRect.right;
		GetItemRect( 0, &ItemRect, LVIR_BOUNDS );
		nTop = ItemRect.top-1;
	}

	//RECT rect;
	CRect rect;
	CRect rectTemp;
	GetClientRect( &rect );

	// Erase the upper left frame.
	CPen pen( PS_SOLID, 2, m_clrBackColor );
	CPen* oldPen = pDC->SelectObject( &pen );
	pDC->MoveTo( rect.left+1, nTop );
	pDC->LineTo( rect.left+1, rect.bottom );
	pDC->MoveTo( rect.left, nTop );
	pDC->LineTo( rect.right, nTop );
	pDC->SelectObject( oldPen );
	// Erase the bottom part.
	if ( nBottom < rect.bottom )
	{
		rectTemp = rect;
		rectTemp.top = nBottom;
		pDC->FillSolidRect( &rectTemp, m_clrBackColor );
	}
	// Erase the right part.
	if ( nRight < rect.right )
	{
		rectTemp = rect;
		rectTemp.left = nRight;
		pDC->FillSolidRect( &rectTemp, m_clrBackColor );
	}

	return FALSE;
}

//=======================================================================
// Helper Function
//=======================================================================
CCellFormat* CABTPanelEx::GetCell(int nRow, int nCol, BOOL blnInsertMode)
{
	CPtrArray* pPtrArray;

	// Get col array for the specify row.
	pPtrArray = (CPtrArray*)GetItemData( nRow );
	if ( pPtrArray == NULL )
	{
		pPtrArray = new CPtrArray;
		SetItemData( nRow, (DWORD)pPtrArray );
	}
	
	// Get cell format.
	CCellFormat* pCellFormat=NULL;
	int nSize = pPtrArray->GetSize();
	if ( nCol < nSize )
	{
		pCellFormat = (CCellFormat*)(*pPtrArray)[nCol];
	}

	// If insert mode is set, add cell format is there's none.
	if ( (blnInsertMode) && (pCellFormat == NULL) )
	{
		pCellFormat = new CCellFormat( m_clrForeColor, m_clrBackColor );
		pPtrArray->SetAtGrow( nCol, pCellFormat );
	}

	return pCellFormat;
}

//=======================================================================
// Properties
//=======================================================================
void CABTPanelEx::DrawColorText( int nRow, int nCol, LPCTSTR lpszText, COLORREF clrColor )
{
	SetCellForeColor( nRow, nCol, clrColor );
	SetCellForeColor( nRow, nCol, m_clrForeColor, FALSE );
}

BOOL CABTPanelEx::SetCellForeColor(int nRow, int nCol, COLORREF clrColor, BOOL blnUpdate )
{
	CCellFormat* pCellFormat = GetCell( nRow, nCol, TRUE );
	pCellFormat->m_clrForeColor = clrColor;

	if ( blnUpdate )
	{
		CString strItem;
		strItem = GetItemTextEx( nRow, nCol );
		SetItemTextEx( nRow, nCol, strItem );
	}

	return TRUE;
}

COLORREF CABTPanelEx::GetCellForeColor(int nRow, int nCol)
{
	CCellFormat* pCellFormat = GetCell( nRow, nCol );
	if ( pCellFormat )
	{
		return pCellFormat->m_clrForeColor;
	}
	return m_clrForeColor;
}

BOOL CABTPanelEx::SetCellBackColor(int nRow, int nCol, COLORREF clrColor, BOOL blnUpdate )
{
	CCellFormat* pCellFormat = GetCell( nRow, nCol, TRUE );
	pCellFormat->m_clrBackColor = clrColor;

	if ( blnUpdate )
	{
		CString strItem;
		strItem = GetItemTextEx( nRow, nCol );
		SetItemTextEx( nRow, nCol, strItem );
	}

	return TRUE;
}

COLORREF CABTPanelEx::GetCellBackColor(int nRow, int nCol)
{
	CCellFormat* pCellFormat = GetCell( nRow, nCol );
	if ( pCellFormat )
	{
		return pCellFormat->m_clrBackColor;
	}
	return m_clrBackColor;
}

BOOL CABTPanelEx::SetForeColor(COLORREF clrColor, BOOL blnUpdate)
{
	m_clrForeColor = clrColor;
	if ( blnUpdate )
	{
		SetRedraw();
	}
	return TRUE;
}

COLORREF CABTPanelEx::GetForeColor()
{
	return m_clrForeColor;
}

BOOL CABTPanelEx::SetBackColor(COLORREF clrColor, BOOL blnUpdate)
{
	m_clrBackColor = clrColor;
	SetBkColor( clrColor );
	if ( blnUpdate )
	{
		SetRedraw();
	}
	return TRUE;
}

COLORREF CABTPanelEx::GetBackColor()
{
	return m_clrBackColor;
}

void CABTPanelEx::SetTextMatrix(int nRow, int nCol, LPCTSTR lpszText)
{
	SetItemTextEx( nRow, nCol, lpszText );
}

CString CABTPanelEx::GetTextMatrix(int nRow, int nCol)
{
	return GetItemTextEx( nRow, nCol );
}

void CABTPanelEx::SetColAlignment(int nCol, int nMode)
{
	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_FMT;
	lvColumn.fmt = nMode;
	SetColumn( nCol, &lvColumn );
	Invalidate();
}

int CABTPanelEx::GetColAlignment(int nCol)
{
	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_FMT;
	GetColumn( nCol, &lvColumn );

	return lvColumn.fmt;
}

void CABTPanelEx::SetRows(int nRows, BOOL blnRedraw)
{
	int nNum = GetItemCount();

	if ( blnRedraw )
		SetRedraw( FALSE );
	// When current # of rows is greater than set value.
	if ( nNum > nRows )
	{
		// Delete cell style for the deleted row.
		ClearAllStyle( TRUE, nRows, nNum );
		int nDiff = nNum-nRows;

		for ( int j=0; j<nDiff; j++ )
			DeleteItem( nRows );
		m_blnEraseBkg = TRUE;
		if ( blnRedraw )
			SetRedraw( TRUE );
		return;
	}

	// Append rows.
	for ( int i=nNum; i<nRows; i++ )
	{
		InsertItem( i, _T("") );
	}

	if ( blnRedraw )
		SetRedraw( TRUE );
}

int CABTPanelEx::GetRows()
{
	return GetItemCount();
}

void CABTPanelEx::SetCols(int nCols, BOOL blnRedraw, int nFormat, int nWidth )
{
	CHeaderCtrl* pHeadCtrl = GetHeaderCtrl();
	int nNum = pHeadCtrl->GetItemCount();

	if ( blnRedraw )
		SetRedraw( FALSE );
	// When current # of columns is greater than set value.
	if ( nNum > nCols )
	{
		// Delete cell style for the deleted columns.
		ClearAllStyle( FALSE, 0, GetItemCount(), nCols, nNum );

		int nDiff = nNum-nCols;
		for ( int j=0; j<nDiff; j++ )
			DeleteColumn( nCols );
		// Clear the background and redraw the screen.
		m_blnEraseBkg = TRUE;
		SetRedraw( TRUE );
		Invalidate();
		return;
	}

	// Append columns.
	for ( int i=nNum; i<nCols; i++ )
	{
		InsertColumn( i, _T(""), nFormat, nWidth, i );
	}


	if ( blnRedraw )
	{
		SetRedraw( TRUE );
		Invalidate();
	}
}

int CABTPanelEx::GetCols()
{
	CHeaderCtrl* pHeadCtrl = GetHeaderCtrl();
	return pHeadCtrl->GetItemCount();
}

void CABTPanelEx::SetScrollBars(int nMode)
{
	BOOL blnShow=FALSE;
	int nCode;
	DWORD dwAdd=0;
	DWORD dwRemove=0;

	switch ( nMode )
	{
		case 0:		// Remove Both scrollbars
			blnShow = FALSE;
			nCode = SB_BOTH;
			dwAdd = LVS_NOSCROLL;
			break;

		case 1:		// Horizontal
			blnShow = TRUE;
			nCode = SB_HORZ;
			dwAdd = LVS_NOSCROLL;
			break;

		case 2:		// Vertical
			blnShow = TRUE;
			nCode = SB_VERT;
			dwAdd = LVS_NOSCROLL;
			break;

		case 3:		// Add Both scrollbars
			blnShow = TRUE;
			nCode = SB_BOTH;
			dwRemove = LVS_NOSCROLL;
			break;

		default:	// Remove scrollbar
			blnShow = FALSE;
			nCode = SB_BOTH;
			dwAdd = LVS_NOSCROLL;
			break;
	}

	ModifyStyle( dwRemove, dwAdd );
	FlatSB_ShowScrollBar( this->GetSafeHwnd(), nCode, blnShow );
	Invalidate();
}

int CABTPanelEx::GetScrollBars()
{
	return 0;
}

void CABTPanelEx::SetGridData(int nRow, int nCol, int nDataType, LPCTSTR lpszText, BOOL blnSkipConversion /*=FALSE*/)
{
	if ( nDataType == ABTEXLABEL )
	{
		SetItemTextEx( nRow, nCol, lpszText, blnSkipConversion );
	}
	else if ( nDataType == ABTEXDATA )
	{
		CCellItem* pItemInfo;
		pItemInfo = new CCellItem( nRow, nCol );
		void* pValue;
		if ( m_map.Lookup( lpszText, pValue ) )
		{
			CCellItem* pTemp = (CCellItem*)pValue;
			delete pTemp;
            pTemp = 0;
		}
		m_map.SetAt( lpszText, pItemInfo );
	}
}

CString CABTPanelEx::GetGridData(int nRow, int nCol, int& nDataType)
{
	CString strKey;
	CCellItem* pItemInfo;
	void* pVoid;

	POSITION pos = m_map.GetStartPosition();
	while ( pos )
	{
		m_map.GetNextAssoc( pos, strKey, pVoid );
		pItemInfo = (CCellItem*)pVoid;
		if ( (nRow==pItemInfo->m_nRow) && (nCol==pItemInfo->m_nCol) )
		{
			nDataType = ABTEXDATA;
			return strKey;
		}
	}
	nDataType = ABTEXLABEL;
	return GetItemTextEx( nRow, nCol );
}

void CABTPanelEx::SetFID(LPCTSTR lpszFid, LPCTSTR lpszValue, BOOL blnColorUpdate)
{
	void* pVoid;
	CCellItem* pItemInfo;

	if ( m_map.Lookup( lpszFid, pVoid ) )
	{
		pItemInfo = (CCellItem*)pVoid;
		SetItemTextEx( pItemInfo->m_nRow, pItemInfo->m_nCol, lpszValue );

		// For color update.
		if ( blnColorUpdate )
		{
			if ( m_pmapColorUpdate )
			{
				CCellItem* pUpdateItem;
				if ( m_pmapColorUpdate->Lookup( lpszFid, pVoid ) )
					pUpdateItem = (CCellItem*)pVoid;
				else
				{
					pUpdateItem = new CCellItem;
					*pUpdateItem = *pItemInfo;
					pUpdateItem->m_clrRestore = GetCellForeColor( pUpdateItem->m_nRow, pUpdateItem->m_nCol );
					m_pmapColorUpdate->SetAt( lpszFid, pUpdateItem );
				}
				// Set/reset tick value to maximum.
				pUpdateItem->m_nTick = MAX_TICK;
				// Set cell color to red.
				SetCellForeColor( pUpdateItem->m_nRow, pUpdateItem->m_nCol, m_clrHighLight, FALSE );
			}
		}
	}
}

void CABTPanelEx::SetFID(LPCTSTR lpszFid, LPCTSTR lpszValue, COLORREF clrColor)
{
	void* pVoid;
	CCellItem* pItemInfo;

	if ( m_map.Lookup( lpszFid, pVoid ) )
	{
		pItemInfo = (CCellItem*)pVoid;
		SetCellForeColor( pItemInfo->m_nRow, pItemInfo->m_nCol, clrColor );
		SetItemTextEx( pItemInfo->m_nRow, pItemInfo->m_nCol, lpszValue );
	}
}


CString CABTPanelEx::GetFID(LPCTSTR lpszFid)
{
	void* pVoid;
	CCellItem* pItemInfo;
	
	if ( m_map.Lookup( lpszFid, pVoid ) )
	{
		pItemInfo = (CCellItem*)pVoid;
		return GetItemTextEx( pItemInfo->m_nRow, pItemInfo->m_nCol );
	}

	return _T("");
}

void CABTPanelEx::SetGridLines(BOOL blnGridLines)
{
	DWORD dwStyle = GetExtendedStyle();
	if ( blnGridLines )
		dwStyle |= LVS_EX_GRIDLINES;
	else
		dwStyle &= ~LVS_EX_GRIDLINES;

	SetExtendedStyle( dwStyle );
}

void CABTPanelEx::OnTimer(UINT nIDEvent) 
//void TimerProc( HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime )
{
	// TODO: Add your message handler code here and/or call default
	CString strKey;
	void* pVoid;
	CCellItem* pItemInfo;

	if ( m_pmapColorUpdate == NULL )
		return;

	POSITION pos = m_pmapColorUpdate->GetStartPosition();
	while ( pos )
	{
		m_pmapColorUpdate->GetNextAssoc( pos, strKey, pVoid );
		pItemInfo = (CCellItem*)pVoid;
		if ( --pItemInfo->m_nTick == 0 )
		{
			m_pmapColorUpdate->RemoveKey( strKey );
			SetCellForeColor( pItemInfo->m_nRow, pItemInfo->m_nCol, pItemInfo->m_clrRestore );
			delete pItemInfo;
            pItemInfo = 0;
		}
	}
	
	//CListCtrl::OnTimer(nIDEvent);
}

void CABTPanelEx::SetColorUpdate(BOOL blnColorUpdate)
{
	KillTimer( 1 );

	// Delete all items in the color map if already exist.
	if ( m_pmapColorUpdate )
	{
		CString strKey;
		CCellItem* pItemInfo;
		void* pVoid;

		POSITION posMap = m_pmapColorUpdate->GetStartPosition();
		while ( posMap )
		{
			m_pmapColorUpdate->GetNextAssoc( posMap, strKey, pVoid );
			if ( pVoid )
			{
				pItemInfo = (CCellItem*)pVoid;
				delete pItemInfo;
                pItemInfo = 0;
			}
		}
		m_pmapColorUpdate->RemoveAll();
		delete m_pmapColorUpdate;
		m_pmapColorUpdate = NULL;
	}


	if ( blnColorUpdate )
	{
		// Enable color update.
		m_pmapColorUpdate = new CMapStringToPtr( 101 );
		//SetTimer( 1, 250, NULL );
	}
}

void CABTPanelEx::SetColWidth(int nCol, int nWidth)
{
	SetRedraw( FALSE );
	m_blnSlideMode = FALSE;
	SetColumnWidth( nCol, nWidth );
	m_blnSlideMode = TRUE;
	SetRedraw( TRUE );
}

int CABTPanelEx::GetColWidth(int nCol)
{
	return GetColumnWidth( nCol );
}

BOOL CABTPanelEx::ReadTemplate(LPCTSTR lpszFilename)
{
	FILE* input_file;
	char szLine[1024];
	char szToken[1024];
	int iRow;
	int iCol;
	int iColMax;
	BOOL bFinish;
	char* szStart;
	char* szCurr;
	CString strData;


	// Validate input file.
	if ( (lpszFilename == NULL) || (strlen(lpszFilename)==0) )
		return FALSE;

	// Open file for reading.
	if ( ( input_file = fopen( lpszFilename, "r" ) ) == NULL )
	{
		CString strMsg;
		strMsg.Format( _T("Error opening file(%s)."), lpszFilename );
		AfxMessageBox( strMsg );
		return FALSE;
	}


	// Initialize variables.
	memset( szLine, 0, 1024 );
	iRow    = 0;
	iColMax = 0;
	bFinish = FALSE;
	SetRows( 1 );
	SetCols( 1 );

	SetRedraw( FALSE );


	// Read each item, determine their data type, and put in grid.
	while ( fgets( szLine, 1024, input_file ) )
	{
		iCol    = 0;
		bFinish = FALSE;
		szCurr  = szLine;
		SetRows( iRow+1 );

		while ( !bFinish )
		{
			szStart = strchr( szCurr, '\"' );
			if ( szStart )
			{
				// Expand Column if neccessary.
				if ( iColMax < (iCol+1) )
				{
					iColMax = iCol + 1;
					SetCols( iColMax );
				}
				// Get token and put in grid.
				memset( szToken, 0, 1024 );
				szCurr = ++szStart;
				sscanf( szCurr, "%[^\"]", szToken );
				szCurr += strlen(szToken) + 1;

				switch ( szToken[0] )
				{
					case '@':	// Data
						strData = szToken + 1;
						SetGridData( iRow, iCol, ABTEXDATA, strData );
						break;

					default:	// Label
						strData = szToken + 1;
						SetGridData( iRow, iCol, ABTEXLABEL, szToken );
						//SetTextMatrix( iRow, iCol, szToken );
				}
			}
			else
			{
				bFinish = TRUE;
			}
			iCol++;
		}
		iRow++;
	}

	fclose( input_file );
	SetRedraw( TRUE );

	return TRUE;
}

void CABTPanelEx::SetHighLight(BOOL blnHighLight)
{
	m_blnSelectMode = blnHighLight;
}

BOOL CABTPanelEx::SetHighLightColor(COLORREF clrColor, BOOL blnUpdate)
{
	m_clrHighLight = clrColor;
	if ( blnUpdate )
	{
		SetRedraw();
	}
	return TRUE;
}

BOOL CABTPanelEx::GetHighLight()
{
	return m_blnSelectMode;
}

void CABTPanelEx::SetGridStyleEx(int nStart, int nEnd, int nLinesPerSection, int nOrientation, COLORREF clrColor)
{
	int i;
	int nCol;

	// Set automatic calculate value.
	if ( nStart == -1 )
		nStart = 0;
	if ( nEnd == -1 )
		nEnd = GetRows();

	switch ( nOrientation )
	{
		case ABTEXVERT:
			for ( i=nStart; i<nEnd; i+=nLinesPerSection )
			{
				for ( nCol=0; nCol<GetCols(); nCol++ )
				{
					SetCellBackColor( i, nCol, clrColor, FALSE );
				}
			}
			break;
	}
}

void CABTPanelEx::SetUniformColWidth(int nCols, int nOffset )
{
	// Check for invalidate Cols value.
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	pHeaderCtrl->SetRedraw( FALSE );
	SetRedraw( FALSE );

	// Do not show horizontal scroll for offset value < 0.
	if ( nOffset < 0 )
	{
		nOffset = GetSystemMetrics( SM_CXVSCROLL );
	}

	if ( nCols > 0 )
	{
		// Get size of control.
		CRect rect;
		GetClientRect( &rect );

		// Calculate size of column.
		int nSize = (rect.right-nOffset) / nCols;

		// Set all columns to the new size.
		for ( int i=0; i<nCols; i ++ )
		{
			SetColWidth( i, nSize );
		}
	}
	SetRedraw( TRUE );
	pHeaderCtrl->SetRedraw( TRUE );
}

void CABTPanelEx::ClearMap( BOOL blnDestroyColorUpdateMap, BOOL blnClearMap, BOOL blnRestoreColor )
{
	// Remove whole map.
	CCellItem* pItemInfo;
	void* pVoid;
	CString strKey;

	if ( blnClearMap )
	{
		POSITION pos = m_map.GetStartPosition();
		while ( pos )
		{
			m_map.GetNextAssoc( pos, strKey, pVoid );
			pItemInfo = (CCellItem*)pVoid;
			delete pItemInfo;
			pItemInfo = NULL;
			m_map.RemoveKey( strKey );
		}
		//m_map.RemoveAll();
	}

	// Delete all items in the color udpate map.
	if ( blnDestroyColorUpdateMap )
	{
		if ( m_pmapColorUpdate )
		{
			POSITION posMap = m_pmapColorUpdate->GetStartPosition();
			while ( posMap )
			{
				m_pmapColorUpdate->GetNextAssoc( posMap, strKey, pVoid );
				pItemInfo = (CCellItem*)pVoid;
                if ( blnRestoreColor ) {
					SetCellForeColor( pItemInfo->m_nRow, pItemInfo->m_nCol, pItemInfo->m_clrRestore );
                }
				delete pItemInfo;
                pItemInfo = 0;
			}
			m_pmapColorUpdate->RemoveAll();
			delete m_pmapColorUpdate;
			m_pmapColorUpdate = NULL;
		}
	}
	else
	{
		if ( m_pmapColorUpdate )
		{
			POSITION posMap = m_pmapColorUpdate->GetStartPosition();
			while ( posMap )
			{
				m_pmapColorUpdate->GetNextAssoc( posMap, strKey, pVoid );
				pItemInfo = (CCellItem*)pVoid;
                if ( blnRestoreColor ) {
					SetCellForeColor( pItemInfo->m_nRow, pItemInfo->m_nCol, pItemInfo->m_clrRestore );
                }
				delete pItemInfo;
				pItemInfo = NULL;
				m_pmapColorUpdate->RemoveKey( strKey );
			}
		}
	}
}

void CABTPanelEx::SetFixedRows(BOOL blnFixedRow)
{
	if ( blnFixedRow )
		ModifyStyle( LVS_NOCOLUMNHEADER, 0 );
	else
		ModifyStyle( 0, LVS_NOCOLUMNHEADER );
}

BOOL CABTPanelEx::GetFixedRows()
{
	DWORD dwStyle = GetStyle();
	if ( dwStyle & LVS_NOCOLUMNHEADER )
		return FALSE;
	else
		return TRUE;
}

void CABTPanelEx::SetHeaderText(int nCol, LPSTR lpszText)
{
	//HDITEM hdItem;
	HDITEMW hdItem;
	WCHAR wszItem[256];

	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	if ( pHeaderCtrl == NULL )
		return;

	// Check for valid range.
	if ( pHeaderCtrl->GetItemCount() < nCol )
		return;

	if ( !MultiByteToWideChar( m_nCodePage, 0, lpszText, -1, wszItem, 256 ) )
	{
		TRACE( _T("Error converting to Unicode(CABTPanel::SetHeaderText).\n") );
		return;
	}
	hdItem.mask = HDI_TEXT;
	//hdItem.pszText = lpszText;
	//hdItem.cchTextMax = strlen( lpszText );
	//pHeaderCtrl->SetItem( nCol, &hdItem );
	hdItem.pszText = wszItem;
	hdItem.cchTextMax = wcslen( wszItem );

	::SendMessage( pHeaderCtrl->GetSafeHwnd(), HDM_SETITEMW, (WPARAM)(int)nCol, (LPARAM)(const HDITEMW FAR*)(&hdItem) );
}

CString CABTPanelEx::GetHeaderText(int nCol)
{
	HDITEM hdItem;
	char szItem[256];

	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	memset( szItem, 0, 256 );

	hdItem.mask = HDI_TEXT;
	hdItem.pszText = szItem;
	hdItem.cchTextMax = 256;

	if ( pHeaderCtrl->GetItem( nCol, &hdItem ) )
		return szItem;
	else
		return _T("");
}

void CABTPanelEx::CreateAlternateFont(LPCTSTR lpszFontName, int nSize, int nIndex, BOOL blnBold, BYTE bCharSet)
{
	// Create font use 1 base value 
	if ( ( nIndex < 1 ) || ( nIndex > 2 ) )
		return;

	if ( m_fontAlternate[nIndex].GetSafeHandle() )
	{
		m_fontAlternate[nIndex].DeleteObject();
	}
	CClientDC dc(this);
    LOGFONT lf;
    memset( &lf, 0, sizeof(lf) );
    strcpy( lf.lfFaceName, lpszFontName );
	lf.lfCharSet = bCharSet;
	if ( blnBold )
		lf.lfWeight = FW_BOLD;
    lf.lfHeight = -MulDiv(nSize, GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY), 72);

	m_fontAlternate[nIndex].CreateFontIndirect( &lf );
	//m_fontAlternate[nIndex].CreatePointFont( nSize*10, lpszFontName );
}

void CABTPanelEx::SetCellFont(int nRow, int nCol, int nIndex, BOOL blnUpdate )
{
	// Font index 0 - default font.
	// Font index > 0 - Alternate font (use base 1)
	if ( ( nIndex < 0 ) || ( nIndex > 2 ) )
		return;

	CCellFormat* pCellFormat = GetCell( nRow, nCol, TRUE );
	pCellFormat->m_nAlternateFontIndex = nIndex;

	if ( blnUpdate )
	{
		CString strItem;
		strItem = GetItemTextEx( nRow, nCol );
		SetItemTextEx( nRow, nCol, strItem );
	}
}

int CABTPanelEx::GetCellFont(int nRow, int nCol, int nIndex)
{
	CCellFormat* pCellFormat = GetCell( nRow, nCol );
	if ( pCellFormat )
	{
		return pCellFormat->m_nAlternateFontIndex;
	}
	return FALSE;
}

void CABTPanelEx::SetFont(CFont *pFont, BOOL bRedraw)
{
	CListCtrl::SetFont( pFont, bRedraw );

	// Create default alternate font (BOLD).
	LOGFONT logFont;
	pFont->GetLogFont( &logFont );
	logFont.lfWeight = FW_BOLD;

	if ( m_fontAlternate[1].GetSafeHandle() )
		m_fontAlternate[1].DeleteObject();

	m_fontAlternate[1].CreateFontIndirect( &logFont );
}

void CABTPanelEx::SetUniformColWidthEx(int nCols, int narExcludeCols[], int nArrayElements, int nOffset )
{
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	pHeaderCtrl->SetRedraw( FALSE );
	SetRedraw( FALSE );

	// Do not show horizontal scroll for offset value < 0.
	if ( nOffset < 0 )
	{
		nOffset = GetSystemMetrics( SM_CXVSCROLL );
	}

	if ( nCols > 0 )
	{
		// Get size of control.
		CRect rect;
		GetClientRect( &rect );

		// Calculate size of column.
		int nSize = (rect.right-nOffset) / nCols;

		// Set all columns to the new size excluding the ones in the exclude list.
		int nRemainCols = nCols;
		int i = 0;
		BOOL blnSkip;
		int nTotalCols = GetCols();
		while ( nRemainCols && ( i < nTotalCols ) )
		{
			blnSkip = FALSE;
			for ( int j=0; j<nArrayElements; j++ )
			{
				if ( narExcludeCols[j] == i )
				{
					blnSkip = TRUE;
					break;
				}
			}
			if ( !blnSkip )
			{
				SetColWidth( i, nSize );
				nRemainCols--;
			}
			i++;
		}
		/*
		for ( int i=0; i<nCols; i ++ )
		{
			SetColWidth( i, nSize );
		}
		*/
	}
	SetRedraw( TRUE );
	pHeaderCtrl->SetRedraw( TRUE );
}

void CABTPanelEx::Clear( BOOL blnClearStyle )
{
	int nRows = GetRows();
	int nCols = GetCols();

	//SetRedraw( FALSE );
	for ( int nRow=0; nRow<nRows; nRow++ )
	{
		for ( int nCol=0; nCol<nCols; nCol++ )
		{
			SetItemTextEx( nRow, nCol, _T("") );
		}
	}

	if ( blnClearStyle )
		ClearAllStyle( FALSE );
	//SetRedraw( TRUE );
}

void CABTPanelEx::ClearAllStyle( BOOL blnRemovePtrArray, int nFromRow, int nToRow, int nFromCol, int nToCol )
{
	CPtrArray* pPtrArray;
	CCellFormat* pCellFormat;
	int nTotalCols;

	if ( nToRow == -1 )
		nToRow = GetItemCount();

	// Removes all cell items and col arrays.
	for ( int i=nFromRow; i<nToRow; i++ )
	{
		pPtrArray = (CPtrArray*)GetItemData( i );
		if ( pPtrArray )
		{
			if ( nToCol == -1 )
				nTotalCols = pPtrArray->GetSize();
			for ( int j=nFromCol; j<nTotalCols; j++ )
			{
				// Remove cell format.
				pCellFormat = (CCellFormat*)(*pPtrArray)[j];
				if ( pCellFormat )
				{
					delete pCellFormat;
					pCellFormat = NULL;
					(*pPtrArray)[j] = NULL;
				}
			}
			// Remove ptr array.
			if ( blnRemovePtrArray )
			{
				delete pPtrArray;
				pPtrArray = NULL;
				SetItemData( i, 0 );
			}
		}
	}
}

BOOL CABTPanelEx::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	//NMHDR* nmhdr = (NMHDR*)lParam;

	// For columns that will jump to a specify width without sliding, set
	// m_blnSlideMode to FALSE. But when user drag the header to resize
	// the column, it should not set it to FALSE because it will have a
	// wrong column size when updating.
	if ( !m_blnSlideMode )
	{
		HD_NOTIFY* pHDN = (HD_NOTIFY*)lParam;
		switch ( pHDN->hdr.code )
		{
			case HDN_ITEMCHANGINGW:
			case HDN_ITEMCHANGINGA:
				*pResult = FALSE;
				return TRUE;
		}
	}
	
	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

BOOL CABTPanelEx::DeleteItemWithStyle( int nItem )
{
	CPtrArray* pPtrArray = (CPtrArray*)GetItemData( nItem );
	if ( pPtrArray )
	{
		for ( int i=0; i<pPtrArray->GetSize(); i++ )
		{
			CCellFormat* pCellFormat = (CCellFormat*)(*pPtrArray)[i];
            if ( pCellFormat ) {
				delete pCellFormat;
                pCellFormat = 0;
            }
		}
		delete pPtrArray;
        pPtrArray = 0;
	}

	return DeleteItem( nItem );
}

void CABTPanelEx::SetItemTextEx(int nRow, int nCol, LPCTSTR lpszText, BOOL blnSkipConversion /*FALSE*/)
{
	WCHAR wszItem[256];
    int nCodePage = (blnSkipConversion) ? 1252 : m_nCodePage;
	if ( MultiByteToWideChar( nCodePage, 0, lpszText, -1, wszItem, 256 ) == 0 )
	{
		TRACE( _T("Error converting to Unicode(CABTPanel::SetItemTextEx).\n") );
		return;
	}
	/*
	LVITEMW lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nRow;
	lvItem.iSubItem = nCol;
	lvItem.stateMask = 0;
	lvItem.pszText = wszItem;
	lvItem.cchTextMax = wcslen( wszItem );
	lvItem.iImage = 0;
	lvItem.iIndent = 0;
	lvItem.lParam = 0;
	*/
	LVITEMW lvItem;
	lvItem.iSubItem = nCol;
	lvItem.pszText = wszItem;
	::SendMessage( GetSafeHwnd(), LVM_SETITEMTEXTW, (WPARAM)nRow, (LPARAM)(const LV_ITEMW FAR*)(&lvItem) );
}

CString CABTPanelEx::GetItemTextEx(int nRow, int nCol)
{
	CString strItem;
	WCHAR wszItem[256];
	LVITEMW lvItem;
	lvItem.iSubItem = nCol;
	lvItem.pszText = wszItem;
	lvItem.cchTextMax = 256;
	::SendMessage( GetSafeHwnd(), LVM_GETITEMTEXTW, (WPARAM)nRow, (LPARAM)(LV_ITEM FAR *)&lvItem );

	if ( WideCharToMultiByte( m_nCodePage, 0, wszItem, -1, strItem.GetBuffer(256), 256, NULL, NULL ) == 0 )
	{
		strItem.ReleaseBuffer();
		TRACE( _T("Error converting to multibyte(CABTPanel::GetItemTextEx).\n") );
		return _T("");
	}
	strItem.ReleaseBuffer();
	return strItem;
}

void CABTPanelEx::CreateChineseAlternateFont(LPCTSTR lpszFontName, int nSize)
{
	if ( m_fontAlternate[1].GetSafeHandle() )
	{
		m_fontAlternate[1].DeleteObject();
	}

	LOGFONT logFont;
    {
        CDC* pDC = GetDC();
	    logFont.lfHeight = -MulDiv(nSize, GetDeviceCaps(pDC->GetSafeHdc(),LOGPIXELSY), 72 );
        ReleaseDC( pDC );
    }
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = FW_BOLD;
	logFont.lfItalic = FALSE;
	logFont.lfUnderline = FALSE;
	logFont.lfStrikeOut = FALSE;
	logFont.lfCharSet = CHINESEBIG5_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = DEFAULT_QUALITY;
	logFont.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy( logFont.lfFaceName, lpszFontName );

	m_fontAlternate[1].CreateFontIndirect( &logFont );
}

void CABTPanelEx::SetCellAlign(int nRow, int nCol, int nAlign)
{
	CCellFormat* pCellFormat = GetCell( nRow, nCol, TRUE );
	pCellFormat->m_nAlign = nAlign;

	/*
	if ( blnUpdate )
	{
		CString strItem;
		strItem = GetItemTextEx( nRow, nCol );
		SetItemTextEx( nRow, nCol, strItem );
	}
	*/
}

void CABTPanelEx::SetColAlign(int nCol, int nAlign)
{
	for ( int i = 0; i < GetRows(); i++ ) {
		CCellFormat* pCellFormat = GetCell( i, nCol, TRUE );
		pCellFormat->m_nAlign = nAlign;
	}
}


UINT CABTPanelEx::GetCellAlign(int nRow, int nCol)
{
	CCellFormat* pCellFormat = GetCell( nRow, nCol );
	if ( pCellFormat )
	{
		return pCellFormat->m_nAlign;
	}

	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_FMT;
	GetColumn( nCol, &lvColumn );
	return lvColumn.fmt;
}

CPoint CABTPanelEx::HitTestEx(const CPoint &pt)
{
	CPoint ptItem(-1,-1);
	if ( GetRows() <= 0 )
		return ptItem;

	CRect rect;
	int nHitRow = -1;
	int nHitCol = -1;
	int nCols = GetCols();
	int nRows = GetRows();
	int nScrollX = GetScrollPos(SB_HORZ);
	int nScrollY = GetScrollPos(SB_VERT);
	GetItemRect( nScrollY, &rect, LVIR_BOUNDS );
	int nX = pt.x + nScrollX;
	int nY = pt.y + (nScrollY*rect.bottom);
	int* pnOrder = new int[nCols];
	GetColumnOrderArray( pnOrder, nCols );
	for ( int i=0; i<nCols; i++ )
	{
		nX -= GetColWidth( pnOrder[i] );
		if ( nX < 0 )
		{
			nHitCol = i;
			break;
		}
	}

	for ( int i=nScrollY; i<nRows; i++ )
	{
		GetItemRect( i, rect, LVIR_BOUNDS );
		if ( pt.y < rect.bottom )
		{
			nHitRow = i;
			break;
		}
	}

	// Map to corresponding column order array.
	ptItem.x = pnOrder[nHitCol];
	ptItem.y = nHitRow;

    delete[] pnOrder;
    pnOrder = 0;

	return ptItem;
}

void CABTPanelEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if ( m_fnLButtonDown )
	{
		SetCapture();
		m_bRestoreCapture = TRUE;
		m_fnLButtonDown( nFlags, point, (void*)GetParent() );
	}
	else
	{
		CListCtrl::OnLButtonDown(nFlags, point);
	}
}

void CABTPanelEx::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if ( m_bRestoreCapture )
	{
		::ReleaseCapture();
		m_bRestoreCapture = FALSE;
	}
	if ( m_fnLButtonUp )
	{
		m_fnLButtonUp( nFlags, point, (void*)GetParent() );
	}
	else
	{
		CListCtrl::OnLButtonUp(nFlags, point);
	}
}

void CABTPanelEx::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if ( m_fnRButtonDown )
	{
		SetCapture();
		m_bRestoreCapture = TRUE;
		m_fnRButtonDown( nFlags, point, (void*)GetParent() );
	}
	else
	{
		CListCtrl::OnRButtonDown(nFlags, point);
	}
}

void CABTPanelEx::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if ( m_bRestoreCapture )
	{
		::ReleaseCapture();
		m_bRestoreCapture = FALSE;
	}
	if ( m_fnRButtonUp )
	{
		m_fnRButtonUp( nFlags, point, (void*)GetParent() );
	}
	else
	{
		CListCtrl::OnRButtonUp(nFlags, point);
	}
}

void CABTPanelEx::SetLButtonDownFn(void (__cdecl *fnLButtonDown)(UINT,CPoint,void *))
{
	m_fnLButtonDown = fnLButtonDown;
}

void CABTPanelEx::SetLButtonUpFn(void (__cdecl *fnLButtonUp)(UINT,CPoint,void *))
{
	m_fnLButtonUp = fnLButtonUp;
}

void CABTPanelEx::SetRButtonDownFn(void (__cdecl *fnRButtonDown)(UINT,CPoint,void *))
{
	m_fnRButtonDown = fnRButtonDown;
}

void CABTPanelEx::SetRButtonUpFn(void (__cdecl *fnRButtonUp)(UINT,CPoint,void *))
{
	m_fnRButtonUp = fnRButtonUp;
}

BOOL CABTPanelEx::SetColOrder(LPCTSTR lpszOrder)
{
	int nToken;
	int nNoOfTokens;
	char* szOrderBuffer;
	int nBufferSize;
	char* szToken;
	int nCount;
	int* narOrder = NULL;
	char szDelimit[] = " ";

	nCount = GetCols();

	if ( nCount == 0 )
		return FALSE;

	nNoOfTokens = 0;
	narOrder = new int[nCount];
	nBufferSize = strlen(lpszOrder) + 1;
	szOrderBuffer = new char[nBufferSize];
	strcpy( szOrderBuffer, lpszOrder );

	szToken = strtok( szOrderBuffer, szDelimit );
	while ( szToken != NULL )
	{
		nToken = atoi( szToken );
		narOrder[nNoOfTokens] = nToken;
		nNoOfTokens++;
		if ( nNoOfTokens >= nCount )
			break;
		szToken = strtok( NULL, szDelimit );
	}

	BOOL bResult = SetColumnOrderArray( nNoOfTokens, narOrder );

	delete[] szOrderBuffer;
    szOrderBuffer = 0;

	delete[] narOrder;
    narOrder = 0;

	return bResult;
}


BOOL CABTPanelEx::GetColOrder(CString &strOrder)
{
	int nCount = GetCols();
	int* narOrder;
	char szPos[30];

	if ( nCount == 0 )
		return FALSE;

	narOrder = new int[nCount];
	if ( GetColumnOrderArray( narOrder, nCount ) == FALSE )
	{
		delete[] narOrder;
        narOrder = 0;
		return FALSE;
	}

	strOrder = _T("");

	for ( int i=0; i<nCount; i++ )
	{
		_itoa( narOrder[i], szPos, 10 );
		strcat( szPos, _T(" ") );
		strOrder += szPos;
	}

	delete[] narOrder;
    narOrder = 0;

	return TRUE;
}

BOOL CABTPanelEx::SetColWidth(LPCTSTR lpszColWidth)
{
	int nToken;
	int nNoOfTokens;
	char* szColWidthBuffer;
	int nBufferSize;
	char* szToken;
	int nCount;
	char szDelimit[] = " ";

	nCount = GetCols();

	if ( nCount == 0 )
		return FALSE;

	nNoOfTokens = 0;
	nBufferSize = strlen(lpszColWidth) + 1;
	szColWidthBuffer = new char[nBufferSize];
	strcpy( szColWidthBuffer, lpszColWidth );

	szToken = strtok( szColWidthBuffer, szDelimit );
	while ( szToken != NULL )
	{
		nToken = atoi( szToken );
		SetColWidth( nNoOfTokens, nToken );
		nNoOfTokens++;
		if ( nNoOfTokens >= nCount )
			break;
		szToken = strtok( NULL, szDelimit );
	}

	delete[] szColWidthBuffer;
    szColWidthBuffer = 0;

	return TRUE;
}

BOOL CABTPanelEx::GetColWidth(CString &strColWidth)
{
	int nWidth;
	char szWidth[30];
	int nCount;
	
	nCount = GetCols();

	if ( nCount == 0 )
		return FALSE;

	strColWidth = _T("");

	for ( int i=0; i<nCount; i++ )
	{
		nWidth = GetColWidth( i );
		_itoa( nWidth, szWidth, 10 );
		strcat( szWidth, _T(" ") );
		strColWidth += szWidth;
	}

	return TRUE;
}

BOOL CABTPanelEx::SaveColSettings(LPCTSTR lpszFilename, LPCTSTR lpszSectionName, int nMode)
{
	CString strOrder;
	CString strWidth;
	CString strWidthKey;
	BOOL blnResult1;
	BOOL blnResult2;

	if ( GetColOrder( strOrder ) == FALSE )
		return FALSE;
	if ( GetColWidth( strWidth ) == FALSE )
		return FALSE;

	strWidthKey.Format( _T("ColWidth(%d)"), nMode );
	blnResult1 = ArchiveSetting( lpszFilename, lpszSectionName, _T("ColOrder"), strOrder, TRUE );
	blnResult2 = ArchiveSetting( lpszFilename, lpszSectionName, strWidthKey, strWidth, TRUE );

	if ( !(blnResult1|blnResult2) )
		return FALSE;

	return TRUE;
}

BOOL CABTPanelEx::LoadColSettings(LPCTSTR lpszFilename, LPCTSTR lpszSectionName, int nMode)
{
	CString strOrder;
	CString strWidth;
	CString strWidthKey;

	strWidthKey.Format( _T("ColWidth(%d)"), nMode );
	ArchiveSetting( lpszFilename, lpszSectionName, _T("ColOrder"), strOrder, FALSE );
	ArchiveSetting( lpszFilename, lpszSectionName, strWidthKey, strWidth, FALSE );

	if ( SetColOrder( strOrder ) == FALSE )
		return FALSE;
	if ( SetColWidth( strWidth ) == FALSE )
		return FALSE;

	// Force update screen.
	Invalidate();

	return TRUE;
}

BOOL CABTPanelEx::ArchiveSetting(LPCTSTR lpszFilename, 
								 LPCTSTR lpszSectionName, 
								 LPCTSTR lpszKeyName, 
								 CString& strText, 
								 BOOL bSave)
{
	CString strDirName;
	CString strFullFilename;
	BOOL blnResult;

	// Get current directory.
	DWORD nNumChar = GetCurrentDirectory( 255, strDirName.GetBuffer(255) );
	strDirName.ReleaseBuffer();
	if ( nNumChar > 0 )
	{
		// Create full path name.
		strFullFilename.Format( _T("%s\\Layout\\AutoSave\\%s"), strDirName, lpszFilename );
	}
	else
	{
		TRACE( _T("Unable to get current directory") );
		return FALSE;
	}


	if ( bSave )	// Save to ini
	{
		blnResult = WritePrivateProfileString( lpszSectionName, lpszKeyName, strText, strFullFilename );
	}
	else			// Load from ini
	{
		const int nSize = 500;
		blnResult = GetPrivateProfileString( lpszSectionName, lpszKeyName, _T(""), strText.GetBuffer(nSize), nSize, strFullFilename );
		strText.ReleaseBuffer();
	}

	return blnResult;
}

void CABTPanelEx::ResetColOrder( BOOLEAN blnRefresh )
{
	int* narOrder = NULL;
	int nCount    = 0;

	// Get total number of columns.
	nCount = GetCols();

	if ( nCount == 0 )
		return;

	// Reset order to original.
	narOrder = new int[nCount];
	for ( int i=0; i<nCount; i++ )
	{
		narOrder[i] = i;
	}

	SetColumnOrderArray( nCount, narOrder );

	if ( blnRefresh )
		Invalidate();

	// Clear up.
	delete[] narOrder;
    narOrder = 0;
}

void CABTPanelEx::SetCellDrawArrow( int nRow, int nCol, BOOL blnEnabled, COLORREF clrUpArrow, COLORREF clrDnArrow )
{
	CCellFormat* pCellFormat = GetCell( nRow, nCol, TRUE );
	pCellFormat->m_blnDrawArrow = blnEnabled;
	pCellFormat->m_clrUpArrow = clrUpArrow;
	pCellFormat->m_clrDnArrow = clrDnArrow;
}

BOOL CABTPanelEx::GetCellDrawArrow(int nRow, int nCol)
{
	CCellFormat* pCellFormat = GetCell( nRow, nCol );
	if ( pCellFormat )
	{
		return pCellFormat->m_blnDrawArrow;
	}
	return FALSE;
}

void CABTPanelEx::SetCellDrawBorder(int nRow, int nCol, BOOL blnEnabled, COLORREF clrBorder)
{
	if (blnEnabled == TRUE) {
		CCellFormat* pCellFormat = GetCell(nRow, nCol, TRUE);
		pCellFormat->m_blnDrawBorder = TRUE;
		pCellFormat->m_clrBorder = clrBorder;
	}
	else {
		CCellFormat* pCellFormat = GetCell(nRow, nCol);
		if (pCellFormat) {
			pCellFormat->m_blnDrawBorder = FALSE;
		}
	}
}

BOOL CABTPanelEx::GetCellDrawBorder(int nRow, int nCol, COLORREF& clrBorder)
{
	CCellFormat* pCellFormat = GetCell( nRow, nCol );
	if ( pCellFormat )
	{
		clrBorder = pCellFormat->m_clrBorder;
		return pCellFormat->m_blnDrawBorder;
	}
	return FALSE;
}

int CABTPanelEx::GetCodePage()
{
	return m_nCodePage;
}

void CABTPanelEx::SetCodePage(int nCodePage)
{
	m_nCodePage = nCodePage;
}

int CABTPanelEx::GetCellDrawEdge(int nRow, int nCol, COLORREF& clrEdge)
{
	CCellFormat* pCellFormat = GetCell(nRow, nCol);
	if (pCellFormat) {
		clrEdge = pCellFormat->m_clrEdge;
		return pCellFormat->m_nDrawEdge;
	}

	return 0;
}

void CABTPanelEx::SetCellDrawEdge(int nRow, int nCol, int nEdge, COLORREF clrEdge)
{
	CCellFormat *pCellFormat = GetCell(nRow, nCol, TRUE);
	if (pCellFormat) {
		pCellFormat->m_nDrawEdge = nEdge;
		pCellFormat->m_clrEdge = clrEdge;
	}
}

void CABTPanelEx::SetUseOwnerDraw( bool value )
{
	m_use_owner_draw = value;
}