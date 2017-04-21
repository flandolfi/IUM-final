//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    DropdownMenu.cpp									//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WiiMIDI_GUI.h"

DropdownMenu::DropdownMenu(const wxDouble& x, const wxDouble& y,
		const int& num_opt, const char** options, const wxDouble& width, int def)
{
	X = x;
	Y = y;
	CTX_Width = width;
	menu = new ContextMenu(_DEFAULT_R_,_DEFAULT_H_, num_opt, options, true, CTX_DROPDOWN_MENU, 0, width, def);
	def_option = wxString("-- Select option --");
	btn = new Button(wxString(wxT("⌵")), width + _DEFAULT_H_*0.1, _DEFAULT_H_*0.1,
			NO_BLUR | NO_FILL | TRANSPARENT | BOLD_FONT, BTN_DROPDOWN);
	btn->SetSize(_DEFAULT_H_*0.8, _DEFAULT_H_*0.8);
	btn->SetDependingContextMenu(menu);
}

void DropdownMenu::Paint(wxGraphicsContext *gc)
{
	wxString str(menu->GetLastSelected() != -1? menu->GetButton(menu->GetLastSelected())->GetLabel() : def_option);
	wxDouble w, h;
	gc->PushState();
	gc->Translate(X, Y);
	gc->SetPen(wxPen(L1));
	gc->SetBrush(*wxTRANSPARENT_BRUSH);
	gc->DrawRoundedRectangle(0, 0, CTX_Width + _DEFAULT_R_*2, _DEFAULT_H_, _DEFAULT_R_);
	btn->Paint(gc);
	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), L1);
	gc->GetTextExtent(str, &w, &h, NULL, NULL);
	gc->DrawText(str, (CTX_Width + _DEFAULT_R_ - w)*0.5, (_DEFAULT_H_ - h)*0.5);
	gc->PopState();
}

bool DropdownMenu::Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p_w)
{
	wxPoint2DDouble p(p_w.m_x - X, p_w.m_y - Y);
	return btn->Contains(p, gc);
}

void DropdownMenu::SetOnChangeFunction(void (*fn)(void*), void* arg) {
	if(menu != NULL)
		menu->SetOnChangeFunction(fn, arg);
}

int DropdownMenu::GetSelected() {
	return menu->GetLastSelected();
}

void DropdownMenu::SetDefault() {
	menu->SetDefault();
}