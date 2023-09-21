#type vertex
#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 4) in vec2 a_TexCoords;

out vec2 v_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
    v_TexCoords = a_TexCoords;    
    gl_Position = u_ViewProjection * u_Transform * u_Model *vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
out vec4 FragColor;

in vec2 v_TexCoords;

uniform vec3 u_Color;

uniform sampler2D texture_diffuse1;

void main()
{    
	if(v_TexCoords.x != -1000)
		FragColor = texture(texture_diffuse1, v_TexCoords);
	else
		FragColor = vec4(u_Color, 1.0);
}