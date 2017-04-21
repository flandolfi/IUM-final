//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    RackManager.cpp									//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WindowTypes.h"
#include "../RackUnits.h"

RackManager::RackManager(wxFrame* owner)
{
	list<RackUnit*>::iterator i;
	Parent = owner;
	selected = NULL;
	area = wxRect2DDouble(0, 0, 450, 350);
	area.SetCentre(wxPoint2DDouble(Parent->GetClientSize().GetWidth()*0.5, Parent->GetClientSize().GetHeight()*0.5));
	bt_left = new Button(wxString("<<"), (area.m_width - _DEFAULT_W_)*0.5,
			area.m_height*0.5 - _DEFAULT_H_*2.3, NO_FILL, BTN_RACK_MAN);
	bt_right = new Button(wxString(">>"), (area.m_width - _DEFAULT_W_)*0.5,
			area.m_height*0.5 - _DEFAULT_H_*0.7, NO_FILL, BTN_RACK_MAN);
	bt_ok = new Button(wxString("Ok"), area.m_width - _DEFAULT_W_,
			area.m_height - _DEFAULT_H_, NO_FILL, BTN_RACK_MAN);
	bt_cancel = new Button(wxString("Cancel"), area.m_width - _DEFAULT_W_*2.3,
			area.m_height - _DEFAULT_H_, NO_FILL, BTN_RACK_MAN);
	for (i = Rack::AvailableUnits.begin(); i != Rack::AvailableUnits.end(); i++) {
		available_units.push_back(new Button((*i)->GetName(), 0, 0, TRANSPARENT | NO_BORDER | RECT_SHAPED | NO_BLUR, BTN_RACK_MAN));
		available_units.back()->SetSize(area.m_width * 0.5 - _DEFAULT_W_ * 0.7, _FONT_SIZE_ * 1.5);
	}
	Update();
}

void RackManager::Show()
{
	Window::Show();
	Update();
}

RackManager::~RackManager()
{
	delete bt_left;
	delete bt_right;
	delete bt_ok;
	delete bt_cancel;
}

void RackManager::Update()
{
	list<RackUnit*>::iterator i;
	active_units.clear();
	for (i = Rack::ActiveUnits.begin(); i != Rack::ActiveUnits.end(); i++) {
		active_units.push_back(new Button((*i)->GetName(), 0, 0, TRANSPARENT | NO_BORDER | RECT_SHAPED | NO_BLUR, BTN_RACK_MAN));
		active_units.back()->SetSize(area.m_width * 0.5 - _DEFAULT_W_ * 0.7, _FONT_SIZE_ * 1.5);
	}
}

void RackManager::Paint(wxGraphicsContext* gc)
{
	list<Button*>::iterator i;
	wxDouble x_pos, y_pos;
	DrawBackground(gc);
	DrawContainer(gc, wxString("Rack Manager"), area);
	gc->PushState();
	gc->Translate(area.m_x, area.m_y);
	bt_left->Paint(gc);
	bt_right->Paint(gc);
	bt_ok->Paint(gc);
	bt_cancel->Paint(gc);
	gc->SetPen(wxPen(L1));
	gc->SetBrush(*wxTRANSPARENT_BRUSH);
	gc->DrawRoundedRectangle(0, 0, area.m_width*0.5 - _DEFAULT_W_*0.7, area.m_height - _DEFAULT_H_*2, _DEFAULT_R_);
	gc->DrawRoundedRectangle(area.m_width*0.5 + _DEFAULT_W_*0.7, 0,
			area.m_width*0.5 - _DEFAULT_W_*0.7, area.m_height - _DEFAULT_H_*2, _DEFAULT_R_);
	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), L1);
	gc->DrawText(wxString(" Available Units "), _DEFAULT_R_, -_FONT_SIZE_*0.85, gc->CreateBrush(D1));
	gc->DrawText(wxString(" Active Units "), area.m_width*0.5 + _DEFAULT_W_*0.7 + _DEFAULT_R_,
			-_FONT_SIZE_*0.85, gc->CreateBrush(D1));
	x_pos = 0;
	y_pos = _FONT_SIZE_ * 1.5;
	for(i = available_units.begin(); i != available_units.end(); i++) {
		(*i)->SetLocation(x_pos, y_pos);
		(*i)->Paint(gc);
		y_pos += (*i)->GetSize().y + 5;
	}
	x_pos = area.m_width*0.5 + _DEFAULT_W_*0.7;
	y_pos = _FONT_SIZE_ * 1.5;
	for(i = active_units.begin(); i != active_units.end(); i++) {
		(*i)->SetLocation(x_pos, y_pos);
		(*i)->Paint(gc);
		y_pos += (*i)->GetSize().y + 5;
	}
	gc->PopState();
}

bool RackManager::Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p_w)
{
	if(!area.Contains(p_w))
		return false;

	wxPoint2DDouble p(p_w.m_x - area.m_x, p_w.m_y - area.m_y);
	list<Button*>::iterator i;

	for(i = available_units.begin(); i != available_units.end(); i++)
		if((*i)->Contains(p, gc))
			return true;

	for(i = active_units.begin(); i != active_units.end(); i++)
		if((*i)->Contains(p, gc))
			return true;

	return bt_ok->Contains(p, gc) || bt_cancel->Contains(p, gc) || bt_left->Contains(p, gc) || bt_right->Contains(p, gc);
}

int RackManager::Click()
{
	if(Button::LastClicked == bt_left) {
		if(selected != NULL) {
			active_units.remove(selected);
		}
	} else if(Button::LastClicked == bt_right) {
		if(selected != NULL) {
			list<Button*>::iterator i;
			for(i = active_units.begin(); i != active_units.end(); i++)
				if((*i)->GetLabel().IsSameAs(selected->GetLabel(), true))
					break;
			if(i == active_units.end()) {
				active_units.push_back(new Button(selected->GetLabel(), 0, 0, TRANSPARENT | NO_BORDER | RECT_SHAPED | NO_BLUR, BTN_RACK_MAN));
				active_units.back()->SetSize(area.m_width * 0.5 - _DEFAULT_W_ * 0.7, _FONT_SIZE_ * 1.5);
			}
		}
	} else if(Button::LastClicked == bt_ok) {
		list<Button*>::iterator i;
		list<RackUnit*>::iterator j;
		Rack::ActiveUnits.clear();
		for(i = active_units.begin(); i != active_units.end(); i++)
			for(j = Rack::AvailableUnits.begin(); j != Rack::AvailableUnits.end(); j++)
				if((*i)->GetLabel().IsSameAs((*j)->GetName(), true))
					Rack::Add((*j));
		Hide();
		return 0;
	} else if(Button::LastClicked == bt_cancel)
		Hide();
	else {
		if(selected != NULL)
			selected->SetStyle(TRANSPARENT | NO_BORDER | RECT_SHAPED | NO_BLUR);
		selected = Button::LastClicked;
		selected->SetStyle(TRANSPARENT| NO_BORDER | BOLD_FONT | RECT_SHAPED | NO_BLUR);
	}
	return -1;
}

void RackManager::DoubleClick() {
	bool found = false;
	if(selected != NULL && Button::CurrentButton != bt_cancel && Button::CurrentButton != bt_ok &&
			Button::CurrentButton != bt_left && Button::CurrentButton != bt_right) {
		list<Button*>::iterator i;
		for(i = active_units.begin(); i != active_units.end(); i++)
			if((*i) == selected) {
				active_units.remove(selected);
				return;
			} else if((*i)->GetLabel().IsSameAs(selected->GetLabel(), true))
				found = true;

		if(!found) {
			active_units.push_back(new Button(selected->GetLabel(), 0, 0, TRANSPARENT | NO_BORDER | RECT_SHAPED | NO_BLUR, BTN_RACK_MAN));
			active_units.back()->SetSize(area.m_width * 0.5 - _DEFAULT_W_ * 0.7, _FONT_SIZE_ * 1.5);
		}
	}
}