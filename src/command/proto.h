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

/// @file proto.h
/// @brief Command prototypes.
/// @ingroup command

#include "aegisub/context.h"

namespace cmd {

typedef std::map<const char*, void(*)(agi::Context*)> cmdMap;
typedef std::pair<const char*, void(*)(agi::Context*)> cmdPair;

void am_manager(agi::Context *c);
void app_about(agi::Context *c);
void app_display_audio_subs(agi::Context *c);
void app_display_full(agi::Context *c);
void app_display_subs(agi::Context *c);
void app_display_video_subs(agi::Context *c);
void app_exit(agi::Context *c);
void app_language(agi::Context *c);
void app_log(agi::Context *c);
void app_new_window(agi::Context *c);
void app_options(agi::Context *c);
void app_updates(agi::Context *c);
void audio_close(agi::Context *c);
void audio_open(agi::Context *c);
void audio_open_blank(agi::Context *c);
void audio_open_noise(agi::Context *c);
void audio_open_video(agi::Context *c);
void audio_view_spectrum(agi::Context *c);
void audio_view_waveform(agi::Context *c);
void edit_line_copy(agi::Context *c);
void edit_line_cut(agi::Context *c);
void edit_line_delete(agi::Context *c);
void edit_line_duplicate(agi::Context *c);
void edit_line_duplicate_shift(agi::Context *c);
void edit_line_join_as_karaoke(agi::Context *c);
void edit_line_join_concatenate(agi::Context *c);
void edit_line_join_keep_first(agi::Context *c);
void edit_line_paste(agi::Context *c);
void edit_line_paste_over(agi::Context *c);
void edit_line_recombine(agi::Context *c);
void edit_line_split_by_karaoke(agi::Context *c);
void edit_line_swap(agi::Context *c);
void edit_redo(agi::Context *c);
void edit_search_replace(agi::Context *c);
void edit_undo(agi::Context *c);
void grid_line_next(agi::Context *c);
void grid_line_prev(agi::Context *c);
void grid_tag_cycle_hiding(agi::Context *c);
void grid_tags_hide(agi::Context *c);
void grid_tags_show(agi::Context *c);
void grid_tags_simplify(agi::Context *c);
void help_bugs(agi::Context *c);
void help_contents(agi::Context *c);
void help_files(agi::Context *c);
void help_forums(agi::Context *c);
void help_irc(agi::Context *c);
void help_website(agi::Context *c);
void keyframe_close(agi::Context *c);
void keyframe_open(agi::Context *c);
void keyframe_save(agi::Context *c);
void medusa_enter(agi::Context *c);
void medusa_next(agi::Context *c);
void medusa_play(agi::Context *c);
void medusa_play_after(agi::Context *c);
void medusa_play_before(agi::Context *c);
void medusa_previous(agi::Context *c);
void medusa_shift_end_back(agi::Context *c);
void medusa_shift_end_forward(agi::Context *c);
void medusa_shift_start_back(agi::Context *c);
void medusa_shift_start_forward(agi::Context *c);
void medusa_stop(agi::Context *c);
void main_audio(agi::Context *c);
void main_automation(agi::Context *c);
void main_edit(agi::Context *c);
void main_edit_sort_lines(agi::Context *c);
void main_file(agi::Context *c);
void main_help(agi::Context *c);
void main_subtitle(agi::Context *c);
void main_subtitle_insert_lines(agi::Context *c);
void main_subtitle_join_lines(agi::Context *c);
void main_subtitle_sort_lines(agi::Context *c);
void main_timing(agi::Context *c);
void main_timing_make_times_continous(agi::Context *c);
void main_video(agi::Context *c);
void main_video_override_ar(agi::Context *c);
void main_video_set_zoom(agi::Context *c);
void main_view(agi::Context *c);
void recent_audio(agi::Context *c);
void recent_keyframe(agi::Context *c);
void recent_subtitle(agi::Context *c);
void recent_timecode(agi::Context *c);
void recent_video(agi::Context *c);
void subtitle_attachment(agi::Context *c);
void subtitle_find(agi::Context *c);
void subtitle_find_next(agi::Context *c);
void subtitle_insert_after(agi::Context *c);
void subtitle_insert_after_videotime(agi::Context *c);
void subtitle_insert_before(agi::Context *c);
void subtitle_insert_before_videotime(agi::Context *c);
void subtitle_new(agi::Context *c);
void subtitle_open(agi::Context *c);
void subtitle_open_charset(agi::Context *c);
void subtitle_open_video(agi::Context *c);
void subtitle_properties(agi::Context *c);
void subtitle_save(agi::Context *c);
void subtitle_save_as(agi::Context *c);
void subtitle_select_visible(agi::Context *c);
void subtitle_spellcheck(agi::Context *c);
void subtitle_tags_show(agi::Context *c);
void time_continous_end(agi::Context *c);
void time_continous_start(agi::Context *c);
void time_frame_current(agi::Context *c);
void time_shift(agi::Context *c);
void time_snap_end_video(agi::Context *c);
void time_snap_frame(agi::Context *c);
void time_snap_scene(agi::Context *c);
void time_snap_start_video(agi::Context *c);
void time_sort_end(agi::Context *c);
void time_sort_start(agi::Context *c);
void time_sort_style(agi::Context *c);
void timecode_close(agi::Context *c);
void timecode_open(agi::Context *c);
void timecode_save(agi::Context *c);
void tool_assdraw(agi::Context *c);
void tool_export(agi::Context *c);
void tool_font_collector(agi::Context *c);
void tool_line_select(agi::Context *c);
void tool_resampleres(agi::Context *c);
void tool_style_assistant(agi::Context *c);
void tool_style_manager(agi::Context *c);
void tool_time_kanji(agi::Context *c);
void tool_time_postprocess(agi::Context *c);
void tool_translation_assistant(agi::Context *c);
void video_aspect_cinematic(agi::Context *c);
void video_aspect_custom(agi::Context *c);
void video_aspect_default(agi::Context *c);
void video_aspect_full(agi::Context *c);
void video_aspect_wide(agi::Context *c);
void video_close(agi::Context *c);
void video_detach(agi::Context *c);
void video_details(agi::Context *c);
void video_focus_seek(agi::Context *c);
void video_frame_next(agi::Context *c);
void video_frame_play(agi::Context *c);
void video_frame_prev(agi::Context *c);
void video_jump(agi::Context *c);
void video_jump_end(agi::Context *c);
void video_jump_start(agi::Context *c);
void video_open(agi::Context *c);
void video_open_dummy(agi::Context *c);
void video_show_overscan(agi::Context *c);
void video_zoom_100(agi::Context *c);
void video_zoom_200(agi::Context *c);
void video_zoom_50(agi::Context *c);
void video_zoom_in(agi::Context *c);
void video_zoom_out(agi::Context *c);

} // namespace cmd
