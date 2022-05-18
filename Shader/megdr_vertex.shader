#version 330 core

layout(location = 0) in int m_nRadius;
layout(location = 1) in int m_nAreoid;
layout(location = 2) in float m_fLongitude;
layout(location = 3) in float m_fLatitude;

uniform float m_fPaletteValueMin;
uniform float m_fPaletteValueMax;
uniform float m_fScale;
uniform int	m_nLines;
uniform int	m_nLineSamples;
uniform int	m_fBaseHeight;

uniform mat4 m_mModel;
uniform mat4 m_mView;
uniform mat4 m_mPerspective;

smooth out float fPaletteIndex;

void main()
{

	//float fLongitude = float((gl_VertexID / 2) % m_nLineSamples) / float(m_nLineSamples) * 3.14159;
	//float fLatitude = float((gl_VertexID / 2) / m_nLineSamples) / float(m_nLines) * 2.0 * 3.1415926 - 3.1415926;

	float fLongitude = m_fLongitude;
	float fLatitude = m_fLatitude;

	float fTopology = m_nRadius - m_nAreoid;

	float fDistance = (float(m_nAreoid + m_fBaseHeight) + m_fScale * fTopology) / m_fBaseHeight;
	fDistance = 1.0f;

	vec3 position;

	position.x = fDistance * cos(fLatitude) * sin(fLongitude);
	position.y = fDistance * sin(fLatitude);
	position.z = fDistance * cos(fLatitude) * cos(fLongitude);

	gl_Position = m_mPerspective * m_mView * m_mModel * vec4(position, 1.0);

	fPaletteIndex = (fTopology - m_fPaletteValueMin) / (m_fPaletteValueMax - m_fPaletteValueMin);

	fPaletteIndex = min(max(fPaletteIndex, 0.0), 1.0);
}