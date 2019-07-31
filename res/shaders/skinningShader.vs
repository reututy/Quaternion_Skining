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

uniform mat4 MV;
uniform mat4 Projection;
uniform mat4 Normal;
uniform vec4 dqRot[5];
uniform vec4 dqTrans[5];
uniform int index;

void main()
{	
	int i = 0;
	int x = -1;
	vec4 b_rot = vec4(0);
	vec4 b_trans = vec4(0);
	
	if(index == 20)
	{
		b_rot += dqRot[0] * weights[0]; 
		b_trans += dqTrans[0] * weights[0]; 
		b_rot += dqRot[0] * weights[1]; 
		b_trans += dqTrans[0] * weights[1]; 
		b_rot += dqRot[1] * weights[2]; 
		b_trans += dqTrans[1] * weights[2]; 
	}
	else if(index == 22)
		x=0;
	else if(index == 24)
		x=1;
	else if(index == 26)
		x=2;
	else if(index == 28)
	{
		b_rot += dqRot[3] * weights[0]; 
		b_trans += dqTrans[3] * weights[0]; 
		b_rot += dqRot[4] * weights[1]; 
		b_trans += dqTrans[4] * weights[1]; 
		b_rot += dqRot[4] * weights[2]; 
		b_trans += dqTrans[4] * weights[2]; 
	}
	if(x != -1)
	{
		for (i=x; i<3+x; i++)
		{
			b_rot += dqRot[i] * weights[i-x]; 
			b_trans += dqTrans[i] * weights[i-x]; 
		}
	}
	
	vec4 c_rot_n = b_rot / length(b_rot);
	vec4 c_trans_n = b_trans / length(b_rot);

	float w0 = c_rot_n[0];
	float x0 = c_rot_n[1];
	float y0 = c_rot_n[2];
	float z0 = c_rot_n[3];
	
	float wE = c_trans_n[0];
	float xE = c_trans_n[1];
	float yE = c_trans_n[2];
	float zE = c_trans_n[3];
	
	float t0 = 2.0*(-wE*x0 + xE*w0 - yE*z0 + zE*y0);
	float t1 = 2.0*(-wE*y0 + xE*z0 + yE*w0 - zE*x0);
	float t2 = 2.0*(-wE*z0 - xE*y0 + yE*x0 + zE*w0);
	
	mat4 M = mat4(1.0 - 2.0*y0*y0 - 2.0*z0*z0,
					2.0*x0*y0 + 2.0*w0*z0,
					2.0*x0*z0 - 2.0*w0*y0,
					0,
					
					2.0*x0*y0 - 2.0*w0*z0,
					1 - 2.0*x0*x0 -2.0*z0*z0,
					2.0*y0*z0 +2.0*w0*x0,
					0,
					
					2.0*x0*z0 + 2.0*w0*y0,
					2.0*y0*z0 - 2.0*w0*z0,
					1 - 2.0*x0*x0 - 2.0*y0*y0,
					0,
					
					t0, t1, t2, 1.0);

	
	texCoord0 = texCoords;
	color0 = color;
	position0 = vec3(M * vec4(position, 1.0));
	normal0 = (M * vec4(normal, 0.0)).xyz;
	gl_Position = Projection * M * vec4(position, 1.0); //you must have gl_Position
	

	//else
	/*
		{
			position0 = vec3(Normal * vec4(position, 1.0));
			normal0 = (Normal * vec4(normal, 0.0)).xyz;
			gl_Position = Projection * MV * vec4(position, 1.0); //you must have gl_Position
		}
		*/
}
