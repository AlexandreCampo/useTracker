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

#ifndef DIALOGIMAGEBACKGROUNDDIFFMOG2_H
#define DIALOGIMAGEBACKGROUNDDIFFMOG2_H

//(*Headers(DialogBackgroundDiffMOG2)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/BackgroundDiffMOG2.h"

class DialogBackgroundDiffMOG2: public wxDialog
{
	public:

    std::vector<BackgroundDiffMOG2*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogBackgroundDiffMOG2(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogBackgroundDiffMOG2();

		//(*Declarations(DialogBackgroundDiffMOG2)
		wxCheckBox* CheckBoxShadowDetect;
		wxSpinCtrl* SpinCtrlHistory;
		wxCheckBox* CheckBoxRestrictToZone;
		wxSpinCtrl* SpinCtrlThreshold;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxSpinCtrl* SpinCtrlLearningRate;
		wxSpinCtrl* SpinCtrlZone;
		wxButton* ButtonOk;
		wxRadioBox* RadioBoxOperator;
		wxStaticText* StaticText2;
		wxStaticText* StaticText6;
		//*)

		wxSpinCtrlDouble* SpinCtrlDoubleThreshold;
		wxSpinCtrlDouble* SpinCtrlDoubleLearningRate;

		
	protected:

		//(*Identifiers(DialogBackgroundDiffMOG2)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL_HISTORY;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL_NMIXTURES;
		static const long ID_STATICTEXT3;
		static const long ID_SPINCTRL_LEARNINGRATE;
		static const long ID_CHECKBOX3;
		static const long ID_RADIOBOX2;
		static const long ID_CHECKBOX1;
		static const long ID_STATICTEXT6;
		static const long ID_SPINCTRL_ZONE;
		static const long ID_BUTTON1;
		//*)

		static const long ID_SPINCTRLD_THRESHOLD;
		static const long ID_SPINCTRLD_LEARNINGRATE;

	private:

		//(*Handlers(DialogBackgroundDiffMOG2)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnCheckBoxRestrictToZoneClick(wxCommandEvent& event);
		void OnSpinCtrlZoneChange(wxSpinEvent& event);
		void OnRadioBoxOperatorSelect(wxCommandEvent& event);
		void OnSpinCtrlHistoryChange(wxSpinEvent& event);
		void OnSpinCtrlThresholdChange(wxSpinDoubleEvent& event);
		void OnCheckBoxShadowDetectClick(wxCommandEvent& event);
		void OnSpinCtrlLearningRateChange(wxSpinDoubleEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
