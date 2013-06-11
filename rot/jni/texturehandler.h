#ifndef __TEXTUREHANDLER_H__
#define __TEXTUREHANDLER_H__

struct Image
{
public:
  ~Image(){
    if(data){
      delete(data);
    }
  }

  int width;
  int height;
  unsigned char* data;
};

#endif
