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

typedef std::map<std::string, Command*> cmdMap;
typedef std::pair<std::string, Command*> cmdPair;

cmdMap map;

int id(std::string name) {

	cmdMap::iterator index;

	if ((index = map.find(name)) != map.end()) {
		int id = std::distance(map.begin(), index);
		return id;
	}
	// XXX: throw
	printf("cmd::id NOT FOUND (%s)\n", name.c_str());
	return 60003;
}

void call(agi::Context *c, const int id) {

	cmdMap::iterator index(map.begin());
	std::advance(index, id);

	if (index != map.end()) {
		LOG_D("event/command") << index->first << " " << "(Id: " << id << ")";
		(*index->second)(c);
	} else {
		LOG_W("event/command/not_found") << "EVENT ID NOT FOUND: " << id;
		// XXX: throw
	}
}

int count() {
	return map.size();
}

#define CMD_INSERT(a) \
	Command *a##_obj = new a(); \
	map.insert(cmdPair(a::name(), a##_obj));


void command_init() {
	LOG_D("command/init") << "Populating command map";

	CMD_INSERT(am_manager)
	CMD_INSERT(app_about)
	CMD_INSERT(app_display_audio_subs)
	CMD_INSERT(app_display_full)
	CMD_INSERT(app_display_subs)
	CMD_INSERT(app_display_video_subs)
	CMD_INSERT(app_exit)
	CMD_INSERT(app_language)
	CMD_INSERT(app_log)
	CMD_INSERT(app_new_window)
	CMD_INSERT(app_options)
	CMD_INSERT(app_updates)
	CMD_INSERT(audio_close)
	CMD_INSERT(audio_open)
	CMD_INSERT(audio_open_blank)
	CMD_INSERT(audio_open_noise)
	CMD_INSERT(audio_open_video)
	CMD_INSERT(audio_view_spectrum)
	CMD_INSERT(audio_view_waveform)
	CMD_INSERT(edit_line_copy)
	CMD_INSERT(edit_line_cut)
	CMD_INSERT(edit_line_delete)
	CMD_INSERT(edit_line_duplicate)
	CMD_INSERT(edit_line_duplicate_shift)
	CMD_INSERT(edit_line_join_as_karaoke)
	CMD_INSERT(edit_line_join_concatenate)
	CMD_INSERT(edit_line_join_keep_first)
	CMD_INSERT(edit_line_paste)
	CMD_INSERT(edit_line_paste_over)
	CMD_INSERT(edit_line_recombine)
	CMD_INSERT(edit_line_split_by_karaoke)
	CMD_INSERT(edit_line_swap)
	CMD_INSERT(edit_redo)
	CMD_INSERT(edit_search_replace)
	CMD_INSERT(edit_undo)
	CMD_INSERT(grid_line_next)
	CMD_INSERT(grid_line_prev)
	CMD_INSERT(grid_tag_cycle_hiding)
	CMD_INSERT(grid_tags_hide)
	CMD_INSERT(grid_tags_show)
	CMD_INSERT(grid_tags_simplify)
	CMD_INSERT(help_bugs)
	CMD_INSERT(help_contents)
	CMD_INSERT(help_files)
	CMD_INSERT(help_forums)
	CMD_INSERT(help_irc)
	CMD_INSERT(help_website)
	CMD_INSERT(keyframe_close)
	CMD_INSERT(keyframe_open)
	CMD_INSERT(keyframe_save)
	CMD_INSERT(main_audio)
	CMD_INSERT(main_automation)
	CMD_INSERT(main_edit)
	CMD_INSERT(main_edit_sort_lines)
	CMD_INSERT(main_file)
	CMD_INSERT(main_help)
	CMD_INSERT(main_main_subtitle)
	CMD_INSERT(main_subtitle_insert_lines)
	CMD_INSERT(main_subtitle_join_lines)
	CMD_INSERT(main_subtitle_sort_lines)
	CMD_INSERT(main_timing)
	CMD_INSERT(main_timing_make_times_continous)
	CMD_INSERT(main_video)
	CMD_INSERT(main_video_set_zoom)
	CMD_INSERT(main_view)
	CMD_INSERT(medusa_enter)
	CMD_INSERT(medusa_next)
	CMD_INSERT(medusa_play)
	CMD_INSERT(medusa_play_after)
	CMD_INSERT(medusa_play_before)
	CMD_INSERT(medusa_previous)
	CMD_INSERT(medusa_shift_end_back)
	CMD_INSERT(medusa_shift_end_forward)
	CMD_INSERT(medusa_shift_start_back)
	CMD_INSERT(medusa_shift_start_forward)
	CMD_INSERT(medusa_stop)
	CMD_INSERT(recent_audio)
	CMD_INSERT(recent_keyframe)
	CMD_INSERT(recent_subtitle)
	CMD_INSERT(recent_timecode)
	CMD_INSERT(recent_video)
	CMD_INSERT(subtitle_attachment)
	CMD_INSERT(subtitle_find)
	CMD_INSERT(subtitle_find_next)
	CMD_INSERT(subtitle_insert_after)
	CMD_INSERT(subtitle_insert_after_videotime)
	CMD_INSERT(subtitle_insert_before)
	CMD_INSERT(subtitle_insert_before_videotime)
	CMD_INSERT(subtitle_new)
	CMD_INSERT(subtitle_open)
	CMD_INSERT(subtitle_open_charset)
	CMD_INSERT(subtitle_open_video)
	CMD_INSERT(subtitle_properties)
	CMD_INSERT(subtitle_save)
	CMD_INSERT(subtitle_save_as)
	CMD_INSERT(subtitle_select_visiblek)
	CMD_INSERT(subtitle_spellcheck)
	CMD_INSERT(subtitle_tags_show)
	CMD_INSERT(time_continous_end)
	CMD_INSERT(time_continous_start)
	CMD_INSERT(time_frame_current)
	CMD_INSERT(time_shift)
	CMD_INSERT(time_snap_end_video)
	CMD_INSERT(time_snap_frame)
	CMD_INSERT(time_snap_scene)
	CMD_INSERT(time_snap_start_video)
	CMD_INSERT(time_sort_end)
	CMD_INSERT(time_sort_start)
	CMD_INSERT(time_sort_style)
	CMD_INSERT(timecode_close)
	CMD_INSERT(timecode_open)
	CMD_INSERT(timecode_save)
	CMD_INSERT(tool_assdraw)
	CMD_INSERT(tool_export)
	CMD_INSERT(tool_font_collector)
	CMD_INSERT(tool_line_select)
	CMD_INSERT(tool_resampleres)
	CMD_INSERT(tool_style_assistant)
	CMD_INSERT(tool_style_manager)
	CMD_INSERT(tool_time_kanji)
	CMD_INSERT(tool_time_postprocess)
	CMD_INSERT(tool_translation_assistant)
	CMD_INSERT(video_aspect_cinematic)
	CMD_INSERT(video_aspect_custom)
	CMD_INSERT(video_aspect_default)
	CMD_INSERT(video_aspect_full)
	CMD_INSERT(video_aspect_wide)
	CMD_INSERT(video_close)
	CMD_INSERT(video_detach)
	CMD_INSERT(video_details)
	CMD_INSERT(video_focus_seek)
	CMD_INSERT(video_frame_next)
	CMD_INSERT(video_frame_play)
	CMD_INSERT(video_frame_prev)
	CMD_INSERT(video_jump)
	CMD_INSERT(video_jump_end)
	CMD_INSERT(video_jump_start)
	CMD_INSERT(video_open)
	CMD_INSERT(video_open_dummy)
	CMD_INSERT(video_show_overscan)
	CMD_INSERT(video_zoom_100)
	CMD_INSERT(video_zoom_200)
	CMD_INSERT(video_zoom_50)
	CMD_INSERT(video_zoom_in)
	CMD_INSERT(video_zoom_out)
}

} // namespace cmd
