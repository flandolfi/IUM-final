//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    Window.cpp											//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WindowTypes.h"

Window* Window::CurrentWindow = NULL;
GhostWindow Window::Ghost = GhostWindow();

void Window::DrawBackground(wxGraphicsContext *gc)
{
	wxColour bgC;
	bgC.Set(0, 0, 0, AlphaBackground);
	gc->SetBrush(wxBrush(bgC));
	gc->SetPen(wxPen(wxColour(bgC)));
	gc->DrawRectangle(0, 0, Parent->GetClientSize().GetWidth(), Parent->GetClientSize().GetHeight());
}

void Window::DrawContainer(wxGraphicsContext *gc, const wxString& title, const wxRect2DDouble& area_cont)
{
	wxDouble w, h, i1, i2;
	wxRect2DDouble area(area_cont.m_x - _CONT_DIST_*1.5, area_cont.m_y - _CONT_DIST_ - _PANEL_W_*0.5,
		area_cont.m_width + _CONT_DIST_*3, area_cont.m_height + _CONT_DIST_*2 + _PANEL_W_*0.5);
	wxFont font(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_));
	gc->SetBrush(D1);
	gc->SetPen(L3);
	gc->SetFont(font, D1);
	gc->PushState();
	gc->Translate(area.m_x, area.m_y);
	gc->DrawRoundedRectangle(0, 0, area.m_width, area.m_height, _DEFAULT_R_);
	gc->GetTextExtent(title, &w, &h, &i1, &i2);
	wxGraphicsPath path(gc->CreatePath());
	path.MoveToPoint(_DEFAULT_R_, 0);
	path.AddCurveToPoint(_DEFAULT_R_ + _TAB_GAP_*_TAB_S_, 0, _DEFAULT_R_ + _TAB_GAP_*(1 - _TAB_S_), -_PANEL_W_*0.5,
		_DEFAULT_R_ + _TAB_GAP_, -_PANEL_W_*0.5);
	path.AddLineToPoint(_DEFAULT_R_ + _TAB_GAP_ + w, -_PANEL_W_*0.5);
	path.AddCurveToPoint(_DEFAULT_R_ + w + _TAB_GAP_*(_TAB_S_ + 1), -_PANEL_W_*0.5,
		_DEFAULT_R_ + w + _TAB_GAP_*(2 -_TAB_S_), 0, _DEFAULT_R_ + w + _TAB_GAP_*2, 0);
	path.AddCurveToPoint(_DEFAULT_R_ + w + _TAB_GAP_*(2 - _TAB_S_), 0,
		_DEFAULT_R_ + w + _TAB_GAP_*(_TAB_S_ + 1), _PANEL_W_*0.5, _DEFAULT_R_ + _TAB_GAP_ + w, _PANEL_W_*0.5);
	path.AddLineToPoint(_DEFAULT_R_ + _TAB_GAP_, _PANEL_W_*0.5);
	path.AddCurveToPoint(_DEFAULT_R_ + _TAB_GAP_*(1 - _TAB_S_), _PANEL_W_*0.5, _DEFAULT_R_ + _TAB_GAP_*_TAB_S_, 0, 
		_DEFAULT_R_, 0);
	gc->SetBrush(L1);
	gc->SetPen(L3);
	gc->DrawPath(path);
	gc->DrawText(title, _DEFAULT_R_ + _TAB_GAP_, -h*0.5);
	gc->PopState();
}

void Window::FitRectToContainer(wxRect2DDouble *rect)
{
	rect->m_x += _CONT_DIST_*1.5;
	rect->m_y += _CONT_DIST_ + _PANEL_W_*0.5;
	rect->m_width -= _CONT_DIST_*3;
	rect->m_height -= (_CONT_DIST_*2 + _PANEL_W_*0.5);
}

void Window::Show()
{
	AlphaBackground = 0;
	CurrentWindow = this;
	Animation::Add(&AlphaBackground, 0, 200);
}

void Window::Hide()
{
	Ghost.Hide(this);
}