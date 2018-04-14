#version 330
// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float weather_time;


// Output color
layout(location = 0) out  vec4 color;

void main()
{

		// default scrolling sky
		color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x + weather_time/800.f, texcoord.y));

}
