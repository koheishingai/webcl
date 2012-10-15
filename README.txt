README for WebCL/Chromium

---------------
Tested platform
---------------
- OS: Window 7
- GPU: Intel IvyBridge Platform with OpenCL-OpenGL interop supported.

---------------------
Tested Chromium version
---------------------
21.0.1163.0

------------
How to build
------------
1. Following http://dev.chromium.org/developers/how-tos/get-the-code to get
   Chromium 21.0.1163.0 source code:
   gclient config https://src.chromium.org/chrome/releases/21.0.1163.0
2. Following http://www.chromium.org/developers/how-tos/build-instructions-windows
   to make sure you can build chromium successfully first.
3. Download the webcl-chromium and patch it to your Chromium source. Note:
   Source ----Map to Chromium----> src/third_party/WebKit/Source
   Tools ----Map to Chromium----> src/third_party/WebKit/Tools
   chromium-src ----Map to Chromium----> src/
4. Download Intel OpenCL libary and install it.
5. Add Intel OpenCL library path to Visual Studio:
   a. Add  $(INTELOCLSDKROOT)\include; to the beginning of the 'IncludePath' property in %LOCALAPPDATA%\Microsoft\MSBuild\v4.0\Microsoft.Cpp.Win32.user.props. Note, these files only exist after you have launched and exited Visual Studio at least once.
   b. Add  $(INTELOCLSDKROOT)\lib\x86; to the beginning of the 'LibraryPath' property in the same file. At this point the .props file will look like this:

	<?xml version="1.0" encoding="utf-8"?>
	<Project DefaultTargets="Build" ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
	  <PropertyGroup>
		<IncludePath>$(INTELOCLSDKROOT)\include;$(IncludePath)</IncludePath>
		<LibraryPath>$(INTELOCLSDKROOT)\lib\x86;$(LibraryPath)</LibraryPath>
	  </PropertyGroup>
	</Project>
6. Build the patched chromium in VS.

------------------
How to run samples
------------------
1. Run chrome.exe in cmd with below parameters:
   --single-process  --in-process-webgl --disable-accelerated-compositing --use-gl=desktop --allow-file-access-from-files
   
   Note: it may crash at first startup because single-process mode is not supported by Chromium officially now and have some bugs.
2. In Chromium tab, run chrome://gpu to check whether webgl is enabled.
3. Hello example:
	goto Examples/WebCL/Hello/index.html
4. N-body example:
	goto Examples/WebCL/Nbody/index.html
5. Deform example:
	goto Examples/WebCL/Deform/index.html
