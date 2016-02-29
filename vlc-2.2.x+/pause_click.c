/*****************************************************************************
 * pause_click.c : A filter that allows to pause/play a video by a mouse click
 *****************************************************************************
 * Copyright (C) 2014 Maxim Biro
 *
 * Authors: Maxim Biro <nurupo.contributions@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# define N_(str) (str)
#endif

#define UNUSED(x) (void)(x)

#include <vlc_common.h>
#include <vlc_filter.h>
#include <vlc_interface.h>
#include <vlc_mouse.h>
#include <vlc_playlist.h>
#include <vlc_plugin.h>

int OpenFilter(vlc_object_t *);
int OpenInterface(vlc_object_t *);

intf_thread_t *p_intf = NULL;

#define TO_CHAR(num) ( 'A' + (char)(num) )
#define FROM_CHAR(c) ((int)( (c) - 'A' ))

static const char buttons[] = {
    TO_CHAR(MOUSE_BUTTON_LEFT),
    TO_CHAR(MOUSE_BUTTON_CENTER),
    TO_CHAR(MOUSE_BUTTON_RIGHT),
    TO_CHAR(MOUSE_BUTTON_WHEEL_UP),
    TO_CHAR(MOUSE_BUTTON_WHEEL_DOWN),
    TO_CHAR(MOUSE_BUTTON_WHEEL_LEFT),
    TO_CHAR(MOUSE_BUTTON_WHEEL_RIGHT),
    0
    };

static const char *const ppsz_buttons_values[] = {
    buttons,
    buttons+1,
    buttons+2,
    buttons+3,
    buttons+4,
    buttons+5,
    buttons+6
    };

static const char *const ppsz_buttons_descriptions[] = {
    "Left Button",
    "Middle Button",
    "Right Button",
    "Scroll Up",
    "Scroll Down",
    "Scroll Left",
    "Scroll Right",
    };

#define MOUSE_BUTTON "mouse-button-setting"
#define MOUSE_BUTTON_TEXT N_("Mouse Gesture")
#define MOUSE_BUTTON_LONGTEXT N_("Defines the mouse gesture that will pause/play the video")
#define MOUSE_BUTTON_DEFAULT (buttons+1)
int mouse_btn = MOUSE_BUTTON_CENTER;

vlc_module_begin()
    set_description("Pause/Play video on mouse click")
    set_shortname("Pause-on-click")
    set_capability("video filter2", 0)
    set_category(CAT_VIDEO)
    set_subcategory(SUBCAT_VIDEO_VFILTER)
    set_callbacks(OpenFilter, NULL)
        add_submodule()
        set_capability("interface", 0)
        set_category(CAT_INTERFACE)
        set_subcategory(SUBCAT_INTERFACE_CONTROL)
	add_string(MOUSE_BUTTON, MOUSE_BUTTON_DEFAULT, MOUSE_BUTTON_TEXT, MOUSE_BUTTON_LONGTEXT, false)
        	change_string_list(ppsz_buttons_values, ppsz_buttons_descriptions)
        set_callbacks(OpenInterface, NULL)
vlc_module_end()

int mouse(filter_t *p_filter, vlc_mouse_t *p_mouse_out, const vlc_mouse_t *p_mouse_old, const vlc_mouse_t *p_mouse_new)
{
    UNUSED(p_filter);
    UNUSED(p_mouse_out);

    if (p_intf != NULL && vlc_mouse_HasPressed(p_mouse_old, p_mouse_new, mouse_btn)) {
        playlist_t* p_playlist = pl_Get(p_intf);
        playlist_Control(p_playlist, (playlist_Status(p_playlist) == PLAYLIST_RUNNING ? PLAYLIST_PAUSE : PLAYLIST_PLAY), 0);
    }

    // don't propagate any mouse change
    return VLC_EGENERIC;
}

picture_t *filter(filter_t *p_filter, picture_t *p_pic_in)
{
    UNUSED(p_filter);

    // don't alter picture
    return p_pic_in;
}

int OpenFilter(vlc_object_t *p_this)
{
    filter_t *p_filter = (filter_t *)p_this;

    p_filter->pf_video_filter = filter;
    p_filter->pf_video_mouse = mouse;

    return VLC_SUCCESS;
}

int OpenInterface(vlc_object_t *p_this)
{
    p_intf = (intf_thread_t*) p_this;

    char* psz_btn = var_CreateGetStringCommand(p_intf, MOUSE_BUTTON);
    mouse_btn = FROM_CHAR(psz_btn[0]);
    free(psz_btn);

    return VLC_SUCCESS;
}
