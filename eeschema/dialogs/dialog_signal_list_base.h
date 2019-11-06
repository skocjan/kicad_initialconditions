///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include "dialog_shim.h"
#include <wx/string.h>
#include <wx/srchctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_SIGNAL_LIST_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_SIGNAL_LIST_BASE : public DIALOG_SHIM
{
	private:

	protected:
		wxSearchCtrl* m_searchCtrl;
		wxListBox* m_signals;
		wxStdDialogButtonSizer* m_sdbSizer;
		wxButton* m_sdbSizerOK;
		wxButton* m_sdbSizerCancel;

		// Virtual event handlers, overide them in your derived class
		virtual void onSearchCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void onKeyDownInSearchBox( wxKeyEvent& event ) { event.Skip(); }
		virtual void onSearchTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCharInSignalList( wxKeyEvent& event ) { event.Skip(); }
		virtual void onSignalAdd( wxCommandEvent& event ) { event.Skip(); }


	public:

		DIALOG_SIGNAL_LIST_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~DIALOG_SIGNAL_LIST_BASE();

};

