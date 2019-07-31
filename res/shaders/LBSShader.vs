#version 330

attribute vec3 position;
attribute vec3 color;
attribute vec3 normal;
attribute vec2 texCoords;
attribute vec3 weights;

out vec2 texCoord0;
out vec3 normal0;
out vec3 color0;
out vec3 position0;

uniform mat4 Projection;
uniform mat4 pre_mat;
uniform mat4 curr_mat;
uniform mat4 post_mat;

void main()
{
	//LBS calculations:
	
	//vec4 new_pos = vec4(1.0);
	mat4 model = mat4(1.0);
	mat4 new_mat = mat4(0.0);
	
	weights = normalize(weights);
	model = scale(model, vec3(0.005f, 0.005f, 0.005f)); 
	
	//new_pos += weights[0]*pre_mat*vec4(position, 1.0);
	//new_pos += weights[1]*curr_mat*vec4(position, 1.0);
	//new_pos += weights[2]*post_mat*vec4(position, 1.0);
	
	new_mat += weights[0]*pre_mat;
	new_mat += weights[1]*curr_mat;
	new_mat += weights[2]*post_mat;
	
	texCoord0 = texCoords;
	color0 = color;
	normal0 = mat3(transpose(inverse(new_mat))) * normal;
	position0 = vec3(new_mat * vec4(position, 1.0));
	gl_Position = Projection * model * vec4(position, 1.0);
	
	/*
	texCoord0 = texCoords;
	color0 = color;
	normal0 = (curr_mat * vec4(normal, 0.0)).xyz;
	position0 = vec3(new_mat * vec4(position, 1.0));
	//gl_Position = Projection * new_pos * curr_mat * vec4(position, 1.0); //you must have gl_Position
	gl_Position = Projection * new_pos * curr_mat;
	*/
}
