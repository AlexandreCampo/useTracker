
solution "arucoColor"
   configurations { "debug", "release", "profile" }
 
   project "arucoColor"
      kind "StaticLib"
      language "C++"
      files { "arucoColor.h", "arucoColor.cpp", "marker.h", "marker.cpp", "dictionary.h", "dictionary.cpp" }
 
      includedirs {"/usr/include/opencv"}

      configuration "release"
         buildoptions {"-std=c++11"}
         defines { "NDEBUG" }
         flags { "OptimizeSpeed", "EnableSSE", "EnableSSE2", "FloatFast", "NoFramePointer"}    

      configuration "debug"
         buildoptions {"-std=c++11"}
         defines { "DEBUG" }
         flags { "Symbols" }

      configuration "profile"
         buildoptions {"-std=c++11"}
         defines { "PROFILE" }
         flags { "Symbols" }
	 buildoptions { "-pg" }	


if _ACTION == "clean" then
  os.execute ("rm -f *.deb")
end


