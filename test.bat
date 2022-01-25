for /R %%f in (*.vert *.frag ) do (
	echo Compiling: %%f
	%VULKAN_SDK%/Bin/glslc.exe %%f -o %%f.spv
	echo.
	)
PAUSE