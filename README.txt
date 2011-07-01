README for WebCL/WebKit

---------------
Tested platform
---------------
- OS: Mac OSX
- GPU: Nvidia GPU with OpenCL 1.0 support

---------------------
Tested WebKit version
---------------------
r78407

------------
How to build
------------
1. Download a zipped file from Downloads menu to your work directory (let's say, ~/work).
2. tar -zvxf webcl-20110630.tar.gz
3. cd webkit
4. Tools/Scripts/build-webkit

------------------
How to run samples
------------------
0. Enable WebGL
	$ defaults write com.apple.Safari WebKitWebGLEnabled -bool YES
1. N-body example:
	$ Tools/Scripts/run-safari Examples/WebCL/Nbody/index.html
2. Deform example:
	$ Tools/Scripts/run-safari Examples/WebCL/Deform/index.html
3. Sobel image filter example:
	$ Tools/Scripts/run-safari Examples/WebCL/Sobel/index.html
   
-----------------------------------
How to update source code using svn
-----------------------------------
0. cd ~/work
1. svn checkout https://webcl.googlecode.com/svn/trunk/ webcl --usename <you_email_address>
2. cp -Ri webcl/* webkit
