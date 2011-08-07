#include "UITexture.h"

#include <string>

#include "TextureManager.h" // TextureManager
#include "Video.h" // Texture

UITexture::UITexture( float xPos, float yPos, float w, float h, const std::string& tex )
: UIFrame( xPos, yPos, w, h )
, texture( TextureManager::newTexture( tex ) )
, _textureFilename( tex )
, highlight( false )
, clickFunc( NULL )
, id( 0 )
{
}

UITexture::~UITexture()
{
  if( texture )
  {
    TextureManager::delbyname( _textureFilename );
    texture = NULL;
  }
}

void UITexture::setTexture( OpenGL::Texture* tex )
{
  //! \todo Free current texture.
  //! \todo New reference?
  texture = tex;
}

void UITexture::setTexture( const std::string& textureFilename )
{
  if( texture )
  {
    TextureManager::delbyname( _textureFilename );
    texture = NULL;
  }
  _textureFilename = textureFilename;
  texture = TextureManager::newTexture( textureFilename );
}

OpenGL::Texture* UITexture::getTexture( )
{
  return texture;
}

void UITexture::render() const
{
  glColor3f( 1.0f, 1.0f, 1.0f );

  OpenGL::Texture::setActiveTexture();
  OpenGL::Texture::enableTexture();
  
  texture->bind();

  glBegin( GL_TRIANGLE_STRIP );
  glTexCoord2f( 0.0f, 0.0f );
  glVertex2f( x, y );
  glTexCoord2f( 1.0f, 0.0f );
  glVertex2f( x + width, y );
  glTexCoord2f( 0.0f, 1.0f );
  glVertex2f( x, y + height );
  glTexCoord2f( 1.0f, 1.0f );
  glVertex2f( x + width, y + height );
  glEnd();
  
  OpenGL::Texture::disableTexture();

  if( highlight )
  {
    glColor3f( 1.0f, 0.0f, 0.0f );
    glBegin( GL_LINE_LOOP );
    glVertex2f( x - 1.0f, y );
    glVertex2f( x + width, y );
    glVertex2f( x + width, y + height );
    glVertex2f( x - 1.0f, y + height );
    glEnd();
  }
}

UIFrame *UITexture::processLeftClick( float /*mx*/, float /*my*/ )
{
  if( clickFunc )
  {
    clickFunc( this, id );
    return this;
  }
  return 0;
}

void UITexture::setClickFunc( void (*f)( UIFrame *, int ), int num )
{
  clickFunc = f;
  id = num;
}
