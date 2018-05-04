#ifndef GFXGUIMFC_INCLUDED // -*- C++ -*-
#define GFXGUIMFC_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Minimalist GUI framework built using MFC.

  This package mimics the baseline GUI framework originally implemented
  on top of FLTK in <gui.h>.  It remains incomplete, and does not
  support all the features provided by the FLTK-based version.

  At this point, the two GUI implementations are close to
  source-compatible, but not quite.  The main difference is in the
  startup issues -- main() vs. InitInstance().

  $Id: gui-mfc.h 427 2004-09-27 04:45:31Z garland $

 ************************************************************************/

#include "mfc.h"
#include "../gl.h"

namespace gfx
{


class Canvas : public CFrameWnd
{
private:
    int last_click[2];

public:
    Canvas();

    CStatusBar *status_line;

private:
    int pixfmt;
    HGLRC glcontext;

protected:
    inline void make_current(HDC dc) { wglMakeCurrent(dc, glcontext); }
    inline void finish(HDC dc) { SwapBuffers(dc); }

    void immediate_redraw();

    int decode_mouse_button(UINT flags, int which=0);
    void do_mouse_down(int which, UINT flags, CPoint where);
    void do_mouse_up(int which, UINT flags, CPoint where);
    void do_mouse_move(UINT flags, CPoint where);

public:
    void post_redraw();

protected:
    //
    // Override selected MFC virtual functions
    //
    BOOL PreCreateWindow(CREATESTRUCT &cs);

protected:
    //
    // Define MFC event handlers
    //
    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT type, int width, int height);

    afx_msg void OnActivate(UINT state, CWnd *other, BOOL is_minimized);
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC *dc);

    afx_msg void OnLButtonDown(UINT flags, CPoint point);
    afx_msg void OnLButtonUp(UINT flags, CPoint point);
    afx_msg void OnRButtonDown(UINT flags, CPoint point);
    afx_msg void OnRButtonUp(UINT flags, CPoint point);
    afx_msg void OnMButtonDown(UINT flags, CPoint point);
    afx_msg void OnMButtonUp(UINT flags, CPoint point);
    afx_msg void OnMouseMove(UINT flags, CPoint point);
    afx_msg void OnChar(UINT ch, UINT repcount, UINT flags);

    DECLARE_MESSAGE_MAP()
};


class MfcGUI : public CWinApp
{
private:
    UINT timer_id;

public:
    MfcGUI();
    
    virtual BOOL InitInstance();

    Canvas *canvas;
    float default_fps, target_fps;

    void status(const char *format, ...);

    void animate(bool will=true);

public:

    virtual void update_animation();
    virtual void setup_for_drawing();
    virtual void draw_contents();
    virtual bool mouse_down(int *where, int which);
    virtual bool mouse_up(int *where, int which);
    virtual bool mouse_drag(int *where, int *last, int which);
    virtual bool key_press(int key);
};

} // namespace gfx

// GFXGUIMFC_INCLUDED
#endif
