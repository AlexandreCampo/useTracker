

solution "tracker"
   configurations { "debug", "release" , "profile" }

   --- locations
   includedirs { "./ ./functions" }
   libdirs { }


   libdirs { os.findlib("boost_program_options"), os.findlib("tinyxml2") }
   links { "pthread", "avdevice", "avfilter", "avformat", "avcodec", "dl", "va", "bz2", "z", "swscale", "avutil", "m", "png", "x264", "tinyxml2", "boost_program_options" }

    buildoptions { "`pkg-config --libs opencv`"}

   project "tracker"
      kind "ConsoleApp"
      language "C++"
      files { "**.h", "Parameters.cpp", "PipelineFunction.cpp", "TrackerMain.cpp", "Utils.cpp", "./functions/*.cpp"  }

      configuration "release"
         defines { "NDEBUG" }
         flags { "OptimizeSpeed", "EnableSSE", "EnableSSE2", "FloatFast", "NoFramePointer"}    

      configuration "debug"
         defines { "DEBUG" }
         flags { "Symbols" }
 
      configuration "profile"
         defines { "DEBUG" }
         flags { "Symbols" }
	 buildoptions { "-pg" }	
	 linkoptions { "-pg" }	
