//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    Scrollbar.cpp										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WiiMIDI_GUI.h"

Scrollbar* Scrollbar::Current = NULL;
Scrollbar* Scrollbar::Scrolling = NULL;

Scrollbar::Scrollbar(const wxRect2DDouble& a, bool ver)
{
	area = a;
	isVertical = ver;
	isVisible = true;
	perc = 0;
	delta = _DELTA_SCR_*9;
	if (isVertical)	{
		minus = Button(wxString(wxT("▵")), 0, 0, NO_FILL | TRANSPARENT | NO_BLUR, BTN_SCR_MINUS);
		plus = Button(wxString(wxT("▿")), 0, area.m_height - area.m_width, NO_FILL | TRANSPARENT |NO_BLUR, BTN_SCR_PLUS);
		scroller = Button(wxString(""), 0, area.m_width, NO_BLUR | TRANSPARENT | NO_FILL, BTN_SCROLL);
	} else {
		minus = Button(wxString(wxT("◃")), 0, 0, NO_FILL | TRANSPARENT | NO_BLUR, BTN_SCR_MINUS);
		plus = Button(wxString(wxT("▹")), area.m_width - area.m_height, 0, TRANSPARENT | NO_FILL | NO_BLUR, BTN_SCR_PLUS);
		scroller = Button(wxString(""), area.m_height, 0, NO_BLUR | TRANSPARENT | NO_FILL, BTN_SCROLL);
	}
	wxDouble diam(isVertical? area.m_width : area.m_height);
	minus.SetSize(diam, diam);
	plus.SetSize(diam, diam);
	scroller.SetSize(diam, diam);
}

void Scrollbar::SetArea(const wxRect2DDouble& a)
{
	area = a;
	if (isVertical) {
		plus.SetLocation(0, area.m_height - area.m_width);
		if(Scrolling == NULL)
			scroller.SetLocation(0, area.m_width + (area.m_height - 3*area.m_width)*perc);
	} else {
		plus.SetLocation(area.m_width - area.m_height, 0);
		if(Scrolling == NULL)
			scroller.SetLocation(area.m_height + (area.m_width - 3*area.m_height)*perc, 0);
	}
	wxDouble diam(isVertical? area.m_width : area.m_height);
	minus.SetSize(diam, diam);
	plus.SetSize(diam, diam);
	scroller.SetSize(diam, diam);
}

void Scrollbar::SetVertical()
{
	isVertical = true;
	minus.SetLocation(0, 0);
	plus.SetLocation(0, area.m_height - area.m_width);
	scroller.SetLocation(0, area.m_width + (area.m_height - 3*area.m_width)*perc);
}

void Scrollbar::SetHorizontal()
{
	isVertical = false;
	minus.SetLocation(0, 0);
	plus.SetLocation(area.m_width - area.m_height, 0);
	scroller.SetLocation(area.m_height + (area.m_width - 3*area.m_height)*perc, 0);
}

void Scrollbar::Paint(wxGraphicsContext *gc)
{
	if(isVisible) {
		gc->PushState();
		gc->Translate(area.m_x, area.m_y);
		gc->SetPen(wxColour(L1));
		if(isVertical) {
			gc->StrokeLine(area.m_width*0.5, area.m_width, area.m_width*0.5, scroller.GetLocation().m_y);
			gc->StrokeLine(area.m_width*0.5, area.m_width + scroller.GetLocation().m_y, area.m_width*0.5, area.m_height - area.m_width);
		} else {
			gc->StrokeLine(area.m_height, area.m_height*0.5, scroller.GetLocation().m_x, area.m_height*0.5);
			gc->StrokeLine(area.m_height + scroller.GetLocation().m_x, area.m_height*0.5, area.m_width - area.m_height, area.m_height*0.5);
		}
		minus.Paint(gc);
		plus.Paint(gc);
		scroller.Paint(gc);
		gc->PopState();
	}
}

bool Scrollbar::Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p_w)
{
	wxPoint2DDouble p(p_w.m_x - area.m_x, p_w.m_y - area.m_y);
	if(plus.Contains(p, gc) || minus.Contains(p, gc) || scroller.Contains(p, gc)) {
		Current = this;
		return true;
	}

	return false;
}

wxDouble Scrollbar::GetPercentage()
{
	return perc;
}

void Scrollbar::SetPercentage(const wxDouble& p)
{
	perc = fmax(0, fmin(1, p));
	if(isVertical)
		scroller.SetLocation(scroller.GetLocation().m_x, area.m_width + (area.m_height - 3*area.m_width)*perc);
	else
		scroller.SetLocation(area.m_height + (area.m_width - 3*area.m_height)*perc, scroller.GetLocation().m_y);
}

void Scrollbar::IncreasePosition()
{
	perc = fmin(1, perc + delta);
	if(isVertical) 
		scroller.SetLocation(scroller.GetLocation().m_x, area.m_width + (area.m_height - 3*area.m_width)*perc);
	else
		scroller.SetLocation(area.m_height + (area.m_width - 3*area.m_height)*perc, scroller.GetLocation().m_y);
}

void Scrollbar::DecreasePosition()
{
	perc = fmax(0, perc - delta);
	if(isVertical) 
		scroller.SetLocation(scroller.GetLocation().m_x, area.m_width + (area.m_height - 3*area.m_width)*perc);
	else
		scroller.SetLocation(area.m_height + (area.m_width - 3*area.m_height)*perc, scroller.GetLocation().m_y);
}

void Scrollbar::ResetPosition()
{
	perc = 0;
	if(isVertical)
		scroller.SetLocation(scroller.GetLocation().m_x, area.m_width);
	else
		scroller.SetLocation(area.m_height, scroller.GetLocation().m_y);
}

void Scrollbar::Drag(const wxPoint2DDouble& p_w)
{
	// wxPoint2DDouble offset(p_w.m_x - scroller.GetLocation().m_x, p_w.m_y - scroller.GetLocation().m_y);
	wxDouble x = isVertical? 0 : fmin(fmax(area.m_height - old_pos.m_x, p_w.m_x - scroll_from.m_x), area.m_width - 2*area.m_height - old_pos.m_x);
	wxDouble y = isVertical? fmin(fmax(area.m_width - old_pos.m_y, p_w.m_y - scroll_from.m_y), area.m_height - 2*area.m_width - old_pos.m_y) : 0;
	perc = isVertical? (scroller.GetLocation().m_y - area.m_width)/(area.m_height - 3*area.m_width) : (scroller.GetLocation().m_x - area.m_height)/(area.m_width - 3*area.m_height);
	scroller.SetLocation(old_pos.m_x + x, old_pos.m_y + y);
}