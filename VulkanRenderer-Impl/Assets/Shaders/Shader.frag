#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPosition;

layout(location = 0) out vec4 outColor;

void main() 
{
  float depth = length(fragPosition);

  depth = clamp(depth / 100.0, 0.0, 1.0);

  outColor = vec4(vec3(depth), 1.0);
}