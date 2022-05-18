#version 330 core

uniform sampler1D tTexture;

smooth in float fPaletteIndex;

void main()
{
	vec3 vColor = texture(tTexture, fPaletteIndex).rgb;
	gl_FragColor = vec4(vColor, 1.0);
	//gl_FragColor = vec4(1.0, 0.2, 0.8, 1.0);
}