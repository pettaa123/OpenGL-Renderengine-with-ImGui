// Flat Color Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_Position;
out vec3 v_Normal;

void main()
{
	v_Position = a_Position;
	gl_Position = u_ViewProjection * u_Transform * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec3 v_Position;

uniform vec3 u_Color;
uniform vec3 u_LightColor;

void main()
{
	color = vec4(u_Color, 1.0);
}