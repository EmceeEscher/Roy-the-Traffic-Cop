#version 330 
// Input attributes
in vec3 in_position;
in vec3 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;

// vec4.x = old_offset, vec4.y = new_offset, vec4.z = show_offset, vec4.w = flip
uniform vec4 date_digit_0;
uniform vec4 date_digit_1;
uniform vec4 month_digit_0;
uniform vec4 month_digit_1;

void main()
{
	vec3 mod_tc = in_texcoord;
	vec3 mod_tf = in_position;

	if (in_texcoord.z == 0.1){
		mod_tc.x += date_digit_0.y;
	}
	if (in_texcoord.z == 0.11  ){
		if (date_digit_0.w == -1){
			mod_tc.x += date_digit_0.y;
		}else{
			mod_tc.x += date_digit_0.x;
		}
	}
	if (in_texcoord.z == 1){
		mod_tc.x += date_digit_0.z;
	}

	if (in_texcoord.z == 0.2){
		mod_tc.x += date_digit_1.y;
	}
	if (in_texcoord.z == 0.22  ){
		if (date_digit_1.w == -1){
			mod_tc.x += date_digit_1.y;
		}else{
			mod_tc.x += date_digit_1.x;
		}
	}
	if (in_texcoord.z == 2){
		mod_tc.x += date_digit_1.z;
	}

	if (in_texcoord.z == 0.3){
		mod_tc.x += month_digit_0.y;
	}
	if (in_texcoord.z == 0.33){
		if (month_digit_0.w == -1){
			mod_tc.x += month_digit_0.y;
		}else{
			mod_tc.x += month_digit_0.x;
		}
	}
	if (in_texcoord.z == 3){
		mod_tc.x += month_digit_0.z;
	}

	if (in_texcoord.z == 0.4){
		mod_tc.x += month_digit_1.y;
	}
	if (in_texcoord.z == 0.44){
		if (month_digit_1.w == -1){
			mod_tc.x += month_digit_1.y;
		}else{
			mod_tc.x += month_digit_1.x;
		}
	}
	if (in_texcoord.z == 4){
		mod_tc.x += month_digit_1.z;
	}










	if (in_position.z == 0.1){
		if (date_digit_0.w < 0){
			mod_tf.y = -1*mod_tf.y * date_digit_0.w;	
		}else{
			mod_tf.y = mod_tf.y * date_digit_0.w;	
		}
	}
	if (in_position.z == 0.2){
		if (date_digit_1.w < 0){
			mod_tf.y = -1*mod_tf.y * date_digit_1.w;	
		}else{
			mod_tf.y = mod_tf.y * date_digit_1.w;	
		}
	}
	if (in_position.z == 0.3){
		if (month_digit_0.w < 0){
			mod_tf.y = -1*mod_tf.y * month_digit_0.w;	
		}else{
			mod_tf.y = mod_tf.y * month_digit_0.w;	
		}
	}
	if (in_position.z == 0.4){
		if (month_digit_1.w < 0){
			mod_tf.y = -1*mod_tf.y * month_digit_1.w;	
		}else{
			mod_tf.y = mod_tf.y * month_digit_1.w;	
		}
	}

	vec3 pos = projection * transform * vec3(mod_tf.xy, 1.0);
	texcoord = vec2(mod_tc.xy);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}