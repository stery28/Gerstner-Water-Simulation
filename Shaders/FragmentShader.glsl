#version 430

layout(location = 0) in vec3 f_color;

uniform sampler2D textureImage;
uniform samplerCube texture_cubemap;
uniform vec3 camera_position;
uniform vec3 light_position;
uniform vec3 Color;
uniform int shininess;
uniform bool reflective;
uniform bool has_texture;

in vec3 world_position;
in vec3 world_normal;
in vec2 texcoord;

//const vec3 light_position = vec3(10, 7, 0);
const vec3 light_color = vec3(0.5f);
const float light_radius = 30.0f;

layout(location = 0) out vec4 out_color;

// ----------------------- Phong -----------------------
const vec3 ld = vec3(0.3);	// Diffuse factor
const vec3 ls = vec3(0.3);	// Specular factor
const float specular_exponent = 40.0;	// Specular exponent
const vec3 ambient = vec3(0.1f, 0.1f, 0.1f);
//const vec3 Color = vec3(0, 0.2f, 0.7f);
/*vec4 phong2() {
	vec3 L = normalize(light_position - world_position);
	vec3 N = world_normal;
	vec3 V = normalize(camera_position - world_position);
	vec3 H = normalize(L + V);
	//vec3 diffuse = ld * light_color * max(N * L, 0);
	vec3 diffuse = Color;
	float intensity = max(dot(N, L), 0);
	vec3 specular = vec3(0);
	if (intensity > 0)
	{
		specular = ls * light_color * pow(max(dot(N, H), 0), specular_exponent);
		float d = distance(light_position, world_position);
		float f = 1 / pow(d, 2);
	}
	//return vec4(f*(diffuse + specular), 1.0);
	return vec4(max(intensity*diffuse + specular, ambient), 1.0f);
}*/

vec4 phong3(vec3 Color) {
	vec3 L, V, H, N;
	L = normalize(light_position - world_position);
	V = normalize(camera_position - world_position);
	H = normalize(L + V);
	N = normalize(world_normal);
	float ambient_strength = 0.3f;
	vec3 ambient = ambient_strength * light_color;
	float diff = max(dot(N, L), 0.7f);
	vec3 diffuse = diff * light_color;
	float specular_strength = 0.5f;//0.5f;
	vec3 R = reflect(-L, N);
	float spec = pow(max(dot(V, R), 0.0f), shininess);
	vec3 specular = specular_strength * spec * light_color;
	vec3 result = (ambient + diffuse + specular) * Color;
	
	//if (diff == 0)
		//return vec4(0, 1, 0, 1.0f);

	return vec4(result, 1.0f);
}

/*vec4 phong(vec3 w_pos, vec3 w_N)
{
	vec3 L, V, H;

	float ka = 0.25, kd = 0.5, ks = 0.5;

	float factor;

	//vec3 diffuse = vec3(0, 0, 0);
	vec3 diffuse = light_color;
	vec3 specular = light_color;
	int material_shininess = 16;

	float lighting = 0;
	float diffuseFact, specularFact;

	//vec3 tmp_color = ((factor + ka) * diffuse.xyz + factor * specular.xyz) * light_color;
	vec3 tmp_color;

	L = normalize(light_position - w_pos);
	V = normalize(camera_position - w_pos);
	H = normalize(L + V);

	//factor = 1/(pow(distance(world_position, L), 2));
	factor = 1 / (pow(distance(light_position, world_position), 2));

	lighting = 0;
	if (max(dot(L, w_N), 0) > 0) {
		lighting = 1;
	}
	//factor = factor * lighting;
	diffuseFact = max(dot(L, w_N), 0);
	specularFact = lighting * pow(max(dot(w_N, H), 0), material_shininess);

	//tmp_color += ((diffuseFact + ka) * diffuse.xyz + specularFact * specular.xyz) * light_color * factor;
	tmp_color = (diffuseFact * kd + ka + specularFact * ks) * light_color * factor * Color;

	//tmp_color += ((diffuseFact + ka) * kd + specularFact * ks) * light_color * factor * Color;
	return vec4(tmp_color, 1);
}*/

vec3 myReflect()
{
	return reflect(world_position - camera_position, normalize(world_normal));
}

vec3 myRefract(float refractive_index)
{
	return refract(world_position - camera_position, normalize(world_normal), 1.0f / refractive_index);
}

void main()
{
	//out_color = vec4(f_color, 0);
	//out_color = vec4(0, 0.2f, 0.7f, 0);
	//out_color = phong(world_position, normalize(world_normal)); //
	//out_color = phong3();
	//out_color = vec4(world_position, 1);
	vec4 fragment_color;
	if (reflective)
	{
		vec4 reflect_color = texture(texture_cubemap, myReflect());
		vec4 refract_color = texture(texture_cubemap, myRefract(1.33));
		float refractive_factor = dot(normalize(camera_position - world_position), normalize(world_normal)); // Fresnel Effect
		refractive_factor = pow(refractive_factor, 2.0f);
		fragment_color = mix(reflect_color, refract_color, refractive_factor);
		fragment_color = mix(fragment_color, vec4(Color, 1.0f), 0.4f);
	}
	else 
	{
		if (has_texture)
		{
			fragment_color = texture2D(textureImage, texcoord);
		}
		else
		{
			fragment_color = vec4(Color, 1.0f);
		}
	}
	out_color = phong3(fragment_color.xyz);

	//out_color = phong2(); // Best one
	//out_color = vec4(normalize(world_normal), 1);

	/*if (world_normal.xyz == vec3(0))
		out_color = vec4(0, 1, 0, 1);*/
}