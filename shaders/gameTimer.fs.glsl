#version 330
// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int invert_backside;

// Output color
layout(location = 0) out  vec4 gl_FragColor;

void main()
{
gl_FragColor = vec4(fcolor.x, fcolor.yz, 1.0) * 
			texture(sampler0, vec2(texcoord.x, texcoord.y));

}