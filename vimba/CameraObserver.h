/*=============================================================================
  Copyright (C) 2012 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        CameraObserver.h

  Description: The camera observer that is used for notifications from VimbaCPP
               regarding a change in the camera list.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#ifndef AVT_VMBAPI_EXAMPLES_CAMERAOBSERVER
#define AVT_VMBAPI_EXAMPLES_CAMERAOBSERVER

#include <VimbaCPP/Include/VimbaCPP.h>

namespace AVT {
namespace VmbAPI {

class CameraObserver :  public ICameraListObserver
{

  public:
    // This is our callback routine that will be executed every time a camera was plugged in or out
    virtual void CameraListChanged( CameraPtr pCamera, UpdateTriggerType reason );

//  signals:
    // The camera list changed signal that passes the new camera and the its state directly
    void CameraListChangedSignal( int reason );
};

}} // namespace AVT::VmbAPI::Examples

#endif
