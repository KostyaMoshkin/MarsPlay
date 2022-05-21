#version 330 core

uniform sampler1D tTexture;

smooth in float fPaletteIndex;

void main()
{

	gl_FragColor = vec4(texture(tTexture, min(max(fPaletteIndex, 0.001), 0.999)).rgb, 1.0);
}