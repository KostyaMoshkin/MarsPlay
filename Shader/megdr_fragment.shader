#version 330 core

uniform sampler1D tTexture;

smooth in float fPaletteIndex;

void main()
{
	gl_FragColor = vec4(texture(tTexture, fPaletteIndex).rgb, 1.0);
}