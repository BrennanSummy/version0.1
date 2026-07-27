#pragma once

#include <iostream>

#include <QOpenGLFunctions_3_3_Core> // Update version if necessary
#include <QOpenGLTexture>

#include "external/stb_image.h"

// The texture class handles image importing and texture sampling/scaling
class Texture
{
    private:
        std::unique_ptr<QOpenGLTexture> m_texture;
        int m_width, m_height, m_bytesPerPixel;
        void configureSampling();
    
    public:
        void updateData(char* matrixData);
        Texture(const std::string& path);
        Texture(int matrixWidth, int matrixHeight, char* matrixData);
        ~Texture();

        void bind(unsigned int slot = 0) const;
        void release(unsigned int slot = 0) const;

};