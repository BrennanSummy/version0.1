#pragma once

#include <iostream>

#include <QOpenGLFunctions_3_3_Core> // Update version if necessary

#include "external/stb_image.h"

// The texture class handles image importing and texture sampling/scaling
class Texture
{
    private:

        std::string m_filePath;
        unsigned int m_texture_ID;
        unsigned char* m_localBuffer;
        int m_width, m_height, m_bytesPerPixel;
    
    public:
    Texture(const std::string& path);
    ~Texture();

    void bind(unsigned int slot = 0) const;
    void unBind() const;

};