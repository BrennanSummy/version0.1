#include "Texture.h"
//debug
#include <filesystem>
// This is a C++ feature where you can define object values before even entering the constructor body
Texture::Texture(const std::string& path): m_width(0),m_height(0),m_bytesPerPixel(0)
{
    std::cout << "Looking in: "<<std::filesystem::current_path().c_str()
    << " looking for " << path.c_str()<< std::endl;
    QImage img1(QString::fromStdString(path));
    QImage img = img1.convertToFormat(QImage::Format_RGBA8888).flipped();
    qDebug() << img.size();
    qDebug() << img.format();

    m_texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    m_texture->setSize(img.width(),img.height());
    m_texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
    m_texture->allocateStorage();

    m_texture->setData(QOpenGLTexture::RGBA,QOpenGLTexture::UInt8,img.bits());

    configureSampling();
    m_texture->setWrapMode(QOpenGLTexture::Repeat);
}

Texture::Texture(int matrixWidth, int matrixHeight, char* matrixData): m_width(matrixWidth), m_height(matrixHeight),m_bytesPerPixel(0)
{
    //// Prime GL for handling the texture
    //m_texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    //m_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    //std::cout << "Tex width: " << m_width << std::endl;
    //std::cout << "Tex height: " << m_height << std::endl;
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
    //m_texture->setWrapMode(QOpenGLTexture::Repeat);
    m_texture->setWrapMode(QOpenGLTexture::ClampToBorder);
    m_texture->setBorderColor(0,0,0,0);
    m_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    m_texture->generateMipMaps(3);
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
