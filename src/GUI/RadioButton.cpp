//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    RadioButton.cpp									//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WiiMIDI_GUI.h"

RadioButton* RadioButton::Current = NULL;

RadioButton::RadioButton(const wxDouble& x, const wxDouble& y)
{
	X = x;
	Y = y;
	selected = 0;
	isOn = -1;
	num_opts = 0;
	Width = _DEFAULT_R_;
}

RadioButton::~RadioButton() {
	Reset();
}

void RadioButton::Reset() {
	bts.clear();
	labels.clear();
}

void RadioButton::CenterY(const wxDouble &y) {
	Y = y - num_opts*_DEFAULT_R_*1.5*0.5 + _DEFAULT_R_*0.5;
}

void RadioButton::AddOption(const wxString &label) {
	labels.push_back(label);
	bts.push_back(Button(wxString(""), 0, num_opts*_DEFAULT_R_*1.5, NO_BLUR | TRANSPARENT, BTN_RADIO));
	bts[num_opts].SetSize(_DEFAULT_R_, _DEFAULT_R_);
	num_opts++;
}

void RadioButton::Paint(wxGraphicsContext *gc)
{
	wxDouble w, h, index;
	vector<Button>::iterator i;
	gc->PushState();
	gc->Translate(X, Y);
	for(i = bts.begin(), index = 0; i != bts.end(); i++, index++)
	{
		i->SetStyle(index == selected? LIGHT_THEME | NO_BLUR : TRANSPARENT | NO_BLUR);
		i->Paint(gc);
		gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), wxColour(L1));
		gc->GetTextExtent(labels[index], &w, &h, NULL, NULL);
		gc->DrawText(labels[index], _DEFAULT_R_*2, index*_DEFAULT_R_*1.5 + (_DEFAULT_R_ - h)*0.5);
		Width = fmax(Width, _DEFAULT_R_*2 + w);
	}
	gc->PopState();
}

bool RadioButton::Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p_w)
{
	wxPoint2DDouble p(p_w.m_x - X, p_w.m_y - Y);
	vector<Button>::iterator i;
	isOn = -1;
	int index;
	for (i = bts.begin(), index = 0; i != bts.end(); i++, index++)
		if((*i).Contains(p, gc) || wxRect2DDouble((*i).GetLocation().m_x + _DEFAULT_R_*0.5,
				(*i).GetLocation().m_y, Width - _DEFAULT_R_*0.5, _DEFAULT_R_).Contains(p)) {
			isOn = index;
			Current = this;
			return true;
		}
	return false;
}

void RadioButton::Click() {
	if(Current != NULL && Current->isOn != -1){
		Current->selected = Current->isOn;
		Current->OnChange();
	}
}

void RadioButton::Select(int i) {
	if(i >= 0 && i < num_opts)
		selected = i;
}

void RadioButton::OnChange() {
	if(Current != NULL && Current->custom_fn != NULL)
		Current->custom_fn(Current->fn_arg);
}

