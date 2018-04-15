
#version 330
// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float time;

// Output color
layout(location = 0) out  vec4 color;

bool light_bar_area(float x_coord, float y_coord)
{
	return (x_coord > 0.63 && x_coord < 0.69 && ((y_coord < 0.8 && y_coord > 0.55) || (y_coord < 0.45 && y_coord > 0.20)));
}

void main()
{
	if (sin(time/20) >= 0 && light_bar_area(texcoord.x, texcoord.y))
	{
	vec3 white_colour = fcolor;
		white_colour.x = 255.0;
		white_colour.y = 255.0;
		white_colour.z = 255.0;
		color = vec4(white_colour, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}
	else
	{
		color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}
}


