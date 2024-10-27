#version 450

layout(binding = 0) uniform UniformBufferObject 
{
  mat4 model;
  mat4 view;
  mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

void main() 
{
  vec4 worldPos = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
  gl_Position = worldPos;
}