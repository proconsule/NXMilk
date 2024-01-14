#include "imgloader.h"
#include "nxmp-gfx.h"


#include "stb_image.h"
#include <turbojpeg.h>

enum{
	JPGFILE,
	PNGFILE,
	BMPFILE,
	WEBPFILE,
	GIFFILE
	
};



CImgLoader::CImgLoader(std::string basepath){

#ifdef OPENGL_BACKEND
	/* Home Icons */
	
	/*
	Utility::TxtLoadFromFile(basepath+"/sdcard.png",&icons.SdCardTexture.id,&icons.SdCardTexture.width,&icons.SdCardTexture.height);
	Utility::TxtLoadFromFile(basepath+"/usb.png",&icons.UsbTexture.id,&icons.UsbTexture.width,&icons.UsbTexture.height);
	Utility::TxtLoadFromFile(basepath+"/network.png",&icons.NetworkTexture.id,&icons.NetworkTexture.width,&icons.NetworkTexture.height);
	Utility::TxtLoadFromFile(basepath+"/enigma2.png",&icons.Enigma2Texture.id,&icons.Enigma2Texture.width,&icons.Enigma2Texture.height);
	Utility::TxtLoadFromFile(basepath+"/playlist.png",&icons.PlaylistTexture.id,&icons.PlaylistTexture.width,&icons.PlaylistTexture.height);
	Utility::TxtLoadFromFile(basepath+"/upnp.png",&icons.UPNPTexture.id,&icons.UPNPTexture.width,&icons.UPNPTexture.height);
	Utility::TxtLoadFromFile(basepath+"/info.png",&icons.InfoTexture.id,&icons.InfoTexture.width,&icons.InfoTexture.height);
	Utility::TxtLoadFromFile(basepath+"/settings.png",&icons.SettingsTexture.id,&icons.SettingsTexture.width,&icons.SettingsTexture.height);
	Utility::TxtLoadFromFile(basepath+"/exit.png",&icons.ExitTexture.id,&icons.ExitTexture.width,&icons.ExitTexture.height);
	*/
	
	/* File Browser Icons */
	TxtLoadFromFile(basepath+"/folder.png",&icons.FolderTexture.id,&icons.FolderTexture.width,&icons.FolderTexture.height);
	TxtLoadFromFile(basepath+"/file.png",&icons.FileTexture.id,&icons.FileTexture.width,&icons.FileTexture.height);
	
	TxtLoadFromFile(basepath+"/play.png",&icons.PlayIcon.id,&icons.PlayIcon.width,&icons.PlayIcon.height);
	TxtLoadFromFile(basepath+"/pause.png",&icons.PauseIcon.id,&icons.PauseIcon.width,&icons.PauseIcon.height);
	
	/*
	Utility::TxtLoadFromFile(basepath+"/file.png",&icons.FileTexture.id,&icons.FileTexture.width,&icons.FileTexture.height);
	Utility::TxtLoadFromFile(basepath+"/archive.png",&icons.ArchiveTexture.id,&icons.ArchiveTexture.width,&icons.ArchiveTexture.height);
	Utility::TxtLoadFromFile(basepath+"/image.png",&icons.ImageTexture.id,&icons.ImageTexture.width,&icons.ImageTexture.height);
	Utility::TxtLoadFromFile(basepath+"/gui/dpad-up.png",&icons.GUI_D_UP.id,&icons.GUI_D_UP.width,&icons.GUI_D_UP.height);
	Utility::TxtLoadFromFile(basepath+"/gui/dpad-down.png",&icons.GUI_D_DOWN.id,&icons.GUI_D_DOWN.width,&icons.GUI_D_DOWN.height);
	Utility::TxtLoadFromFile(basepath+"/gui/dpad-left.png",&icons.GUI_D_LEFT.id,&icons.GUI_D_LEFT.width,&icons.GUI_D_LEFT.height);
	Utility::TxtLoadFromFile(basepath+"/gui/dpad-right.png",&icons.GUI_D_RIGHT.id,&icons.GUI_D_RIGHT.width,&icons.GUI_D_RIGHT.height);	
	Utility::TxtLoadFromFile(basepath+"/gui/a-but.png",&icons.GUI_A_BUT.id,&icons.GUI_A_BUT.width,&icons.GUI_A_BUT.height);
	Utility::TxtLoadFromFile(basepath+"/gui/b-but.png",&icons.GUI_B_BUT.id,&icons.GUI_B_BUT.width,&icons.GUI_B_BUT.height);
	Utility::TxtLoadFromFile(basepath+"/gui/x-but.png",&icons.GUI_X_BUT.id,&icons.GUI_X_BUT.width,&icons.GUI_X_BUT.height);
	Utility::TxtLoadFromFile(basepath+"/gui/y-but.png",&icons.GUI_Y_BUT.id,&icons.GUI_Y_BUT.width,&icons.GUI_Y_BUT.height);
	*/
	
	
#endif

}

CImgLoader::~CImgLoader(){

#ifdef OPENGL_BACKEND

	/* Home Icons */
	
	
	/* File Browser Icons */
	
	glDeleteTextures(1, &icons.FolderTexture.id);
	glDeleteTextures(1, &icons.FileTexture.id);
	
	glDeleteTextures(1, &icons.PlayIcon.id);
	glDeleteTextures(1, &icons.PauseIcon.id);
	
	icons.FolderTexture.id = 0;
	icons.FileTexture.id = 0;
	icons.PlayIcon.id = 0;
	icons.PauseIcon.id = 0;
	
	
#endif


	
}

int guessImageMemoryFormat(unsigned char *_img_data){
	int guessret = -1;
	if(_img_data[0]=='G' && _img_data[1]=='I' && _img_data[2]=='F' && _img_data[3]=='8') {
		guessret = GIFFILE;
	}
	else if(_img_data[0]==0xff && _img_data[1]==0xd8){
		guessret = JPGFILE;
	}
	else if(_img_data[0]==0x89 && _img_data[1]==0x50 && _img_data[2]==0x4e && _img_data[3]==0x47) {
		guessret = PNGFILE;
	}
	else if(_img_data[0]==0x42 && _img_data[1]==0x4d) {
		guessret = BMPFILE;
	}
	
	
	return guessret;
}	


int guessImageFormat(std::string path){
	char testread[5];
	FILE *fptr = fopen(path.c_str(), "rb");
	int guessret = -1;
	
	
	
	if(fgets(testread,5,fptr)) {
		testread[4]=0;
		if(!strcmp((char *)testread,"GIF8")) {
			guessret = GIFFILE;
		}
		else if(testread[0]==0xff && testread[1]==0xd8){
			guessret = JPGFILE;
		}
		else if(testread[0]==0x89 && testread[1]==0x50 && testread[2]==0x4e && testread[3]==0x47) {
			guessret = PNGFILE;
		}
		else if(testread[0]==0x42 && testread[1]==0x4d) {
			guessret = BMPFILE;
		}
	}
	fclose(fptr);
	return guessret;
}	



#ifdef DEKO3D_BACKEND


unsigned char * jpg_decode(unsigned char *_img_data,int _size,int *width, int *height, int *channels){
	
	tjhandle _jpegDecompressor = tjInitDecompress();

	int jpegSubsamp;
	tjDecompressHeader2(_jpegDecompressor, _img_data, _size, width, height, &jpegSubsamp);

	int img_w = *width;
	int img_h = *height;

	unsigned char * buffer = (unsigned char *)malloc(img_w*img_h*4);
	*channels = 4;
	tjDecompress2(_jpegDecompressor, _img_data, _size, buffer, img_w, 0/*pitch*/, img_h, TJPF_RGBA, TJFLAG_FASTDCT);

	tjDestroy(_jpegDecompressor);
	return buffer;
}


Texture CImgLoader::OpenImageMemory(unsigned char *_img_data,int _size){
	
	int myformat = guessImageMemoryFormat(_img_data);
	if(myformat == -1){
		return {};
	}
	
	if(myformat == PNGFILE || myformat == BMPFILE || myformat == GIFFILE){
		int width, height, channels;
		unsigned char* image_data = stbi_load_from_memory(_img_data,_size, &width, &height, NULL, 4);
		channels = 4;
		return nxmpgfx::load_texture_from_mem(image_data,width,height,channels,DkImageFormat_RGBA8_Unorm, 0,nxmpgfx::getMaxSamplers()-1);
	}
	if(myformat == JPGFILE){
		int width, height, channels;
		unsigned char* image_data = jpg_decode(_img_data,_size, &width, &height,&channels);
		return nxmpgfx::load_texture_from_mem(image_data,width,height,channels,DkImageFormat_RGBA8_Unorm, 0,nxmpgfx::getMaxSamplers()-1);
	}
}


Texture CImgLoader::OpenImageFile(std::string path){
	
	int myformat = guessImageFormat(path);
	if(myformat == -1){
		return {};
	}
	
	int width, height;
	
	if(myformat == JPGFILE || myformat == PNGFILE || myformat == BMPFILE || myformat == GIFFILE){
		return nxmpgfx::load_texture(path,DkImageFormat_RGBA8_Unorm, 0,nxmpgfx::getMaxSamplers()-1);
	} 
	
	return {};
}

#endif


bool CImgLoader::TxtLoadFromFile(std::string filename, GLuint* out_texture, int* out_width, int* out_height){
#ifdef OPENGL_BACKEND
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filename.c_str(), &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return false;
		
		GLuint id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
		
		//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    
		*out_texture = id;
		*out_width = image_width;
		*out_height = image_height;
#endif		
		return true;

}
