#version 430
layout(location = 0) out vec4 out_color;

uniform sampler2D river_texture;

in vec2 texcoord;

void main()
{
	out_color = texture2D(river_texture, texcoord);
}