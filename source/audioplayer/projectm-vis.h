#ifndef NXMP_PROJECTM_VIS_H
#define NXMP_PROJECTM_VIS_H

#include <string>
#include <vector>

#include "projectM-4/projectM.h"
#include "projectM-4/playlist.h"



class CProjectMVis{
public:
	CProjectMVis(unsigned int _width,unsigned int _height,std::string _milkdir = "",std::string _texturedir = "");
	~CProjectMVis();
	
	projectm_handle getHandle();
	projectm_playlist_handle getPlaylistHandle();
	
	void Draw();
	
	void AddFloat(float * data,int samples);
	void AddInt16(int16_t * data,int samples);
	
	void NextVisPreset();
	void PrevVisPreset();
	
	void ToogleVis();
	bool VisEnabled();
	
	std::vector<std::string> getPlaylistItems();
	std::string getCurrentPlaylistItem();
	
	
private:
	projectm_handle projectM{nullptr};
	projectm_playlist_handle playlist{nullptr};
	unsigned int width;
	unsigned int height;
	
	std::string milkdir;
	std::string texturedir;
	
	bool vis_enabled = true;
	
};



#endif