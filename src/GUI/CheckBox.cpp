//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    CheckBox.cpp										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WiiMIDI_GUI.h"

CheckBox* CheckBox::Current = NULL;

CheckBox::CheckBox(const wxDouble& x, const wxDouble& y)
{
	X = x;
	Y = y;
	isOn = -1;
	num_opts = 0;
}

CheckBox::~CheckBox() {
	Reset();
}

void CheckBox::Reset() {
	bts.clear();
	labels.clear();
	checks.clear();
}

void CheckBox::Toggle(int i) {
	if(i >= 0 && i < num_opts)
		checks[i] = !checks[i];
}

void CheckBox::CenterY(const wxDouble &y) {
	Y = y - num_opts*_DEFAULT_R_*1.5*0.5 + _DEFAULT_R_*0.5;
}

void CheckBox::AddOption(const wxString &label) {
	labels.push_back(label);
	bts.push_back(Button(wxString(""), 0, num_opts*_DEFAULT_R_*1.5, NO_BLUR | TRANSPARENT | RECT_SHAPED, BTN_CHECKBOX));
	bts[num_opts].SetSize(_DEFAULT_R_, _DEFAULT_R_);
	checks.push_back(false);
	num_opts++;
}

void CheckBox::Paint(wxGraphicsContext *gc)
{
	wxDouble w, h, index;
	vector<Button>::iterator i;
	gc->PushState();
	gc->Translate(X, Y);
	for(i = bts.begin(), index = 0; i != bts.end(); i++, index++)
	{
		if(checks[index])
			i->SetLabel(wxString(wxT("✔")));
		else
			i->SetLabel(wxString(wxT("")));
		i->Paint(gc);
		gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), wxColour(L1));
		gc->GetTextExtent(labels[index], &w, &h, NULL, NULL);
		gc->DrawText(labels[index], _DEFAULT_R_*2, index*_DEFAULT_R_*1.5 +(_DEFAULT_R_ - h)*0.5);
	}
	gc->PopState();
}

bool CheckBox::Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p_w)
{
	wxPoint2DDouble p(p_w.m_x - X, p_w.m_y - Y);
	vector<Button>::iterator i;
	int index;
	for (i = bts.begin(), index = 0; i != bts.end(); i++, index++)
		if((*i).Contains(p, gc)) {
			Current = this;
			isOn = index;
			return true;
		}
	return false;
}

void CheckBox::Click() {
	if(Current->isOn != -1)
		Current->Toggle(Current->isOn);
}

bool CheckBox::IsChecked(int i) {
	return i >= 0 && i < num_opts? checks[i] : false;
}

