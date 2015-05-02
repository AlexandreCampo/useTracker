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

#ifndef DIALOGEXTRACTBLOBS_H
#define DIALOGEXTRACTBLOBS_H

//(*Headers(DialogExtractBlobs)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/ExtractBlobs.h"

class DialogExtractBlobs: public wxDialog
{
	public:

    std::vector<ExtractBlobs*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogExtractBlobs(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogExtractBlobs();

		//(*Declarations(DialogExtractBlobs)
		wxButton* buttonOk;
		wxCheckBox* CheckBoxOutput;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxSpinCtrl* SpinCtrlMaxSize;
		wxSpinCtrl* SpinCtrlExtractBlobsSize;
		wxCheckBox* CheckBoxRecordLabels;
		wxStaticText* StaticText2;
		wxFilePickerCtrl* FilePickerCtrl1;
		//*)

	protected:

		//(*Identifiers(DialogExtractBlobs)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT3;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT2;
		static const long ID_CHECKBOX2;
		static const long ID_CHECKBOX1;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_BUTTON2;
		//*)

	private:

		//(*Handlers(DialogExtractBlobs)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlExtractBlobsSizeChange(wxSpinEvent& event);
		void OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event);
		void OnCheckBoxOutputClick(wxCommandEvent& event);
		void OnCheckBoxRecordLabelsClick(wxCommandEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnTextCtrl1Text(wxCommandEvent& event);
		void OnSpinCtrlMaxSizeChange(wxSpinEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
