//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    Monitor.h 											//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __MONITOR_H__
#define __MONITOR_H__

//Key Codes
#define KEY_LEFT		0
#define KEY_RIGHT		1
#define KEY_UP			2
#define KEY_DOWN		3
#define KEY_A 			4
#define KEY_B			5
#define KEY_PLUS		6
#define KEY_MINUS		7
#define KEY_HOME		8
#define KEY_1			9
#define KEY_2			10
#define KEY_ACC_X		11
#define KEY_ACC_Y		12
#define KEY_ACC_Z		13
#define KEY_MP_X		14
#define KEY_MP_Y		15
#define KEY_MP_Z		16

//Ellipse Codes
#define _ELLIPSE_PEN_W_		5
#define _AXES_PEN_W_		2
#define NORTH_SIDE			1
#define SOUTH_SIDE			2

#include "WiiMIDI_GUI.h"
#include "WindowTypes.h"

class Coordinate
{
	public:
		wxDouble x, y, z;
		Coordinate();
		Coordinate(const wxDouble& px, const wxDouble& py, const wxDouble& pz);
		Coordinate& operator+=(const wxDouble& d);
		Coordinate& operator+=(const Coordinate& c);
		Coordinate& operator-=(const wxDouble& d);
		Coordinate& operator-=(const Coordinate& c);
		Coordinate& operator*=(const wxDouble& d);
		Coordinate& operator*=(const Coordinate& c);
		Coordinate& operator=(const Coordinate& c);
		Coordinate operator+(const Coordinate& r);
		Coordinate operator-(const Coordinate& r);
		Coordinate operator*(const wxDouble& r);
		Coordinate operator/(const wxDouble& r);
};

class Monitor
{
	private:
		static Coordinate acc, mp, abs_acc, last_acc, abs_mp, acc_val, mp_val;
		static bool init;
		static unsigned int key_status[11];
		Button *bt_expand;
		wxRect2DDouble area;
		ContextMenu *r_click;
		static Window *mon, *ctrl;
		static void DrawEllipseThroughPoint(wxGraphicsContext *gc, const wxDouble& x, const wxDouble& y,
				const wxDouble& a, const wxDouble& angle=0, const unsigned int side=0);
		static wxDouble NormalizeAcc(const wxDouble& param);
		static wxDouble NormalizeAbsAcc(const wxDouble& param);
		static wxDouble NormalizeMP(const wxDouble& param);
		static wxDouble NormalizeAbsMP(const wxDouble& param);
		static Coordinate MapToSphere(const Coordinate& c);

	public:
		Monitor(const wxRect2DDouble& a);
		~Monitor();
		ContextMenu* GetCTX() { return r_click; }
		static void SetMonitorWindow(Window* m) { mon = m; }
		static void SetCtrlWindow(Window* c) { ctrl = c; }
		void Paint(wxGraphicsContext *gc);
		static void OnCTXChange(Monitor* cm);
		static void UpdateAcceleration(const wxDouble& x, const wxDouble& y, const wxDouble& z);
		static void UpdateMotionPlus(const wxDouble& x, const wxDouble& y, const wxDouble& z);
		static void UpdateAbsMP(const wxDouble& x, const wxDouble& y, const wxDouble& z);
		static void UpdateKeyStatus(const unsigned int& code, const unsigned int& state);
		static unsigned int GetKey(const unsigned int& code);
		static Coordinate GetAcc() { return acc; }
		static Coordinate GetAbsAcc() { return abs_acc; }
		static Coordinate GetMP() { return mp; }
		static Coordinate GetAbsMP() { return abs_mp; }
		static Coordinate GetMPValues() { return mp_val; }
		static Coordinate GetAccValues() { return acc_val; }
		static void DrawButtonsMonitor(wxGraphicsContext *gc, const wxRect2DDouble& area, bool light_theme=true);
		static void DrawMonitorBar(wxGraphicsContext *gc, const Coordinate& c, const wxRect2DDouble& area, bool light_theme=true);
		static void DrawGyroscope(wxGraphicsContext *gc, const wxRect2DDouble& area, bool light_theme=true);
		static void DrawSphere(wxGraphicsContext *gc, const wxRect2DDouble& area, bool light_theme=true,
				const wxDouble& beta=33, const wxDouble& teta=13, const wxDouble& step=15);
		static void DrawMPMonitor(wxGraphicsContext *gc, wxRect2DDouble const &area, bool light_theme=true);
		static void DrawAccMonitor(wxGraphicsContext *gc, wxRect2DDouble const &area, bool light_theme=true);
		static void DrawAccValues(wxGraphicsContext *gc, wxRect2DDouble const &area, bool light_theme=true);
		static void DrawMPValues(wxGraphicsContext *gc, wxRect2DDouble const &area, bool light_theme=true);
		static void DrawBtsValues(wxGraphicsContext *gc, wxRect2DDouble const &area, bool light_theme=true);
		bool Contains(wxGraphicsContext* gc, const wxPoint2DDouble& p);
};

#endif
