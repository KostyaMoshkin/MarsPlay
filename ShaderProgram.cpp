#include "ShaderProgram.h"
#include "ShaderLoader.h"

#include "Vocabulary.h"

#include <iostream>

namespace GL {

    ShaderProgram::ShaderProgram()
    {
        m_bShaderError = false;

        m_nProgramId = UINT_MAX;
    }

    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(m_nProgramId);
    }

    bool ShaderProgram::init()
    {
        if (m_bShaderError)
            return false;

        compileProgram();

        if (m_nProgramId == UINT_MAX)
            return false;

        return true;
    }

    bool ShaderProgram::addShader(int nShaderResourceID_, int shaderType_)
    {
        std::string xShader = shader::ShaderLoader::get_ShaderBody(nShaderResourceID_);

        if (xShader.empty())
            return false;

        if (m_sShaderFile.empty())
            m_sShaderFile = std::string(ShaderName::getName(nShaderResourceID_));
        else
        {
            m_sShaderFile += ";";
            m_sShaderFile += std::string(ShaderName::getName(nShaderResourceID_));
        }

        unsigned int vs = compileShader(shaderType_, xShader.c_str());

        if (vs == UINT_MAX)
        {
            m_bShaderError = true;
            return false;
        }

        m_vShader.push_back(vs);

        return true;
    }

    unsigned int ShaderProgram::compileShader(unsigned int sh_type, const char* source_)
    {
        unsigned int id = glCreateShader(sh_type);
        glShaderSource(id, 1, &source_, nullptr);

        glCompileShader(id);

        int nCompileStatus;
        glGetShaderiv(id, GL_COMPILE_STATUS, &nCompileStatus);

        if (nCompileStatus == GL_FALSE)
        {
            std::cout << "Shader compilation of file " << m_sShaderFile << std::endl;

            GLsizei logLength;
            GLchar  log[1024];
            glGetShaderInfoLog(id, sizeof(log), &logLength, log);

            std::cout << "Shader info log: " << std::endl << log << std::endl;

            glDeleteShader(id);

            return UINT_MAX;
        }

        return id;
    }

    void ShaderProgram::compileProgram()
    {
        m_nProgramId = glCreateProgram();

        for (size_t i = 0; i < m_vShader.size(); ++i)
        {
            if (m_vShader[i] != UINT_MAX)
                glAttachShader(m_nProgramId, m_vShader[i]);
        }

        glLinkProgram(m_nProgramId);

        int nLinkStatus;
        glGetProgramiv(m_nProgramId, GL_LINK_STATUS, &nLinkStatus);

        if (nLinkStatus == GL_FALSE)
        {
            int nLog_length = 0;
            glGetProgramiv(m_nProgramId, GL_INFO_LOG_LENGTH, &nLog_length);

            char buf[2048];
            glGetProgramInfoLog(m_nProgramId, nLog_length, &nLog_length, buf);

            std::cout << std::endl << "ShaderProgram::compileProgram()" << std::endl << "The function 'glGetProgramiv' return an error:" << buf << std::endl << "In shader: " << m_sShaderFile.c_str() << std::endl << std::endl;

            for (size_t i = 0; i < m_vShader.size(); ++i)
            {
                if (m_vShader[i] != UINT_MAX)
                    glDeleteShader(m_vShader[i]);
            }

            m_nProgramId = UINT_MAX;
            return;
        }

        for (size_t i = 0; i < m_vShader.size(); ++i)
        {
            if (m_vShader[i] != UINT_MAX)
                glDeleteShader(m_vShader[i]);
        }
    }

    void ShaderProgram::bound() const
    {
        glUseProgram(m_nProgramId);
    }

    void ShaderProgram::unbound() const
    {
        glUseProgram(0);
    }

    void ShaderProgram::setUniformMat4f(const char* name_, const GLfloat* mat4_)
    {
        int nUniformLocation = 0;
        if (!getUniformLocation(name_, nUniformLocation))
            return;

        glUniformMatrix4fv(nUniformLocation, 1, GL_FALSE, mat4_);
    }

    void ShaderProgram::setUniformVecf(const char* name_, const GLfloat* vec3_)
    {
        int nUniformLocation = 0;
        if (!getUniformLocation(name_, nUniformLocation))
            return;

        glUniform3fv(nUniformLocation, 1, vec3_);
    }

    void ShaderProgram::setUniformVec4f(const char* name_, const GLfloat* vec4_)
    {
        int nUniformLocation = 0;
        if (!getUniformLocation(name_, nUniformLocation))
            return;

        glUniform4fv(nUniformLocation, 1, vec4_);
    }

    void ShaderProgram::setUniformVeci(const char* name_, const GLint* ivec3_)
    {
        int nUniformLocation = 0;
        if (!getUniformLocation(name_, nUniformLocation))
            return;

        glUniform3iv(nUniformLocation, 1, ivec3_);
    }

    void ShaderProgram::setUniformIVec2(const char* name_, const GLint* ivec2_)
    {
        int nUniformLocation = 0;
        if (!getUniformLocation(name_, nUniformLocation))
            return;

        glUniform2iv(nUniformLocation, 1, ivec2_);
    }

    void ShaderProgram::setUniform1f(const char* name_, const GLfloat* fVal_)
    {
        int nUniformLocation = 0;
        if (!getUniformLocation(name_, nUniformLocation))
            return;

        glUniform1f(nUniformLocation, *fVal_);
    }

    void ShaderProgram::setUniform1i(const char* name_, const GLint* nVal_)
    {
        int nUniformLocation = 0;
        if (!getUniformLocation(name_, nUniformLocation))
            return;

        glUniform1i(nUniformLocation, *nVal_);
    }

    bool ShaderProgram::getUniformLocation(const char* name_, int& nUniformLocation_)
    {
        nUniformLocation_ = glGetUniformLocation(m_nProgramId, name_);
        if (nUniformLocation_ < 0)
        {
            return false;
        }

        return true;
    }


}