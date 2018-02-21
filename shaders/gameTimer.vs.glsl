#version 330 
// Input attributes
in vec3 in_position;
in vec3 in_texcoord;

// Passed to fragment shader
out vec3 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform float date_0_offset;
uniform float date_1_offset;

void main()
{
	vec3 tmp_txcoord = in_texcoord;
	//first digit
	if (in_texcoord.z == 0){
		tmp_txcoord.x += date_0_offset;
	}
	//second digit
	if (in_texcoord.z == 1){
		tmp_txcoord.x += date_1_offset;
	}
	texcoord = vec3(tmp_txcoord);
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}