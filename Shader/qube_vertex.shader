#version 460 core

layout(location = 0) in vec3 m_vPosition;
layout(location = 1) in vec3 m_vColor;

uniform mat4 m_mModel;
uniform mat4 m_mView;
uniform mat4 m_mPerspective;

smooth out vec3 vColor;

void main()
{
  gl_Position = m_mPerspective * m_mView * m_mModel * vec4(m_vPosition, 1.0);

  vColor = m_vColor;
}