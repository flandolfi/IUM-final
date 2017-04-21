//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    TabsPanel.cpp										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WiiMIDI_GUI.h"

TabsPanel* TabsPanel::Current = NULL;

TabsPanel::TabsPanel(const wxRect2DDouble& rect, int ntabs, const char *labels[])
{
	area = rect;
	tabs_area = wxRect2DDouble(area.m_x, area.m_y, _PANEL_W_ + _TAB_BORDER_, area.m_y);
	free_area = wxRect2DDouble(area.m_x + _PANEL_W_ + _CONT_DIST_, area.m_y + _PANEL_W_*0.5 + _CONT_DIST_,
		area.m_width - (_CONT_DIST_*2), area.m_height - (_PANEL_W_ + _CONT_DIST_*2));
	current = 0;
	focus = -1;
	n_tabs = ntabs;
	for(int i = 0; i < n_tabs; i++)
		tabs.push_back(wxString(labels[i]));
}

wxRect2DDouble TabsPanel::GetFreeArea()
{
	return free_area;
}

int TabsPanel::CurrentTab()
{
	return current;
}

bool TabsPanel::Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p)
{
	wxGraphicsPath path;
	int tab;
	focus = -1;
	Current = NULL;

	for(tab = 0; tab < n_tabs; tab++)
	{
		path = gc->CreatePath();
		path.MoveToPoint(area.m_x + _PANEL_W_, area.m_y + _TAB_DIM_*tab);
		path.AddCurveToPoint(area.m_x + _PANEL_W_, area.m_y + _TAB_DIM_*tab + _TAB_GAP_*_TAB_S_,
			area.m_x, area.m_y + _TAB_DIM_*tab + _TAB_GAP_*(1 - _TAB_S_), area.m_x, area.m_y + _TAB_DIM_*tab + _TAB_GAP_);
		path.AddLineToPoint(area.m_x, area.m_y + _TAB_DIM_*(tab + 1));
		path.AddCurveToPoint(area.m_x, area.m_y + _TAB_DIM_*(tab + 1) + _TAB_GAP_ *_TAB_S_,
			area.m_x + _PANEL_W_, area.m_y + _TAB_DIM_*(tab + 1) + _TAB_GAP_*(1 - _TAB_S_), area.m_x + _PANEL_W_,
			area.m_y + _TAB_DIM_*(tab + 1) + _TAB_GAP_);
		path.CloseSubpath();
		if (path.Contains(p))
		{
			Current = this;
			focus = tab;
			break;
		}
	}

	return focus != -1;
}

void TabsPanel::Click()
{
	if(focus >= 0)
		current = focus;
}

void TabsPanel::DrawTabs(wxGraphicsContext *gc, int tab)
{
	if((tab >= 0) && (tab < n_tabs))
	{
		if(tab <= current)
			DrawTabs(gc, int(tab - 1));
		if(tab >= current)
			DrawTabs(gc, int(tab + 1));
		wxColour bgC = (tab == current)? D1 : ((tab == focus)? L2 : L1);
		wxColour bdC((tab == current)? L3 : ((tab == focus)? L3 : L2));
		wxColour txtC((tab == current)? L3 : ((tab == focus)? D3 : D1));
		gc->SetBrush(wxBrush(bgC));
		gc->SetPen(wxPen(bdC));
		gc->SetFont(wxFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_)), txtC);
		wxDouble w, h, i1, i2;
		gc->GetTextExtent(tabs[tab], &w, &h, &i1, &i2);
		wxGraphicsPath path(gc->CreatePath());
		path.MoveToPoint(area.m_x + _PANEL_W_, area.m_y + _TAB_DIM_*tab);
		path.AddCurveToPoint(area.m_x + _PANEL_W_, area.m_y + _TAB_DIM_*tab + _TAB_GAP_*_TAB_S_,
			area.m_x, area.m_y + _TAB_DIM_*tab + _TAB_GAP_*(1 - _TAB_S_), area.m_x, area.m_y + _TAB_DIM_*tab + _TAB_GAP_);
		path.AddLineToPoint(area.m_x, area.m_y + _TAB_DIM_*(tab + 1));
		path.AddCurveToPoint(area.m_x, area.m_y + _TAB_DIM_*(tab + 1) + _TAB_GAP_ *_TAB_S_,
			area.m_x + _PANEL_W_, area.m_y + _TAB_DIM_*(tab + 1) + _TAB_GAP_*(1 - _TAB_S_), area.m_x + _PANEL_W_,
			area.m_y + _TAB_DIM_*(tab + 1) + _TAB_GAP_);
		if (tab == current)
		{
			// path.AddLineToPoint(area.m_x + _PANEL_W_, area.m_y + area.m_height);
			gc->SetPen(wxPen(wxColour(L3)));
			gc->StrokePath(path);
			path.AddLineToPoint(area.m_x + _PANEL_W_ + 2, area.m_y + _TAB_DIM_*(tab + 1) + _TAB_GAP_);
			path.AddLineToPoint(area.m_x + _PANEL_W_ + 2, area.m_y + _TAB_DIM_*tab + _TAB_GAP_);
			gc->DrawRoundedRectangle(area.m_x + _PANEL_W_, area.m_y - _DEFAULT_R_, area.m_width - _TAB_BORDER_*2,
				area.m_height - _TAB_BORDER_*2, _DEFAULT_R_);
			gc->FillPath(path);
		} else
			gc->DrawPath(path);
		gc->DrawText(tabs[tab], area.m_x + (_PANEL_W_ - h)*0.5,
			area.m_y + _TAB_DIM_*(tab + 1) + (_TAB_GAP_ - _TAB_DIM_ + w)*0.5, wxDegToRad(90)); 
	} 
}

void TabsPanel::Paint(wxGraphicsContext *gc)
{
	gc->SetBrush(wxBrush(wxColour(D3)));
	gc->SetPen(wxPen(wxColour(D3)));
	DrawTabs(gc, current);
}