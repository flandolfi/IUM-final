//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    ErrorMessage.cpp									//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WindowTypes.h"

ErrorMessage::ErrorMessage(wxFrame* owner)
{
	AlphaBackground = 0;
	Parent = owner;
	err_btn = new Button(wxString(wxT("Ok")), 0, 0, NO_FILL, BTN_ERR);
}

ErrorMessage::~ErrorMessage()
{
	delete err_btn;
}

void ErrorMessage::Paint(wxGraphicsContext *gc)
{
	wxDouble w, h, i1, i2, offset = 0;
	wxString title("Error");

	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), L3);
	gc->GetTextExtent(err_str, &w, &h, &i1, &i2);
	wxRect2DDouble rect(0, 0, fmax(100, fmax(w, _DEFAULT_W_ + _BLUR_DIM_*2)), h + _DEFAULT_H_ + _BLUR_DIM_*2 + _TAB_BORDER_*2 + offset);
	rect.SetCentre(wxPoint2DDouble(Parent->GetClientSize().GetWidth()*0.5, Parent->GetClientSize().GetHeight()*0.5));
	DrawBackground(gc);
	DrawContainer(gc, title, rect);
	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), L1);
	gc->DrawText(err_str, rect.m_x, rect.m_y);

	err_btn->SetCentre(rect.GetCentre().m_x, rect.m_y + rect.m_height - _BLUR_DIM_ - _DEFAULT_H_*0.5);
	err_btn->Paint(gc);
}

bool ErrorMessage::Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p)
{
	return err_btn->Contains(p, gc);
}

void ErrorMessage::SetError(const int& e, const wxString& str)
{
	err = e;
	err_str = str;
	Show();
}

int ErrorMessage::Click()
{
	Hide();

	return 0;
}
