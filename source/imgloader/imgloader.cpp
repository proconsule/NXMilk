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


	
	/* File Browser Icons */
	TxtLoadFromFile(basepath+"/folder.png",&icons.FolderTexture.id,&icons.FolderTexture.width,&icons.FolderTexture.height);
	TxtLoadFromFile(basepath+"/file.png",&icons.FileTexture.id,&icons.FileTexture.width,&icons.FileTexture.height);
	
	TxtLoadFromFile(basepath+"/usb.png",&icons.UsbTexture.id,&icons.UsbTexture.width,&icons.UsbTexture.height);
	TxtLoadFromFile(basepath+"/sftp.png",&icons.SFTPTexture.id,&icons.SFTPTexture.width,&icons.SFTPTexture.height);
	TxtLoadFromFile(basepath+"/smb.png",&icons.SMBTexture.id,&icons.SMBTexture.width,&icons.SMBTexture.height);
	TxtLoadFromFile(basepath+"/nfs.png",&icons.NFSTexture.id,&icons.NFSTexture.width,&icons.NFSTexture.height);
	
	
	TxtLoadFromFile(basepath+"/audioplayer/icons/play.png",&icons.PlayIcon.id,&icons.PlayIcon.width,&icons.PlayIcon.height);
	TxtLoadFromFile(basepath+"/audioplayer/icons/pause.png",&icons.PauseIcon.id,&icons.PauseIcon.width,&icons.PauseIcon.height);
	TxtLoadFromFile(basepath+"/audioplayer/icons/ff.png",&icons.FFIcon.id,&icons.FFIcon.width,&icons.FFIcon.height);
	TxtLoadFromFile(basepath+"/audioplayer/icons/rew.png",&icons.RewIcon.id,&icons.RewIcon.width,&icons.RewIcon.height);
	TxtLoadFromFile(basepath+"/audioplayer/icons/prev.png",&icons.PrevIcon.id,&icons.FFIcon.width,&icons.FFIcon.height);
	TxtLoadFromFile(basepath+"/audioplayer/icons/next.png",&icons.NextIcon.id,&icons.RewIcon.width,&icons.RewIcon.height);
	
	TxtLoadFromFile(basepath+"/wav-file.png",&icons.Wav_Icon.id,&icons.Wav_Icon.width,&icons.Wav_Icon.height);
	TxtLoadFromFile(basepath+"/ogg-file.png",&icons.Ogg_Icon.id,&icons.Ogg_Icon.width,&icons.Ogg_Icon.height);
	TxtLoadFromFile(basepath+"/flac-file.png",&icons.Flac_Icon.id,&icons.Flac_Icon.width,&icons.Flac_Icon.height);
	TxtLoadFromFile(basepath+"/mp3-file.png",&icons.Mp3_Icon.id,&icons.Mp3_Icon.width,&icons.Mp3_Icon.height);
	
	TxtLoadFromFile(basepath+"/sdcard.png",&icons.SdCardTexture.id,&icons.SdCardTexture.width,&icons.SdCardTexture.height);
	
	TxtLoadFromFile(basepath+"/network.png",&icons.NetworkTexture.id,&icons.NetworkTexture.width,&icons.NetworkTexture.height);
	TxtLoadFromFile(basepath+"/exit.png",&icons.ExitTexture.id,&icons.ExitTexture.width,&icons.ExitTexture.height);
	
	TxtLoadFromFile(basepath+"/cdimage.png",&icons.CDImage_Icon.id,&icons.CDImage_Icon.width,&icons.CDImage_Icon.height);
	
	TxtLoadFromFile(basepath+"/archive.png",&icons.ArchiveTexture.id,&icons.ArchiveTexture.width,&icons.ArchiveTexture.height);
	
	

}

CImgLoader::~CImgLoader(){


	/* Home Icons */
	
	
	/* File Browser Icons */
	
	glDeleteTextures(1, &icons.FolderTexture.id);
	glDeleteTextures(1, &icons.FileTexture.id);
	
	glDeleteTextures(1, &icons.FFIcon.id);
	glDeleteTextures(1, &icons.RewIcon.id);
	glDeleteTextures(1, &icons.PrevIcon.id);
	glDeleteTextures(1, &icons.NextIcon.id);
	glDeleteTextures(1, &icons.PauseIcon.id);

	glDeleteTextures(1, &icons.Wav_Icon.id);
	glDeleteTextures(1, &icons.Ogg_Icon.id);
	glDeleteTextures(1, &icons.Flac_Icon.id);
	glDeleteTextures(1, &icons.Mp3_Icon.id);

	
	
	icons.FolderTexture.id = -1;
	icons.FileTexture.id = -1;
	icons.PlayIcon.id = -1;
	icons.PauseIcon.id = -1;
	


	
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
