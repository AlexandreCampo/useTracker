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

#ifndef DIALOGTRACKER_H
#define DIALOGTRACKER_H

//(*Headers(DialogTracker)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
//*)

#include <wx/stattext.h>
#include <wx/spinctrl.h>

#include "PipelinePlugin.h"
#include "plugins/Tracker.h"

class DialogTracker: public wxDialog
{
	public:

    vector<Tracker*> plugin;
    void SetPlugin (vector<PipelinePlugin*> pfv);
    wxStaticBoxSizer* StaticBoxSizerVE;
    wxFlexGridSizer* FlexGridSizerMain;

		DialogTracker(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogTracker();

		//(*Declarations(DialogTracker)
		wxStaticText* StaticTextTrailLength;
		wxButton* buttonOk;
		wxCheckBox* CheckBoxReplay;
		wxCheckBox* CheckBoxUseVE;
		wxSpinCtrl* SpinCtrlTrailLength;
		wxCheckBox* CheckBoxOutput;
		wxStaticText* StaticText1a;
		wxButton* ButtonLoadHistory;
		wxCheckBox* CheckBoxVEZone;
		wxFilePickerCtrl* FilePickerCtrl1;
		//*)

		wxStaticText* StaticText1;
		wxSpinCtrl* SpinCtrlEntitiesCount;
		wxStaticText* StaticText2;
		wxSpinCtrlDouble* SpinCtrlMinInterdistance;
		wxStaticText* StaticText3;
		wxSpinCtrlDouble* SpinCtrlMaxMotion;
		wxStaticText* StaticText4;
		wxSpinCtrlDouble* SpinCtrlExtrapolationDecay;
		wxStaticText* StaticText5;
		wxSpinCtrl* SpinCtrlExtrapolationHistorySize;
		wxStaticText* StaticText6;
		wxSpinCtrlDouble* SpinCtrlExtrapolationTimeout;

		wxStaticText* StaticText10;
		wxSpinCtrlDouble* SpinCtrlVEDistance;
		wxStaticText* StaticText11;
		wxSpinCtrlDouble* SpinCtrlVEDelay;
		wxStaticText* StaticText12;
		wxSpinCtrlDouble* SpinCtrlVELifetime;

	protected:

		//(*Identifiers(DialogTracker)
		static const long ID_CHECKBOX2;
		static const long ID_STATICTEXT1A;
		static const long ID_CHECKBOX3;
		static const long ID_CHECKBOX1;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_CHECKBOX4;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXTTRAILENGTH;
		static const long ID_SPINCTRLTRAILLENGTH;
		static const long ID_BUTTON2;
		//*)

		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT3;
		static const long ID_SPINCTRL3;
		static const long ID_STATICTEXT4;
		static const long ID_SPINCTRL4;
		static const long ID_STATICTEXT5;
		static const long ID_SPINCTRL5;
		static const long ID_STATICTEXT6;
		static const long ID_SPINCTRL6;
		static const long ID_STATICTEXT10;
		static const long ID_SPINCTRL8;
		static const long ID_STATICTEXT11;
		static const long ID_SPINCTRL9;
		static const long ID_STATICTEXT12;
		static const long ID_SPINCTRL10;


	private:

		//(*Handlers(DialogTracker)
		void OnButtonOkClick(wxCommandEvent& event);
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event);
		void OnCheckBoxOutputClick(wxCommandEvent& event);
		void OnCheckBoxOutputClick1(wxCommandEvent& event);
		void OnCheckBoxReplayClick(wxCommandEvent& event);
		void OnCheckBoxUseVEClick(wxCommandEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnButtonLoadHistoryClick(wxCommandEvent& event);
		void OnSpinCtrlTrailLengthChange(wxSpinEvent& event);
		//*)

		void OnSpinCtrlExtrapolationTimeoutChange(wxSpinDoubleEvent& event);
		void OnSpinCtrlExtrapolationHistorySizeChange(wxSpinEvent& event);
		void OnSpinCtrlExtrapolationDecayChange(wxSpinDoubleEvent& event);
		void OnSpinCtrlMaxMotionChange(wxSpinDoubleEvent& event);
		void OnSpinCtrlMinInterdistanceChange(wxSpinDoubleEvent& event);
		void OnSpinCtrlEntitiesCountChange(wxSpinEvent& event);
		void OnSpinCtrlVELifetimeChange(wxSpinDoubleEvent& event);
		void OnSpinCtrlVEDistanceChange(wxSpinDoubleEvent& event);
		void OnSpinCtrlVEDelayChange(wxSpinDoubleEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif
