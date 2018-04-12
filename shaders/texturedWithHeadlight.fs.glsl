#version 330
// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int turn_direction; //0 = straight, 1 = left, 2 = right
uniform int headlight_on; //0 = off, 1 = on

// Output color
layout(location = 0) out  vec4 color;

bool is_in_y_range(float y_coord) {
	return ((y_coord < 0.3 && y_coord > 0.17 && (turn_direction == 0 || turn_direction == 2))   //right headlight should turn on for straight and right
		 || (y_coord > 0.7 && y_coord < 0.83 && (turn_direction == 0 || turn_direction == 1))); //left headlight should turn on for straight and left
}

void main()
{
	float car_x_coord = mod(texcoord.x * 8.0, 1.0);

	if(headlight_on == 1 && car_x_coord > 0.92 && is_in_y_range(texcoord.y)) { //headlight is on, add yellow tint to headlight region
		//x = Red, y = Green, z = Blue
		vec3 yellow_color = fcolor;
		yellow_color.x = 20.0;
		yellow_color.y = 20.0;
		color = vec4(yellow_color, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	} else { //headlight is off, just use regular texture color
		color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}
}
