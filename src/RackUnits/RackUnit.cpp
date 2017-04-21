//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    RackUnit.cpp										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../RackUnits.h"

RackUnit* RackUnit::Current = NULL;
RackUnit* RackUnit::DraggingUnit = NULL;
bool RackUnit::is_dragging = false;
wxPoint2DDouble RackUnit::StartDD = wxPoint2DDouble();
list<RackUnit*> RackUnit::previous_elements;
list<RackUnit*> RackUnit::next_elements;

void RackUnit::SetHeight(const wxDouble& h) { Height = h; }

wxDouble RackUnit::GetHeight() { return Height + TitleBarWidth - OffsetY; }

void RackUnit::SetWidth(const wxDouble& w) { Width = w; }

wxDouble RackUnit::GetWidth() { return Width; }

wxDouble RackUnit::GetUnitHeight() { return Height - OffsetY; }

void RackUnit::SetYPosition(const wxDouble& p) { Position = p; }

wxDouble RackUnit::GetYPosition() { return Position + (is_dragging? swap_pos*DraggingUnit->GetHeight() : 0); }

wxDouble RackUnit::GetUnitPosition() { return Position + TitleBarWidth; }

void RackUnit::SetName(const wxString& s) { Name = s; }

wxString RackUnit::GetName() { return Name; }

void RackUnit::InitTriggers(bool ButtonTriggered = false)
{
	const char *axis[] = { "Accelerometer X", "Accelerometer Y", "Accelerometer Z", "MotionPlus X", "MotionPlus Y", "MotionPlus Z" };
	const char *bts[] = { "Left", "Right", "Up", "Down", "A", "B", "+", "-", "Home", "1", "2" };
	const char *s[] = { "Reset Trigger", "Collapse", "Remove Unit", "Show Rack Manager" };

	r_click = new ContextMenu(0, 0, 4, s, false, CTX_RIGHT_CLICK, LIGHT_THEME);
	button_triggered = ButtonTriggered;
	if(ButtonTriggered)
		Trigger = new DropdownMenu(60, (Height - _DEFAULT_H_)*0.5, 11, bts);
	else
		Trigger = new DropdownMenu(60, (Height - _DEFAULT_H_)*0.5, 6, axis);
	Trigger->SetOnChangeFunction((void (*)(void *)) &RackUnit::OnTriggerChange, (void*) this);
	r_click->SetOnChangeFunction((void (*)(void *)) &RackUnit::OnCTXChange, (void*) this);
	r_click->GetButton(0)->Disable();
}

void RackUnit::DrawTriggerMenu(wxGraphicsContext *gc) {
	wxDouble w, h, i1, i2;
	wxString str("Trigger:");
	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), L1);
	gc->GetTextExtent(str, &w, &h, &i1, &i2);
	gc->DrawText(str, 15, (Height - h) * 0.5);
	Trigger->Paint(gc);
}

void RackUnit::Collapse() {
	Animation::Add(&OffsetY, OffsetY, Height);
	collapsed = true;
}

void RackUnit::Expand() {
	Animation::Add(&OffsetY, OffsetY, 0);
	collapsed = false;
}

bool RackUnit::IsCollapsed() {
	return collapsed;
}

void RackUnit::DrawBox(wxGraphicsContext *gc, const wxString& label, const wxDouble &x, const wxDouble &width) {
	wxDouble area_perc = 0.8, w, h, i1, i2;
	wxGraphicsPath path(gc->CreatePath());
	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), L1);
	gc->SetPen(wxPen(wxColour(L1)));
	gc->GetTextExtent(label, &w, &h, &i1, &i2);
	path.MoveToPoint(x + _DEFAULT_R_, Height*(1 - area_perc)*0.5);
	path.AddArc(x + _DEFAULT_R_, Height*(1 - area_perc)*0.5 + _DEFAULT_R_, _DEFAULT_R_, -M_PI*0.5, -M_PI, false);
	path.AddLineToPoint(path.GetCurrentPoint().m_x, path.GetCurrentPoint().m_y + area_perc*Height - _DEFAULT_R_*2);
	path.AddArc(path.GetCurrentPoint().m_x + _DEFAULT_R_, path.GetCurrentPoint().m_y, _DEFAULT_R_, -M_PI, -M_PI*1.5, false);
	path.AddLineToPoint(path.GetCurrentPoint().m_x + width - 2*_DEFAULT_R_, path.GetCurrentPoint().m_y);
	path.AddArc(path.GetCurrentPoint().m_x, path.GetCurrentPoint().m_y - _DEFAULT_R_, _DEFAULT_R_, -M_PI*1.5,- M_PI*2, false);
	path.AddLineToPoint(path.GetCurrentPoint().m_x, path.GetCurrentPoint().m_y - area_perc*Height + 2*_DEFAULT_R_);
	path.AddArc(path.GetCurrentPoint().m_x - _DEFAULT_R_, path.GetCurrentPoint().m_y, _DEFAULT_R_, 0, -M_PI*0.5, false);
	path.AddLineToPoint(path.GetCurrentPoint().m_x - width + 2*_DEFAULT_R_ + w, path.GetCurrentPoint().m_y);
	gc->StrokePath(path);
	gc->DrawText(label, x + _DEFAULT_R_, Height*(1 - area_perc)*0.5 - _FONT_SIZE_*0.85);
}

void RackUnit::DrawUnitContainer(wxGraphicsContext *gc) {
	wxDouble w, h, i1, i2;
	wxColour bg = D1;
	bg.Set(bg.Red(), bg.Green(), bg.Blue(), wxColourBase::ChannelType(AlphaDD));
	gc->SetPen(wxNullPen);
	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(true), L1);
	gc->GetTextExtent(Name, &w, &h, &i1, &i2);
	TitleBarWidth = h + 5;
	gc->SetPen(wxNullPen);
	gc->SetBrush(wxBrush(bg));
	gc->DrawRectangle(0, Position, Width, GetHeight());
	gc->DrawText(Name, 3 + _DEFAULT_R_*2 , Position + 5);
	gc->SetPen(wxPen(wxColour(L1)));
	gc->StrokeLine(_DEFAULT_R_*2  + w + 8, Position + 5 + h*0.5, Width - 10, Position + 5 + h*0.5);
	collapse->SetLabel(IsCollapsed()? wxString(wxT("▷")) : wxString(wxT("▽")));
	close->SetLabel(this == Current? wxString(wxT("✕")) : wxString(wxT("∘")));
	close->SetLocation(4, 6 + (h - _DEFAULT_R_)*0.5 + Position);
	collapse->SetLocation(2 + _DEFAULT_R_, 6 + (h - _DEFAULT_R_)*0.5 + Position);
	collapse->Paint(gc);
	close->Paint(gc);
}

RackUnit::~RackUnit() { delete Trigger; }

bool RackUnit::Contains(wxGraphicsContext *gc, const wxPoint2DDouble& p_w) {
	wxPoint2DDouble p(p_w.m_x, p_w.m_y - Position - TitleBarWidth);
	if(Trigger->Contains(gc, p) || close->Contains(p_w, gc) || collapse->Contains(p_w, gc)) {
		is_on_titlebar = false;
		Current = this;
		return true;
	} else if(p_w.m_y >= Position && p.m_y < GetUnitHeight()) {
		is_on_titlebar = p_w.m_y - Position >= 0 && p_w.m_y - Position <= TitleBarWidth;
		Current = this;
		return true;
	}
	return false;
}

void RackUnit::SetLevel(int l) { Level = l < 0? 0 : l > 127? 127 : l; }

void RackUnit::SetNote(int n) { Note = n < 0? 0 : n > 127? 127 : n; }

void RackUnit::SetBend(int b) { Bend = b < 0? 0 : b > 0x3FFF? 0x3FFF : b; }

int RackUnit::GetNote() { return -1; }

int RackUnit::GetLevel() { return -1; }

int RackUnit::GetBend() { return -1; }

void RackUnit::Click() {}

void RackUnit::ClickCurrent() {
	if(Current != NULL)
		Current->Click();
}

void RackUnit::Close() {
	Animation::Add(&Alpha, 0, 255)->Queue(&OffsetY, OffsetY, TitleBarWidth + Height)->
			QueueFunction((void (*)(void*)) Rack::Remove, (void*) this);
	closing = true;
}

void RackUnit::DrawFading(wxGraphicsContext* gc) {
	if(closing) {
		wxColour bg = D2;
		bg.Set(bg.Red(), bg.Green(), bg.Blue(), wxColourBase::ChannelType(Alpha));
		gc->SetPen(wxNullPen);
		gc->SetBrush(bg);
		gc->DrawRectangle(0, GetYPosition(), Width, GetHeight() + TitleBarWidth);
	}
}

void RackUnit::Reset() {
	Alpha = 0;
	closing = false;
	OffsetY = 0;
	collapsed = false;
	ResetTrigger();
}

void RackUnit::StartDragging(const wxPoint2DDouble& p) {
	if(Current == NULL || !Current->is_on_titlebar)
		return;
	list<RackUnit*>::iterator i;
	bool found = false;
	previous_elements.clear();
	next_elements.clear();
	Current->OffsetDD = wxPoint2DDouble();
	StartDD = p;
	for(i = Rack::ActiveUnits.begin(); i != Rack::ActiveUnits.end(); i++)
		if(*i == Current)
			found = true;
		else if(!found)
			previous_elements.push_back(*i);
		else
			next_elements.push_back(*i);
	DraggingUnit = Current;
	Animation::Add(&(DraggingUnit->AlphaDD), DraggingUnit->AlphaDD, 150);
	is_dragging = true;
}

void RackUnit::Drag(const wxPoint2DDouble &p) {
	if(!is_dragging)
		return;
	DraggingUnit->OffsetDD = wxPoint2DDouble(p.m_x - StartDD.m_x, p.m_y - StartDD.m_y);
	if(!previous_elements.empty() && previous_elements.back()->swap_pos < 1 && DraggingUnit->Position +
			DraggingUnit->OffsetDD.m_y < previous_elements.back()->GetYPosition() +
			previous_elements.back()->GetHeight()*0.5) {
		next_elements.push_front(previous_elements.back());
		previous_elements.pop_back();
		next_elements.front()->swap_pos = min(next_elements.front()->swap_pos + 1, 1);
		Animation::Add(&(next_elements.front()->OffsetPos),	next_elements.front()->OffsetPos,
				next_elements.front()->swap_pos*DraggingUnit->GetHeight());
	} else if(!next_elements.empty() && next_elements.front()->swap_pos > -1 && DraggingUnit->Position +
			DraggingUnit->OffsetDD.m_y + DraggingUnit->GetHeight() > next_elements.front()->GetYPosition() +
			next_elements.front()->GetHeight()*0.5) {
		previous_elements.push_back(next_elements.front());
		next_elements.pop_front();
		previous_elements.back()->swap_pos = max(previous_elements.back()->swap_pos - 1, -1);
		Animation::Add(&(previous_elements.back()->OffsetPos), previous_elements.back()->OffsetPos,
				previous_elements.back()->swap_pos*DraggingUnit->GetHeight());
	}
}

void RackUnit::Drop() {
	if(!is_dragging)
		return;
	wxDouble Y = 0;
	Rack::ActiveUnits.clear();
	while(!previous_elements.empty()) {
		Animation::Add(&(previous_elements.front()->OffsetPos), previous_elements.front()->OffsetPos +
				previous_elements.front()->Position - Y, 0);
		previous_elements.front()->OffsetPos +=	previous_elements.front()->Position - Y;
		previous_elements.front()->swap_pos = 0;
		previous_elements.front()->SetYPosition(Y);
		Rack::ActiveUnits.push_back(previous_elements.front());
		previous_elements.pop_front();
		Y += Rack::ActiveUnits.back()->GetHeight();
	}
	Rack::ActiveUnits.push_back(DraggingUnit);
	DraggingUnit->OffsetDD.m_y += DraggingUnit->Position - Y;
	DraggingUnit->swap_pos = 0;
	DraggingUnit->SetYPosition(Y);
	Y += Rack::ActiveUnits.back()->GetHeight();
	while(!next_elements.empty()) {
		Animation::Add(&(next_elements.front()->OffsetPos), next_elements.front()->OffsetPos +
				next_elements.front()->Position - Y, 0);
		next_elements.front()->OffsetPos +=	next_elements.front()->Position - Y;
		next_elements.front()->swap_pos = 0;
		next_elements.front()->SetYPosition(Y);
		Rack::ActiveUnits.push_back(next_elements.front());
		next_elements.pop_front();
		Y += Rack::ActiveUnits.back()->GetHeight();
	}
	Animation::Add(&(DraggingUnit->OffsetDD.m_x), DraggingUnit->OffsetDD.m_x, 0);
	Animation::Add(&(DraggingUnit->OffsetDD.m_y), DraggingUnit->OffsetDD.m_y, 0);
	Animation::Add(&(DraggingUnit->AlphaDD), DraggingUnit->AlphaDD, 255);
	is_dragging = false;
}

bool RackUnit::IsDragging() {
	if (is_dragging)
		return true;

	for (list<RackUnit *>::iterator i = Rack::ActiveUnits.begin(); i != Rack::ActiveUnits.end(); i++)
		if ((*i)->OffsetPos != 0 || (*i)->OffsetDD.m_y != 0)
			return true;

	return false;
}

void RackUnit::OnTriggerChange(RackUnit* unit) {
	for(int i = 0; i < 17; i++)
		if(Rack::Handlers[i] == unit)
			Rack::Handlers[i] = NULL;
	if(unit->Trigger->GetSelected() != -1) {
		Rack::SetHandler(unit->GetTrigger(), unit);
		unit->GetCTX()->GetButton(0)->Disable();
	} else
		unit->GetCTX()->GetButton(0)->Enable();
}

void RackUnit::OnCTXChange(RackUnit* unit) {
	switch(unit->GetCTX()->GetLastSelected()) {
		case 0:
			unit->ResetTrigger();
			break;

		case 1:
			if(unit->IsCollapsed()) {
				unit->Expand();
				unit->GetCTX()->GetButton(1)->SetLabel("Collapse");
			} else {
				unit->Collapse();
				unit->GetCTX()->GetButton(1)->SetLabel("Expand");
			}
			break;

		case 2:
			unit->Close();
			break;

		case 3:
			if(Rack::GetRackManager() != NULL)
				Rack::GetRackManager()->Show();
			break;

		default:
			break;
	}
}

int RackUnit::GetTrigger() {
	return Trigger->GetSelected() == -1? -1 : Trigger->GetSelected() + (button_triggered? 0 : 11);
}