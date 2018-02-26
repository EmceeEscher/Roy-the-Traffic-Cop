#version 330
// From vertex shader
in vec3 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int invert_backside;

// Output color
layout(location = 0) out  vec4 gl_FragColor;

void main()
{
	if (texcoord.z == 0){
		gl_FragColor = vec4(fcolor.x, fcolor.yz, 1.0) * 
			texture(sampler0, vec2(texcoord.x, texcoord.y));
	}

	if (texcoord.z == 1){
		gl_FragColor = vec4(fcolor.x, fcolor.yz, 1.0) * 
			texture(sampler0, vec2(texcoord.x, texcoord.y));
	}

}