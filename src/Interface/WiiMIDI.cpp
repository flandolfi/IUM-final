//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    WiiMIDI.cpp										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WiiMIDI.h"

const wxColour L1(_LIGHT_CL1_);
const wxColour L2(_LIGHT_CL2_);
const wxColour L3(_LIGHT_CL3_);
const wxColour D1(_DARK_CL1_);
const wxColour D2(_DARK_CL2_);
const wxColour D3(_DARK_CL3_);

DECLARE_APP(WiiMIDI)
IMPLEMENT_APP(WiiMIDI)

bool WiiMIDI::OnInit()
{
	uid_t uid = getuid(), euid = geteuid();

	if(uid != 0 || uid != euid) {
		ExitMainLoop();
		wxExecute("gksudo -- ./WiiMIDI", wxEXEC_SYNC | wxEXEC_SHOW_CONSOLE | wxEXEC_BLOCK);
		return false;
	}

	Interface* interf = new Interface(wxT("WiiMIDI"));
	interf->Show(true);
	
	return true;
}