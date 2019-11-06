///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "dialog_signal_list_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_SIGNAL_LIST_BASE::DIALOG_SIGNAL_LIST_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_searchCtrl = new wxSearchCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT );
	#ifndef __WXMAC__
	m_searchCtrl->ShowSearchButton( false );
	#endif
	m_searchCtrl->ShowCancelButton( true );
	bSizer6->Add( m_searchCtrl, 0, wxALL|wxEXPAND, 5 );

	m_signals = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_NEEDED_SB|wxLB_SORT );
	m_signals->SetMinSize( wxSize( 450,400 ) );

	bSizer6->Add( m_signals, 1, wxALL|wxEXPAND, 5 );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();

	bSizer6->Add( m_sdbSizer, 0, wxEXPAND|wxALL, 5 );


	this->SetSizer( bSizer6 );
	this->Layout();
	bSizer6->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_searchCtrl->Connect( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler( DIALOG_SIGNAL_LIST_BASE::onSearchCancel ), NULL, this );
	m_searchCtrl->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( DIALOG_SIGNAL_LIST_BASE::onKeyDownInSearchBox ), NULL, this );
	m_searchCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DIALOG_SIGNAL_LIST_BASE::onSearchTextEnter ), NULL, this );
	m_signals->Connect( wxEVT_CHAR, wxKeyEventHandler( DIALOG_SIGNAL_LIST_BASE::onCharInSignalList ), NULL, this );
	m_signals->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( DIALOG_SIGNAL_LIST_BASE::onSignalAdd ), NULL, this );
}

DIALOG_SIGNAL_LIST_BASE::~DIALOG_SIGNAL_LIST_BASE()
{
	// Disconnect Events
	m_searchCtrl->Disconnect( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler( DIALOG_SIGNAL_LIST_BASE::onSearchCancel ), NULL, this );
	m_searchCtrl->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( DIALOG_SIGNAL_LIST_BASE::onKeyDownInSearchBox ), NULL, this );
	m_searchCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( DIALOG_SIGNAL_LIST_BASE::onSearchTextEnter ), NULL, this );
	m_signals->Disconnect( wxEVT_CHAR, wxKeyEventHandler( DIALOG_SIGNAL_LIST_BASE::onCharInSignalList ), NULL, this );
	m_signals->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( DIALOG_SIGNAL_LIST_BASE::onSignalAdd ), NULL, this );

}
