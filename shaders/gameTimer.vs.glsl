#version 330 
// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform float flip_offset_actual;
uniform int invert_backside;

void main()
{
	vec2 tmp_txcoord = in_texcoord;
	tmp_txcoord.x += flip_offset_actual;

	texcoord = vec2(tmp_txcoord);
	vec3 pos = projection * transform * vec3(in_position.x,invert_backside*in_position.y, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}