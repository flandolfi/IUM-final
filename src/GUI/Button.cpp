//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    Button.cpp											//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WiiMIDI_GUI.h"

unsigned int Button::CurrentID = 0;
Button* Button::CurrentButton = NULL;
Button* Button::LastClicked = NULL;

Button::Button(const wxString& str, const wxDouble& x,
	const wxDouble& y, unsigned int st, unsigned int bt_id)
{
    id = bt_id;
	IsOn = false;
	clicked = false;
	enabled = true;
    DependingContextMenu = NULL;
	label = str;
	style = st;
    area = wxRect2DDouble(x, y, _DEFAULT_W_, _DEFAULT_H_);
    radius = area.m_height*0.5;
}

void Button::SetLabel(const wxString& str)
{
	label = str;
}

void Button::SetLocation(const wxDouble& x, const wxDouble& y)
{
	area.m_x = x;
	area.m_y = y;
}

wxPoint2DDouble Button::GetLocation()
{
	return area.GetLeftTop();
}

void Button::SetSize(const wxDouble& w, const wxDouble& h)
{
	area.m_width = w;
	area.m_height = h;
	radius = h*0.5;
}

wxSize Button::GetSize()
{
	return area.GetSize();
}

void Button::SetCentre(const wxDouble& x, const wxDouble& y)
{
	area.SetCentre(wxPoint2DDouble(x, y));
}

wxPoint2DDouble Button::GetCentre()
{
	return area.GetCentre();
}

void Button::Paint(wxGraphicsContext *gc)
{
	wxColour bgC, txtC, baseC;
	if (enabled)
	{
        if (DependingContextMenu != NULL) {
			if(ContextMenu::IsActive && ContextMenu::CurrentMenu == DependingContextMenu)
				IsOn = true;
			DependingContextMenu->SetTransform(gc->GetTransform());
		}

		if(style & LIGHT_THEME)
		{
			bgC = (style & TRANSPARENT)? wxTransparentColour : (style & NO_FILL)? L1 : clicked? L3 : IsOn? L2 : L1;
			txtC = clicked ? D3 : IsOn ? D2 : D1;
		} else
		{
			txtC = clicked? L3 : IsOn? L2 : L1;
			bgC = (style & TRANSPARENT)? wxTransparentColour : (style & NO_FILL)? D1 : clicked? D3 : IsOn? D2 : D1;
		}
		baseC = txtC;
		baseC.Set(txtC.Red(), txtC.Green(), txtC.Blue(), 0);
        IsOn = false;

		if(!(style & NO_BLUR) && !(style & RECT_SHAPED))
		{
			gc->SetPen(wxPen(baseC, 1, wxPENSTYLE_SOLID));
			wxRect2DDouble rBrush(area.m_x -_BLUR_DIM_, area.m_y - _BLUR_DIM_,
				(radius + _BLUR_DIM_)*2, (radius + _BLUR_DIM_)*2);
			gc->SetBrush(gc->CreateRadialGradientBrush(rBrush.m_x + rBrush.m_width*0.5,
				rBrush.m_y + rBrush.m_height*0.5, rBrush.m_x + rBrush.m_width*0.5,
				rBrush.m_y + rBrush.m_height*0.5, radius + _BLUR_DIM_, txtC, baseC));
			gc->DrawRectangle(rBrush.m_x, rBrush.m_y, rBrush.m_width*0.5, rBrush.m_height);
			rBrush = wxRect2DDouble(area.m_x + area.m_width - radius*2 - _BLUR_DIM_,
				area.m_y - _BLUR_DIM_, (radius + _BLUR_DIM_)*2, (radius + _BLUR_DIM_)*2);
			gc->SetBrush(gc->CreateRadialGradientBrush(rBrush.m_x + rBrush.m_width*0.5,
				rBrush.m_y + rBrush.m_height*0.5, rBrush.m_x + rBrush.m_width*0.5,
				rBrush.m_y + rBrush.m_height*0.5, radius + _BLUR_DIM_, txtC, baseC));
			gc->DrawRectangle(rBrush.m_x + rBrush.m_width*0.5, rBrush.m_y, rBrush.m_width*0.5, rBrush.m_height);
            rBrush = wxRect2DDouble(area.m_x + radius, area.m_y - _BLUR_DIM_,
                area.m_width - 2*radius, area.m_height*0.5 + _BLUR_DIM_);
            gc->SetBrush(gc->CreateLinearGradientBrush(rBrush.m_x, rBrush.m_y,
                rBrush.m_x, rBrush.m_y + rBrush.m_height, baseC, txtC));
            gc->DrawRectangle(rBrush.m_x, rBrush.m_y, rBrush.m_width, rBrush.m_height);
            rBrush = wxRect2DDouble(area.m_x + radius, area.m_y + area.m_height*0.5,
                area.m_width - 2*radius, area.m_height*0.5 + _BLUR_DIM_);
            gc->SetBrush(gc->CreateLinearGradientBrush(rBrush.m_x, rBrush.m_y,
                rBrush.m_x, rBrush.m_y + rBrush.m_height, txtC, baseC));
            gc->DrawRectangle(rBrush.m_x, rBrush.m_y, rBrush.m_width, rBrush.m_height);
		}
	} else
	{
		bgC = (style & LIGHT_THEME)? L1 : D1;
		txtC = D3;
	}
	gc->SetBrush(wxBrush(bgC, wxBRUSHSTYLE_SOLID));
	if(style & NO_BORDER)
		gc->SetPen(wxPen(bgC, 1, wxPENSTYLE_SOLID));
	else
		gc->SetPen(wxPen(txtC, 1, wxPENSTYLE_SOLID));
	if(style & RECT_SHAPED)
		gc->DrawRectangle(area.m_x, area.m_y, area.m_width, area.m_height);
	else
		gc->DrawRoundedRectangle(area.m_x, area.m_y, area.m_width, area.m_height, radius);
	gc->SetPen(wxPen(txtC, 1, wxPENSTYLE_SOLID));
	if(style & BOLD_FONT)
		font = wxFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold());
	else
		font = wxFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_));
	gc->SetFont(font, txtC);
	wxDouble w, h, i1, i2;
	gc->GetTextExtent(label, &w, &h, &i1, &i2);
	gc->DrawText(label,
		area.m_x + (area.m_width - w)*0.5,
		area.m_y + (area.m_height - h - i1*0.5)*0.5);
}

bool Button::Contains(const wxPoint2DDouble& p, wxGraphicsContext *gc)
{
	if (style & RECT_SHAPED)
		IsOn = area.Contains(p);
	else
	{
		wxGraphicsPath path(gc->CreatePath());
		path.AddRoundedRectangle(area.m_x, area.m_y, area.m_width, area.m_height, radius);
		IsOn = path.Contains(p);
	}

    if(IsOn) {
        CurrentID = id;
        CurrentButton = this;
        if (DependingContextMenu != NULL && ContextMenu::IsActive && DependingContextMenu->MenuType == CTX_MENU_PANEL &&
				ContextMenu::CurrentMenu->MenuType == DependingContextMenu->MenuType)
            DependingContextMenu->SetCurrent();
    } else {
        CurrentID = 0;
        CurrentButton = NULL;
    }

	return IsOn;
}

void Button::Click()
{
    LastClicked = CurrentButton;
    if(LastClicked != NULL)
        LastClicked->clicked = true;
	if(CurrentButton != NULL && CurrentButton->DependingContextMenu != NULL)
		CurrentButton->DependingContextMenu->SetCurrent();
}

void Button::UnClick()
{
    if(LastClicked != NULL)
        LastClicked->clicked = false;
    LastClicked = NULL;
}

void Button::ResetCurrentButton()
{
    if(CurrentButton != NULL)
        CurrentButton->IsOn = false;
    CurrentButton = NULL;
}
