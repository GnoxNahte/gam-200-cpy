#pragma once
#include "../core/Platform.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <array>

class ShaderHelper {
public:
    static GLuint compileShader(GLenum type, const char* src) {
        GLuint shader = glCreateShader(type);
        if (!shader) { LOGE("glCreateShader failed (type=0x%x)", type); return 0; }
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        GLint ok = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            GLint len = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
            std::string log(static_cast<size_t>(len), '\0');
            glGetShaderInfoLog(shader, len, nullptr, &log[0]);
            LOGE("Shader compile error:\n%s", log.c_str());
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }
    static GLuint buildProgram(const char* vertSrc, const char* fragSrc){
        
        GLuint vs = compileShader(GL_VERTEX_SHADER,   vertSrc);
        if (!vs) return 0;
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragSrc);
        if (!fs) { glDeleteShader(vs); return 0; }

        GLuint prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);
        glLinkProgram(prog);

        GLint ok = GL_FALSE;
        glGetProgramiv(prog, GL_LINK_STATUS, &ok);
        if (!ok) {
            GLint len = 0;
            glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
            std::string log(static_cast<size_t>(len), '\0');
            glGetProgramInfoLog(prog, len, nullptr, &log[0]);
            LOGE("Program link error:\n%s", log.c_str());
            glDeleteProgram(prog);
            prog = 0;
        }
        glDetachShader(prog, vs);
        glDetachShader(prog, fs);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return prog;
    }

    // static GLuint linkProgram(GLuint vert, GLuint frag);

    static std::string loadFile(const char* filename)
    {
        std::array<std::string, 4> possiblePaths = {
            "assets/shaders/",
            "",
            "shaders/",
            "assets/"
        };
        
        std::string path {};
        for (int i = 0; i < possiblePaths.size(); ++i)
        {
            path = possiblePaths[i] + filename;
            if (std::filesystem::exists(path))
                break;
        }
        
        // if (path.empty())
        //     throw;
        
        std::ifstream ifs {path, std::ios::in | std::ios::binary};

        const auto sz = std::filesystem::file_size(path);
        std::string result(sz, '\0');

        ifs.read(result.data(), sz);
        
        return result;
    }

    static GLuint buildProgramFromFile(const char* vertFile, const char* fragFile) {
        std::string vertShader = loadFile(vertFile),
                    fragShader = loadFile(fragFile);

        return buildProgram(vertShader.c_str(), fragShader.c_str());
    }
};
