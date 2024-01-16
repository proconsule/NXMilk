#ifndef NXMILK_IMGLOADER_H
#define NXMILK_IMGLOADER_H


#include "nxmp-gfx.h"


#ifdef OPENGL_BACKEND

typedef struct {
	/* Home Icons */
	Tex SdCardTexture;
	Tex UsbTexture;
	Tex NetworkTexture;
	Tex UPNPTexture;
	Tex Enigma2Texture;
	Tex PlaylistTexture;
	Tex InfoTexture;
	Tex SettingsTexture;
	Tex ExitTexture;
	
	/* File Browser Icons */
	Tex FolderTexture;
	Tex FileTexture;
	Tex ImageTexture;
	Tex ArchiveTexture;
	Tex GUI_D_UP;
	Tex GUI_D_DOWN;
	Tex GUI_D_LEFT;
	Tex GUI_D_RIGHT;
	Tex GUI_A_BUT;
	Tex GUI_B_BUT;
	Tex GUI_X_BUT;
	Tex GUI_Y_BUT;
	
	/* Networks Icons */
	Tex ShareAddTexture;
	Tex FTPTexture;
	Tex HTTPTexture;
	Tex SFTPTexture;
	Tex SMBTexture;
	Tex NFSTexture;
	
	/* Info Icons */
	Tex NXMPBannerTexture;
	Tex FFMPEGTexture;
	Tex MPVTexture;
	
	
	/* Player Icons */
	Tex PlayIcon;
	Tex PauseIcon;
	Tex FFIcon;
	Tex RewIcon;
	Tex PrevIcon;
	Tex NextIcon;
	
	Tex StopIcon;
	Tex MuteIcon;
	Tex VolumeIcon;
	Tex LoopIcon;
	Tex NoLoopIcon;
		
} nxmpicon_struct;
	
#endif


class CImgLoader{
public:
		CImgLoader(std::string basepath);
		~CImgLoader();
		nxmpicon_struct icons;
		
		
#ifdef OPENGL_BACKEND
		bool TxtLoadFromFile(std::string filename, GLuint* out_texture, int* out_width, int* out_height);
		Tex OpenImageFile(std::string path);
		Tex OpenImageMemory(unsigned char *_img_data,int _size);
#endif


};




#endif