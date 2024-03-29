/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int gappx     = 8;        /* gaps between windows */
static const unsigned int snap      = 16;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 6;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;     /* 0 means no bar */
static const int topbar             = 1;     /* 0 means bottom bar */
static const int horizpadbar        = 0;        /* horizontal padding for statusbar */
static const int vertpadbar         = 6;        /* vertical padding for statusbar */
#define ICONSIZE ( bh - 4 )   /* icon size */
#define ICONSPACING 8 /* space between icon and title */
static const char *fonts[]          = { "nkmono:size=12", "Font Awesome 6 Free:size=12:style=solid" };
static const char dmenufont[]       = "nkmono:size=12";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#dddddd";
static const char col_gray4[]       = "#ffffff";
static const char col_cyan[]        = "#005577";
static const char col_purple[]      = "#6921cf";
static const char *colors[][3]      = {
	/*               fg           bg           border   */
	[SchemeNorm] = { col_gray3,   col_gray1,   col_gray2   },
	[SchemeSel]  = { col_gray4,   col_purple,  col_purple  },
	[SchemeHid]  = { col_purple,  col_gray1,   col_purple  },
};

static const char *const autostart[] = {
	"mpv", "--no-video", "--volume=70", "/home/ninekex/sfx/windows_xp_startup.mp3", NULL,
	NULL /* terminate */
};

/* tagging */
static const char *tags[] = { "", "", "", "", "", "", "", "", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
    /* monitor 1 is the second one and 0 is the primary -1 is the current one i guess*/
	/* class         instance    title                  tags mask     isfloating   monitor */
	{ "Gimp",        NULL,       NULL,                  0,            1,           -1 },
	{ "firefox",     NULL,       NULL,                  1,            0,            0 },
	{ "Spotify",     NULL,       NULL,                  1,            0,            1 },
	{ "firefox",     "Toolkit",  "Picture-in-Picture",  1,            1,            0 },
	{ "discord",     NULL,       NULL,                  1 << 3,       0,            0 },
	{ "Blender",     NULL,       "Blender Preferences", 0,            1,            0 },
	{ "tidal-hifi",  NULL,       NULL,                  1,            0,            1 },
	{ "krita",       NULL,       NULL,                  1 << 8,       0,            0 },
	{ "Cairo-clock", NULL,       NULL,                  1 << 8,       1,            0 },
	{ "MultiMC",     NULL,       NULL,                  1 << 1,       0,            0 },
	{ "Steam",       NULL,       NULL,                  1 << 7,       0,            0 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int decorhints  = 1;    /* 1 means respect decoration hints */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]  = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_purple, "-sf", col_gray4, NULL };
static const char *termcmd[]   = { "st", NULL };
static const char *filecmd[]   = { "nautilus", NULL };
static const char *kritacmd[]  = { "krita", NULL };
static const char *ffcmd[]     = { "firefox", NULL };
static const char *scrotcmd[]  = { "sc", NULL };
static const char *scrotcmdS[] = { "scs", NULL };
static const char *killdwm[]   = { "pkill", "dwm", NULL };

static const char *upvol[]     = { "amixer", "-q", "sset", "Master", "2%+", NULL };
static const char *downvol[]   = { "amixer", "-q", "sset", "Master", "2%-", NULL };
static const char *mute[]      = { "amixer", "-q", "-D", "pulse", "sset", "Master", "toggle", NULL };
static const char *play[]      = { "playerctl", "--ignore-player=firefox", "play-pause", NULL };
static const char *stop[]      = { "playerctl", "--ignore-player=firefox", "stop", NULL };
static const char *nextS[]     = { "playerctl", "--ignore-player=firefox", "next", NULL };
static const char *prevS[]     = { "playerctl", "--ignore-player=firefox", "previous", NULL };
/* static const char *scrotcmdS[] = { "scrot", "~/Pictures/screenshots/'%Y-%m-%d_$wx$h.png'", "-s", NULL }; */

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_y,      spawn,          {.v = filecmd } },
	{ MODKEY|ShiftMask,             XK_r,      spawn,          {.v = kritacmd } },
	{ MODKEY|ShiftMask,             XK_w,      spawn,          {.v = ffcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstackvis,  {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstackvis,  {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,      focusstackhid,  {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      focusstackhid,  {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_x,      setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },
	{ MODKEY|ShiftMask,             XK_minus,  setgaps,        {.i = gappx } },
	{ MODKEY,                       XK_s,      show,           {0} },
	{ MODKEY,                       XK_u,      hide,           {0} },
	{ 0,                            XK_Print,  spawn,          { .v = scrotcmd  } },
	{ MODKEY,                       XK_Print,  spawn,          { .v = scrotcmdS } },
    { MODKEY,                       XK_n,      spawn,          { .v = play    } },
    { 0,             XF86XK_AudioRaiseVolume,  spawn,          { .v = upvol   } },
    { 0,             XF86XK_AudioLowerVolume,  spawn,          { .v = downvol } },
    { 0,             XF86XK_AudioMute,         spawn,          { .v = mute    } },
    { 0,             XF86XK_AudioPlay,         spawn,          { .v = play    } },
    { 0,             XF86XK_AudioStop,         spawn,          { .v = stop    } },
    { 0,             XF86XK_AudioNext,         spawn,          { .v = nextS   } },
    { 0,             XF86XK_AudioPrev,         spawn,          { .v = prevS   } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ShiftMask,             XK_F4,     spawn,          { .v = killdwm } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button1,        togglewin,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

