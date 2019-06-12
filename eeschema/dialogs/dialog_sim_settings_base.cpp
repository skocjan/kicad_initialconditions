///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr  6 2019)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "dialog_sim_settings_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_SIM_SETTINGS_BASE::DIALOG_SIM_SETTINGS_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_simPages = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_simPages->SetMinSize( wxSize( 650,-1 ) );

	m_pgAC = new wxPanel( m_simPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );


	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );

	wxString m_acScaleChoices[] = { _("Decade"), _("Octave"), _("Linear") };
	int m_acScaleNChoices = sizeof( m_acScaleChoices ) / sizeof( wxString );
	m_acScale = new wxRadioBox( m_pgAC, wxID_ANY, _("Frequency scale"), wxDefaultPosition, wxDefaultSize, m_acScaleNChoices, m_acScaleChoices, 1, wxRA_SPECIFY_COLS );
	m_acScale->SetSelection( 0 );
	m_acScale->Hide();

	bSizer3->Add( m_acScale, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );


	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( m_pgAC, wxID_ANY, _("Number of points:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_acPointsNumber = new wxTextCtrl( m_pgAC, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_acPointsNumber, 0, wxALL|wxEXPAND, 5 );


	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( m_pgAC, wxID_ANY, _("Start frequency:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_acFreqStart = new wxTextCtrl( m_pgAC, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_acFreqStart, 0, wxALL|wxEXPAND, 5 );

	m_staticText19 = new wxStaticText( m_pgAC, wxID_ANY, _("Hertz"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	fgSizer1->Add( m_staticText19, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );

	m_staticText3 = new wxStaticText( m_pgAC, wxID_ANY, _("Stop frequency:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_acFreqStop = new wxTextCtrl( m_pgAC, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_acFreqStop, 0, wxALL|wxEXPAND, 5 );

	m_staticText110 = new wxStaticText( m_pgAC, wxID_ANY, _("Hertz"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText110->Wrap( -1 );
	fgSizer1->Add( m_staticText110, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );


	bSizer3->Add( fgSizer1, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_skipOpAc = new wxCheckBox( m_pgAC, wxID_ANY, _("Do not run an operating point (OP) sim. at start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_skipOpAc->SetToolTip( _("Option has no effect if non-linear device is present in circuit") );

	bSizer3->Add( m_skipOpAc, 0, wxALIGN_CENTER_HORIZONTAL, 5 );


	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );


	m_pgAC->SetSizer( bSizer3 );
	m_pgAC->Layout();
	bSizer3->Fit( m_pgAC );
	m_simPages->AddPage( m_pgAC, _("AC"), false );
	m_pgDC = new wxPanel( m_simPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer21;
	sbSizer21 = new wxStaticBoxSizer( new wxStaticBox( m_pgDC, wxID_ANY, _("DC sweep source 1:") ), wxVERTICAL );

	m_dcEnable1 = new wxCheckBox( sbSizer21->GetStaticBox(), wxID_ANY, _("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer21->Add( m_dcEnable1, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText41 = new wxStaticText( sbSizer21->GetStaticBox(), wxID_ANY, _("DC source:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText41->Wrap( -1 );
	fgSizer21->Add( m_staticText41, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_dcSource1 = new wxComboBox( sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	fgSizer21->Add( m_dcSource1, 0, wxALL, 5 );


	fgSizer21->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText51 = new wxStaticText( sbSizer21->GetStaticBox(), wxID_ANY, _("Starting voltage:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	fgSizer21->Add( m_staticText51, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_dcStart1 = new wxTextCtrl( sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer21->Add( m_dcStart1, 0, wxALL, 5 );

	m_staticText511 = new wxStaticText( sbSizer21->GetStaticBox(), wxID_ANY, _("Volts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText511->Wrap( -1 );
	fgSizer21->Add( m_staticText511, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );

	m_staticText61 = new wxStaticText( sbSizer21->GetStaticBox(), wxID_ANY, _("Final voltage:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText61->Wrap( -1 );
	fgSizer21->Add( m_staticText61, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_dcStop1 = new wxTextCtrl( sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer21->Add( m_dcStop1, 0, wxALL, 5 );

	m_staticText512 = new wxStaticText( sbSizer21->GetStaticBox(), wxID_ANY, _("Volts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText512->Wrap( -1 );
	fgSizer21->Add( m_staticText512, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );

	m_staticText71 = new wxStaticText( sbSizer21->GetStaticBox(), wxID_ANY, _("Increment step:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText71->Wrap( -1 );
	fgSizer21->Add( m_staticText71, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_dcIncr1 = new wxTextCtrl( sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer21->Add( m_dcIncr1, 0, wxALL, 5 );

	m_staticText513 = new wxStaticText( sbSizer21->GetStaticBox(), wxID_ANY, _("Volts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText513->Wrap( -1 );
	fgSizer21->Add( m_staticText513, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );


	sbSizer21->Add( fgSizer21, 1, wxALIGN_CENTER_HORIZONTAL, 5 );


	bSizer4->Add( sbSizer21, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_pgDC, wxID_ANY, _("DC sweep source 2:") ), wxVERTICAL );

	m_dcEnable2 = new wxCheckBox( sbSizer2->GetStaticBox(), wxID_ANY, _("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_dcEnable2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText4 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("DC source:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer2->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_dcSource2 = new wxComboBox( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	fgSizer2->Add( m_dcSource2, 0, wxALL, 5 );


	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText5 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("Starting voltage:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer2->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_dcStart2 = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_dcStart2, 0, wxALL, 5 );

	m_staticText52 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("Volts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText52->Wrap( -1 );
	fgSizer2->Add( m_staticText52, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );

	m_staticText6 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("Final voltage:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer2->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_dcStop2 = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_dcStop2, 0, wxALL, 5 );

	m_staticText53 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("Volts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText53->Wrap( -1 );
	fgSizer2->Add( m_staticText53, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );

	m_staticText7 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("Increment step:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer2->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_dcIncr2 = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_dcIncr2, 0, wxALL, 5 );

	m_staticText54 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, _("Volts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText54->Wrap( -1 );
	fgSizer2->Add( m_staticText54, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );


	sbSizer2->Add( fgSizer2, 1, wxALIGN_CENTER_HORIZONTAL, 5 );


	bSizer4->Add( sbSizer2, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_pgDC, wxID_ANY, _("Tolerances") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer125;
	fgSizer125 = new wxFlexGridSizer( 4, 3, 0, 0 );
	fgSizer125->SetFlexibleDirection( wxBOTH );
	fgSizer125->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText32 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, _("Abs. current error tolerance:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32->Wrap( -1 );
	fgSizer125->Add( m_staticText32, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_absTol = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, _("1p"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer125->Add( m_absTol, 0, wxALL, 5 );

	m_staticText1101 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, _("Ampers"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1101->Wrap( -1 );
	fgSizer125->Add( m_staticText1101, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText321 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, _("Rel. tolerance:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText321->Wrap( -1 );
	fgSizer125->Add( m_staticText321, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_relTol = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, _("0.001"), wxDefaultPosition, wxDefaultSize, 0 );
	m_relTol->SetToolTip( _("0.001 = 0.1%") );

	fgSizer125->Add( m_relTol, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText11011 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, _("-"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11011->Wrap( -1 );
	m_staticText11011->SetToolTip( _("(0.001 = 0.1%)") );

	fgSizer125->Add( m_staticText11011, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText3211 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, _("Voltage error tolerance:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3211->Wrap( -1 );
	fgSizer125->Add( m_staticText3211, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_vnTol = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, _("1u"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer125->Add( m_vnTol, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText110111 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, _("Volts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText110111->Wrap( -1 );
	fgSizer125->Add( m_staticText110111, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_rShuntOn = new wxCheckBox( sbSizer3->GetStaticBox(), wxID_ANY, _("Mitigate computational issues"), wxDefaultPosition, wxDefaultSize, 0 );
	m_rShuntOn->SetToolTip( _("Check this option when you experience troubles solving circuit with XSPICE ‘A’ models ") );

	fgSizer125->Add( m_rShuntOn, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_rShunt = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, _("1.0e12"), wxDefaultPosition, wxDefaultSize, 0 );
	m_rShunt->SetToolTip( _("Introduces a resistor of a given value from each analog node to ground. The value of the resistor should be high enough to not interfere with circuit operations") );

	fgSizer125->Add( m_rShunt, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText1101111 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, _("Ohm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1101111->Wrap( -1 );
	fgSizer125->Add( m_staticText1101111, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	sbSizer3->Add( fgSizer125, 1, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticText11011111 = new wxStaticText( sbSizer3->GetStaticBox(), wxID_ANY, _("Note: These options will affect OP analysis as well, which is executed prior to AC and TRAN analysis"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11011111->Wrap( -1 );
	sbSizer3->Add( m_staticText11011111, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	bSizer4->Add( sbSizer3, 0, wxEXPAND, 5 );


	m_pgDC->SetSizer( bSizer4 );
	m_pgDC->Layout();
	bSizer4->Fit( m_pgDC );
	m_simPages->AddPage( m_pgDC, _("DC Transfer"), true );
	m_pgDistortion = new wxPanel( m_simPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_pgDistortion->Hide();

	m_simPages->AddPage( m_pgDistortion, _("Distortion"), false );
	m_pgNoise = new wxPanel( m_simPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_pgNoise->Hide();

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );


	bSizer15->Add( 0, 0, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText14 = new wxStaticText( m_pgNoise, wxID_ANY, _("Measured node"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer7->Add( m_staticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_noiseMeas = new wxComboBox( m_pgNoise, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	fgSizer7->Add( m_noiseMeas, 0, wxALL, 5 );


	fgSizer7->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText15 = new wxStaticText( m_pgNoise, wxID_ANY, _("Reference node"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer7->Add( m_staticText15, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_noiseRef = new wxComboBox( m_pgNoise, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	fgSizer7->Add( m_noiseRef, 0, wxALL, 5 );

	m_staticText23 = new wxStaticText( m_pgNoise, wxID_ANY, _("(optional; default GND)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer7->Add( m_staticText23, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText16 = new wxStaticText( m_pgNoise, wxID_ANY, _("Noise source"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer7->Add( m_staticText16, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_noiseSrc = new wxComboBox( m_pgNoise, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	fgSizer7->Add( m_noiseSrc, 0, wxALL, 5 );


	fgSizer7->Add( 0, 0, 1, wxEXPAND, 5 );


	bSizer15->Add( fgSizer7, 0, wxALIGN_CENTER_HORIZONTAL, 5 );


	bSizer15->Add( 0, 0, 1, wxEXPAND, 5 );

	wxString m_noiseScaleChoices[] = { _("Decade"), _("Octave"), _("Linear") };
	int m_noiseScaleNChoices = sizeof( m_noiseScaleChoices ) / sizeof( wxString );
	m_noiseScale = new wxRadioBox( m_pgNoise, wxID_ANY, _("Frequency scale"), wxDefaultPosition, wxDefaultSize, m_noiseScaleNChoices, m_noiseScaleChoices, 1, wxRA_SPECIFY_COLS );
	m_noiseScale->SetSelection( 0 );
	bSizer15->Add( m_noiseScale, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );


	bSizer15->Add( 0, 0, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText11 = new wxStaticText( m_pgNoise, wxID_ANY, _("Number of points"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer11->Add( m_staticText11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_noisePointsNumber = new wxTextCtrl( m_pgNoise, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_noisePointsNumber, 0, wxALL, 5 );

	m_staticText21 = new wxStaticText( m_pgNoise, wxID_ANY, _("Start frequency [Hz]"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	fgSizer11->Add( m_staticText21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_noiseFreqStart = new wxTextCtrl( m_pgNoise, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_noiseFreqStart, 0, wxALL, 5 );

	m_staticText31 = new wxStaticText( m_pgNoise, wxID_ANY, _("Stop frequency [Hz]"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	fgSizer11->Add( m_staticText31, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_noiseFreqStop = new wxTextCtrl( m_pgNoise, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_noiseFreqStop, 0, wxALL, 5 );


	bSizer15->Add( fgSizer11, 0, wxALIGN_CENTER_HORIZONTAL, 5 );


	bSizer15->Add( 0, 0, 1, wxEXPAND, 5 );


	m_pgNoise->SetSizer( bSizer15 );
	m_pgNoise->Layout();
	bSizer15->Fit( m_pgNoise );
	m_simPages->AddPage( m_pgNoise, _("Noise"), false );
	m_pgOP = new wxPanel( m_simPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_pgOP->Hide();

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );


	bSizer8->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText13 = new wxStaticText( m_pgOP, wxID_ANY, _("This tab has no settings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	bSizer8->Add( m_staticText13, 0, wxALIGN_CENTER, 5 );


	bSizer8->Add( 0, 0, 1, wxEXPAND, 5 );


	m_pgOP->SetSizer( bSizer8 );
	m_pgOP->Layout();
	bSizer8->Fit( m_pgOP );
	m_simPages->AddPage( m_pgOP, _("Operating Point"), false );
	m_pgPoleZero = new wxPanel( m_simPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_pgPoleZero->Hide();

	m_simPages->AddPage( m_pgPoleZero, _("Pole-Zero"), false );
	m_pgSensitivity = new wxPanel( m_simPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_pgSensitivity->Hide();

	m_simPages->AddPage( m_pgSensitivity, _("Sensitivity"), false );
	m_pgTransferFunction = new wxPanel( m_simPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_pgTransferFunction->Hide();

	m_simPages->AddPage( m_pgTransferFunction, _("Transfer Function"), false );
	m_pgTransient = new wxPanel( m_simPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );


	bSizer81->Add( 0, 0, 1, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_pgTransient, wxID_ANY, _("Simulation") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText151 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("Time step:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText151->Wrap( -1 );
	fgSizer6->Add( m_staticText151, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_transStep = new wxTextCtrl( sbSizer4->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_transStep, 0, wxALL, 5 );

	m_staticText1511 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("seconds"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1511->Wrap( -1 );
	fgSizer6->Add( m_staticText1511, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );

	m_staticText161 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("Final time:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText161->Wrap( -1 );
	fgSizer6->Add( m_staticText161, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_transFinal = new wxTextCtrl( sbSizer4->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_transFinal, 0, wxALL, 5 );

	m_staticText1512 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("seconds"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1512->Wrap( -1 );
	fgSizer6->Add( m_staticText1512, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );

	m_staticText17 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("Initial time:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	fgSizer6->Add( m_staticText17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_transInitial = new wxTextCtrl( sbSizer4->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_transInitial, 0, wxALL, 5 );

	m_staticText1513 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("seconds"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1513->Wrap( -1 );
	fgSizer6->Add( m_staticText1513, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxRIGHT, 5 );

	m_staticText241 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("Method:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText241->Wrap( -1 );
	fgSizer6->Add( m_staticText241, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxString m_intgMethodChoices[] = { _("trapezoidal"), _("Gear") };
	int m_intgMethodNChoices = sizeof( m_intgMethodChoices ) / sizeof( wxString );
	m_intgMethod = new wxChoice( sbSizer4->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_intgMethodNChoices, m_intgMethodChoices, 0 );
	m_intgMethod->SetSelection( 1 );
	m_intgMethod->SetToolTip( _("Trapezoidal – less precise but faster. Gear – more accurate and slower") );

	fgSizer6->Add( m_intgMethod, 0, wxALL, 5 );


	fgSizer6->Add( 0, 0, 1, wxEXPAND, 5 );


	sbSizer4->Add( fgSizer6, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_UIC = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, _("Use initial conditions"), wxDefaultPosition, wxDefaultSize, 0 );
	m_UIC->SetToolTip( _("When unchecked, OP analysis results are taken as starting values for simulation. When checked, IC values are taken into account.") );

	sbSizer4->Add( m_UIC, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );


	bSizer81->Add( sbSizer4, 0, wxALIGN_CENTER, 5 );

	wxStaticBoxSizer* sbSizer41;
	sbSizer41 = new wxStaticBoxSizer( new wxStaticBox( m_pgTransient, wxID_ANY, _("Tolerances") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer61;
	fgSizer61 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer61->SetFlexibleDirection( wxBOTH );
	fgSizer61->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText15142 = new wxStaticText( sbSizer41->GetStaticBox(), wxID_ANY, _("Charge tolerance:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15142->Wrap( -1 );
	fgSizer61->Add( m_staticText15142, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_chgTol = new wxTextCtrl( sbSizer41->GetStaticBox(), wxID_ANY, _("0.01p"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer61->Add( m_chgTol, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText151111 = new wxStaticText( sbSizer41->GetStaticBox(), wxID_ANY, _("Coulombs"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText151111->Wrap( -1 );
	fgSizer61->Add( m_staticText151111, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_trTolOn = new wxCheckBox( sbSizer41->GetStaticBox(), wxID_ANY, _("Transient tolerance:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer61->Add( m_trTolOn, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_trTol = new wxTextCtrl( sbSizer41->GetStaticBox(), wxID_ANY, _("7"), wxDefaultPosition, wxDefaultSize, 0 );
	m_trTol->SetToolTip( _("Estimate of the factor by which SPICE overestimates the actual truncation error. Default value: 7 (or 1 when XSPICE ‘A’ devices are present).") );

	fgSizer61->Add( m_trTol, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	sbSizer41->Add( fgSizer61, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	bSizer81->Add( sbSizer41, 0, wxALIGN_CENTER, 5 );


	bSizer81->Add( 0, 0, 1, wxEXPAND, 5 );


	m_pgTransient->SetSizer( bSizer81 );
	m_pgTransient->Layout();
	bSizer81->Fit( m_pgTransient );
	m_simPages->AddPage( m_pgTransient, _("Transient"), false );
	m_pgCustom = new wxPanel( m_simPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_staticText18 = new wxStaticText( m_pgCustom, wxID_ANY, _("Spice directives:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	bSizer2->Add( m_staticText18, 0, wxALL, 5 );

	m_customTxt = new wxTextCtrl( m_pgCustom, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	m_customTxt->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	bSizer2->Add( m_customTxt, 1, wxALL|wxEXPAND, 5 );

	m_loadDirectives = new wxButton( m_pgCustom, wxID_ANY, _("Load directives from schematic"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_loadDirectives, 0, wxALL|wxEXPAND, 5 );


	m_pgCustom->SetSizer( bSizer2 );
	m_pgCustom->Layout();
	bSizer2->Fit( m_pgCustom );
	m_simPages->AddPage( m_pgCustom, _("Custom"), false );

	bSizer1->Add( m_simPages, 1, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer1251;
	fgSizer1251 = new wxFlexGridSizer( 1, 7, 0, 0 );
	fgSizer1251->SetFlexibleDirection( wxBOTH );
	fgSizer1251->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_tempText = new wxStaticText( this, wxID_ANY, _("Temp:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_tempText->Wrap( -1 );
	fgSizer1251->Add( m_tempText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	m_temp = new wxTextCtrl( this, wxID_ANY, _("20"), wxDefaultPosition, wxDefaultSize, 0 );
	m_temp->SetToolTip( _("Operating temperature of the circuit.") );

	fgSizer1251->Add( m_temp, 0, wxALL|wxEXPAND, 5 );

	m_staticText11012 = new wxStaticText( this, wxID_ANY, _("ºC"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11012->Wrap( -1 );
	fgSizer1251->Add( m_staticText11012, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 5 );


	fgSizer1251->Add( 90, 0, 1, 0, 5 );

	m_staticText3213 = new wxStaticText( this, wxID_ANY, _("Tnom:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3213->Wrap( -1 );
	fgSizer1251->Add( m_staticText3213, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	m_tnom = new wxTextCtrl( this, wxID_ANY, _("20"), wxDefaultPosition, wxDefaultSize, 0 );
	m_tnom->SetToolTip( _("Temperature at which the given model parameters are valid. If TEMP <> TNOM, parameters are internally recalculated to TEMP.") );

	fgSizer1251->Add( m_tnom, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText110113 = new wxStaticText( this, wxID_ANY, _("ºC"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText110113->Wrap( -1 );
	fgSizer1251->Add( m_staticText110113, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );


	bSizer9->Add( fgSizer1251, 1, wxALIGN_CENTER|wxALL, 5 );


	bSizer1->Add( bSizer9, 0, wxEXPAND, 5 );

	m_fixPassiveVals = new wxCheckBox( this, wxID_ANY, _("Adjust passive symbol values (e.g. M -> Meg; 100 nF -> 100n)"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_fixPassiveVals, 0, wxALL, 5 );

	m_fixIncludePaths = new wxCheckBox( this, wxID_ANY, _("Add full path for .include library directives"), wxDefaultPosition, wxDefaultSize, 0 );
	m_fixIncludePaths->SetValue(true);
	bSizer1->Add( m_fixIncludePaths, 0, wxALL, 5 );

	m_fixSaveCurrents = new wxCheckBox( this, wxID_ANY, _("Save currents"), wxDefaultPosition, wxDefaultSize, 0 );
	m_fixSaveCurrents->SetValue(true);
	m_fixSaveCurrents->SetToolTip( _("Save currents through all terminals of the following devices: M, J, Q, D, R, C, L, B, F, G, W, S, I. Recommended only for small circuits!") );

	bSizer1->Add( m_fixSaveCurrents, 0, wxALL, 5 );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();

	bSizer1->Add( m_sdbSizer, 0, wxALL|wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DIALOG_SIM_SETTINGS_BASE::onInitDlg ) );
	m_rShuntOn->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_SIM_SETTINGS_BASE::onRshuntCheck ), NULL, this );
	m_trTolOn->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_SIM_SETTINGS_BASE::onTransientToleranceCheck ), NULL, this );
	m_loadDirectives->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_SIM_SETTINGS_BASE::onLoadDirectives ), NULL, this );
}

DIALOG_SIM_SETTINGS_BASE::~DIALOG_SIM_SETTINGS_BASE()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DIALOG_SIM_SETTINGS_BASE::onInitDlg ) );
	m_rShuntOn->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_SIM_SETTINGS_BASE::onRshuntCheck ), NULL, this );
	m_trTolOn->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_SIM_SETTINGS_BASE::onTransientToleranceCheck ), NULL, this );
	m_loadDirectives->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_SIM_SETTINGS_BASE::onLoadDirectives ), NULL, this );

}
