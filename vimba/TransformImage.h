/*=============================================================================
  Copyright (C) 2014 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ImageTransform.h

  Description: Image transformation of Vimba images

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

#ifndef TRANSFORM_IMAGE_H_
#define TRANSFORM_IMAGE_H_

#include <vector>
#include <string>

#include <AVTImageTransform/Include/VmbTransform.h>

namespace AVT {
namespace VmbAPI {

VmbErrorType TransformImage( const FramePtr & SourceFrame, std::vector<VmbUchar_t> & DestinationData, const std::string &DestinationFormat )
{
    if( SP_ISNULL( SourceFrame) )
    {
        return VmbErrorBadParameter;
    }
    VmbErrorType        Result;
    VmbPixelFormatType  InputFormat;
    VmbUint32_t         InputWidth,InputHeight;
    Result = SP_ACCESS( SourceFrame )->GetPixelFormat( InputFormat ) ;
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    Result = SP_ACCESS( SourceFrame )->GetWidth( InputWidth );
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    Result = SP_ACCESS( SourceFrame )->GetHeight( InputHeight );
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    // Prepare source image
    VmbImage SourceImage;
    SourceImage.Size = sizeof( SourceImage );
    Result = static_cast<VmbErrorType>( VmbSetImageInfoFromPixelFormat( InputFormat, InputWidth, InputHeight, &SourceImage ));
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    VmbUchar_t *DataBegin = NULL;
    Result = SP_ACCESS( SourceFrame )->GetBuffer( DataBegin );
    if( VmbErrorSuccess != Result ) 
    {
        return Result;
    }
    SourceImage.Data = DataBegin;
    // Prepare destination image
    VmbImage DestinationImage;
    DestinationImage.Size = sizeof( DestinationImage );
    Result = static_cast<VmbErrorType>( VmbSetImageInfoFromString( DestinationFormat.c_str(), static_cast<VmbUint32_t>(DestinationFormat.size()), InputWidth, InputHeight, &DestinationImage) );
    if ( VmbErrorSuccess != Result )
    {
        return Result;
    }
    const size_t ByteCount = ( DestinationImage.ImageInfo.PixelInfo.BitsPerPixel * InputWidth* InputHeight ) / 8 ;
    DestinationData.resize( ByteCount );
    DestinationImage.Data = &*DestinationData.begin();
    // Transform data
    Result = static_cast<VmbErrorType>( VmbImageTransform( &SourceImage, &DestinationImage, NULL , 0 ));
    return Result;
}

VmbErrorType TransformImage( const FramePtr & SourceFrame, std::vector<VmbUchar_t> & DestinationData, const std::string &DestinationFormat, const VmbFloat_t *Matrix )
{
    if( SP_ISNULL( SourceFrame ))
    {
        return VmbErrorBadParameter;
    }
    if ( NULL == Matrix )
    {
        return VmbErrorBadParameter;
    }
    VmbErrorType        Result;
    VmbPixelFormatType  InputFormat;
    VmbUint32_t         InputWidth,InputHeight;
    Result = SP_ACCESS( SourceFrame )->GetPixelFormat( InputFormat ) ;
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    Result = SP_ACCESS( SourceFrame )->GetWidth( InputWidth );
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    Result = SP_ACCESS( SourceFrame )->GetHeight( InputHeight );
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    // Prepare source image
    VmbImage SourceImage;
    SourceImage.Size = sizeof( SourceImage );
    Result = static_cast<VmbErrorType>( VmbSetImageInfoFromPixelFormat( InputFormat, InputWidth, InputHeight, &SourceImage ));
    if( VmbErrorSuccess != Result)
    {
        return Result;
    }
    VmbUchar_t *DataBegin = NULL;
    Result = SP_ACCESS( SourceFrame )->GetBuffer( DataBegin );
    if( VmbErrorSuccess != Result ) 
    {
        return Result;
    }
    SourceImage.Data = DataBegin;
    // Prepare destination image
    VmbImage DestinationImage;
    DestinationImage.Size = sizeof( DestinationImage );
    Result = static_cast<VmbErrorType>( VmbSetImageInfoFromString( DestinationFormat.c_str(), static_cast<VmbUint32_t>(DestinationFormat.size()), InputWidth, InputHeight, &DestinationImage ));
    if ( VmbErrorSuccess != Result )
    {
        return Result;
    }
    const size_t ByteCount = ( DestinationImage.ImageInfo.PixelInfo.BitsPerPixel * InputWidth* InputHeight ) / 8 ;
    DestinationData.resize( ByteCount );
    DestinationImage.Data = &*DestinationData.begin();
    // Setup Transform parameter

    // Transform data
    VmbTransformInfo TransformInfo;
    Result = static_cast<VmbErrorType>( VmbSetColorCorrectionMatrix3x3( Matrix, &TransformInfo ));
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    Result = static_cast<VmbErrorType>( VmbImageTransform( &SourceImage, &DestinationImage, &TransformInfo , 1 ));
    return Result;
}

}}
#endif
