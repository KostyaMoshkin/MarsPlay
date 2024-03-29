#version 330 core

layout(location = 0) in int m_nRadius;
layout(location = 1) in int m_nTopography;

uniform float m_fPaletteValueMin;
uniform float m_fPaletteValueMax;
uniform float m_fScale;
uniform int	m_nLines;
uniform int	m_nLineSamples;
uniform float m_fBaseHeight;

uniform mat4 m_mView;
uniform mat4 m_mPerspective;

smooth out float fPaletteIndex;

void main()
{
	float fAreoid = m_nRadius - m_nTopography;
	float fDistance = (m_fScale * m_nTopography + fAreoid) / m_fBaseHeight + 1.0;

	float fLatitude = float(gl_VertexID / m_nLineSamples) / float(m_nLines - 1) * 3.1415926 - 3.1415926 / 2.0;
	float fLongitude = float(gl_VertexID % m_nLineSamples) / float(m_nLineSamples) * 3.1415926 * 2.0;

	vec3 vPosition = fDistance * vec3(
		cos(fLatitude) * sin(fLongitude),
		sin(fLatitude),
		cos(fLatitude) * cos(fLongitude));

	gl_Position = m_mPerspective * m_mView * vec4(vPosition, 1.0);

	fPaletteIndex = (1.0f * m_nTopography - m_fPaletteValueMin) / (m_fPaletteValueMax - m_fPaletteValueMin);
}