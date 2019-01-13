#version 430
layout(lines) in;

layout(triangle_strip, max_vertices = 256) out;

uniform mat4 View;
uniform mat4 Projection;
uniform vec3 control_p1, control_p2, control_p3, control_p4;
uniform int no_of_instances;

uniform int no_of_generated_points;

uniform float river_thickness;
uniform float time;

in int instance[2];

out vec2 texcoord;


vec3 rotateY(vec3 point, float u)
{
	float x = point.x * cos(u) - point.z *sin(u);
	float z = point.x * sin(u) + point.z *cos(u);
	return vec3(x, point.y, z);
}

vec3 translateX(vec3 point, float t)
{
	return vec3(point.x + t, point.y, point.z);
}

vec3 bezier(float t)
{
	return control_p1 * pow((1 - t), 3) + control_p2 * 3 * t * pow((1 - t), 2) + control_p3 * 3 * pow(t, 2) * (1 - t) + control_p4 * pow(t, 3);
}

void main()
{

	if (instance[0] < no_of_instances)
	{
		float flow_speed = -2.0f;
		
		for (int i = 0; i < no_of_generated_points; i++) {
			float delta_current = 1.0 * i * 1 / no_of_generated_points;in_color = vec4(0,1,0,0); EmitVertex();

			gl_Position = Projection * View * vec4(translateX(bezier(delta_current), -river_thickness/2), 1);
			texcoord = vec2(delta_current * time * flow_speed, 0.0f);
			EmitVertex();

			gl_Position = Projection * View * vec4(translateX(bezier(delta_current), river_thickness/2), 1);
			texcoord = vec2(delta_current * time * flow_speed, 1.0f);
			EmitVertex();
		}

		EndPrimitive();
	}
}
