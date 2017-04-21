//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    Monitor.cpp										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../Monitor.h"
#include "../RackUnits.h"

Coordinate Monitor::acc = Coordinate();
Coordinate Monitor::mp = Coordinate();
Coordinate Monitor::abs_acc = Coordinate();
Coordinate Monitor::last_acc = Coordinate();
Coordinate Monitor::abs_mp = Coordinate();
Coordinate Monitor::acc_val = Coordinate();
Coordinate Monitor::mp_val = Coordinate();
Window* Monitor::mon = NULL;
Window* Monitor::ctrl = NULL;
bool Monitor::init = true;
unsigned int Monitor::key_status[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

Monitor::Monitor(const wxRect2DDouble& a) {
	const char *s[] = { "Show details", "Controller settings" };
	area = a;
	r_click = new ContextMenu(0, 0, 2, s, false, CTX_RIGHT_CLICK, 0);
	r_click->SetOnChangeFunction((void (*)(void*)) Monitor::OnCTXChange, this);
	bt_expand = new Button(wxString(wxT("∘")), a.m_x + 3, a.m_y + 5,
			LIGHT_THEME | BOLD_FONT | NO_BLUR | NO_BORDER | NO_FILL | RECT_SHAPED, BTN_MONITOR);
	bt_expand->SetSize(_DEFAULT_R_, _DEFAULT_R_);
}

Monitor::~Monitor() {
	delete bt_expand;
	delete r_click;
}

void Monitor::DrawEllipseThroughPoint(wxGraphicsContext *gc, const wxDouble& x,
	const wxDouble& y, const wxDouble& a, const wxDouble& angle, const unsigned int side)
{
	wxDouble sign = wxSign(y);
	if(side == NORTH_SIDE)
		gc->Clip(-a - _ELLIPSE_PEN_W_, 0, (a + _ELLIPSE_PEN_W_)*2, -sign*(a + _ELLIPSE_PEN_W_));
	if(side == SOUTH_SIDE)
		gc->Clip(-a - _ELLIPSE_PEN_W_, 0, (a + _ELLIPSE_PEN_W_)*2, sign*(a + _ELLIPSE_PEN_W_));
	
	if (x == 0)
	{
		if(wxAbs(y) == a)
		{
			if (cos(angle) == 0)
			{
				gc->ResetClip();
				gc->StrokeLine(0, -a, 0, a);
			}
			else
				gc->DrawEllipse(-wxAbs(a*cos(angle)), -a, wxAbs(a*cos(angle)*2), 2*a);
		}
		else if (y == 0)
		{
			gc->ResetClip();
			gc->StrokeLine(-a, 0, a, 0);
		}
		else
			gc->DrawEllipse(-a, -wxAbs(y), a*2, wxAbs(y)*2);
	} else
	{
		if (wxAbs(x) == a)
		{
			gc->ResetClip();
			sign = wxSign(sin(angle)*cos(angle));
			if(side == NORTH_SIDE)
				gc->Clip(-a - _ELLIPSE_PEN_W_, 0, (a + _ELLIPSE_PEN_W_)*2, -sign*(a + _ELLIPSE_PEN_W_));
			if(side == SOUTH_SIDE)
				gc->Clip(-a - _ELLIPSE_PEN_W_, 0, (a + _ELLIPSE_PEN_W_)*2, sign*(a + _ELLIPSE_PEN_W_));
			gc->DrawEllipse(-a, -wxAbs(a*cos(angle)), a*2, wxAbs(a*cos(angle))*2);
		}
		else
		{
			// wxDouble b = wxAbs((long double) y/sin(acos(x/a)));
			wxDouble b = wxAbs(a*y/sqrt((a + x)*(a - x)));
			if (b == 0)
			{
				gc->ResetClip();
				gc->StrokeLine(a, 0, -a, 0);
			} else if (wxAbs(b) == a)
			{
				if (cos(angle) == 0)
				{
					gc->ResetClip();
					gc->StrokeLine(a, 0, -a, 0);
				} else
				{
					gc->Rotate(M_PI*0.5 - angle);
					gc->DrawEllipse(-a, -wxAbs(a*cos(angle)), a*2, wxAbs(a*cos(angle))*2);
					gc->Rotate(angle - M_PI*0.5);
				}
			} else
				gc->DrawEllipse(-a, -b, a*2, b*2);
		}
	}
	gc->ResetClip();
}

void Monitor::DrawSphere(wxGraphicsContext *gc, const wxRect2DDouble& area, bool light_theme,
	const wxDouble& beta, const wxDouble& teta, const wxDouble& step)
{
	wxDouble a = area.m_width*0.5, b, alpha, alpha_r, beta_r = wxDegToRad(beta), step_r = wxDegToRad(step), teta_r = wxDegToRad(teta);
    wxColour C1, C2, C3, C4;
	C2 = light_theme? wxColour(0, 0, 0, 75) : wxColour(L1.Red(), L1.Green(), L1.Blue(), 50);
	C3 = light_theme? wxColour(0, 0, 0, 50) : L1;
	C4 = light_theme? *wxBLACK : L3;
	C1 = light_theme? *wxBLACK : L2;
	wxPen pen(C2);
	gc->SetBrush(wxNullBrush);
	gc->PushState();
	gc->Translate(area.m_x + a, area.m_y + area.m_height*0.5);
	gc->PushState();
	gc->Rotate(wxDegToRad(-90 - teta));
	for (alpha = -teta; alpha < 180 - teta; alpha += step)
	{
		if (alpha == -teta || alpha == (90 - teta))
			pen.SetColour(C1);
		else
			pen.SetColour(C2);
		gc->SetPen(pen);
		if (!alpha || alpha == 180)
			gc->StrokeLine(-a, 0, a, 0);
		else
		{
			alpha_r = wxDegToRad(alpha);
			b = a*cos(beta_r)*sin(alpha_r)*sqrt(1/(1 - cos(beta_r)*cos(beta_r)*cos(alpha_r)*cos(alpha_r)));
			gc->Clip(-a, 0, a*2, b);
			gc->DrawEllipse(-a, -wxAbs(b), a*2, wxAbs(b)*2);		
			gc->ResetClip();
		}
		if(beta_r)
			gc->Rotate(step_r);			
	}
	gc->PopState();
	gc->PushState();
	gc->Rotate(wxDegToRad(-90));
	for (alpha = 0; alpha <= 180; alpha += step)
	{
		if (alpha == 90)
			pen.SetColour(C1);
		else
			pen.SetColour(C2);
		gc->SetPen(pen);
		alpha_r = wxDegToRad(alpha);
		gc->Clip(-a, -a, a*(2 - cos(alpha_r)), 2*a);
		gc->DrawEllipse(a*(cos(beta_r)*cos(alpha_r) - sin(alpha_r)*sin(beta_r)),
			-a*sin(alpha_r), 2*a*sin(beta_r)*sin(alpha_r), 2*a*sin(alpha_r));
		gc->ResetClip();		
	}
	gc->Rotate(wxDegToRad(90));
	//Arrow Section - BEGIN
	wxColour tr(0, 0, 0, 0);
	Coordinate cdt(MapToSphere(abs_acc));
	cdt *= a;
	wxDouble x=-cdt.x, y=-cdt.z, z=-cdt.y, gamma=-teta_r, delta=beta_r;
	wxPoint2DDouble p(x*cos(gamma) + z*sin(gamma), -y*cos(delta) + x*cos(gamma)*sin(delta) - z*sin(delta)*sin(gamma));
	wxDouble r = a*0.25;
	gc->SetPen(wxNullPen);
	gc->SetBrush(gc->CreateRadialGradientBrush(p.m_x, p.m_y, p.m_x, p.m_y, r*(1 + abs_acc.y*0.5), C4, tr));
	gc->DrawRectangle(p.m_x - r*(1 + abs_acc.y*0.5), p.m_y - r*(1 + abs_acc.y*0.5), r*2*(1 + abs_acc.y*0.5), r*2*(1 + abs_acc.y*0.5));
	gc->SetBrush(wxNullBrush);
	gc->Rotate(wxDegToRad(-90 - teta));
	//Arrow Section - END
	for (alpha = -teta; alpha < 180 - teta; alpha += step)
	{
		if (alpha == -teta || alpha == (90 - teta))
			pen.SetColour(C4);
		else
			pen.SetColour(C3);
		gc->SetPen(pen);
		if (!alpha || alpha == 180)
			gc->StrokeLine(-a, 0, a, 0);
		else
		{
			alpha_r = wxDegToRad(alpha);
			b = a*cos(beta_r)*sin(alpha_r)*sqrt(1/(1 - cos(beta_r)*cos(beta_r)*cos(alpha_r)*cos(alpha_r)));
			gc->Clip(-a, -b, a*2, b);
			gc->DrawEllipse(-a, -wxAbs(b), a*2, wxAbs(b)*2);		
			gc->ResetClip();
		}
		if(beta_r)
			gc->Rotate(step_r);			
	}
	gc->PopState();
	gc->Rotate(wxDegToRad(-90));
	for (alpha = 0; alpha <= 180; alpha += step)
	{
		if (alpha == 90)
			pen.SetColour(C4);
		else
			pen.SetColour(C3);
		gc->SetPen(pen);
		alpha_r = wxDegToRad(alpha);
		gc->Clip(-a, -a, a*(1 + cos(alpha_r)), 2*a);
		gc->DrawEllipse(a*(cos(beta_r)*cos(alpha_r) - sin(alpha_r)*sin(beta_r)),
			-a*sin(alpha_r), 2*a*sin(beta_r)*sin(alpha_r), 2*a*sin(alpha_r));
		gc->ResetClip();		
	}
	gc->PopState();
}

Coordinate Monitor::MapToSphere(const Coordinate& c)
{
	return Coordinate(c.x*sqrt(1 - c.y*c.y*0.5 - c.z*c.z*0.5 + c.y*c.y*c.z*c.z/3),
		c.y*sqrt(1 - c.x*c.x*0.5 - c.z*c.z*0.5 + c.x*c.x*c.z*c.z/3),
		c.z*sqrt(1 - c.y*c.y*0.5 - c.x*c.x*0.5 + c.y*c.y*c.x*c.x/3));
}

void Monitor::DrawGyroscope(wxGraphicsContext *gc, const wxRect2DDouble& area, bool light_theme)
{
	wxDouble a, r, gap, alpha, beta, gamma, teta, sign;
	wxColour C1, C2, C3, C4;
	C1 = light_theme? D1 : L1;
	C2 = light_theme? D2 : L2;
	C3 = light_theme? D3 : L3;
	C4 = light_theme? wxColour(0, 0, 0, 255) : D3;
	r = area.m_width*0.1;
	gap = (area.m_width*0.5 - r)/3;
	alpha = abs_mp.z;
	beta = abs_mp.y;
	gamma = abs_mp.x;
	sign = (sin(beta) < 0)? -1 : 1;
	teta = (cos(beta) == 0 || cos(alpha) == 1)? 0 : sign*acos(cos(beta)/sqrt(1 - cos(alpha)*cos(alpha)*sin(beta)*sin(beta)));

	gc->PushState();

	gc->Translate(area.m_x + area.m_width*0.5, area.m_y + area.m_height*0.5);
	gc->SetBrush(wxNullBrush);
	a = r + 3*gap;
	gc->Rotate(teta);
	gc->SetPen(wxPen(C3, _ELLIPSE_PEN_W_));
	DrawEllipseThroughPoint(gc, a, a*cos(gamma), a, gamma, SOUTH_SIDE);
	gc->SetPen(wxPen(C3, _AXES_PEN_W_));
	a = r + 2*gap;
	gc->StrokeLine((a + gap), 0, a, 0);
	gc->Rotate(beta - teta);
	gc->SetPen(wxPen(C2, _ELLIPSE_PEN_W_));
	DrawEllipseThroughPoint(gc, a*cos(alpha)*cos(M_PI*0.5 - beta),
		a*cos(alpha)*sin(M_PI*0.5 - beta), a, beta, SOUTH_SIDE);
	gc->Rotate(-beta);
	a = r + gap;
	gc->SetPen(wxPen(C2, _AXES_PEN_W_));
	gc->StrokeLine(0, (a + gap)*cos(alpha), 0, a*cos(alpha));
	gc->SetPen(wxPen(C1, _ELLIPSE_PEN_W_));
	DrawEllipseThroughPoint(gc, 0, a*cos(alpha), a, alpha, SOUTH_SIDE);
	gc->SetPen(wxPen(C1, _AXES_PEN_W_));
	gc->StrokeLine(-a, 0, a, 0);
	gc->SetPen(wxPen(C4, _ELLIPSE_PEN_W_));
	gc->SetBrush(C4);
	gc->DrawEllipse(-r, -r, r*2, r*2); //ROTOR
	gc->SetBrush(wxNullBrush);
	gc->SetPen(wxPen(C1, _ELLIPSE_PEN_W_));
	DrawEllipseThroughPoint(gc, 0, a*cos(alpha), a, alpha, NORTH_SIDE);
	gc->SetPen(wxPen(C2, _AXES_PEN_W_));
	gc->StrokeLine(0, -(a + gap)*cos(alpha), 0, -a*cos(alpha));
	a = r + 2*gap;
	gc->Rotate(beta);
	gc->SetPen(wxPen(C2, _ELLIPSE_PEN_W_));
	DrawEllipseThroughPoint(gc, a*cos(alpha)*cos(M_PI*0.5 - beta),
		a*cos(alpha)*sin(M_PI*0.5 - beta), a, beta, NORTH_SIDE);
	gc->SetPen(wxPen(C3, _AXES_PEN_W_));
	gc->Rotate(teta - beta);
	gc->StrokeLine(-(a + gap), 0, -a, 0);
	a = r + 3*gap;
	gc->SetPen(wxPen(C3, _ELLIPSE_PEN_W_));
	DrawEllipseThroughPoint(gc, a, a*cos(gamma), a, gamma, NORTH_SIDE);

	gc->PopState();
}

void Monitor::DrawMPMonitor(wxGraphicsContext *gc, const wxRect2DDouble& area, bool light_theme)
{
	wxColour C1, B1;
	C1 = light_theme? D1 : L1;
	B1 = light_theme? L1 : D1;
    wxDouble gap = area.m_height*0.125;
    gc->SetBrush(wxNullBrush);
    gc->SetPen(wxColour(C1));
    gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C1);
    gc->DrawRoundedRectangle(area.m_x, area.m_y, area.m_width, area.m_height, _DEFAULT_R_);
    gc->DrawText(wxString(" Gyroscope "), area.m_x + _DEFAULT_R_, area.m_y - _FONT_SIZE_*0.85, gc->CreateBrush(B1));
    DrawGyroscope(gc, wxRect2DDouble(area.m_x + gap, area.m_y + gap, area.m_height - 2*gap, area.m_height - 2*gap));
    DrawMonitorBar(gc, mp, wxRect2DDouble(area.m_x + area.m_height, area.m_y + gap, area.m_width - area.m_height - gap, area.m_height - 2*gap));
}

void Monitor::DrawAccMonitor(wxGraphicsContext *gc, const wxRect2DDouble& area, bool light_theme)
{
	wxColour C1, B1;
	C1 = light_theme? D1 : L1;
	B1 = light_theme? L1 : D1;
    wxDouble gap = area.m_height*0.125;
    gc->SetBrush(wxNullBrush);
    gc->SetPen(wxColour(C1));
    gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C1);
    gc->DrawRoundedRectangle(area.m_x, area.m_y, area.m_width, area.m_height, _DEFAULT_R_);
    gc->DrawText(wxString(" Accelerometer "), area.m_x + _DEFAULT_R_, area.m_y - _FONT_SIZE_*0.85, gc->CreateBrush(B1));
    DrawSphere(gc, wxRect2DDouble(area.m_x + gap, area.m_y + gap, area.m_height - 2*gap, area.m_height - 2*gap));
    DrawMonitorBar(gc, abs_acc, wxRect2DDouble(area.m_x + area.m_height, area.m_y + gap, area.m_width - area.m_height - gap, area.m_height - 2*gap));
}

void Monitor::DrawButtonsMonitor(wxGraphicsContext *gc, const wxRect2DDouble& area, bool light_theme)
{
	wxColour C1, B1;
	C1 = light_theme? D1 : L1;
	B1 = light_theme? L1 : D1;
	wxDouble u = area.m_height*0.25;
	unsigned int flags_off = light_theme? LIGHT_THEME | NO_BLUR : NO_BLUR;
	unsigned int flags_on = light_theme? NO_BLUR : LIGHT_THEME | NO_BLUR;
	wxGraphicsPath gp(gc->CreatePath());
	Button bts[11];
	bts[0] = Button(wxString(wxT("◃")), 0, 0, flags_off | NO_BORDER);
	bts[1] = Button(wxString(wxT("▹")), 0, 0, flags_off | NO_BORDER);
	bts[2] = Button(wxString(wxT("▵")), 0, 0, flags_off | NO_BORDER);
	bts[3] = Button(wxString(wxT("▿")), 0, 0, flags_off | NO_BORDER);
	bts[4] = Button(wxString(wxT("A")), 0, 0, flags_off);
	bts[5] = Button(wxString(wxT("B")), 0, 0, flags_off);
	bts[6] = Button(wxString(wxT("+")), 0, 0, flags_off);
	bts[7] = Button(wxString(wxT("-")), 0, 0, flags_off);
	bts[8] = Button(wxString(wxT("⌂")), 0, 0, flags_off);
	bts[9] = Button(wxString(wxT("1")), 0, 0, flags_off);
	bts[10] = Button(wxString(wxT("2")), 0, 0, flags_off);
	bts[0].SetSize(u*0.5, u*0.5);
	bts[1].SetSize(u*0.5, u*0.5);
	bts[2].SetSize(u*0.5, u*0.5);
	bts[3].SetSize(u*0.5, u*0.5);
	bts[4].SetSize(1.4*u, 1.4*u);
	bts[5].SetSize(1.4*u, 1.4*u);
	bts[6].SetSize(0.7*u, 0.7*u);
	bts[7].SetSize(0.7*u, 0.7*u);
	bts[8].SetSize(0.7*u, 0.7*u);
	bts[9].SetSize(1.4*u, 1.4*u);
	bts[10].SetSize(1.4*u, 1.4*u);
	bts[0].SetCentre(u, 2*u);
	bts[1].SetCentre(3*u, 2*u);
	bts[2].SetCentre(2*u, u);
	bts[3].SetCentre(2*u, 3*u);
	bts[4].SetCentre(4.5*u, 2.8*u);
	bts[5].SetCentre(5*u, 1.2*u);
	bts[6].SetCentre(6.5*u, u);
	bts[7].SetCentre(6*u, 3*u);
	bts[8].SetCentre(6.25*u, 2*u);
	bts[9].SetCentre(7.5*u, 2.8*u);
	bts[10].SetCentre(8*u, 1.2*u);

	gp.AddRoundedRectangle(u*1.5, u*0.5, u, u*3, u*0.25);
	gp.AddRoundedRectangle(u*0.5, u*1.5, u*3, u, u*0.25);

    gc->SetBrush(wxNullBrush);
    gc->SetPen(wxColour(C1));
    gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C1);
    gc->DrawRoundedRectangle(area.m_x, area.m_y, area.m_width, area.m_height, _DEFAULT_R_);
    gc->DrawText(wxString(" Buttons "), area.m_x + _DEFAULT_R_, area.m_y - _FONT_SIZE_*0.85, gc->CreateBrush(B1));
	gc->PushState();
	gc->Translate(area.m_x + (area.m_width - u*9)*0.5, area.m_y);
	gc->SetPen(wxPen(C1, 2));
	gc->SetBrush(wxBrush(B1));
	gc->DrawPath(gp);
	gc->FillPath(gp, wxWINDING_RULE);

	for(int i = 0; i < 11; i++)
	{
		bts[i].style = key_status[i] ? (bts[i].style - (bts[i].style & flags_off)) | flags_on :
				(bts[i].style - (bts[i].style & flags_on)) | flags_off;
		bts[i].Paint(gc);
	}

	gc->PopState();
}

void Monitor::DrawMonitorBar(wxGraphicsContext *gc, const Coordinate& c, const wxRect2DDouble& area, bool light_theme)
{
	wxColour C1, C2, C3, B1;
	C1 = light_theme? D1 : L1;
	C2 = light_theme? D2 : L2;
	C3 = light_theme? D3 : L3;
	B1 = light_theme? L1 : D1;
	wxDouble lt_dim, bar_w, bar_h, x, y, i1, i2, dim;
	lt_dim = area.m_height/3;
	bar_w = (area.m_width - lt_dim)*0.5;
	bar_h = lt_dim*0.5;
	wxGraphicsBrush left, right;
	wxString str;
	wxColour tr;

	tr.Set(0, 0, 0, 0);
	left = gc->CreateLinearGradientBrush(lt_dim, 0, lt_dim + bar_w, 0, C3, C1);
	right = gc->CreateLinearGradientBrush(lt_dim + bar_w, 0, lt_dim + 2*bar_w, 0, C1, C3);

	gc->PushState();
	gc->Translate(area.m_x, area.m_y);
	gc->PushState();
	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C1);
	gc->SetPen(wxNullPen);
	for(int i = 0; i < 3; i++)
	{
		dim = (i == 0)? c.x : ((i == 1)? c.y : c.z);
		str.Printf((i == 0)? "X:" : ((i == 1)? "Y:" : "Z:"));
		gc->GetTextExtent(str, &x, &y, &i1, &i2);
		gc->DrawText(str, (lt_dim - x)*0.5, (lt_dim - y)*0.5);
		gc->SetBrush((c.x >= 0)? right : left);
		gc->DrawRectangle(lt_dim + bar_w, bar_h*0.5, dim*bar_w, bar_h);
		gc->Translate(0, lt_dim);
	}
	gc->PopState();
	gc->SetPen(wxPen(C3));
	gc->StrokeLine(lt_dim + bar_w, 0, lt_dim + bar_w, area.m_height);
	gc->PopState();
}


void Monitor::DrawAccValues(wxGraphicsContext * gc, wxRect2DDouble const& area, bool light_theme) {
	wxColour C1;
	C1 = light_theme? D1 : L1;
	wxDouble col_w, row_w, h, w, i1, i2, X, Y;
	int i, j;
	wxString str;
	col_w = area.m_width/4;
	row_w = area.m_height/4;

	gc->PushState();
	gc->Translate(area.m_x, area.m_y);

	for(i = 0, Y = area.m_height/8; i < 4; i++, Y += row_w)
		for (j = 0, X = area.m_width/8; j < 4; j++, X += col_w) {
			switch (j) {
				case 0:
					gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(), C1);
					str = wxString(i == 1 ? "                    X:" :
							i == 2 ? "                    Y:" : i == 3 ? "                    Z:" : "");
					break;

				case 1:
					if (i == 0) {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(), C1);
						str = wxString(wxT("Acceleration (m/s²)"));
					} else {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C1);
						str.Printf(wxT("%.1f"), (i == 1 ? acc_val.x : i == 2 ? acc_val.y : acc_val.z)*0.1);
					}
					break;

				case 2:
					if (i == 0) {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(), C1);
						str = wxString("Relative");
					} else {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C1);
						str.Printf(wxT("%.1f"), (i == 1 ? acc.x : i == 2 ? acc.y : acc.z)*0.1);
					}
					break;

				case 3:
					if (i == 0) {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(), C1);
						str = wxString("Normalized");
					} else {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C1);
						str.Printf(wxT("%f"), (i == 1 ? abs_acc.x : i == 2 ? abs_acc.y : abs_acc.z));
					}
					break;

				default:
					break;
			}
			gc->GetTextExtent(str, &w, &h, &i1, &i2);
			gc->DrawText(str, X - w*0.5, Y - h*0.5);
		}
}

void Monitor::DrawMPValues(wxGraphicsContext *gc, wxRect2DDouble const &area, bool light_theme) {
	wxColour C1;
	C1 = light_theme? D1 : L1;
	wxDouble col_w, row_w, h, w, i1, i2, X, Y;
	int i, j;
	wxString str;
	col_w = area.m_width/4;
	row_w = area.m_height/4;

	gc->PushState();
	gc->Translate(area.m_x, area.m_y);

	for(i = 0, Y = area.m_height/8; i < 4; i++, Y += row_w)
		for (j = 0, X = area.m_width/8; j < 4; j++, X += col_w) {
			switch (j) {
				case 0:
					gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(), C1);
					str = wxString(i == 1 ? "                    X:" :
							i == 2 ? "                    Y:" : i == 3 ? "                    Z:" : "");
					break;

				case 1:
					if (i == 0) {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(), C1);
						str = wxString("Speed (rad/s)");
					} else {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C1);
						str.Printf(wxT("%d"), (int) (i == 1 ? mp_val.x : i == 2 ? mp_val.y : mp_val.z));
					}
					break;

				case 2:
					if (i == 0) {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(), C1);
						str = wxString("Normalized");
					} else {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C1);
						str.Printf(wxT("%f"), (i == 1 ? mp.x : j == 2 ? mp.y : mp.z));
					}
					break;

				case 3:
					if (i == 0) {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(), C1);
						str = wxString("Angle (rad)");
					} else {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C1);
						str.Printf(wxT("%f \uE0C6"), (i == 1 ? abs_mp.x : i == 2 ? abs_mp.y : abs_mp.z)/M_PI);
					}
					break;

				default:
					break;
			}
			gc->GetTextExtent(str, &w, &h, &i1, &i2);
			gc->DrawText(str, X - w*0.5, Y - h*0.5);
		}
}

void Monitor::DrawBtsValues(wxGraphicsContext *gc, wxRect2DDouble const &area, bool light_theme) {
	wxColour C1, C2;
	C1 = light_theme? D1 : L1;
	C2 = light_theme? D3 : L3;
	wxDouble col_w, row_w, h, w, i1, i2, X, Y;
	int i, j;
	wxString str;
	col_w = area.m_width/3;
	row_w = area.m_height/12;

	gc->PushState();
	gc->Translate(area.m_x, area.m_y);

	for(i = 0, Y = area.m_height/24; i < 12; i++, Y += row_w) {
		for (j = 0, X = area.m_width/8; j < 3; j++, X += col_w) {
			switch (j) {
				case 0:
					gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(), C1);
					switch(i) {
						case 0:
							str = wxString("");
							break;

						case 1:
							str = wxString("Left");
							break;

						case 2:
							str = wxString("Right");
							break;

						case 3:
							str = wxString("Up");
							break;

						case 4:
							str = wxString("Down");
							break;

						case 5:
							str = wxString("A");
							break;

						case 6:
							str = wxString("B");
							break;

						case 7:
							str = wxString("Plus");
							break;

						case 8:
							str = wxString("Minus");
							break;

						case 9:
							str = wxString("Home");
							break;

						case 10:
							str = wxString("One");
							break;

						case 11:
							str = wxString("Two");
							break;

						default:
							break;
					}
					break;

				case 1:
					if (i == 0) {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(), C1);
						str = wxString("Status");
					} else if(GetKey((unsigned int) i - 1) > 0)  {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C2);
						str = wxString("PRESSED");
					} else {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C1);
						str = wxString("Idle");
					}
					break;

				case 2:
					if (i == 0) {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(), C1);
						str = wxString("Handler");
					} else {
						gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_), C1);
						str = Rack::GetHandler(i - 1) == NULL? wxString("None") : Rack::GetHandler(i - 1)->GetName();
					}
					break;

				default:
					break;
			}
			gc->GetTextExtent(str, &w, &h, &i1, &i2);
			gc->DrawText(str, X - w*0.5, Y - h*0.5);
			if(j == 0)
				X -= col_w*0.5;
		}

		if(i == 0)
			Y += row_w*0.5;
	}
}

wxDouble Monitor::NormalizeAcc(const wxDouble& param)
{	
	return 2.0*atan(param)/M_PI;
}

wxDouble Monitor::NormalizeAbsAcc(const wxDouble& param)
{
	return 2.0*atan(param*_NORM_FACTOR_)/M_PI;
}

wxDouble Monitor::NormalizeMP(const wxDouble& param)
{
	return 2.0*atan(param*_NORM_FACTOR_*_NORM_FACTOR_)/M_PI;
}

wxDouble Monitor::NormalizeAbsMP(const wxDouble& param)
{
	return param*_NORM_FACTOR_*_NORM_FACTOR_*0.05;
}

void Monitor::UpdateAcceleration(const wxDouble& x, const wxDouble& y, const wxDouble& z)
{
	acc_val = Coordinate(x, y, z);
	abs_acc = Coordinate(NormalizeAbsAcc(x), NormalizeAbsAcc(y), NormalizeAbsAcc(z));
	if(!init)
		acc = Coordinate(NormalizeAcc(acc_val.x - last_acc.x), 
			NormalizeAcc(acc_val.y - last_acc.y), NormalizeAcc(acc_val.z - last_acc.z));
	else
		init = false;
	last_acc = acc_val; 
}

void Monitor::UpdateMotionPlus(const wxDouble& x, const wxDouble& y, const wxDouble& z)
{
	mp_val = Coordinate(x, y, z);
	mp = Coordinate(NormalizeMP(x), NormalizeMP(y), NormalizeMP(z));
}

void Monitor::UpdateAbsMP(const wxDouble& x, const wxDouble& y, const wxDouble& z)
{
	int k;
	abs_mp += Coordinate(NormalizeAbsMP(x), NormalizeAbsMP(y), NormalizeAbsMP(z));
	k = (int) (abs_mp.x/(2*M_PI));
	abs_mp.x -= 2*k*M_PI;
	k = (int) (abs_mp.y/(2*M_PI));
	abs_mp.y -= 2*k*M_PI;
	k = (int) (abs_mp.z/(2*M_PI));
	abs_mp.z -= 2*k*M_PI;
}

void Monitor::UpdateKeyStatus(const unsigned int& code, const unsigned int& state)
{
	if ((code >= 0) && (code < 11))
		key_status[code] = state;
}

unsigned int Monitor::GetKey(const unsigned int& code)
{
	return key_status[code];
}

void Monitor::Paint(wxGraphicsContext *gc)
{
	wxDouble w, h, i1, i2, gap = area.m_height*0.1;
	gc->SetFont(wxFontInfo(_FONT_SIZE_).FaceName(_FONT_NAME_).Bold(true), D1);
	gc->GetTextExtent(wxString(" Monitors "), &w, &h, &i1, &i2);
	gc->SetBrush(wxBrush(L1));
	gc->SetPen(wxPen(L1));
	gc->DrawRectangle(area.m_x, area.m_y, area.m_width, area.m_height);
	gc->DrawText(wxString(" Monitors "), 3 + _DEFAULT_R_, area.m_y + 3, gc->CreateBrush(wxBrush(L1)));
	gc->SetPen(wxPen(wxColour(D1)));
	gc->StrokeLine(w + 6 + _DEFAULT_R_, area.m_y + 3 + h*0.5, area.m_width - 10, area.m_y + 3 + h*0.5);
	bt_expand->Paint(gc);
	wxRect2DDouble rect = wxRect2DDouble(area.m_x, area.m_y + h - 2, area.m_width, area.m_height - h + 2);
	rect.m_width = rect.m_width/3 - 2*gap;
    rect.m_height -= gap*2;
    rect.m_x += gap;
    rect.m_y += gap;
    DrawAccMonitor(gc, rect);
    rect.m_x += area.m_width/3;
    DrawMPMonitor(gc, rect);
    rect.m_x += area.m_width/3;
    DrawButtonsMonitor(gc, rect);
}

bool Monitor::Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p) {
	if(area.Contains(p)) {
		bt_expand->SetLabel(wxString(wxT("➕")));
		bt_expand->Contains(p, gc);
		return true;
	}
	bt_expand->SetLabel(wxString(wxT("∘")));
	return false;
}

void Monitor::OnCTXChange(Monitor *cm) {
	if(cm == NULL)
		return;

	switch(cm->GetCTX()->GetLastSelected()) {
		case 0:
			if(mon != NULL)
				mon->Show();
			break;

		case 1:
			if(ctrl != NULL)
				ctrl->Show();
			break;

		default:
			break;
	}
}