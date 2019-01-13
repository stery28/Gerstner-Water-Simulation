#version 330
 
uniform sampler2D textureImage;

in vec2 texcoord;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = texture2D(textureImage, texcoord);
}