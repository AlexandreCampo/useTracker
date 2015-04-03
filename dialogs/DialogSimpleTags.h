#ifndef DIALOGSIMPLETAGS_H
#define DIALOGSIMPLETAGS_H

//(*Headers(DialogSimpleTags)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/SimpleTags.h"

class DialogSimpleTags: public wxDialog
{
public:
    
    std::vector<SimpleTags*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

    DialogSimpleTags(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
    virtual ~DialogSimpleTags();
    
    //(*Declarations(DialogSimpleTags)
    wxSpinCtrl* SpinCtrlTagHeight;
    wxCheckBox* CheckBoxOutput;
    wxStaticText* StaticText1;
    wxSpinCtrl* SpinCtrlTagWidth;
    wxButton* ButtonOk;
    wxStaticText* StaticText2;
    wxFilePickerCtrl* FilePickerCtrl1;
    //*)
    
protected:
    
    //(*Identifiers(DialogSimpleTags)
    static const long ID_STATICTEXT1;
    static const long ID_SPINCTRL1;
    static const long ID_STATICTEXT2;
    static const long ID_SPINCTRL2;
    static const long ID_CHECKBOX1;
    static const long ID_FILEPICKERCTRL1;
    static const long ID_BUTTON1;
    //*)
    
private:
    
    //(*Handlers(DialogSimpleTags)
    void OnButtonOkClick(wxCommandEvent& event);
    void OnCheckBoxOutputClick(wxCommandEvent& event);
    void OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event);
    void OnSpinCtrlTagWidthChange(wxSpinEvent& event);
    void OnSpinCtrlTagHeightChange(wxSpinEvent& event);
    //*)
    
    DECLARE_EVENT_TABLE()
};

#endif
