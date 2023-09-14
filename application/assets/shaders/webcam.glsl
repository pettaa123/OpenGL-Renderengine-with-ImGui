#type vertex
#version 430 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
    v_TexCoords = a_TexCoords;    
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 430 core
out vec4 FragColor;

in vec2 v_TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 u_Color;

void main()
{   
	if(v_TexCoords.x != -1000)
		FragColor = texture(texture_diffuse1, v_TexCoords);
	else
		FragColor = vec4(u_Color, 1.0);
}