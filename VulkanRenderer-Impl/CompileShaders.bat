@ECHO OFF
CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/Shader.vert -o assets/shaders/Shader.vert.spv
CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/Shader.frag -o assets/shaders/Shader.frag.spv
PAUSE