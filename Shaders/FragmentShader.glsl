#version 430

layout(location = 0) in vec3 f_color;

uniform sampler2D texture_1;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(f_color, 0);
}