#version 460 core

layout(location = 0) in float m_fLatitude;
layout(location = 1) in float m_fLongitude;
layout(location = 2) in float m_fRadius;
layout(location = 3) in float m_fTopology;

uniform float m_fPaletteValueMin;
uniform float m_fPaletteValueMax;
uniform float m_fScale;

uniform mat4 m_mModel;
uniform mat4 m_mView;
uniform mat4 m_mPerspective;

smooth out float fPaletteIndex;

void main()
{
	float fDistance = (m_fRadius + m_fScale * m_fTopology) / 3300000.0;

	vec3 position;

	position.x = fDistance * cos(m_fLatitude) * sin(m_fLongitude);
	position.y = fDistance * sin(m_fLatitude);
	position.z = fDistance * cos(m_fLatitude) * cos(m_fLongitude);

	gl_Position = m_mPerspective * m_mView * m_mModel * vec4(position, 1.0);

	fPaletteIndex = (m_fTopology - m_fPaletteValueMin) / (m_fPaletteValueMax - m_fPaletteValueMin);

	fPaletteIndex = min(max(fPaletteIndex, 0.0), 1.0);
}