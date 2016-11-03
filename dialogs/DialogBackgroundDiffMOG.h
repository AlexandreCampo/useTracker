/*----------------------------------------------------------------------------*/
/*    Copyright (C) 2015 Alexandre Campo                                      */
/*                                                                            */
/*    This file is part of USE Tracker.                                       */
/*                                                                            */
/*    USE Tracker is free software: you can redistribute it and/or modify     */
/*    it under the terms of the GNU General Public License as published by    */
/*    the Free Software Foundation, either version 3 of the License, or       */
/*    (at your option) any later version.                                     */
/*                                                                            */
/*    USE Tracker is distributed in the hope that it will be useful,          */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/*    GNU General Public License for more details.                            */
/*                                                                            */
/*    You should have received a copy of the GNU General Public License       */
/*    along with USE Tracker.  If not, see <http://www.gnu.org/licenses/>.    */
/*----------------------------------------------------------------------------*/

#ifndef DIALOGIMAGEBACKGROUNDDIFFMOG_H
#define DIALOGIMAGEBACKGROUNDDIFFMOG_H

//(*Headers(DialogBackgroundDiffMOG)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/BackgroundDiffMOG.h"

class DialogBackgroundDiffMOG: public wxDialog
{
	public:

    std::vector<BackgroundDiffMOG*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogBackgroundDiffMOG(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogBackgroundDiffMOG();

		//(*Declarations(DialogBackgroundDiffMOG)
		wxSpinCtrl* SpinCtrlBackgroundDiffMOGNoiseSigma;
		wxCheckBox* CheckBoxRestrictToZone;
		wxSpinCtrl* SpinCtrlBackgroundDiffMOGBackgroundRatio;
		wxSpinCtrl* SpinCtrlBackgroundDiffMOGHistory;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxSpinCtrl* SpinCtrlZone;
		wxButton* ButtonOk;
		wxRadioBox* RadioBoxOperator;
		wxSpinCtrl* SpinCtrlBackgroundDiffMOGLearningRate;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText2;
		wxSpinCtrl* SpinCtrlBackgroundDiffMOGNMixtures;
		wxStaticText* StaticText6;
		//*)

		wxSpinCtrlDouble* SpinCtrlDoubleBackgroundDiffMOGBackgroundRatio;
		wxSpinCtrlDouble* SpinCtrlDoubleBackgroundDiffMOGNoiseSigma;
		wxSpinCtrlDouble* SpinCtrlDoubleBackgroundDiffMOGLearningRate;

		
	protected:

		//(*Identifiers(DialogBackgroundDiffMOG)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL_HISTORY;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL_NMIXTURES;
		static const long ID_STATICTEXT3;
		static const long ID_SPINCTRL_BGRATIO;
		static const long ID_STATICTEXT4;
		static const long ID_SPINCTRL_NOISESIGMA;
		static const long ID_STATICTEXT5;
		static const long ID_SPINCTRL_LEARNINGRATE;
		static const long ID_RADIOBOX2;
		static const long ID_CHECKBOX1;
		static const long ID_STATICTEXT6;
		static const long ID_SPINCTRL_ZONE;
		static const long ID_BUTTON1;
		//*)

		static const long ID_SPINCTRLD_BGRATIO;
		static const long ID_SPINCTRLD_NOISESIGMA;
		static const long ID_SPINCTRLD_LEARNINGRATE;

	private:

		//(*Handlers(DialogBackgroundDiffMOG)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlBackgroundDiffMOGSizeChange(wxSpinEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnSpinCtrlBackgroundDiffMOGNMixturesChange(wxSpinEvent& event);
		void OnSpinCtrlBackgroundDiffMOGHistoryChange(wxSpinEvent& event);
		void OnSpinCtrlBackgroundDiffMOGBackgroundRatioChange(wxSpinDoubleEvent& event);
		void OnSpinCtrlBackgroundDiffMOGNoiseSigmaChange(wxSpinDoubleEvent& event);
		void OnSpinCtrlBackgroundDiffMOGLearningRateChange(wxSpinDoubleEvent& event);
		void OnCheckBoxRestrictToZoneClick(wxCommandEvent& event);
		void OnSpinCtrlZoneChange(wxSpinEvent& event);
		void OnRadioBoxOperatorSelect(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
