#include "projectm-vis.h"



CProjectMVis::CProjectMVis(unsigned int _width,unsigned int _height,std::string _milkdir,std::string _texturedir,bool usebuiltin){
	width = _width;
	height = _height;
	projectM = projectm_create();
	projectm_set_window_size(projectM, width, height);
	projectm_set_aspect_correction(projectM,  true);
	projectm_set_preset_duration(projectM,  1000);
	
	playlist = projectm_playlist_create(projectM);
	milkdir = _milkdir;
	texturedir = _texturedir;
	
	if (!texturedir.empty()){
		if(usebuiltin){
			const char* texturePathList[2]{&texturedir.c_str()[0],"romfs:/presets/Textures/"};
			projectm_set_texture_search_paths(projectM, texturePathList, 2);
		}else{
			const char* texturePathList[1]{&texturedir.c_str()[0]};
			projectm_set_texture_search_paths(projectM, texturePathList, 1);
		}
		
	}else{
		const char* texturePathList[1]{"romfs:/presets/Textures/"};
		projectm_set_texture_search_paths(projectM, texturePathList, 1);
	}
	
	if (!milkdir.empty())
	{
		projectm_playlist_add_path(playlist, milkdir.c_str(), true, false);
		if(usebuiltin){
			projectm_playlist_add_path(playlist, "romfs:/presets/milk/", true, false);
		}
		projectm_playlist_sort(playlist, 0, projectm_playlist_size(playlist), SORT_PREDICATE_FILENAME_ONLY, SORT_ORDER_ASCENDING);
	}else{
		if(usebuiltin){
			projectm_playlist_add_path(playlist, "romfs:/presets/milk/", true, false);
		}
	}
	projectm_playlist_set_position(playlist, 0, true);
}

projectm_handle CProjectMVis::getHandle(){
	return projectM;
}

projectm_playlist_handle CProjectMVis::getPlaylistHandle(){
	return playlist;
}

CProjectMVis::~CProjectMVis(){
	projectm_playlist_destroy(playlist);
	projectm_destroy(projectM);
}

void CProjectMVis::Draw(){
	projectm_opengl_render_frame(projectM);
}

void CProjectMVis::AddFloat(float * data,int samples){
	projectm_pcm_add_float(projectM, data, samples,
                           PROJECTM_STEREO);
}

void CProjectMVis::AddInt16(int16_t * data,int samples){
	float dstbuf[samples*2] = {0};
	for(int i=0;i<samples*2;i++){
		dstbuf[i] = (float)data[i]/(1<<15);
	}
	projectm_pcm_add_float(projectM, dstbuf, samples,
                           PROJECTM_STEREO);
}

void CProjectMVis::NextVisPreset(){
	projectm_playlist_play_next(playlist, true);
}

void CProjectMVis::PrevVisPreset(){
	projectm_playlist_play_previous(playlist, true);
}

bool CProjectMVis::ToogleVis(){
	vis_enabled = !vis_enabled;
	return vis_enabled;
}

bool CProjectMVis::VisEnabled(){
	return vis_enabled;
}

std::vector<std::string> CProjectMVis::getPlaylistItems(){
	std::vector<std::string> retvec;
	char ** playlist_items = projectm_playlist_items(playlist,0,projectm_playlist_size(playlist));
	int count = 0;
	while(true){
		if(playlist_items[count] == NULL)break;
		std::string entry = playlist_items[count++];
		retvec.push_back(entry);
	}
	return retvec;
}

std::string CProjectMVis::getCurrentPlaylistItem(){
	if(projectm_playlist_size(playlist) > 0){
		std::string fullpath = projectm_playlist_item(playlist,projectm_playlist_get_position(playlist));
		return fullpath.substr(fullpath.find_last_of("/")+1);	
	}
	return "";
}


		