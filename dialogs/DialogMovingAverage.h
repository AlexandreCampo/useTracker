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

#ifndef DIALOGIMAGEMOVINGAVERAGE_H
#define DIALOGIMAGEMOVINGAVERAGE_H

//(*Headers(DialogMovingAverage)
#include <wx/spinctrl.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/MovingAverage.h"

class DialogMovingAverage: public wxDialog
{
	public:

    std::vector<MovingAverage*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);


		DialogMovingAverage(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogMovingAverage();

		//(*Declarations(DialogMovingAverage)
		wxSpinCtrl* SpinCtrlMovingAverageLength;
		wxStaticText* StaticText1;
		wxButton* ButtonOk;
		wxStaticText* StaticText2;
		wxSpinCtrl* SpinCtrlSegmentationThreshold;
		//*)

	protected:

		//(*Identifiers(DialogMovingAverage)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL2;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(DialogMovingAverage)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlMovingAverageLengthChange(wxSpinEvent& event);
		void OnSpinCtrlSegmentationThresholdChange(wxSpinEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
