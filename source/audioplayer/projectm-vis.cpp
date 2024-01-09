#include "projectm-vis.h"



CProjectMVis::CProjectMVis(unsigned int _width,unsigned int _height,std::string _milkdir,std::string _texturedir){
	width = _width;
	height = _height;
	projectM = projectm_create();
	projectm_set_window_size(projectM, width, height);
	projectm_set_aspect_correction(projectM,  true);
	projectm_set_preset_duration(projectM,  1000);
	
	playlist = projectm_playlist_create(projectM);
	milkdir = _milkdir;
	texturedir = _texturedir;
	
	if (!texturedir.empty())
        {
            const char* texturePathList[1]{&texturedir[0]};
            projectm_set_texture_search_paths(projectM, texturePathList, 1);
        }
	
	if (!milkdir.empty())
	{
		projectm_playlist_add_path(playlist, milkdir.c_str(), true, false);
		projectm_playlist_sort(playlist, 0, projectm_playlist_size(playlist), SORT_PREDICATE_FILENAME_ONLY, SORT_ORDER_ASCENDING);
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

void CProjectMVis::ViewSpectrum(){
	projectm_load_preset_file(projectM,"romfs:/presets/milk/spectrum.milk",false);
	
}