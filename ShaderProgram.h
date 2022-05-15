#pragma once

#include <GLEW/glew.h>

#include <string>
#include <memory>
#include <vector>

namespace GL {

	class ShaderProgram;
	using ShaderProgramPtr = std::shared_ptr<ShaderProgram>;

    // Обеспечивает загрузку шейдеров, передачу параметров в шейдеры
    class ShaderProgram
    {
        std::vector<unsigned> m_vShader;

        bool m_bShaderError;

        unsigned m_nProgramId;

        std::string m_sShaderFile;

    public:
        struct ShaderType
        {
            static int Vertex() { return GL_VERTEX_SHADER; }
            static int Geometry() { return GL_GEOMETRY_SHADER; }
            static int Fragment() { return GL_FRAGMENT_SHADER; }
            static int TessControl() { return GL_TESS_CONTROL_SHADER; }
            static int TessEvaluation() { return GL_TESS_EVALUATION_SHADER; }
            static int Compute() { return GL_COMPUTE_SHADER; }
        };

    public:
        ShaderProgram();
        ~ShaderProgram();

        static ShaderProgramPtr Create() { return std::make_shared<ShaderProgram>(); }

    public:
        bool init();

    public:
        void bound() const;
        void unbound() const;

        bool addShader(const char* sVertexShaderFile_, int shaderType_);
        bool addShader(int nhaderResourceID_, int shaderType_);

    public:
        void setUniformMat4f(const char* name_, const GLfloat* mat4_);
        void setUniformVecf(const char* name_, const GLfloat* vec3_);
        void setUniformVec4f(const char* name_, const GLfloat* vec4_);
        void setUniformVeci(const char* name_, const GLint* ivec3_);
        void setUniformIVec2(const char* name_, const GLint* ivec2_);
        void setUniform1f(const char* name_, const GLfloat* fVal_);
        void setUniform1i(const char* name_, const GLint* nVal_);

    private:
        void compileProgram();

        unsigned compileShader(unsigned int sh_type, const char* source_);

        bool getUniformLocation(const char* name_, int& id_);
    };

}

