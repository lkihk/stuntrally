#include "pch.h"
#include "settings_com.h"
#include <stdio.h>


void SETcom::SerializeCommon(bool w, CONFIGFILE & c)
{
	//  menu
	Param(c,w, "game.start_in_main", startInMain);
	Param(c,w, "game.in_menu", inMenu);				Param(c,w, "game.in_main", isMain);
	
	//  misc
	Param(c,w, "misc.autostart", autostart);		Param(c,w, "misc.ogredialog", ogre_dialog);
	Param(c,w, "misc.escquit", escquit);
	
	Param(c,w, "misc.language", language);			Param(c,w, "misc.mouse_capture", mouse_capture);
	Param(c,w, "misc.screenshot_png", screen_png);

	//  video
	Param(c,w, "video.windowx", windowx);			Param(c,w, "video.windowy", windowy);
	Param(c,w, "video.fullscreen", fullscreen);		Param(c,w, "video.vsync", vsync);

	Param(c,w, "video.fsaa", fsaa);
	Param(c,w, "video.buffer", buffer);				Param(c,w, "video.rendersystem", rendersystem);

	Param(c,w, "video.limit_fps", limit_fps);
	Param(c,w, "video.limit_fps_val", limit_fps_val);	Param(c,w, "video.limit_sleep", limit_sleep);


	//  graphics  ----
	Param(c,w, "graph_detail.preset", preset);
	Param(c,w, "graph_shadow.shader_mode", shader_mode);	Param(c,w, "graph_shadow.lightmap_size", lightmap_size);
	
	Param(c,w, "graph_detail.anisotropy", anisotropy);
	Param(c,w, "graph_detail.tex_filter", tex_filt);		Param(c,w, "graph_detail.tex_size", tex_size);
	Param(c,w, "graph_detail.ter_mtr", ter_mtr);			Param(c,w, "graph_detail.ter_tripl", ter_tripl);
	
	Param(c,w, "graph_detail.view_dist", view_distance);	Param(c,w, "graph_detail.ter_detail", terdetail);
	Param(c,w, "graph_detail.ter_dist", terdist);			Param(c,w, "graph_detail.road_dist", road_dist);
	
	//  water
	Param(c,w, "graph_reflect.water_reflect", water_reflect); Param(c,w, "graph_reflect.water_refract", water_refract);
	Param(c,w, "graph_reflect.water_rttsize", water_rttsize);
	
	//  shadow
	Param(c,w, "graph_shadow.dist", shadow_dist);			Param(c,w, "graph_shadow.size", shadow_size);
	Param(c,w, "graph_shadow.count",shadow_count);			Param(c,w, "graph_shadow.type", shadow_type);

	//  veget
	Param(c,w, "graph_veget.grass", grass);
	Param(c,w, "graph_veget.trees_dist", trees_dist);		Param(c,w, "graph_veget.grass_dist", grass_dist);
	Param(c,w, "graph_veget.use_imposters", use_imposters); Param(c,w, "graph_veget.imposters_only", imposters_only);


	//  hud
	Param(c,w, "hud_show.fps", show_fps);

	//  gui tracks  ---
	Param(c,w, "gui_tracks.view", tracks_view);
	Param(c,w, "gui_tracks.sort", tracks_sort);		Param(c,w, "gui_tracks.sortup", tracks_sortup);

	//  cols, filt
	/*int i,v;
	std::string s;
	gui_tracks
	for (v=0; v<2; ++v)
	{
		s = "";
		for (i=0; i<18; ++i)
		{

		}
	}/**/
}


SETcom::SETcom()   ///  Defaults
	//  menu
	:isMain(1), startInMain(1), inMenu(0)

	//  misc
	,autostart(0), ogre_dialog(0), escquit(0)
	,language("")  // "" = autodetect lang
	,mouse_capture(true), screen_png(0)

	//  video
	,windowx(800), windowy(600)
	,fullscreen(false), vsync(false)

	,buffer("FBO"), fsaa(0)
	,rendersystem("Default")

	,limit_fps(0), limit_fps_val(60.f), limit_sleep(-1)

	//  graphics
	,preset(4)
	,shader_mode(""), horizon(0), lightmap_size(0) //-

	,tex_filt(2), anisotropy(4), view_distance(2000.f)
	,terdetail(1.f), terdist(300.f), road_dist(1.f)
	,tex_size(1), ter_mtr(2), ter_tripl(0)

	,water_reflect(0), water_refract(0), water_rttsize(0)
	,shadow_type(Sh_Depth), shadow_size(2), shadow_count(3), shadow_dist(1000.f)
	,grass(1.f), trees_dist(1.f), grass_dist(1.f), use_imposters(true), imposters_only(false)

	//  hud
	,show_fps(0)

	//  gui tracks
	,tracks_view(0), tracks_sort(2), tracks_sortup(1)
{

	//  tracks list columns  --
	const static bool colVis[2][18] =
	{{0,0,1, 0,0,0, 1,1, 0,0,0,0,0,0,0,0,0,0},
	 {1,0,1, 1,1,1, 1,1, 1,1,1,1,1,1,1,1,1,1}};
	int i,v;
	for (v=0; v<2; ++v)
	for (i=0; i<18; ++i)
		col_vis[v].push_back(colVis[v][i]);
		
	const static char colFil[2][13] =
	{{01, 0,0, 0,0,0,0,0,0,0,0,0,0},
	 {30, 9,9, 9,9,9,9,9,9,9,9,9,9}};
	for (v=0; v<2; ++v)
	for (i=0; i<18; ++i)
		col_fil[v].push_back(colFil[v][i]);
}