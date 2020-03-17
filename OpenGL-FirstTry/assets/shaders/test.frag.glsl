#version 450 core

layout (location = 0) out vec4 o_Color;

uniform vec4 u_Color;
in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;

uniform sampler2D u_Texture;

void main()
{
//	o_Color = vec4(v_TexIndex, v_TexIndex, v_TexIndex, 1.0);
    int index = int(v_TexIndex);
    o_Color = texture(u_Texture, v_TexCoord);
}