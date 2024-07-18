@ECHO OFF
FOR %%I IN (Assets/Shaders/*.frag) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.vert) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
PAUSE