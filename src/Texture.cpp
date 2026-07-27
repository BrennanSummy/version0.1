#include "Texture.h"
//debug
#include <filesystem>
// This is a C++ feature where you can define object values before even entering the constructor body
Texture::Texture(const std::string& path): m_width(0),m_height(0),m_bytesPerPixel(0)
{
    std::cout << "Looking in: "<<std::filesystem::current_path().c_str()
    << " looking for " << path.c_str()<< std::endl;
    stbi_set_flip_vertically_on_load(1);
    // Load the image assuming 4 channels (RGBA)
    unsigned char* rawData = stbi_load(path.c_str(),&m_width,&m_height,&m_bytesPerPixel,4);
    // Only run the GL stuff if the texture actually loads
    if (rawData)
    {
    //// Prime GL for handling the texture
    m_texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    m_texture->setSize(m_width,m_height);
    m_texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
    m_texture->allocateStorage();
    //glGenTextures(1, &m_texture_ID);
    //glBindTexture(GL_TEXTURE_2D, m_texture_ID);
    
    //// Params for texture sampling
    //// Wrapping
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //// Scaling
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //// Hand the image data to GL
    m_texture->setData(QOpenGLTexture::RGBA,QOpenGLTexture::UInt8,rawData);
    
    configureSampling();
    //int border = 0;
    //glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,m_width,m_height,border,GL_RGBA,GL_UNSIGNED_BYTE,m_localBuffer);

    //// Free the image memory
    //stbi_image_free(m_localBuffer);
    }
    else
    {
        const char* errorMsg = stbi_failure_reason();
        std::cout << "Texture failed to load: "<< errorMsg << std::endl;
    }

}

Texture::Texture(int matrixWidth, int matrixHeight, char* matrixData): m_width(matrixWidth), m_height(matrixHeight),m_bytesPerPixel(0)
{
    //// Prime GL for handling the texture
    m_texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    std::cout << "Tex width: " << m_width << std::endl;
    std::cout << "Tex height: " << m_height << std::endl;
    m_texture->setSize(m_width,m_height);
    m_texture->setFormat(QOpenGLTexture::R8U);
    m_texture->allocateStorage();
    m_texture->setData(QOpenGLTexture::Red_Integer,QOpenGLTexture::UInt8, matrixData);
    configureSampling();
}

void Texture::updateData(char* matrixData)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    m_texture->setData(QOpenGLTexture::Red_Integer,QOpenGLTexture::UInt8, matrixData);
    //m_texture->setData(QOpenGLTexture::Red_Integer,QOpenGLTexture::UInt8, matrixData);
}

void Texture::configureSampling()
{
    m_texture->setWrapMode(QOpenGLTexture::Repeat);
    m_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    //m_texture->generateMipMaps(3);
}

Texture::~Texture()
{
    if(m_texture){m_texture->destroy();}
}

void Texture::bind(unsigned int slot) const
{
    m_texture->bind(slot);
}

void Texture::release(unsigned int slot) const
{
    m_texture->release(slot);
}