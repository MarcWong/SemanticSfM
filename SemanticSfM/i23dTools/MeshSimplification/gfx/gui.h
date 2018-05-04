#ifndef GFXGUI_INCLUDED // -*- C++ -*-
#define GFXGUI_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Minimalist GUI framework.

  This package implements a baseline GUI framework for use in
  GFX-based applications.  Only a very specific kind of interface is
  supported: one where the application window consists primarily of an
  OpenGL drawing canvas.

  $Id: gui.h 443 2005-06-14 00:53:40Z garland $

 ************************************************************************/

#include "gl.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Output.H>

namespace gfx
{

class MxGUI;

class MxGLCanvas : public Fl_Gl_Window
{
private:
    int last_click[2];
    MxGUI *app;

public:
    // Override selected FLTK window methods
    //
    virtual void draw();
    virtual int handle(int event);
    virtual void resize(int x, int y, int w, int h);

public:
    MxGLCanvas(int x, int y, int w, int h, const char *label=NULL);
    void attach_app(MxGUI *a);
};

class MxGUI
{
private:
    int w_offset, h_offset;
    Fl_Window *create_window(int xw=640, int yw=480, int pad=5);

public:
    // This is the public interface of MxGUI available to the application.
    //
    Fl_Window *toplevel;
    MxGLCanvas *canvas;
    Fl_Output *status_line;
    Fl_Menu_Bar *menu_bar;
    Fl_Menu_Item *menu_layout;
    float default_fps, target_fps;

    static MxGUI *current;	// There should only be one.

    MxGUI();
    virtual ~MxGUI() {}

    virtual void initialize(int argc, char **argv,
			    Fl_Menu_Item *layout=NULL,
			    int xw=640, int yw=480);
    virtual int run();

    int status(const char *fmt, ...);
    void animate(bool will);
    bool snapshot_to_file(int format, const char *filenamep=NULL);
    void resize_canvas(int width, int height);
    void lock_size();
    void unlock_size();

    void title(const char *l) { toplevel->label(l); }

    // Menu construction and standard callbacks
    int add_menu(const std::string&, int key, Fl_Callback *cb, int flags=0);
    int add_toggle_menu(const std::string&, int key, bool& val, int flags=0);
    static void cb_toggle(Fl_Menu_ *m, bool *flag);

public:
    //
    // Callback functions that get executed in response to menu commands.
    virtual void cb_new();
    virtual void cb_exit();
    virtual void cb_snapshot(int);
    virtual void cb_animate(Fl_Menu_ *m);
    virtual void cb_fps();
    virtual void cb_vga_size(int width);  // uses 4:3  aspect ratio
    virtual void cb_hdtv_size(int width); // uses 16:9 aspect ratio
    virtual void cb_dv_size(int width);   // uses 3:2  aspect ratio

    virtual void cb_save_view_to_file();
    virtual void cb_load_view_from_file();
    virtual bool save_view_to_file();
    virtual bool load_view_from_file();

public:
    //
    // Applications are customized by overriding the following methods.

    // Override these methods to control the contents of the GL canvas
    virtual void setup_for_drawing();
    virtual void draw_contents();
    virtual void update_animation();

    // Override these methods to receive events from the GL canvas
    virtual bool mouse_down(int *where, int which);
    virtual bool mouse_up(int *where, int which);
    virtual bool mouse_drag(int *where, int *last, int which);
    virtual bool key_press(int key);

    // Override these methods to get command line arguments
    virtual int cmdline_option(int argc, char **argv, int& index);
    virtual void cmdline_file(const char *file);

    // Override these methods to add custom interface elements
    virtual void add_upper_controls(int& yfill, const int pad) {}
    virtual void add_lower_controls(int& yfill, const int pad) {}

    // Override this method to free memory, close files, etc.
    virtual void cleanup_for_exit() {}
};

////////////////////////////////////////////////////////////////////////
//
// This template makes it easier to create FLTK-compliant callbacks.
// In particular, its purpose is to construct static thunks for
// calling member functions of MxGUI-derived classes.
//

template<class Gui>
struct MxBinder
{
    typedef void (Gui::*GuiCommand)();
    typedef void (Gui::*GuiCommand1)(int);
    typedef void (Gui::*GuiCommand2)(Fl_Menu_ *);

    template<GuiCommand cmd>
    static void to(Fl_Widget *, void *data)
    {
	Gui *gui = static_cast<Gui*>(data);
	(gui->*cmd)();
	gui->canvas->redraw();
    }

    template<GuiCommand2 cmd>
    static void to_menu(Fl_Widget *w, void *data)
    {
	Gui *gui = static_cast<Gui*>(data);
	(gui->*cmd)(static_cast<Fl_Menu_ *>(w));
	gui->canvas->redraw();
    }

    template<GuiCommand1 cmd, int i>
    static void to_arg(Fl_Widget *, void *data)
    {
	Gui *gui = static_cast<Gui*>(data);
	(gui->*cmd)(i);
	gui->canvas->redraw();
    }
};

////////////////////////////////////////////////////////////////////////
//
// These macros make static FLTK menu definitions look a little nicer.
//

#define MXGUI_BEGIN_MENU(name) {name, 0, 0, 0, FL_SUBMENU},
#define MXGUI_END_MENU {0},
#define MXGUI_FINISH_MENUBAR {0}

} // namespace gfx

// GFXGUI_INCLUDED
#endif
