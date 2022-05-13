#version 460 core

layout(location = 0) in vec3 m_vPosition;

uniform mat4 m_mModel;
uniform mat4 m_mView;
uniform mat4 m_mPerspective;

smooth out vec3 vColor;

void main()
{
	vec3 position = m_vPosition;
	float fDistance = m_vPosition.z / 3300000.0;

	float fLatitude = m_vPosition.x * 3.1415926 / 180.0;
	float fLongitude = m_vPosition.y * 3.1415926 / 180.0;

	position.x = fDistance * cos(fLatitude) * sin(fLongitude);
	position.y = fDistance * sin(fLatitude);
	position.z = fDistance * cos(fLatitude) * cos(fLongitude);

	gl_Position = m_mPerspective * m_mView * m_mModel * vec4(position, 1.0);

	vColor = vec3(0.8, 0.8, 0.8);
}