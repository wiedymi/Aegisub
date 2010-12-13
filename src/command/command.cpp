// Copyright (c) 2010, Amar Takhar <verm@aegisub.org>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// $Id$

/// @file command.cpp
/// @brief Command system base file.
/// @ingroup command

#include "command.h"
#include "proto.h"
#include <libaegisub/log.h>

namespace cmd {

cmdMap cmd_map;

int id(std::string name) {

	cmdMap::iterator index;

//	if ((index = cmd_map.find(name.c_str())) != cmd_map.end()) {
//  XXX: the above does not work, wtf?
	for (index = cmd_map.begin(); index != cmd_map.end(); index++) {
		if (index->first == name) {
			int id = std::distance(cmd_map.begin(), index);
//			printf("cmd::id %d (%s)\n", id, index->first);
			return id;
		}
	}

	printf("cmd::id NOT FOUND (%s)\n", name.c_str());
	return 60003;
}

void call(agi::Context *c, const int id) {
	cmdMap::iterator index(cmd::cmd_map.begin());
	std::advance(index, id);

	if (index != cmd::cmd_map.end()) {
		LOG_D("event/command") << index->first << " " << "(Id: " << id << ")";
		(index->second)(c);
	} else {
		LOG_W("event/command/not_found") << "EVENT ID NOT FOUND: " << id;
	}
}

int count() {
	return cmd_map.size();
}

void command_init() {
	LOG_D("command/init") << "Populating command map";

	cmd_map.insert(cmdPair("am/manager", &am_manager));
	cmd_map.insert(cmdPair("app/about", &app_about));
	cmd_map.insert(cmdPair("app/display/audio_subs", &app_display_audio_subs));
	cmd_map.insert(cmdPair("app/display/full", &app_display_full));
	cmd_map.insert(cmdPair("app/display/subs", &app_display_subs));
	cmd_map.insert(cmdPair("app/display/video_subs", &app_display_video_subs));
	cmd_map.insert(cmdPair("app/exit", &app_exit));
	cmd_map.insert(cmdPair("app/language", &app_language));
	cmd_map.insert(cmdPair("app/log", &app_log));
	cmd_map.insert(cmdPair("app/new_window", &app_new_window));
	cmd_map.insert(cmdPair("app/options", &app_options));
	cmd_map.insert(cmdPair("app/updates", &app_updates));
	cmd_map.insert(cmdPair("audio/close", &audio_close));
	cmd_map.insert(cmdPair("audio/open", &audio_open));
	cmd_map.insert(cmdPair("audio/open/blank", &audio_open_blank));
	cmd_map.insert(cmdPair("audio/open/noise", &audio_open_noise));
	cmd_map.insert(cmdPair("audio/open/video", &audio_open_video));
	cmd_map.insert(cmdPair("audio/view/spectrum", &audio_view_spectrum));
	cmd_map.insert(cmdPair("audio/view/waveform", &audio_view_waveform));
	cmd_map.insert(cmdPair("edit/line/copy", &edit_line_copy));
	cmd_map.insert(cmdPair("edit/line/cut", &edit_line_cut));
	cmd_map.insert(cmdPair("edit/line/delete", &edit_line_delete));
	cmd_map.insert(cmdPair("edit/line/duplicate", &edit_line_duplicate));
	cmd_map.insert(cmdPair("edit/line/duplicate/shift", &edit_line_duplicate_shift));
	cmd_map.insert(cmdPair("edit/line/join/as_karaoke", &edit_line_join_as_karaoke));
	cmd_map.insert(cmdPair("edit/line/join/concatenate", &edit_line_join_concatenate));
	cmd_map.insert(cmdPair("edit/line/join/keep_first", &edit_line_join_keep_first));
	cmd_map.insert(cmdPair("edit/line/paste", &edit_line_paste));
	cmd_map.insert(cmdPair("edit/line/paste/over", &edit_line_paste_over));
	cmd_map.insert(cmdPair("edit/line/recombine", &edit_line_recombine));
	cmd_map.insert(cmdPair("edit/line/split/by_karaoke", &edit_line_split_by_karaoke));
	cmd_map.insert(cmdPair("edit/line/swap", &edit_line_swap));
	cmd_map.insert(cmdPair("edit/redo", &edit_redo));
	cmd_map.insert(cmdPair("edit/search_replace", &edit_search_replace));
	cmd_map.insert(cmdPair("edit/undo", &edit_undo));
	cmd_map.insert(cmdPair("grid/line/next", &grid_line_next));
	cmd_map.insert(cmdPair("grid/line/prev", &grid_line_prev));
	cmd_map.insert(cmdPair("grid/tag/cycle_hiding", &grid_tag_cycle_hiding));
	cmd_map.insert(cmdPair("grid/tags/hide", &grid_tags_hide));
	cmd_map.insert(cmdPair("grid/tags/show", &grid_tags_show));
	cmd_map.insert(cmdPair("grid/tags/simplify", &grid_tags_simplify));
	cmd_map.insert(cmdPair("help/bugs", &help_bugs));
	cmd_map.insert(cmdPair("help/contents", &help_contents));
	cmd_map.insert(cmdPair("help/files", &help_files));
	cmd_map.insert(cmdPair("help/forums", &help_forums));
	cmd_map.insert(cmdPair("help/irc", &help_irc));
	cmd_map.insert(cmdPair("help/website", &help_website));
	cmd_map.insert(cmdPair("keyframe/close", &keyframe_close));
	cmd_map.insert(cmdPair("keyframe/open", &keyframe_open));
	cmd_map.insert(cmdPair("keyframe/save", &keyframe_save));
	cmd_map.insert(cmdPair("main/audio", &main_audio));
	cmd_map.insert(cmdPair("main/automation", &main_automation));
	cmd_map.insert(cmdPair("main/edit", &main_edit));
	cmd_map.insert(cmdPair("main/edit/sort lines", &main_edit_sort_lines));
	cmd_map.insert(cmdPair("main/file", &main_file));
	cmd_map.insert(cmdPair("main/help", &main_help));
	cmd_map.insert(cmdPair("main/subtitle", &main_subtitle));
	cmd_map.insert(cmdPair("main/subtitle/insert lines", &main_subtitle_insert_lines));
	cmd_map.insert(cmdPair("main/subtitle/sort lines", &main_subtitle_sort_lines));
	cmd_map.insert(cmdPair("main/subtitle/join lines", &main_subtitle_join_lines));
	cmd_map.insert(cmdPair("main/timing", &main_timing));
	cmd_map.insert(cmdPair("main/timing/make times continous", &main_timing_make_times_continous));
	cmd_map.insert(cmdPair("main/video", &main_video));
	cmd_map.insert(cmdPair("main/video/override ar", &main_video_override_ar));
	cmd_map.insert(cmdPair("main/video/set zoom", &main_video_set_zoom));
	cmd_map.insert(cmdPair("main/view", &main_view));
	cmd_map.insert(cmdPair("medusa/enter", &medusa_enter));
	cmd_map.insert(cmdPair("medusa/next", &medusa_next));
	cmd_map.insert(cmdPair("medusa/play", &medusa_play));
	cmd_map.insert(cmdPair("medusa/play/after", &medusa_play_after));
	cmd_map.insert(cmdPair("medusa/play/before", &medusa_play_before));
	cmd_map.insert(cmdPair("medusa/previous", &medusa_previous));
	cmd_map.insert(cmdPair("medusa/shift/end/back", &medusa_shift_end_back));
	cmd_map.insert(cmdPair("medusa/shift/end/forward", &medusa_shift_end_forward));
	cmd_map.insert(cmdPair("medusa/shift/start/back", &medusa_shift_start_back));
	cmd_map.insert(cmdPair("medusa/shift/start/forward", &medusa_shift_start_forward));
	cmd_map.insert(cmdPair("medusa/stop", &medusa_stop));
	cmd_map.insert(cmdPair("subtitle/attachment", &subtitle_attachment));
	cmd_map.insert(cmdPair("subtitle/find", &subtitle_find));
	cmd_map.insert(cmdPair("subtitle/find/next", &subtitle_find_next));
	cmd_map.insert(cmdPair("subtitle/insert/after", &subtitle_insert_after));
	cmd_map.insert(cmdPair("subtitle/insert/after/videotime", &subtitle_insert_after_videotime));
	cmd_map.insert(cmdPair("subtitle/insert/before", &subtitle_insert_before));
	cmd_map.insert(cmdPair("subtitle/insert/before/videotime", &subtitle_insert_before_videotime));
	cmd_map.insert(cmdPair("subtitle/new", &subtitle_new));
	cmd_map.insert(cmdPair("subtitle/open", &subtitle_open));
	cmd_map.insert(cmdPair("subtitle/open/charset", &subtitle_open_charset));
	cmd_map.insert(cmdPair("subtitle/open/video", &subtitle_open_video));
	cmd_map.insert(cmdPair("subtitle/properties", &subtitle_properties));
	cmd_map.insert(cmdPair("subtitle/save", &subtitle_save));
	cmd_map.insert(cmdPair("subtitle/save/as", &subtitle_save_as));
	cmd_map.insert(cmdPair("subtitle/select/visible", &subtitle_select_visible));
	cmd_map.insert(cmdPair("subtitle/spellcheck", &subtitle_spellcheck));
	cmd_map.insert(cmdPair("subtitle/tags/show", &subtitle_tags_show));
	cmd_map.insert(cmdPair("time/continous/end", &time_continous_end));
	cmd_map.insert(cmdPair("time/continous/start", &time_continous_start));
	cmd_map.insert(cmdPair("time/frame/current", &time_frame_current));
	cmd_map.insert(cmdPair("time/shift", &time_shift));
	cmd_map.insert(cmdPair("time/snap/end_video", &time_snap_end_video));
	cmd_map.insert(cmdPair("time/snap/frame", &time_snap_frame));
	cmd_map.insert(cmdPair("time/snap/scene", &time_snap_scene));
	cmd_map.insert(cmdPair("time/snap/start_video", &time_snap_start_video));
	cmd_map.insert(cmdPair("time/sort/end", &time_sort_end));
	cmd_map.insert(cmdPair("time/sort/start", &time_sort_start));
	cmd_map.insert(cmdPair("time/sort/style", &time_sort_style));
	cmd_map.insert(cmdPair("timecode/close", &timecode_close));
	cmd_map.insert(cmdPair("timecode/open", &timecode_open));
	cmd_map.insert(cmdPair("timecode/save", &timecode_save));
	cmd_map.insert(cmdPair("tool/assdraw", &tool_assdraw));
	cmd_map.insert(cmdPair("tool/export", &tool_export));
	cmd_map.insert(cmdPair("tool/font_collector", &tool_font_collector));
	cmd_map.insert(cmdPair("tool/line/select", &tool_line_select));
	cmd_map.insert(cmdPair("tool/resampleres", &tool_resampleres));
	cmd_map.insert(cmdPair("tool/style/assistant", &tool_style_assistant));
	cmd_map.insert(cmdPair("tool/style/manager", &tool_style_manager));
	cmd_map.insert(cmdPair("tool/time/kanji", &tool_time_kanji));
	cmd_map.insert(cmdPair("tool/time/postprocess", &tool_time_postprocess));
	cmd_map.insert(cmdPair("tool/translation_assistant", &tool_translation_assistant));
	cmd_map.insert(cmdPair("video/aspect/cinematic", &video_aspect_cinematic));
	cmd_map.insert(cmdPair("video/aspect/custom", &video_aspect_custom));
	cmd_map.insert(cmdPair("video/aspect/default", &video_aspect_default));
	cmd_map.insert(cmdPair("video/aspect/full", &video_aspect_full));
	cmd_map.insert(cmdPair("video/aspect/wide", &video_aspect_wide));
	cmd_map.insert(cmdPair("video/close", &video_close));
	cmd_map.insert(cmdPair("video/detach", &video_detach));
	cmd_map.insert(cmdPair("video/details", &video_details));
	cmd_map.insert(cmdPair("video/focus_seek", &video_focus_seek));
	cmd_map.insert(cmdPair("video/frame/next", &video_frame_next));
	cmd_map.insert(cmdPair("video/frame/play", &video_frame_play));
	cmd_map.insert(cmdPair("video/frame/prev", &video_frame_prev));
	cmd_map.insert(cmdPair("video/jump", &video_jump));
	cmd_map.insert(cmdPair("video/jump/end", &video_jump_end));
	cmd_map.insert(cmdPair("video/jump/start", &video_jump_start));
	cmd_map.insert(cmdPair("video/open", &video_open));
	cmd_map.insert(cmdPair("video/open/dummy", &video_open_dummy));
	cmd_map.insert(cmdPair("video/show_overscan", &video_show_overscan));
	cmd_map.insert(cmdPair("video/zoom/100", &video_zoom_100));
	cmd_map.insert(cmdPair("video/zoom/200", &video_zoom_200));
	cmd_map.insert(cmdPair("video/zoom/50", &video_zoom_50));
	cmd_map.insert(cmdPair("video/zoom/in", &video_zoom_in));
	cmd_map.insert(cmdPair("video/zoom/out", &video_zoom_out));


}

} // namespace cmd
