//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    WiimoteThread.cpp									//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../WiiMIDI.h"

WiimoteThread::WiimoteThread(Interface* handler, int num) : wxThread(wxTHREAD_DETACHED)
{
	m_pHandler = handler;
	wiimote_num = num;
}

WiimoteThread::~WiimoteThread()
{
	wxCriticalSectionLocker enter(m_pHandler->m_pThreadCS);
	m_pHandler->m_pThread = NULL;
}

int WiimoteThread::RunInterface(struct xwii_iface *iface)
{
	struct xwii_event event;
	int ret = 0, fds_num;
	struct pollfd fds[2];
	struct xwii_event *evt;
	wxCommandEvent cmd(wxEVT_WIIMOTE_EVENT);

	memset(fds, 0, sizeof(fds));
	fds[0].fd = 0;
	fds[0].events = POLLIN;
	fds[1].fd = xwii_iface_get_fd(iface);
	fds[1].events = POLLIN;
	fds_num = 2;
	bool done = false;

	ret = xwii_iface_watch(iface, true);
	if (ret)
		m_pHandler->err_window->SetError(ERR_WIIMOTE_THREAD, wxString("Error: Cannot initialize hotplug watch descriptor"));

	while (!done) {
		ret = poll(fds, fds_num, -1);
		if (ret < 0) {
			if (errno != EINTR) {
				ret = -errno;
				m_pHandler->err_window->SetError(ERR_WIIMOTE_THREAD, wxString("Error: Cannot poll fds."));
				break;
			}
		}

		ret = xwii_iface_dispatch(iface, &event, sizeof(event));
		if (ret) {
			if (ret != -EAGAIN) {
				m_pHandler->err_window->SetError(ERR_WIIMOTE_THREAD, wxString("Wiimote: Read failed."));
				break;
			}
		} else {
			switch (event.type) 
			{
				case XWII_EVENT_GONE:
					fds[1].fd = -1;
					fds[1].events = 0;
					fds_num = 1;
					ret = 0;
					done = true;
					m_pHandler->err_window->SetError(ERR_WIIMOTE_GONE, wxString("Wiimote disconnected."));
				break;

				case XWII_EVENT_KEY:
					evt = (struct xwii_event*) malloc(sizeof(struct xwii_event));
					memcpy(evt, &event, sizeof(struct xwii_event));
					cmd.SetClientData(evt);
					wxPostEvent(m_pHandler, cmd);
				break;

				case XWII_EVENT_ACCEL:
					evt = (struct xwii_event*) malloc(sizeof(struct xwii_event));
					memcpy(evt, &event, sizeof(struct xwii_event));
					cmd.SetClientData(evt);
					wxPostEvent(m_pHandler, cmd);
				break;

				case XWII_EVENT_MOTION_PLUS:
					evt = (struct xwii_event*) malloc(sizeof(struct xwii_event));
					memcpy(evt, &event, sizeof(struct xwii_event));
					cmd.SetClientData(evt);
					wxPostEvent(m_pHandler, cmd);
				break;

				default:
					//IGNORE
				break;
			}
		}

		if(TestDestroy()) {
			ret = 0;
			done = true;
		}
	}

	return ret;
}

char* WiimoteThread::GetDev(int num)
{
	struct xwii_monitor *mon;
	char *ent;
	int i = 0;

	mon = xwii_monitor_new(false, false);
	if (!mon) {
		m_pHandler->err_window->SetError(ERR_WIIMOTE_THREAD, wxString("Cannot create monitor"));
		return NULL;
	}

	while ((ent = xwii_monitor_poll(mon))) {
		if (++i == num)
			break;
		free(ent);
	}

	xwii_monitor_unref(mon);

	if (!ent)
		m_pHandler->err_window->SetError(ERR_WIIMOTE_THREAD, wxString("Cannot find device."));

	return ent;
}

wxThread::ExitCode WiimoteThread::Entry()
{
	int ret = 0;
	char *path = NULL;

	path = GetDev(wiimote_num);
	ret = xwii_iface_new(&iface, path);
	free(path);

	if (ret)
		m_pHandler->err_window->SetError(ERR_WIIMOTE_THREAD, wxString("Cannot create interface."));
	else {
		ret = xwii_iface_open(iface, xwii_iface_available(iface) | XWII_IFACE_WRITABLE);
		if (ret)
			m_pHandler->err_window->SetError(ERR_WIIMOTE_THREAD, wxString("Cannot open interface."));
		else {
			ret = RunInterface(iface);
			xwii_iface_unref(iface);
			if (ret) 
				m_pHandler->err_window->SetError(ERR_WIIMOTE_THREAD, wxString("Program failed."));
		}
	}

	wxQueueEvent(m_pHandler, new wxCommandEvent(wxEVT_WIIMOTE_CLOSE));

	return (wxThread::ExitCode) 0;
}
