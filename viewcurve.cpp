/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Teunis van Beelen
*
* teuniz@gmail.com
*
***************************************************************************
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
***************************************************************************
*
* This version of GPL is at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*
***************************************************************************
*/



#include "viewcurve.h"


ViewCurve::ViewCurve(QWidget *w_parent) : QWidget(w_parent)
{

  setAttribute(Qt::WA_OpaquePaintEvent);

  mainwindow = (UI_Mainwindow *)w_parent;

  special_pen = new QPen(Qt::SolidPattern, 0, Qt::DotLine, Qt::SquareCap, Qt::BevelJoin);

  annot_marker_pen = new QPen(Qt::SolidPattern, 0, Qt::DashLine, Qt::SquareCap, Qt::BevelJoin);

  epoch_marker_pen = new QPen(Qt::SolidPattern, 0, Qt::DashLine, Qt::SquareCap, Qt::BevelJoin);

  epoch_marker_fat_pen = new QPen(Qt::SolidPattern, 3, Qt::DashLine, Qt::SquareCap, Qt::BevelJoin);

  signal_pen = new QPen(Qt::SolidPattern, 2, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);

  ruler_pen = new QPen(Qt::SolidPattern, 0, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);

  active_markers = (struct active_markersblock *)calloc(1, sizeof(struct active_markersblock));

  use_move_events = 0;
  sidemenu_active = 0;
  draw_zoom_rectangle = 0;
  printing = 0;
  pressed_on_label = 0;
  spanning = 0;

  original_sensitivity = (double *)calloc(1, sizeof(double[MAXSIGNALS]));

  backgroundcolor = Qt::lightGray;
  small_ruler_color = Qt::black;
  big_ruler_color = Qt::darkGray;
  mouse_rect_color = Qt::black;
  text_color = Qt::black;
  signal_color = Qt::blue;
  baseline_color = Qt::darkGray;
  crosshair_1.color = Qt::red;
  crosshair_2.color = Qt::cyan;
  floating_ruler_color = Qt::red;
  annot_marker_color = Qt::white;
  epoch_marker_color = Qt::red;

  crosshair_1.active = 0;
  crosshair_2.active = 0;
  ruler_active = 0;
  crosshair_1.moving = 0;
  crosshair_2.moving = 0;
  ruler_moving = 0;

  crosshair_1.value = 0.0;
  crosshair_2.value = 0.0;

  printsize_x_factor=1.0,
  printsize_y_factor=1.0;

  printfont = new QFont;
  printfont->setFamily("Arial");

  screensamples = (int *)calloc(1, sizeof(int[MAXSIGNALS]));

  cpu_cnt = QThread::idealThreadCount();
  if(cpu_cnt < 1) cpu_cnt = 1;
  if(cpu_cnt > MAXSIGNALS) cpu_cnt = MAXSIGNALS;

  for(int i=0; i<cpu_cnt; i++)
  {
    thr[i] = new drawCurve_stage_1_thread;
  }

  graphicBuf = NULL;
  printBuf = NULL;
  graphicBufWidth = 0;
  blackwhite_printing = 1;
  floating_ruler_value = 0;
}



ViewCurve::~ViewCurve()
{
  if(graphicBuf!=NULL) free(graphicBuf);

  if(active_markers!=NULL) free(active_markers);

  if(original_sensitivity!=NULL) free(original_sensitivity);

  if(screensamples!=NULL) free(screensamples);

  delete printfont;
  delete annot_marker_pen;
  delete epoch_marker_pen;
  delete special_pen;
  delete signal_pen;
  delete ruler_pen;

  for(int i=0; i<cpu_cnt; i++) delete thr[i];
}


void ViewCurve::wheelEvent(QWheelEvent *wheel_event)
{
  long long l_tmp, trshld=100LL;

  if(mainwindow->mousewheelsens < 1) return;

  if(mainwindow->files_open == 0) return;

  if(QApplication::keyboardModifiers() == Qt::ControlModifier and mainwindow->stiffness == 0) // if control key is pressed.
  {
    if(wheel_event->delta() > 0)  // zoom in
    {
      if( (mainwindow->viewtime_sync == VIEWTIME_SYNCED_OFFSET) || (mainwindow->viewtime_sync == VIEWTIME_SYNCED_ABSOLUT) || (mainwindow->viewtime_sync == VIEWTIME_USER_DEF_SYNCED) )
      {
        for(int i=0; i<mainwindow->files_open; i++)
        {
          mainwindow->edfheaderlist[i]->viewtime += (mainwindow->pagetime / 4);       // move quarter a page.

          l_tmp = mainwindow->edfheaderlist[i]->viewtime % TIME_DIMENSION;

          if(l_tmp < trshld)
          {
            mainwindow->edfheaderlist[i]->viewtime -= l_tmp;
          }

          if(l_tmp > (TIME_DIMENSION - trshld))
          {
            mainwindow->edfheaderlist[i]->viewtime += (TIME_DIMENSION - l_tmp);
          }
        }
      }

      if(mainwindow->viewtime_sync == VIEWTIME_UNSYNCED)
      {
        mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime += (mainwindow->pagetime / 4);  // move quarter a page.

        l_tmp = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime % TIME_DIMENSION;

        if(l_tmp < trshld)
        {
          mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime -= l_tmp;
        }

        if(l_tmp > (TIME_DIMENSION - trshld))
        {
          mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime += (TIME_DIMENSION - l_tmp);
        }
      }

      mainwindow->set_pagetime(mainwindow->pagetime / 2);
    }
    else        // zoom out
    {
      if( (mainwindow->viewtime_sync==VIEWTIME_SYNCED_OFFSET) || (mainwindow->viewtime_sync==VIEWTIME_SYNCED_ABSOLUT) || (mainwindow->viewtime_sync==VIEWTIME_USER_DEF_SYNCED) )
      {
        for(int i=0; i<mainwindow->files_open; i++)
        {
          mainwindow->edfheaderlist[i]->viewtime -= (mainwindow->pagetime / 2);


          l_tmp = mainwindow->edfheaderlist[i]->viewtime % TIME_DIMENSION;

          if(l_tmp < trshld)
          {
            mainwindow->edfheaderlist[i]->viewtime -= l_tmp;
          }

          if(l_tmp > (TIME_DIMENSION - trshld))
          {
            mainwindow->edfheaderlist[i]->viewtime += (TIME_DIMENSION - l_tmp);
          }
        }
      }

      if(mainwindow->viewtime_sync==VIEWTIME_UNSYNCED)
      {
        mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime -= (mainwindow->pagetime / 2);

        l_tmp = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime % TIME_DIMENSION;

        if(l_tmp < trshld)
        {
          mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime -= l_tmp;
        }

        if(l_tmp > (TIME_DIMENSION - trshld))
        {
          mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime += (TIME_DIMENSION - l_tmp);
        }
      }

      mainwindow->set_pagetime(2*mainwindow->pagetime);
    }
  }
  else if(mainwindow->stiffness != 2) // moving (stiffness=1), everything (stiffness=0) allowed.
  {
    if( (mainwindow->viewtime_sync==VIEWTIME_SYNCED_OFFSET) || (mainwindow->viewtime_sync==VIEWTIME_SYNCED_ABSOLUT) || (mainwindow->viewtime_sync==VIEWTIME_USER_DEF_SYNCED) )
    {
      for(int i=0; i<mainwindow->files_open; i++)
      {
        if(wheel_event->delta() > 0)
        {
          mainwindow->edfheaderlist[i]->viewtime -= (mainwindow->pagetime / mainwindow->mousewheelsens);
        }
        else
        {
          mainwindow->edfheaderlist[i]->viewtime += (mainwindow->pagetime / mainwindow->mousewheelsens);
        }
      }
    }

    if(mainwindow->viewtime_sync==VIEWTIME_UNSYNCED)
    {
      if(wheel_event->delta() > 0)
      {
        mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime -= (mainwindow->pagetime / mainwindow->mousewheelsens);
      }
      else
      {
        mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime += (mainwindow->pagetime / mainwindow->mousewheelsens);
      }
    }
  }
  else  // stiffness=2 -> moving only a full page.
  {
    if( (mainwindow->viewtime_sync==VIEWTIME_SYNCED_OFFSET) || (mainwindow->viewtime_sync==VIEWTIME_SYNCED_ABSOLUT) || (mainwindow->viewtime_sync==VIEWTIME_USER_DEF_SYNCED) )
    {
      for(int i=0; i<mainwindow->files_open; i++)
      {
        if(wheel_event->delta() > 0)
        {
          mainwindow->edfheaderlist[i]->viewtime -= mainwindow->pagestep;
        }
        else
        {
          mainwindow->edfheaderlist[i]->viewtime += mainwindow->pagestep;
        }
      }
    }

    if(mainwindow->viewtime_sync==VIEWTIME_UNSYNCED)
    {
      if(wheel_event->delta() > 0)
      {
        mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime -= mainwindow->pagetime;
      }
      else
      {
        mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime += mainwindow->pagetime;
      }
    }
  }

  mainwindow->setup_viewbuf();
}


void ViewCurve::mousePressEvent(QMouseEvent *press_event)
{
  int i, j,
      baseline,
      signalcomps,
      m_x,
      m_y,
      h_size;

  struct signalcompblock **signalcomp;

  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  signalcomps = mainwindow->signalcomps;
  signalcomp = mainwindow->signalcomp;
  m_x = press_event->x();
  m_y = press_event->y();

  mouse_press_coordinate_x = m_x;
  mouse_press_coordinate_y = m_y;

  pressed_on_label = 0;

  h_size = (int)(4.0 / mainwindow->pixelsizefactor);

  setFocus(Qt::MouseFocusReason);

  if(press_event->button() == Qt::LeftButton) // Start spanning; Open signalmenu.
  {
    crosshair_1.moving = 0;
    crosshair_2.moving = 0;
    ruler_moving = 0;

    use_move_events = 1;
    setMouseTracking(true);

    mouse_old_x = m_x;
    mouse_old_y = m_y;

    if(ruler_active)
    {
      if((m_y>(ruler_y_position + h_size + 10))&&(m_y<(ruler_y_position  + h_size + 30))&&(m_x>ruler_x_position)&&(m_x<(ruler_x_position + 60)))
      {
        if(floating_ruler_value)
        {
          floating_ruler_value = 0;
        }
        else
        {
          floating_ruler_value = 1;
        }

        update();

        return;
      }
      else if((m_y>ruler_y_position)&&(m_y<(ruler_y_position + ((int)(4.0 / mainwindow->pixelsizefactor))))&&(m_x>ruler_x_position)&&(m_x<(ruler_x_position + 150 + (w / 5))))
      {
        ruler_moving = 1;
      }
    }

    if(!ruler_moving)
    {
      if(crosshair_1.active)
      {
        if( (m_y<crosshair_1.y_position) && (m_y>(crosshair_1.y_position - 51)) && (m_x>crosshair_1.x_position) && (m_x<(crosshair_1.x_position + 171)) )
        {
          crosshair_1.moving = 1;
        }

        if( (m_x>crosshair_1.x_position-10) && (m_x<(crosshair_1.x_position+10)) )
        {
          crosshair_1.moving = 1;
        }
      }
      else  // if crosshair_1.active == 0
      {
        signal_nr = ymouseOverSignal(m_y, -30, 30);
        if( (m_x>110) && signal_nr != -1 )  // If not daneben geklickt ...
        {
          crosshair_1.value = 0.;         // ???
          crosshair_1.active = 1;         // first crosshair active.
          crosshair_1.x_position = m_x;       // position should be where clicked.
          crosshair_1.y_position = m_y+100;     // position of the annotation should be below.
          mainwindow->signalcomp[signal_nr]->hascursor1 = 1;  // cursors are assigned to respective signal.
          mainwindow->signalcomp[signal_nr]->hascursor2 = 1;  // ...
          crosshair_2.active = 1;         // second crosshair active.
          crosshair_2.moving = 1;         // .. and moving.
          crosshair_2.x_position = m_x;       // .. position should start where the cursor is.
          crosshair_2.y_position = m_y+100;     // y_position also below the signal.
          spanning = 1;           // tell the viewcurve that we are spanning something.
        }
      }
    }

    if( (!ruler_moving) && (!crosshair_1.moving) )
    {
      if(crosshair_2.active)
      {
        if((m_y<crosshair_2.y_position)&&(m_y>(crosshair_2.y_position - 80))&&(m_x>crosshair_2.x_position)&&(m_x<(crosshair_2.x_position + 171)))
        {
          crosshair_2.moving = 1;
        }

        if( (m_x>crosshair_2.x_position-10) && (m_x<crosshair_2.x_position + 10) )
        {
          crosshair_2.moving = 1;
        }
      }
    }

    if(mainwindow->annot_editor_active || mainwindow->epoch_editor_active)
    {
      if((!ruler_moving) && (!crosshair_1.moving) && (!crosshair_2.moving)) // If neither ruler nor crosshairs move ...
      {
        for(i=0; i<active_markers->count; i++)
        {

          if( (m_x > active_markers->list[i]->x_pos-5) && (m_x < active_markers->list[i]->x_pos+5) )  // and mouse within range of annotation i
          {
            active_markers->selected = i;
            active_markers->list[i]->selected = 1;
            mainwindow->annotationEditDock->set_selected_annotation(active_markers->list[i]);
            break;
          }

        }
      }
    }

    if( (!ruler_moving) && (!crosshair_1.moving) && (!crosshair_2.moving) ) // If nothing moves ...
    {
      if( (m_x>3) && (m_x<110) )
      {
        if( (signal_nr=ymouseOverSignal(m_y)) != -1)  // If not daneben geklickt ...
        {
          original_screen_offset = mainwindow->signalcomp[signal_nr]->screen_offset;
          mainwindow->signalcomp[signal_nr]->hasoffsettracking = 1;
          pressed_on_label = signal_nr + 1;
          label_press_y_position = m_y;
        }
      }
    }

    if( (!pressed_on_label) && (!ruler_moving) && (!crosshair_1.moving) && (!crosshair_2.moving) )
    {
      // draw_zoom_rectangle = 1; We don't want to be able to zoom.  There's no way back!

      mouse_x = m_x;
      mouse_y = m_y;
    }
  }

  if(press_event->button()==Qt::RightButton)
  {
    for(i=0; i<signalcomps; i++)
    {
      signalcomp[i]->hascursor1 = 0;
      signalcomp[i]->hascursor2 = 0;
      signalcomp[i]->hasoffsettracking = 0;
    }
    crosshair_1.active = 0;
    crosshair_2.active = 0;
    crosshair_1.moving = 0;
    crosshair_2.moving = 0;
    use_move_events = 0;
    setMouseTracking(false);

    for(i=0; i<signalcomps; i++)
    {
      baseline = h / (signalcomps + 1);
      baseline *= (i + 1);

      if((m_y<(baseline-5))&&(m_y>(baseline-24))&&(m_x>3)&&(m_x<110))
      {
        for(j=0; j<signalcomp[i]->num_of_signals; j++)
        {
          original_sensitivity[j] = signalcomp[i]->sensitivity[j];
        }
        original_screen_offset = signalcomp[i]->screen_offset;
        signalcomp[i]->hasgaintracking = 1;
        use_move_events = 1;
        setMouseTracking(true);
        signal_nr = i;

        break;
      }
    }

    if(!use_move_events)
    {
      for(i=0; i<signalcomps; i++)
      {
        signalcomp[i]->hasruler = 0;
      }
      ruler_active = 0;
      ruler_moving = 0;

      update();
    }
  }
}



int ViewCurve::ymouseOverSignal(int m_y, int ymin, int ymax)  // returns which signal.  ymin,ymax:  distances from baseline of signal.
{
  int baseline;

  for(int i=0; i<mainwindow->signalcomps; i++)
  {
    baseline = (i + 1) * h / (mainwindow->signalcomps + 1);

    if( (m_y<baseline-ymin) and (m_y>baseline-ymax) )
    {
      return i;
    }
  }
  return -1;
}



void ViewCurve::mouseReleaseEvent(QMouseEvent *release_event)
{
  int i, j,
      baseline,
      signalcomps,
      m_x,
      m_y;

  double zoomfactor;

  struct signalcompblock **signalcomp;

  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  signalcomps = mainwindow->signalcomps;
  signalcomp = mainwindow->signalcomp;
  m_x = release_event->x();
  m_y = release_event->y();

  if(release_event->button() == Qt::LeftButton)
  {
    if(crosshair_1.active) mainwindow->annotationEditDock->annotEditSetOnset(crosshair_1.time_relative);

    if(crosshair_2.active) mainwindow->annotationEditDock->annotEditSetDuration(crosshair_2.time_relative - crosshair_1.time_relative);

    ruler_moving = 0;
    crosshair_1.moving = 0;
    crosshair_2.moving = 0;
    use_move_events = 0;
    setMouseTracking(false);

    if(draw_zoom_rectangle)
    {
      draw_zoom_rectangle = 0;

      if((m_x>(mouse_press_coordinate_x + 50))&&(m_y > mouse_press_coordinate_y + 50))
      {
        for(i=0; i<mainwindow->files_open; i++)
        {
          mainwindow->zoomhistory->viewtime[mainwindow->zoomhistory->pntr][i] = mainwindow->edfheaderlist[i]->viewtime;
        }
        mainwindow->zoomhistory->pagetime[mainwindow->zoomhistory->pntr] = mainwindow->pagetime;
        for(i=0; i<signalcomps; i++)
        {
          mainwindow->zoomhistory->voltpercm[mainwindow->zoomhistory->pntr][i] = signalcomp[i]->voltpercm;
          mainwindow->zoomhistory->screen_offset[mainwindow->zoomhistory->pntr][i] = signalcomp[i]->screen_offset;

          for(j=0; j<signalcomp[i]->num_of_signals; j++)
          {
            mainwindow->zoomhistory->sensitivity[mainwindow->zoomhistory->pntr][i][j] = signalcomp[i]->sensitivity[j];
          }
        }
        mainwindow->zoomhistory->pntr++;
        mainwindow->zoomhistory->pntr %= 64;

        for(i=0; i<mainwindow->files_open; i++)
        {
          mainwindow->edfheaderlist[i]->viewtime += (long long)(((double)mainwindow->pagetime / (double)w) * (double)mouse_press_coordinate_x);
        }
        mainwindow->pagetime = (long long)((double)mainwindow->pagetime / ((double)w / (double)(m_x - mouse_press_coordinate_x)));
        if(mainwindow->pagetime<1)  mainwindow->pagetime = 1;

        zoomfactor = (double)h / (double)(m_y - mouse_press_coordinate_y);

        for(i=0; i<signalcomps; i++)
        {
          mainwindow->signalcomp[i]->screen_offset = mainwindow->signalcomp[i]->screen_offset * zoomfactor;
          mainwindow->signalcomp[i]->screen_offset += (((double)h * (zoomfactor - 1.0) * (double)(i + 1.0)) / (double)(signalcomps + 1.0));
          mainwindow->signalcomp[i]->screen_offset -= ((double)mouse_press_coordinate_y * zoomfactor);

          mainwindow->signalcomp[i]->voltpercm = mainwindow->signalcomp[i]->voltpercm / ((double)h / (double)(m_y - mouse_press_coordinate_y));

          for(j=0; j<mainwindow->signalcomp[i]->num_of_signals; j++)
          {
            mainwindow->signalcomp[i]->sensitivity[j] =  mainwindow->signalcomp[i]->sensitivity[j] * ((double)h / (double)(m_y - mouse_press_coordinate_y));
          }
        }

        mainwindow->zoomhistory->history_size_tail++;
        mainwindow->zoomhistory->history_size_front = 0;

        mainwindow->setup_viewbuf();
      }
      else
      {
        update();
      }
    }

    for(i=0; i<signalcomps; i++)
    {
      if(signalcomp[i]->hasoffsettracking)
      {
        signalcomp[i]->hasoffsettracking = 0;
        use_move_events = 0;
        setMouseTracking(false);
        update();
      }
    }

    for(i=0; i<signalcomps; i++)
    {
      baseline = h / (signalcomps + 1);
      baseline *= (i + 1);

      if((m_y<(baseline-5))&&(m_y>(baseline-24))&&(m_x>3)&&(m_x<110))
      {
        if(pressed_on_label == (i + 1))
        {
          use_move_events = 0;
          setMouseTracking(false);
          update();

          signal_nr = i;
          exec_sidemenu(i);

          break;
        }
      }
    }

    if(spanning)  // If spanning, that is: annotation markers have been set by button-pressed mouse movement.
    {
      QPoint cursorPosition = QCursor::pos();
      if(mainwindow->annot_editor_active)
        mainwindow->annotationEditDock->eventSelectionMenu(signalcomp[signal_nr]->type, cursorPosition.x(), cursorPosition.y());

      crosshair_1.active = 0;         // Deactivate crosshairs after the annotation
      crosshair_2.active = 0;         // ... has (not) been set.
      mainwindow->signalcomp[signal_nr]->hascursor1 = 0;  // Remove cursors
      mainwindow->signalcomp[signal_nr]->hascursor2 = 0;  // ... from the signal.

      spanning = 0;   // End spanning.
    }


  }

  if(release_event->button()==Qt::RightButton)
  {
    for(i=0; i<signalcomps; i++)
    {
      if(signalcomp[i]->hasgaintracking)
      {
        signalcomp[i]->hasgaintracking = 0;
        use_move_events = 0;
        setMouseTracking(false);
        update();
      }
    }
  }

  pressed_on_label = 0;
}



void ViewCurve::mouseMoveEvent(QMouseEvent *move_event)
{
  int i, j, signalcomps, delta_y;

  double d_temp;

  struct signalcompblock **signalcomp;

  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  if(use_move_events)
  {
    signalcomps = mainwindow->signalcomps;
    signalcomp = mainwindow->signalcomp;
    mouse_x = move_event->x();
    mouse_y = move_event->y();

    if(pressed_on_label)
    {
      if((mouse_y > (label_press_y_position + 10)) || (mouse_y < (label_press_y_position - 10)))
      {
        pressed_on_label = 0;
      }
    }

    if(crosshair_1.moving)
    {
      if(mouse_x<(w-100))
      {
        crosshair_1.x_position += (mouse_x - mouse_old_x);
        mouse_old_x = mouse_x;
        if(crosshair_1.x_position<0)
        {
          crosshair_1.x_position = 0;
        }
      }

      if((mouse_y<(h-30))&&(mouse_y>30))
      {
        crosshair_1.y_position += (mouse_y - mouse_old_y);
        mouse_old_y = mouse_y;
        if(crosshair_1.y_position<1)
        {
          crosshair_1.y_position = 1;
        }
      }
    }

    if(crosshair_2.moving)
    {
      if(mouse_x<(w-100))
      {
        crosshair_2.x_position += (mouse_x - mouse_old_x);
        mouse_old_x = mouse_x;
        if(crosshair_2.x_position<0)
        {
          crosshair_2.x_position = 0;
        }
      }

      if((mouse_y<(h-30))&&(mouse_y>30))
      {
        crosshair_2.y_position += (mouse_y - mouse_old_y);
        mouse_old_y = mouse_y;
        if(crosshair_2.y_position<1)
        {
          crosshair_2.y_position = 1;
        }
      }
    }

    if(ruler_moving)
    {
      if(mouse_x<(w-100))
      {
        ruler_x_position += (mouse_x - mouse_old_x);
        mouse_old_x = mouse_x;
        if(ruler_x_position<1)
        {
          ruler_x_position = 1;
        }
      }

      if(mouse_y<(h-100))
      {
        ruler_y_position += (mouse_y - mouse_old_y);
        mouse_old_y = mouse_y;
        if(ruler_y_position<1)
        {
          ruler_y_position = 1;
        }
      }
    }

    delta_y = mouse_y - mouse_press_coordinate_y;

    for(i=0; i<signalcomps; i++)
    {
      if(signalcomp[i]->hasoffsettracking)
      {
        signalcomp[i]->screen_offset = original_screen_offset + delta_y;
      }

      if(signalcomp[i]->hasgaintracking)
      {
        for(j=0; j<signalcomp[i]->num_of_signals; j++)
        {
          d_temp = original_sensitivity[j] * (1.0 + ((double)(-delta_y) / 50.0));

          if(d_temp>0.000001)
          {
            signalcomp[i]->sensitivity[j] = d_temp;

            d_temp = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].bitvalue / (signalcomp[i]->sensitivity[j] * mainwindow->pixelsizefactor);

            signalcomp[i]->voltpercm = d_temp;
          }
        }

        signalcomp[i]->screen_offset = original_screen_offset * (signalcomp[i]->sensitivity[0] / original_sensitivity[0]);
      }
    }

    if(draw_zoom_rectangle)
    {
      update();
    }
    else
    {
      drawCurve_stage_1();
    }
  }
}



void ViewCurve::paintEvent(QPaintEvent *)
{
  QPainter paint(this);

  drawCurve_stage_2(&paint);
}



void ViewCurve::print_to_printer()
{
  int i, j,
      len;

  char path[MAX_PATH_LENGTH];

  double height_factor;

  struct date_time_struct date_time;


  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  QPrinter print(QPrinter::HighResolution);

  print.setOutputFormat(QPrinter::NativeFormat);
  print.setPageSize(QPrinter::A4);
  print.setOrientation(QPrinter::Landscape);
  print.setCreator(PROGRAM_NAME);

  QPrintDialog printerdialog(&print, this);
  printerdialog.setWindowTitle("Print");

  if(printerdialog.exec()==QDialog::Accepted)
  {
    if(blackwhite_printing)
    {
      backup_colors_for_printing();
    }

    height_factor = ((double)print.pageRect().height()) / 9000.0;

    QPainter paint(&print);

    paint.translate(0, (int)(260.0 * height_factor));

    drawCurve_stage_1(&paint, print.pageRect().width() - 30, (int)((double)print.pageRect().height() - (260.0 * height_factor) - 30), 2);

    paint.translate(0, -(int)(260.0 * height_factor));

    len = strlen(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename);
    for(i=len-1; i>=0; i--)
    {
      if((mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename[i] == '/')||(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename[i] == '\\'))  break;
    }
    i++;

    strcpy(path, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename + i);
    strcat(path, "  ");
    strcat(path, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->patient);
    remove_trailing_spaces(path);

    utc_to_date_time(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime, &date_time);

    date_time.month_str[0] += 32;
    date_time.month_str[1] += 32;
    date_time.month_str[2] += 32;

    sprintf(path + strlen(path), "  %i %s %i  ",
            date_time.day,
            date_time.month_str,
            date_time.year);

    if(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->edfplus||mainwindow->edfheaderlist[mainwindow->sel_viewtime]->bdfplus)
    {
      strcat(path, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->recording + 22);
    }
    else
    {
      strcat(path, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->recording);
    }
    remove_trailing_spaces(path);

    len = strlen(path);
    for(j=0; j<len; j++)
    {
      if(path[j]=='_')
      {
        path[j] = ' ';
      }
    }

    printfont->setPixelSize((int)((double)print.pageRect().width() / 104.0));
    paint.setPen(text_color);
    paint.setFont(*printfont);

    paint.drawText(0, (int)(160.0 * height_factor), path);

    if(blackwhite_printing)
    {
      restore_colors_after_printing();
    }

    drawCurve_stage_1();
  }
}



#if QT_VERSION < 0x050000
void ViewCurve::print_to_postscript()
{
  double height_factor;

  int i, j,
      len;

  char path[MAX_PATH_LENGTH];

  struct date_time_struct date_time;


  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strcpy(path, mainwindow->recent_savedir);
    strcat(path, "/");
  }
  len = strlen(path);
  get_filename_from_path(path + len, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(path);
  strcat(path, ".ps");

  strcpy(path, QFileDialog::getSaveFileName(0, "Print to PostScript", QString::fromLocal8Bit(path), "PostScript files (*.ps *.PS)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  QPrinter print(QPrinter::HighResolution);

  print.setOutputFormat(QPrinter::PostScriptFormat);
  print.setOutputFileName(QString::fromLocal8Bit(path));
  print.setPageSize(QPrinter::A4);
  print.setOrientation(QPrinter::Landscape);
  print.setCreator(PROGRAM_NAME);

  if(blackwhite_printing)
  {
    backup_colors_for_printing();
  }

  height_factor = ((double)print.pageRect().height()) / 9000.0;

  QPainter paint(&print);

  paint.translate(0, (int)(260.0 * height_factor));

  drawCurve_stage_1(&paint, print.pageRect().width() - 30, (int)((double)print.pageRect().height() - (260.0 * height_factor) - 30), 2);

  paint.translate(0, -(int)(260.0 * height_factor));

  len = strlen(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename);
  for(i=len; i>=0; i--)
  {
    if((mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename[i] == '/')||(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename[i] == '\\'))
    {
      break;
    }
  }
  i++;

  strcpy(path, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename + i);
  strcat(path, "  ");
  strcat(path, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->patient);
  remove_trailing_spaces(path);

  utc_to_date_time(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime, &date_time);

  date_time.month_str[0] += 32;
  date_time.month_str[1] += 32;
  date_time.month_str[2] += 32;

  sprintf(path + strlen(path), "  %i %s %i  ",
          date_time.day,
          date_time.month_str,
          date_time.year);

  if(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->edfplus||mainwindow->edfheaderlist[mainwindow->sel_viewtime]->bdfplus)
  {
    strcat(path, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->recording + 22);
  }
  else
  {
    strcat(path, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->recording);
  }

  remove_trailing_spaces(path);

  len = strlen(path);
  for(j=0; j<len; j++)
  {
    if(path[j]=='_')
    {
      path[j] = ' ';
    }
  }

  printfont->setPixelSize((int)((double)print.pageRect().width() / 104.0));
  paint.setPen(text_color);
  paint.setFont(*printfont);

  paint.drawText(0, (int)(160.0 * height_factor), path);

  if(blackwhite_printing)
  {
    restore_colors_after_printing();
  }

  drawCurve_stage_1();
}
#endif



void ViewCurve::print_to_pdf()
{
  double height_factor;

  int i, j,
      len;

  char path[MAX_PATH_LENGTH];

  struct date_time_struct date_time;


  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strcpy(path, mainwindow->recent_savedir);
    strcat(path, "/");
  }
  len = strlen(path);
  get_filename_from_path(path + len, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(path);
  strcat(path, ".pdf");

  strcpy(path, QFileDialog::getSaveFileName(0, "Print to PDF", QString::fromLocal8Bit(path), "PDF files (*.pdf *.PDF)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  QPrinter print(QPrinter::HighResolution);

  print.setOutputFormat(QPrinter::PdfFormat);
  print.setOutputFileName(QString::fromLocal8Bit(path));
  print.setPageSize(QPrinter::A4);
  print.setOrientation(QPrinter::Landscape);
  print.setCreator(PROGRAM_NAME);

  if(blackwhite_printing)
  {
    backup_colors_for_printing();
  }

  height_factor = ((double)print.pageRect().height()) / 9000.0;

  QPainter paint(&print);

  paint.translate(0, (int)(260.0 * height_factor));

  drawCurve_stage_1(&paint, print.pageRect().width() - 30, (int)((double)print.pageRect().height() - (260.0 * height_factor) - 30), 2);

  paint.translate(0, -(int)(260.0 * height_factor));

  len = strlen(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename);
  for(i=len; i>=0; i--)
  {
    if((mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename[i] == '/')||(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename[i] == '\\'))
    {
      break;
    }
  }
  i++;

  strcpy(path, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename + i);
  strcat(path, "  ");
  strcat(path, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->patient);
  remove_trailing_spaces(path);

  utc_to_date_time(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime, &date_time);

  date_time.month_str[0] += 32;
  date_time.month_str[1] += 32;
  date_time.month_str[2] += 32;

  sprintf(path + strlen(path), "  %i %s %i  ",
          date_time.day,
          date_time.month_str,
          date_time.year);

  if(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->edfplus||mainwindow->edfheaderlist[mainwindow->sel_viewtime]->bdfplus)
  {
    strcat(path, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->recording + 22);
  }
  else
  {
    strcat(path, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->recording);
  }

  remove_trailing_spaces(path);

  len = strlen(path);
  for(j=0; j<len; j++)
  {
    if(path[j]=='_')
    {
      path[j] = ' ';
    }
  }

  printfont->setPixelSize((int)((double)print.pageRect().width() / 104.0));
  paint.setPen(text_color);
  paint.setFont(*printfont);

  paint.drawText(0, (int)(160.0 * height_factor), path);

  if(blackwhite_printing)
  {
    restore_colors_after_printing();
  }

  drawCurve_stage_1();
}



void ViewCurve::print_to_image(int w_img, int h_img)
{
  int i, j, len;

  char path[MAX_PATH_LENGTH],
       str[1024];

  struct date_time_struct date_time;


  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strcpy(path, mainwindow->recent_savedir);
    strcat(path, "/");
  }
  len = strlen(path);
  get_filename_from_path(path + len, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(path);
  strcat(path, ".png");

  strcpy(path, QFileDialog::getSaveFileName(0, "Print to Image", QString::fromLocal8Bit(path), "PNG (*.png *.PNG)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  QPixmap pixmap(w_img, h_img);

  QPainter paint(&pixmap);

  paint.fillRect(0, 0, w_img, h_img, backgroundcolor);

  paint.translate(0, 25);

  drawCurve_stage_1(&paint, w_img, h_img - 25, 0);

  paint.translate(0, -25);

  len = strlen(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename);
  for(i=len; i>=0; i--)
  {
    if((mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename[i] == '/')||(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename[i] == '\\'))
    {
      break;
    }
  }
  i++;

  strcpy(str, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename + i);
  strcat(str, "  ");
  strcat(str, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->patient);
  remove_trailing_spaces(str);

  utc_to_date_time(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime, &date_time);

  date_time.month_str[0] += 32;
  date_time.month_str[1] += 32;
  date_time.month_str[2] += 32;

  sprintf(str + strlen(str), "  %i %s %i  ",
          date_time.day,
          date_time.month_str,
          date_time.year);

  if(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->edfplus||mainwindow->edfheaderlist[mainwindow->sel_viewtime]->bdfplus)
  {
    strcat(str, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->recording + 22);
  }
  else
  {
    strcat(str, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->recording);
  }

  len = strlen(str);
  for(j=0; j<len; j++)
  {
    if(str[j]=='_')
    {
      str[j] = ' ';
    }
  }

  remove_trailing_spaces(str);

  printfont->setPixelSize((int)((double)w / 104.0));
  paint.setPen(text_color);
  paint.setFont(*printfont);

  paint.drawText(5, 15, str);

  pixmap.save(QString::fromLocal8Bit(path), "PNG", 90);

  drawCurve_stage_1();
}



void ViewCurve::drawCurve_stage_2(QPainter *painter, int w_width, int w_height, int print_linewidth)
{
  int i, j, x_pix=0,
            signalcomps,
            baseline,
            m_pagetime,
            vert_ruler_offset,
            vertical_distance,
            marker_x,
            marker_dur;

  char *viewbuf,
       string[256],
       str2[32];

  double pagePerSecond,
         duration;

  long long time_ppixel,
       ll_elapsed_time,
       l_time,
       l_tmp,
       d_tmp;

  struct signalcompblock **signalcomp;

  struct annotationblock *annot;

  QFont paintersfont;

  if(mainwindow->exit_in_progress) return;

  pagePerSecond = (double)TIME_DIMENSION / (double)mainwindow->pagetime;

  signalcomps = mainwindow->signalcomps;
  signalcomp = mainwindow->signalcomp;
  viewbuf = mainwindow->viewbuf;

  vertical_distance = 1;

  painter_pixelsizefactor = 1.0 / mainwindow->pixelsizefactor;

  if( !w_width || !w_height )   // If one of these INTs is zero ..
  {
    w = width();
    h = height();

    paintersfont = *mainwindow->myfont;
    paintersfont.setBold(true);
    paintersfont.setWeight(QFont::Black);
    painter->setFont(paintersfont);

    printing = 0;     // .. we aren't printing.
  }
  else
  {
    w = w_width;
    h = w_height;

    printfont->setPixelSize((int)((double)w / 104.0));

    painter->setFont(*printfont);

    printsize_x_factor = ((double)w_width) / ((double)width());
    printsize_y_factor = ((double)w_height) / ((double)height());

    painter_pixelsizefactor *= printsize_y_factor;

    printing = 1;
  }

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->sample_pixel_ratio = (double)signalcomp[i]->samples_on_screen / (double)w;
  }

  painter->fillRect(0, 0, w, h, backgroundcolor);   // Color the background.

// Color the background of annotations:
  if(mainwindow->show_annot_markers)
  {
    for(i=0; i<mainwindow->files_open; i++)
    {
      for(annot=mainwindow->annotationlist[i], j=0; annot!=NULL; annot=annot->next_annotation, j++) // For all annotations in the list
      {
        if((duration = strtod(annot->duration, NULL)) <= 0.0) continue;

        l_tmp = annot->onset - mainwindow->edfheaderlist[i]->starttime_offset;
        d_tmp = (long long)(duration * (double)TIME_DIMENSION);

        if((l_tmp+d_tmp > (mainwindow->edfheaderlist[i]->viewtime - TIME_DIMENSION)) and  // start+duration  should be larger than  page_beginning.
            (!annot->hidden) and (!annot->hidden_in_list) )
        {
          if(l_tmp > (mainwindow->edfheaderlist[i]->viewtime + mainwindow->pagetime)) break;  // Too large.  All other will be too large as well.

          l_tmp -= mainwindow->edfheaderlist[i]->viewtime;

          marker_x = (int)(( ((double)w) / mainwindow->pagetime) * l_tmp);
          marker_dur = marker_x + duration * pagePerSecond * (double)w;

          if(marker_x < 0)  marker_x = 0;
          if(marker_dur > w)  marker_dur = w;

          painter->fillRect(marker_x, 0, marker_dur-marker_x, h, annot_marker_color);
        }

      }
    }
  }

  m_pagetime = (int)(mainwindow->pagetime / TIME_DIMENSION);

  time_ppixel = mainwindow->pagetime / w;

  if(mainwindow->files_open&&mainwindow->signalcomps)
  {
    ll_elapsed_time = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime;
  }
  else
  {
    ll_elapsed_time = 0;
  }

  ruler_pen->setWidth(print_linewidth);

  if(printing)
  {
    ruler_pen->setColor(Qt::black);
    painter->setPen(*ruler_pen);
    painter->drawLine(0, 0, 0, h-1);
    painter->drawLine(0, 0, w-1, 0);
    painter->drawLine(w-1, h-1, w-1, 0);
    painter->drawLine(w-1, h-1, 0, h-1);
  }

  if(m_pagetime <= 20)    // Page shows less than 20 sec.?
  {
    ruler_pen->setColor(small_ruler_color);
    painter->setPen(*ruler_pen);

    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_DIMENSION / 10))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION / 10)))
      {
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 4);
        }
      }

      if(((ll_elapsed_time / TIME_DIMENSION)!=((ll_elapsed_time + time_ppixel) / TIME_DIMENSION)) ||
          ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time > -time_ppixel)))
      {
        if(x_pix)
        {
          ruler_pen->setColor(big_ruler_color);
          painter->setPen(*ruler_pen);
          painter->drawLine(x_pix, 0, x_pix, h);
          ruler_pen->setColor(small_ruler_color);
          painter->setPen(*ruler_pen);
        }
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 7 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 7);
        }
      }

      ll_elapsed_time += time_ppixel;
    }
  }

  else if(m_pagetime < 100)     // .. more than 20 sec, but less than 100.
  {
    ruler_pen->setColor(small_ruler_color);
    painter->setPen(*ruler_pen);

    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_DIMENSION / 5))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION / 5)))
      {
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 4);
        }
      }

      if(( (ll_elapsed_time / TIME_DIMENSION) != ((ll_elapsed_time + time_ppixel) / TIME_DIMENSION)) ||
          ( (ll_elapsed_time < time_ppixel) and (ll_elapsed_time > -time_ppixel) ))
      {
        if(x_pix)
        {
          ruler_pen->setColor(big_ruler_color);
          painter->setPen(*ruler_pen);
          painter->drawLine(x_pix, 0, x_pix, h);
          ruler_pen->setColor(small_ruler_color);
          painter->setPen(*ruler_pen);
        }
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 7 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 7);
        }
      }

      ll_elapsed_time += time_ppixel;
    }
  }

  else if(m_pagetime < 1000)    // .. more than 100 sec, but less than 1000.
  {
    ruler_pen->setColor(small_ruler_color);
    painter->setPen(*ruler_pen);

    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / 33333333LL)!=((ll_elapsed_time + time_ppixel) / 33333333LL))
      {
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 4);
        }
      }

      if(((ll_elapsed_time / (TIME_DIMENSION * 10))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 10))) ||
          ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time > -time_ppixel)))
      {
        if(x_pix)
        {
          ruler_pen->setColor(big_ruler_color);
          painter->setPen(*ruler_pen);
          painter->drawLine(x_pix, 0, x_pix, h);
          ruler_pen->setColor(small_ruler_color);
          painter->setPen(*ruler_pen);
        }
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 7 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 7);
        }
      }

      ll_elapsed_time += time_ppixel;
    }
  }

  else if(m_pagetime < 5000)  // .. more than 1000, but less than 5000
  {
    ruler_pen->setColor(small_ruler_color);
    painter->setPen(*ruler_pen);

    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_DIMENSION * 10))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 10)))
      {
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 4);
        }
      }

      if(((ll_elapsed_time / (TIME_DIMENSION * 60))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 60)))  ||
          ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time > -time_ppixel)))
      {
        if(x_pix)
        {
          ruler_pen->setColor(big_ruler_color);
          painter->setPen(*ruler_pen);
          painter->drawLine(x_pix, 0, x_pix, h);
          ruler_pen->setColor(small_ruler_color);
          painter->setPen(*ruler_pen);
        }
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 7 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 7);
        }
      }

      ll_elapsed_time += time_ppixel;
    }
  }

  else if(m_pagetime < 173000) // more than 5000, but less than 173,000
  {
    ruler_pen->setColor(small_ruler_color);
    painter->setPen(*ruler_pen);

    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_DIMENSION * 600))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 600)))
      {
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 4);
        }
      }

      if( ( (ll_elapsed_time / (TIME_DIMENSION * 3600)) != ((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 3600)) ) ||
          ( (ll_elapsed_time < time_ppixel) && (ll_elapsed_time > -time_ppixel) ) )
      {
        if(x_pix)
        {
          ruler_pen->setColor(big_ruler_color);
          painter->setPen(*ruler_pen);
          painter->drawLine(x_pix, 0, x_pix, h);
          ruler_pen->setColor(small_ruler_color);
          painter->setPen(*ruler_pen);
        }
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 7 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 7);
        }
      }

      ll_elapsed_time += time_ppixel;
    }
  }

  else if(m_pagetime < 3000000) // more than 173,000 sec, but less than 3,000,000 sec.
  {
    ruler_pen->setColor(small_ruler_color);
    painter->setPen(*ruler_pen);

    for(x_pix=0; x_pix<w; x_pix++)
    {
      if( (ll_elapsed_time / (TIME_DIMENSION * 3600)) != ((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 3600)) )
      {
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 4);
        }
      }

      if( ( (ll_elapsed_time / (TIME_DIMENSION * 86400)) != ( (ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 86400) ) ) ||
          ( (ll_elapsed_time < time_ppixel) && (ll_elapsed_time > -time_ppixel) ) )
      {
        if(x_pix)
        {
          ruler_pen->setColor(big_ruler_color);
          painter->setPen(*ruler_pen);
          painter->drawLine(x_pix, 0, x_pix, h);
          ruler_pen->setColor(small_ruler_color);
          painter->setPen(*ruler_pen);
        }
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 7 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 7);
        }
      }

      ll_elapsed_time += time_ppixel;
    }
  }

  if( (viewbuf==NULL) or (graphicBuf==NULL) or (screensamples==NULL) ) return;

  if(mainwindow->show_baselines)  // Should we plot horizontals ..
  {
    vertical_distance = h / (signalcomps + 1);      // Vertical pixels per signal.

    painter->setPen(baseline_color);

    for(i=0; i<signalcomps; i++)
    {
      baseline = vertical_distance * (i + 1);

      painter->drawLine(0, baseline, w, baseline);

      if(signalcomp[i]->voltpercm < 0.1)  strcpy(str2, "%+.3f ");
      else if(signalcomp[i]->voltpercm < 1.0) strcpy(str2, "%+.2f ");
      else if(signalcomp[i]->voltpercm < 10.0)  strcpy(str2, "%+.1f ");
      else          strcpy(str2, "%+.0f ");

      strcat(str2, signalcomp[i]->physdimension);

      for(j=1; j<18; j++) // at most
      {
        vert_ruler_offset = j * painter_pixelsizefactor;  // j centimeters above/below baseline, given in pixel?

        if(signalcomps != 1)
        {
          if(vert_ruler_offset > ((vertical_distance / 2)) - 8) break;
        }

// Check if horizontal rulers are too close to the edges of the drawing area.
        if(printing)
        {
          if(baseline + vert_ruler_offset > h-(15*printsize_y_factor))  break;
          if(baseline - vert_ruler_offset < 15*printsize_y_factor)  break;
        }
        else  // .. not printing
        {
          if(baseline + vert_ruler_offset > h-15) break;    // Too close to the top
          if(baseline - vert_ruler_offset < 15)   break;    // Too close to the bottom
        }

// Draw the ruler below baseline ..
        painter->drawLine(0, baseline - vert_ruler_offset, w, baseline - vert_ruler_offset);

// Set the annotation of the horizontal
        if(printing)
          snprintf(string, 128, str2,
                   ((signalcomp[i]->voltpercm * j) + ((signalcomp[i]->screen_offset * signalcomp[i]->voltpercm) / (painter_pixelsizefactor / printsize_y_factor))) * (double)signalcomp[i]->polarity);

        else
          snprintf(string, 128, str2,
                   ((signalcomp[i]->voltpercm * j) + ((signalcomp[i]->screen_offset * signalcomp[i]->voltpercm) / painter_pixelsizefactor)) * (double)signalcomp[i]->polarity);

        painter->drawText(5*printsize_x_factor, baseline - vert_ruler_offset - (4*printsize_y_factor), string);   // drawText(x, y, string)

// Draw the ruler above baseline ..
        painter->drawLine(0, baseline + vert_ruler_offset, w, baseline + vert_ruler_offset);    // drawLine(x0, y0, x1, y1)

// Set the annotation of the horizontal
        if(printing)
          snprintf(string, 128, str2,
                   (((signalcomp[i]->screen_offset * signalcomp[i]->voltpercm) / (painter_pixelsizefactor / printsize_y_factor)) - (signalcomp[i]->voltpercm * j)) * (double)signalcomp[i]->polarity);

        else
          snprintf(string, 128, str2,
                   (((signalcomp[i]->screen_offset * signalcomp[i]->voltpercm) / painter_pixelsizefactor) - (signalcomp[i]->voltpercm * j)) * (double)signalcomp[i]->polarity);

        painter->drawText(5 * printsize_x_factor, baseline + vert_ruler_offset - (4 * printsize_y_factor), string);
      }
    }
  }

  if(mainwindow->show_annot_markers)
  {
    annot_marker_pen->setColor(text_color);

    annot_marker_pen->setWidth(print_linewidth);

    painter->setPen(*annot_marker_pen);



    for(i=0; i<mainwindow->files_open; i++)
    {
      for(annot=mainwindow->annotationlist[i], j=0; annot!=NULL; annot=annot->next_annotation, j++) // For all annotations in the list
      {
        l_tmp = annot->onset - mainwindow->edfheaderlist[i]->starttime_offset;

        if( (l_tmp > (mainwindow->edfheaderlist[i]->viewtime - TIME_DIMENSION)) and
            (!annot->hidden) and
            (!annot->hidden_in_list) )
        {
          if(l_tmp > (mainwindow->edfheaderlist[i]->viewtime + mainwindow->pagetime)) break;

          l_tmp -= mainwindow->edfheaderlist[i]->viewtime;

          marker_x = (int)(( ((double)w) / mainwindow->pagetime) * l_tmp);

          painter->drawLine(marker_x, 0, marker_x, h);          // Draw vertical mark for the annotation start.

// Marker absolute to beginning.
          l_tmp = annot->onset + mainwindow->edfheaderlist[i]->l_starttime;
          snprintf(string, 32, "%i:%02i:%02i.%04i",
                   ((int)((l_tmp / TIME_DIMENSION)/ 3600LL)) % 24,
                   (int)(((l_tmp / TIME_DIMENSION) % 3600LL) / 60LL),
                   (int)((l_tmp / TIME_DIMENSION) % 60LL),
                   (int)(((l_tmp % TIME_DIMENSION) / 1000LL)));


          remove_trailing_zeros(string);

          if(printing)  painter->drawText(marker_x + (5 * printsize_x_factor), h - (25  * printsize_y_factor), string);
          else    painter->drawText(marker_x + 5,       h - 65 + ((j % 3) * 30),  string);

          strncpy(string, annot->annotation, 20);

          string[20] = 0;

          if(printing)  painter->drawText(marker_x + (5 * printsize_x_factor),  h - (40 * printsize_y_factor),  QString::fromUtf8(string));
          else    painter->drawText(marker_x + 5,       h - 80 + ((j % 3) * 30),  QString::fromUtf8(string));

          if(active_markers->count < MAX_ACTIVE_ANNOT_MARKERS)
          {
            annot->x_pos = marker_x;
            active_markers->list[active_markers->count] = annot;
            active_markers->count++;
          }
        }
      }
    }


    if(mainwindow->files_open > 0)
    {
      epoch_marker_pen->setColor(epoch_marker_color);
      epoch_marker_pen->setWidth(print_linewidth);
      painter->setPen(*epoch_marker_pen);
      for(annot=mainwindow->epochlist[0]; annot!=NULL; annot=annot->next_annotation)
      {
        l_tmp = annot->onset - mainwindow->edfheaderlist[0]->starttime_offset;

        if((l_tmp > (mainwindow->edfheaderlist[0]->viewtime - TIME_DIMENSION)) and (!annot->hidden) and (!annot->hidden_in_list))
        {

          if(annot->selected) painter->setPen(*epoch_marker_fat_pen);

          if(l_tmp > (mainwindow->edfheaderlist[0]->viewtime + mainwindow->pagetime)) break;

          l_tmp -= mainwindow->edfheaderlist[0]->viewtime;

          marker_x = (int)((((double)w) / mainwindow->pagetime) * l_tmp);

          painter->drawLine(marker_x, 0, marker_x, h);

// Marker absolute to beginning.
          l_tmp = annot->onset + mainwindow->edfheaderlist[0]->l_starttime;
          snprintf(string, 32, "%i:%02i:%02i.%04i",
                   ((int)((l_tmp / TIME_DIMENSION)/ 3600LL)) % 24,
                   (int)(((l_tmp / TIME_DIMENSION) % 3600LL) / 60LL),
                   (int)((l_tmp / TIME_DIMENSION) % 60LL),
                   (int)(((l_tmp % TIME_DIMENSION) / 1000LL)));

          remove_trailing_zeros(string);

          if(printing)  painter->drawText(marker_x + (5 * printsize_x_factor), h - (25  * printsize_y_factor), string);
          else    painter->drawText(marker_x + 5,            h - 65,             string);

          strncpy(string, annot->annotation, 20);

          string[20] = 0;

          if(printing) painter->drawText(marker_x + (5 * printsize_x_factor), h - (40 * printsize_y_factor), QString::fromUtf8(string));
          else       painter->drawText(marker_x + 5,          h - 80,        QString::fromUtf8(string));

          if(active_markers->count<MAX_ACTIVE_ANNOT_MARKERS)
          {
            annot->x_pos = marker_x;
            active_markers->list[active_markers->count] = annot;
            active_markers->count++;
          }

          if(annot->selected) painter->setPen(*epoch_marker_pen);
        }
      }
    }


  }

  if(mainwindow->clip_to_pane)
  {
    painter->setClipping(true);

    vertical_distance = h / (signalcomps + 1);
  }

  for(i=0; i<signalcomps; i++)
  {
    if(mainwindow->clip_to_pane)
    {
      painter->setClipRect(0, vertical_distance * i + (vertical_distance / 2), w, vertical_distance);
    }

    if(printing)
    {
      signal_pen->setColor((Qt::GlobalColor)signalcomp[i]->color);
      signal_pen->setWidth(print_linewidth);
      painter->setPen(*signal_pen);
    }
    else
    {
      painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
    }

    for(j=0; j<screensamples[i]; j++)
    {
      painter->drawLine(graphicBuf[j].graphicLine[i].x1,
                        graphicBuf[j].graphicLine[i].y1,
                        graphicBuf[j].graphicLine[i].x2,
                        graphicBuf[j].graphicLine[i].y2);
    }
  }

  painter->setClipping(false);

  for(i=0; i<signalcomps; i++)
  {
    baseline = h / (signalcomps + 1);
    baseline *= (i + 1);

    painter->setPen((Qt::GlobalColor)signalcomp[i]->color);

    if(signalcomp[i]->hascursor1)
    {
      if(printing)
      {
        painter->setPen((Qt::GlobalColor)crosshair_1.color);
        painter->drawLine(0, crosshair_1.y_value, w, crosshair_1.y_value);
        painter->drawLine((int)((double)crosshair_1.x_position * printsize_x_factor), 0, (int)((double)crosshair_1.x_position * printsize_x_factor), h);
        snprintf(string, 128, "%+f %s",
                 crosshair_1.value,
                 signalcomp[i]->physdimension);
        painter->drawText((int)(((double)crosshair_1.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_1.y_position - 40.0) * printsize_y_factor), string);
        snprintf(string, 128, "%2i:%02i:%02i.%04i",
                 (int)(((crosshair_1.time / TIME_DIMENSION)/ 3600LL) % 24LL),
                 (int)(((crosshair_1.time / TIME_DIMENSION) % 3600LL) / 60LL),
                 (int)((crosshair_1.time / TIME_DIMENSION) % 60LL),
                 (int)((crosshair_1.time % TIME_DIMENSION) / 1000LL));
        painter->drawText((int)(((double)crosshair_1.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_1.y_position - 25.0) * printsize_y_factor), string);
        if(signalcomp[i]->alias[0] != 0)
        {
          painter->drawText((int)(((double)crosshair_1.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_1.y_position - 10.0) * printsize_y_factor), signalcomp[i]->alias);
        }
        else
        {
          painter->drawText((int)(((double)crosshair_1.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_1.y_position - 10.0) * printsize_y_factor), signalcomp[i]->signallabel);
        }
        painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
      }
      else
      {
        painter->setPen((Qt::GlobalColor)crosshair_1.color);
        painter->drawLine(0, crosshair_1.y_value, w, crosshair_1.y_value);
        painter->drawLine(crosshair_1.x_position, 0, crosshair_1.x_position, h);
        snprintf(string, 128, "%+f %s",
                 crosshair_1.value,
                 signalcomp[i]->physdimension);
        painter->drawText(crosshair_1.x_position + 5, crosshair_1.y_position - 40, string);
        snprintf(string, 128, "%2i:%02i:%02i.%04i",
                 (int)(((crosshair_1.time / TIME_DIMENSION)/ 3600LL) % 24LL),
                 (int)(((crosshair_1.time / TIME_DIMENSION) % 3600LL) / 60LL),
                 (int)((crosshair_1.time / TIME_DIMENSION) % 60LL),
                 (int)((crosshair_1.time % TIME_DIMENSION) / 1000LL));
        snprintf(string + strlen(string), 32, " (%i:%02i:%02i.%04i)",
                 (int)((crosshair_1.time_relative / TIME_DIMENSION)/ 3600LL),
                 (int)(((crosshair_1.time_relative / TIME_DIMENSION) % 3600LL) / 60LL),
                 (int)((crosshair_1.time_relative / TIME_DIMENSION) % 60LL),
                 (int)((crosshair_1.time_relative % TIME_DIMENSION) / 1000LL));

        painter->drawText(crosshair_1.x_position + 5, crosshair_1.y_position - 25, string);
        if(signalcomp[i]->alias[0] != 0)
        {
          painter->drawText(crosshair_1.x_position + 5, crosshair_1.y_position - 10, signalcomp[i]->alias);
        }
        else
        {
          painter->drawText(crosshair_1.x_position + 5, crosshair_1.y_position - 10, signalcomp[i]->signallabel);
        }
        painter->setPen((Qt::GlobalColor)signalcomp[i]->color);

        //mainwindow->annotationEditDock->annotEditSetOnset(crosshair_1.time_relative);
        //if(crosshair_2.active) mainwindow->annotationEditDock->annotEditSetDuration(crosshair_2.time_relative - crosshair_1.time_relative);
      }
    }

    if(signalcomp[i]->hascursor2)
    {
      if(printing)
      {
        painter->setPen((Qt::GlobalColor)crosshair_2.color);
        painter->drawLine(0, crosshair_2.y_value, w, crosshair_2.y_value);
        painter->drawLine((int)((double)crosshair_2.x_position * printsize_x_factor), 0, (int)((double)crosshair_2.x_position * printsize_x_factor), h);
        snprintf(string, 128, "%+f %s",
                 crosshair_2.value,
                 signalcomp[i]->physdimension);
        painter->drawText((int)(((double)crosshair_2.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_2.y_position - 70.0) * printsize_y_factor), string);
        snprintf(string, 128, "%2i:%02i:%02i.%04i",
                 (int)(((crosshair_2.time / TIME_DIMENSION)/ 3600LL) % 24LL),
                 (int)(((crosshair_2.time / TIME_DIMENSION) % 3600LL) / 60LL),
                 (int)((crosshair_2.time / TIME_DIMENSION) % 60LL),
                 (int)((crosshair_2.time % TIME_DIMENSION) / 1000LL));
        painter->drawText((int)(((double)crosshair_2.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_2.y_position - 55.0) * printsize_y_factor), string);
        snprintf(string, 128, "delta %+f %s",
                 crosshair_2.value - crosshair_1.value,
                 signalcomp[i]->physdimension);
        painter->drawText((int)(((double)crosshair_2.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_2.y_position - 40.0) * printsize_y_factor), string);
        l_time = crosshair_2.time - crosshair_1.time;
        if(l_time<0) l_time = -l_time;
        snprintf(string, 128, "delta %i:%02i:%02i.%04i",
                 (int)((l_time / TIME_DIMENSION)/ 3600LL),
                 (int)(((l_time / TIME_DIMENSION) % 3600LL) / 60LL),
                 (int)((l_time / TIME_DIMENSION) % 60LL),
                 (int)((l_time % TIME_DIMENSION) / 1000LL));
        painter->drawText((int)(((double)crosshair_2.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_2.y_position - 25.0) * printsize_y_factor), string);
        if(signalcomp[i]->alias[0] != 0)
        {
          painter->drawText((int)(((double)crosshair_2.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_2.y_position - 10.0) * printsize_y_factor), signalcomp[i]->alias);
        }
        else
        {
          painter->drawText((int)(((double)crosshair_2.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_2.y_position - 10.0) * printsize_y_factor), signalcomp[i]->signallabel);
        }
        painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
      }
      else
      {
        painter->setPen((Qt::GlobalColor)crosshair_2.color);
        painter->drawLine(0, crosshair_2.y_value, w, crosshair_2.y_value);
        painter->drawLine(crosshair_2.x_position, 0, crosshair_2.x_position, h);
        snprintf(string, 128, "%+f %s",
                 crosshair_2.value,
                 signalcomp[i]->physdimension);
        painter->drawText(crosshair_2.x_position + 5, crosshair_2.y_position - 70, string);
        snprintf(string, 128, "%2i:%02i:%02i.%04i",
                 (int)(((crosshair_2.time / TIME_DIMENSION)/ 3600LL) % 24LL),
                 (int)(((crosshair_2.time / TIME_DIMENSION) % 3600LL) / 60LL),
                 (int)((crosshair_2.time / TIME_DIMENSION) % 60LL),
                 (int)((crosshair_2.time % TIME_DIMENSION) / 1000LL));
        snprintf(string + strlen(string), 32, " (%i:%02i:%02i.%04i)",
                 (int)((crosshair_2.time_relative / TIME_DIMENSION)/ 3600LL),
                 (int)(((crosshair_2.time_relative / TIME_DIMENSION) % 3600LL) / 60LL),
                 (int)((crosshair_2.time_relative / TIME_DIMENSION) % 60LL),
                 (int)((crosshair_2.time_relative % TIME_DIMENSION) / 1000LL));

        painter->drawText(crosshair_2.x_position + 5, crosshair_2.y_position - 55, string);
        snprintf(string, 128, "delta %+f %s",
                 crosshair_2.value - crosshair_1.value,
                 signalcomp[i]->physdimension);
        painter->drawText(crosshair_2.x_position + 5, crosshair_2.y_position - 40, string);
        l_time = crosshair_2.time - crosshair_1.time;
        if(l_time<0) l_time = -l_time;
        snprintf(string, 128, "delta %i:%02i:%02i.%04i",
                 (int)((l_time / TIME_DIMENSION)/ 3600LL),
                 (int)(((l_time / TIME_DIMENSION) % 3600LL) / 60LL),
                 (int)((l_time / TIME_DIMENSION) % 60LL),
                 (int)((l_time % TIME_DIMENSION) / 1000LL));
        painter->drawText(crosshair_2.x_position + 5, crosshair_2.y_position - 25, string);
        if(signalcomp[i]->alias[0] != 0)
        {
          painter->drawText(crosshair_2.x_position + 5, crosshair_2.y_position - 10, signalcomp[i]->alias);
        }
        else
        {
          painter->drawText(crosshair_2.x_position + 5, crosshair_2.y_position - 10, signalcomp[i]->signallabel);
        }
        painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
      }
    }

    if(signalcomp[i]->hasoffsettracking)
    {
      snprintf(string, 128, "offset: %f %s",
               -signalcomp[i]->screen_offset * mainwindow->pixelsizefactor * signalcomp[i]->voltpercm,
               signalcomp[i]->physdimension);
      painter->fillRect(92, baseline, 190, 12, backgroundcolor);
      painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
      painter->drawText(95, baseline + 10, string);
    }

    if(signalcomp[i]->hasgaintracking)
    {
      snprintf(string, 128, "gain: %f %s/cm",
               signalcomp[i]->voltpercm,
               signalcomp[i]->physdimension);
      painter->fillRect(92, baseline, 190, 12, backgroundcolor);
      painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
      painter->drawText(95, baseline + 10, string);
    }
  }

  vertical_distance = h / (signalcomps + 1);

  for(i=0; i<signalcomps; i++)
  {
    baseline = vertical_distance * (i + 1);

    if(signalcomp[i]->alias[0] != 0)
    {
      painter->fillRect(2, baseline - 20, strlen(signalcomp[i]->alias) * 7 + 6, 12, backgroundcolor);
    }
    else
    {
      painter->fillRect(2, baseline - 20, strlen(signalcomp[i]->signallabel) * 7 + 6, 12, backgroundcolor);
    }

    painter->setPen((Qt::GlobalColor)signalcomp[i]->color);

    if(signalcomp[i]->alias[0] != 0)
    {
      painter->drawText(5, baseline - 10, signalcomp[i]->alias);
    }
    else
    {
      painter->drawText(5, baseline - 10, signalcomp[i]->signallabel);
    }

    if(signalcomp[i]->hasruler)
    {
      floating_ruler(painter, ruler_x_position, ruler_y_position, signalcomp[i], print_linewidth);
    }
  }

  if(draw_zoom_rectangle)
  {
    special_pen->setColor(mouse_rect_color);

    painter->setPen(*special_pen);

    painter->drawRect(mouse_press_coordinate_x, mouse_press_coordinate_y, mouse_x - mouse_press_coordinate_x, mouse_y - mouse_press_coordinate_y);
  }

  painter->setPen(text_color);

  if(printing)
  {
    painter->drawText((int)(8.0 * printsize_y_factor), h - (int)(4.0 * printsize_y_factor), mainwindow->viewtime_string);
    painter->drawText(w - (int)(60.0 * printsize_y_factor), h - (int)(4.0 * printsize_y_factor), mainwindow->pagetime_string);
  }
  else
  {
    painter->fillRect(5, h - 14, 180, 12, backgroundcolor);
    painter->drawText(8, h - 4, mainwindow->viewtime_string);
    painter->fillRect(w - 63, h - 14, 60, 12, backgroundcolor);
    painter->drawText(w - 60, h - 4, mainwindow->pagetime_string);
  }
}



void ViewCurve::drawCurve_stage_1(QPainter *painter, int w_width, int w_height, int print_linewidth)
{

// Definitions
  int i, j, k, n, x1, y1, x2, y2,
      signalcomps,
      baseline,
      value,
      minimum,
      maximum,
      runin_samples,
      stat_zero_crossing=0;

  char *viewbuf;

  long long s, s2;

  double dig_value=0.0,
         f_tmp=0.0;

  struct signalcompblock **signalcomp;

  union
  {
    unsigned int one;
    signed int one_signed;
    unsigned short two[2];
    signed short two_signed[2];
    unsigned char four[4];
  } var;


// Initialize and check
  if(mainwindow->exit_in_progress)
  {
    if(graphicBuf != NULL)
    {
      free(graphicBuf);
      graphicBuf = NULL;
    }
    return;
  }

  for(i=0; i<MAXSIGNALS; i++) screensamples[i] = 0;

  signalcomps = mainwindow->signalcomps;    // number of signalcomps
  signalcomp = mainwindow->signalcomp;
  viewbuf = mainwindow->viewbuf;

  painter_pixelsizefactor = 1. / mainwindow->pixelsizefactor;

  if( !w_width || !w_height )   // If one of these INTs is zero ..
  {
    w = width();
    h = height();

    printsize_x_factor = 1.;
    printsize_y_factor = 1.;

    printing = 0;     // .. then we are not printing print.
  }
  else
  {
    w = w_width;
    h = w_height;

    printsize_x_factor = ((double)w_width) / ((double)width());
    printsize_y_factor = ((double)w_height) / ((double)height());

    painter_pixelsizefactor *= printsize_y_factor;

    printing = 1;
  }

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->samples_on_screen < (w / 2))
    {
      signalcomp[i]->sample_pixel_ratio = (((double)signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record / signalcomp[i]->edfhdr->data_record_duration) * ((double)mainwindow->pagetime / (double)TIME_DIMENSION)) / (double)w;
    }
    else
    {
      signalcomp[i]->sample_pixel_ratio = (double)signalcomp[i]->samples_on_screen / (double)w;
    }
  }

  if( (viewbuf==NULL) || (screensamples==NULL) )
  {
    if(graphicBuf!=NULL)
    {
      free(graphicBuf);
      graphicBuf = NULL;
    }
    update();
    return;
  }

  if( (graphicBuf==NULL) || (graphicBufWidth!=w) )
  {
    if(graphicBuf!=NULL)
    {
      free(graphicBuf);
    }

    graphicBuf = (struct graphicBufStruct *)malloc(sizeof(struct graphicBufStruct) * w * 2 + 4);

    graphicBufWidth = w;
  }

  if(graphicBuf==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    messagewindow.exec();
    return;
  }

  if(mainwindow->use_threads)
  {
    n = cpu_cnt;

    if(signalcomps < n)
    {
      n = signalcomps;
    }

// for(i=0; i<n; i++)
// {
//   printf("start:  signalcomp is %08X      screensamples is %08X\n---------------------\n", (int)(signalcomp[i]), (int)(&screensamples[i]));
// }

    for(i=0; i<n; i++)
    {
      thr[i]->init_vars(mainwindow, &signalcomp[0], i,
                        signalcomps, viewbuf, w, h, &screensamples[0], printing,
                        graphicBuf, printsize_x_factor,
                        printsize_y_factor, &crosshair_1, &crosshair_2, cpu_cnt);

      thr[i]->start();
    }

    for(i=0; i<n; i++)
    {
      thr[i]->wait();
    }
  }
  else
  {
    for(i=0; i<signalcomps; i++)
    {
      signalcomp[i]->max_dig_value = -2147483647;
      signalcomp[i]->min_dig_value = 2147483647;
      signalcomp[i]->stat_cnt = 0;
      signalcomp[i]->stat_zero_crossing_cnt = 0;
      signalcomp[i]->stat_sum = 0.0;
      signalcomp[i]->stat_sum_sqr = 0.0;
      signalcomp[i]->stat_sum_rectified = 0.0;

      baseline = h / (signalcomps + 1);
      baseline *= (i + 1);

      signalcomp[i]->pixels_shift = signalcomp[i]->sample_timeoffset_part / signalcomp[i]->sample_pixel_ratio;

      for(s=signalcomp[i]->sample_start; s<signalcomp[i]->samples_on_screen; s++)   // For all signals on the screen ..
      {
        if(s>=signalcomp[i]->sample_stop) break;

        dig_value = 0.0;
        s2 = s + signalcomp[i]->sample_timeoffset - signalcomp[i]->sample_start;

        for(j=0; j<signalcomp[i]->num_of_signals; j++)
        {
          if(signalcomp[i]->edfhdr->bdf)
          {
            var.two[0] = *((unsigned short *)(
                             viewbuf
                             + signalcomp[i]->viewbufoffset
                             + (signalcomp[i]->edfhdr->recordsize * (s2 / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record))
                             + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].buf_offset
                             + ((s2 % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record) * 3)));

            var.four[2] = *((unsigned char *)(
                              viewbuf
                              + signalcomp[i]->viewbufoffset
                              + (signalcomp[i]->edfhdr->recordsize * (s2 / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record))
                              + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].buf_offset
                              + ((s2 % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record) * 3)
                              + 2));

            if(var.four[2]&0x80)
            {
              var.four[3] = 0xff;
            }
            else
            {
              var.four[3] = 0x00;
            }

            f_tmp = var.one_signed;
          }

          if(signalcomp[i]->edfhdr->edf)
          {
            f_tmp = *(((short *)(
                         viewbuf
                         + signalcomp[i]->viewbufoffset
                         + (signalcomp[i]->edfhdr->recordsize * (s2 / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record))
                         + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].buf_offset))
                      + (s2 % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record));
          }

          f_tmp += signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].offset;
          f_tmp *= signalcomp[i]->factor[j];

          dig_value += f_tmp;
        }

        for(k=0; k<signalcomp[i]->filter_cnt; k++)
        {
          if(s==signalcomp[i]->sample_start)
          {
            if(mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime==0)
            {
              reset_filter(dig_value, signalcomp[i]->filter[k]);
            }
            else
            {
              signalcomp[i]->filter[k]->old_input = signalcomp[i]->filterpreset_a[k];
              signalcomp[i]->filter[k]->old_output = signalcomp[i]->filterpreset_b[k];
            }
          }

          dig_value = first_order_filter(dig_value, signalcomp[i]->filter[k]);
        }

        for(k=0; k<signalcomp[i]->ravg_filter_cnt; k++)
        {
          if(s == signalcomp[i]->sample_start)
          {
            if((mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime <= 0) && signalcomp[i]->ravg_filter_setup[k])
            {
              reset_ravg_filter(dig_value, signalcomp[i]->ravg_filter[k]);
            }
            else
            {
              ravg_filter_restore_buf(signalcomp[i]->ravg_filter[k]);
            }

            signalcomp[i]->ravg_filter_setup[k] = 0;
          }

          dig_value = run_ravg_filter(dig_value, signalcomp[i]->ravg_filter[k]);
        }

        for(k=0; k<signalcomp[i]->fidfilter_cnt; k++)
        {
          if(s == signalcomp[i]->sample_start)
          {
            if((mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime <= 0) && signalcomp[i]->fidfilter_setup[k])
            {
              runin_samples = (signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record / signalcomp[i]->edfhdr->data_record_duration) / signalcomp[i]->fidfilter_freq[k];

              runin_samples *= 26;

              if(runin_samples < 10)
              {
                runin_samples = 10;
              }

              for(n=0; n<runin_samples; n++)
              {
                signalcomp[i]->fidfuncp[k](signalcomp[i]->fidbuf[k], dig_value);
              }

              memcpy(signalcomp[i]->fidbuf2[k], signalcomp[i]->fidbuf[k], fid_run_bufsize(signalcomp[i]->fid_run[k]));
            }
            else
            {
              memcpy(signalcomp[i]->fidbuf[k], signalcomp[i]->fidbuf2[k], fid_run_bufsize(signalcomp[i]->fid_run[k]));
            }

            signalcomp[i]->fidfilter_setup[k] = 0;
          }

          dig_value = signalcomp[i]->fidfuncp[k](signalcomp[i]->fidbuf[k], dig_value);
        }

        if(signalcomp[i]->ecg_filter != NULL)
        {
          if(s == signalcomp[i]->sample_start)
          {
            if(mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime <= 0LL)
            {
              reset_ecg_filter(signalcomp[i]->ecg_filter);
            }
            else
            {
              ecg_filter_restore_buf(signalcomp[i]->ecg_filter);
            }
          }

          dig_value = run_ecg_filter(dig_value, signalcomp[i]->ecg_filter);
        }

        if(signalcomp[i]->zratio_filter != NULL)
        {
          if(s == signalcomp[i]->sample_start)
          {
            if(mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime <= 0LL)
            {
              reset_zratio_filter(signalcomp[i]->zratio_filter);
            }
            else
            {
              zratio_filter_restore_buf(signalcomp[i]->zratio_filter);
            }
          }

          dig_value = run_zratio_filter(dig_value, signalcomp[i]->zratio_filter);
        }

        if(printing)
        {
          value = (int)(dig_value * signalcomp[i]->sensitivity[0] * printsize_y_factor) * signalcomp[i]->polarity;
        }
        else
        {
          value = (int)(dig_value * signalcomp[i]->sensitivity[0]) * signalcomp[i]->polarity;

          signalcomp[i]->stat_cnt++;
          signalcomp[i]->stat_sum += dig_value;
          signalcomp[i]->stat_sum_sqr += ((dig_value) * (dig_value));
          if(dig_value < 0)
          {
            signalcomp[i]->stat_sum_rectified += (dig_value * -1.0);
          }
          else
          {
            signalcomp[i]->stat_sum_rectified += dig_value;
          }

          if(s == signalcomp[i]->sample_start)
          {
            if(dig_value < 0.)  stat_zero_crossing = 0;
            else    stat_zero_crossing = 1;
          }
          else
          {
            if(dig_value < 0.0)
            {
              if(stat_zero_crossing)
              {
                stat_zero_crossing = 0;

                signalcomp[i]->stat_zero_crossing_cnt++;
              }
            }
            else
            {
              if(!stat_zero_crossing)
              {
                stat_zero_crossing = 1;

                signalcomp[i]->stat_zero_crossing_cnt++;
              }
            }
          }
        }

        if(((int)dig_value) > signalcomp[i]->max_dig_value)  signalcomp[i]->max_dig_value = dig_value;
        if(((int)dig_value) < signalcomp[i]->min_dig_value)  signalcomp[i]->min_dig_value = dig_value;

        if(printing)  value = baseline - value + (int)(signalcomp[i]->screen_offset * printsize_y_factor);
        else    value = baseline - value + signalcomp[i]->screen_offset;

        if(s >= signalcomp[i]->sample_start)
        {
          x1 = (int)((double)s / signalcomp[i]->sample_pixel_ratio);
          y1 = signalcomp[i]->oldvalue;
          x2 = (int)(((double)s + 1.0) / signalcomp[i]->sample_pixel_ratio);
          y2 = value;

          if(signalcomp[i]->samples_on_screen < (w / 2))
          {
            graphicBuf[screensamples[i]].graphicLine[i].x1 = x1 - signalcomp[i]->pixels_shift;
            graphicBuf[screensamples[i]].graphicLine[i].y1 = y2;
            graphicBuf[screensamples[i]].graphicLine[i].x2 = x2 - signalcomp[i]->pixels_shift;
            graphicBuf[screensamples[i]].graphicLine[i].y2 = y2;

            if(screensamples[i])
            {
              screensamples[i]++;

              graphicBuf[screensamples[i]].graphicLine[i].x1 = x1 - signalcomp[i]->pixels_shift;
              graphicBuf[screensamples[i]].graphicLine[i].y1 = y1;
              graphicBuf[screensamples[i]].graphicLine[i].x2 = x1 - signalcomp[i]->pixels_shift;
              graphicBuf[screensamples[i]].graphicLine[i].y2 = y2;
            }

            screensamples[i]++;
          }
          else
          {
            if(!screensamples[i])
            {
              graphicBuf[screensamples[i]].graphicLine[i].x1 = x1;
              graphicBuf[screensamples[i]].graphicLine[i].y1 = y1;
              graphicBuf[screensamples[i]].graphicLine[i].x2 = x2;
              graphicBuf[screensamples[i]].graphicLine[i].y2 = y2;

              screensamples[i]++;
            }
            else
            {
              if((x1==x2)&&(x1==graphicBuf[screensamples[i]-1].graphicLine[i].x1)&&
                  (graphicBuf[screensamples[i]-1].graphicLine[i].x1==graphicBuf[screensamples[i]-1].graphicLine[i].x2))
              {
                maximum = y1;
                if(y2>maximum)  maximum = y2;
                if(graphicBuf[screensamples[i]-1].graphicLine[i].y1>maximum)  maximum = graphicBuf[screensamples[i]-1].graphicLine[i].y1;
                if(graphicBuf[screensamples[i]-1].graphicLine[i].y2>maximum)  maximum = graphicBuf[screensamples[i]-1].graphicLine[i].y2;

                minimum = y1;
                if(y2<minimum)  minimum = y2;
                if(graphicBuf[screensamples[i]-1].graphicLine[i].y1<minimum)  minimum = graphicBuf[screensamples[i]-1].graphicLine[i].y1;
                if(graphicBuf[screensamples[i]-1].graphicLine[i].y2<minimum)  minimum = graphicBuf[screensamples[i]-1].graphicLine[i].y2;

                graphicBuf[screensamples[i]-1].graphicLine[i].y1 = maximum;
                graphicBuf[screensamples[i]-1].graphicLine[i].y2 = minimum;
              }
              else
              {
                graphicBuf[screensamples[i]].graphicLine[i].x1 = x1;
                graphicBuf[screensamples[i]].graphicLine[i].y1 = y1;
                graphicBuf[screensamples[i]].graphicLine[i].x2 = x2;
                graphicBuf[screensamples[i]].graphicLine[i].y2 = y2;

                if(screensamples[i] < w*2)  screensamples[i]++;
              }
            }
          }
        }

        signalcomp[i]->oldvalue = value;

        if(signalcomp[i]->hascursor1)
        {
          if(printing)
          {
            if(s==((int)((double)(crosshair_1.x_position * printsize_x_factor + (double)signalcomp[i]->pixels_shift) * signalcomp[i]->sample_pixel_ratio)))
            {
              crosshair_1.y_value = value;
              crosshair_1.value = dig_value * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue;
              crosshair_1.time = mainwindow->edfheaderlist[signalcomp[i]->filenum]->l_starttime + mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime + signalcomp[i]->edfhdr->starttime_offset + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_1.x_position);
              crosshair_1.time_relative = mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_1.x_position);
            }
          }
          else
          {
            if(s==((int)((double)(crosshair_1.x_position + signalcomp[i]->pixels_shift) * signalcomp[i]->sample_pixel_ratio)))
            {
              crosshair_1.y_value = value;
              crosshair_1.value = dig_value * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue;
              crosshair_1.time = mainwindow->edfheaderlist[signalcomp[i]->filenum]->l_starttime + mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime + signalcomp[i]->edfhdr->starttime_offset + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_1.x_position);
              crosshair_1.time_relative = mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_1.x_position);
            }
          }
        }

        if(signalcomp[i]->hascursor2)
        {
          if(printing)
          {
            if(s==((int)((double)(crosshair_2.x_position * printsize_x_factor + (double)signalcomp[i]->pixels_shift) * signalcomp[i]->sample_pixel_ratio)))
            {
              crosshair_2.y_value = value;
              crosshair_2.value = dig_value * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue;
              crosshair_2.time = mainwindow->edfheaderlist[signalcomp[i]->filenum]->l_starttime + mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime + signalcomp[i]->edfhdr->starttime_offset + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_2.x_position);
              crosshair_2.time_relative = mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_2.x_position);
            }
          }
          else
          {
            if(s==((int)((double)(crosshair_2.x_position + signalcomp[i]->pixels_shift) * signalcomp[i]->sample_pixel_ratio)))
            {
              crosshair_2.y_value = value;
              crosshair_2.value = dig_value * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue;
              crosshair_2.time = mainwindow->edfheaderlist[signalcomp[i]->filenum]->l_starttime + mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime + signalcomp[i]->edfhdr->starttime_offset + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_2.x_position);
              crosshair_2.time_relative = mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_2.x_position);
            }
          }
        }
      }
    }
  }

  if(printing)  drawCurve_stage_2(painter, w_width, w_height, print_linewidth);
  else    update();
}



void drawCurve_stage_1_thread::init_vars(UI_Mainwindow *mainwindow_a, struct signalcompblock **signalcomp_a, int i_a,
    int signalcomps_a, char *viewbuf_a, int w_a, int h_a, int *screensamples_a, int printing_a,
    struct graphicBufStruct *graphicBuf_a, double printsize_x_factor_a,
    double printsize_y_factor_a, struct crossHairStruct *crosshair_1_a,
    struct crossHairStruct *crosshair_2_a, int cpu_cnt_a)
{
  mainwindow = mainwindow_a;
  signalcomp_b = signalcomp_a;
  i = i_a;
  signalcomps = signalcomps_a;
  viewbuf = viewbuf_a;
  w = w_a;
  h = h_a;
  screensamples_b = screensamples_a;
  printing = printing_a;
  graphicBuf = graphicBuf_a;
  printsize_x_factor = printsize_x_factor_a;
  printsize_y_factor = printsize_y_factor_a;
  crosshair_1 = crosshair_1_a;
  crosshair_2 = crosshair_2_a;
  cpu_cnt = cpu_cnt_a;

// printf("init_vars(): i is %i   signalcomp_b is %08X      screensamples_b is %08X\n------------------------\n",
//        i, (int)signalcomp_b[i], (int)screensamples_b);

}


void drawCurve_stage_1_thread::run()
{
  int j, k, n, x1, y1, x2, y2,
      baseline,
      value,
      minimum,
      maximum,
      runin_samples,
      stat_zero_crossing;

  long long s, s2;

  double dig_value,
         f_tmp;

  union
  {
    unsigned int one;
    signed int one_signed;
    unsigned short two[2];
    signed short two_signed[2];
    unsigned char four[4];
  } var;


  for(; i < signalcomps; i += cpu_cnt)
  {

// printf("i is %i   cpu_cnt is %i\n", i, cpu_cnt);

    screensamples = &screensamples_b[i];

    signalcomp = signalcomp_b[i];


// printf("i is %i   signalcomp is %08X      screensamples is %08X\n",
//        i, (int)signalcomp, (int)screensamples);

    f_tmp = 0.0;
    dig_value=0.0;
    stat_zero_crossing=0;

    signalcomp->max_dig_value = -2147483647;
    signalcomp->min_dig_value = 2147483647;
    signalcomp->stat_cnt = 0;
    signalcomp->stat_zero_crossing_cnt = 0;
    signalcomp->stat_sum = 0.0;
    signalcomp->stat_sum_sqr = 0.0;
    signalcomp->stat_sum_rectified = 0.0;

    baseline = h / (signalcomps + 1);
    baseline *= (i + 1);

    signalcomp->pixels_shift = signalcomp->sample_timeoffset_part / signalcomp->sample_pixel_ratio;

    for(s=signalcomp->sample_start; s<signalcomp->samples_on_screen; s++)
    {
      if(s >= signalcomp->sample_stop) break;

      dig_value = 0.0;
      s2 = s + signalcomp->sample_timeoffset - signalcomp->sample_start;

      for(j=0; j<signalcomp->num_of_signals; j++)
      {
        if(signalcomp->edfhdr->bdf)
        {
          var.two[0] = *((unsigned short *)(
                           viewbuf
                           + signalcomp->viewbufoffset
                           + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
                           + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset
                           + ((s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record) * 3)));

          var.four[2] = *((unsigned char *)(
                            viewbuf
                            + signalcomp->viewbufoffset
                            + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
                            + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset
                            + ((s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record) * 3)
                            + 2));

          if(var.four[2]&0x80)
          {
            var.four[3] = 0xff;
          }
          else
          {
            var.four[3] = 0x00;
          }

          f_tmp = var.one_signed;
        }

        if(signalcomp->edfhdr->edf)
        {
          f_tmp = *(((short *)(
                       viewbuf
                       + signalcomp->viewbufoffset
                       + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
                       + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset))
                    + (s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record));
        }

        f_tmp += signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].offset;
        f_tmp *= signalcomp->factor[j];

        dig_value += f_tmp;
      }

      for(k=0; k<signalcomp->filter_cnt; k++)
      {
        if(s == signalcomp->sample_start)
        {
          if(mainwindow->edfheaderlist[signalcomp->filenum]->viewtime == 0)
          {
            reset_filter(dig_value, signalcomp->filter[k]);
          }
          else
          {
            signalcomp->filter[k]->old_input = signalcomp->filterpreset_a[k];
            signalcomp->filter[k]->old_output = signalcomp->filterpreset_b[k];
          }
        }

        dig_value = first_order_filter(dig_value, signalcomp->filter[k]);
      }

      for(k=0; k<signalcomp->ravg_filter_cnt; k++)
      {
        if(s == signalcomp->sample_start)
        {
          if((mainwindow->edfheaderlist[signalcomp->filenum]->viewtime <= 0) && signalcomp->ravg_filter_setup[k])
          {
            reset_ravg_filter(dig_value, signalcomp->ravg_filter[k]);
          }
          else
          {
            ravg_filter_restore_buf(signalcomp->ravg_filter[k]);
          }

          signalcomp->ravg_filter_setup[k] = 0;
        }

        dig_value = run_ravg_filter(dig_value, signalcomp->ravg_filter[k]);
      }

      for(k=0; k<signalcomp->fidfilter_cnt; k++)
      {
        if(s==signalcomp->sample_start)
        {
          if((mainwindow->edfheaderlist[signalcomp->filenum]->viewtime <= 0) && signalcomp->fidfilter_setup[k])
          {
            runin_samples = (signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record / signalcomp->edfhdr->data_record_duration) / signalcomp->fidfilter_freq[k];

            runin_samples *= 26;

            if(runin_samples < 10)
            {
              runin_samples = 10;
            }

            for(n=0; n<runin_samples; n++)
            {
              signalcomp->fidfuncp[k](signalcomp->fidbuf[k], dig_value);
            }

            memcpy(signalcomp->fidbuf2[k], signalcomp->fidbuf[k], fid_run_bufsize(signalcomp->fid_run[k]));
          }
          else
          {
            memcpy(signalcomp->fidbuf[k], signalcomp->fidbuf2[k], fid_run_bufsize(signalcomp->fid_run[k]));
          }

          signalcomp->fidfilter_setup[k] = 0;
        }

        dig_value = signalcomp->fidfuncp[k](signalcomp->fidbuf[k], dig_value);
      }

      if(signalcomp->ecg_filter != NULL)
      {
        if(s==signalcomp->sample_start)
        {
          if(mainwindow->edfheaderlist[signalcomp->filenum]->viewtime <= 0LL)
          {
            reset_ecg_filter(signalcomp->ecg_filter);
          }
          else
          {
            ecg_filter_restore_buf(signalcomp->ecg_filter);
          }
        }

        dig_value = run_ecg_filter(dig_value, signalcomp->ecg_filter);
      }

      if(signalcomp->zratio_filter != NULL)
      {
        if(s==signalcomp->sample_start)
        {
          if(mainwindow->edfheaderlist[signalcomp->filenum]->viewtime <= 0LL)
          {
            reset_zratio_filter(signalcomp->zratio_filter);
          }
          else
          {
            zratio_filter_restore_buf(signalcomp->zratio_filter);
          }
        }

        dig_value = run_zratio_filter(dig_value, signalcomp->zratio_filter);
      }

      if(printing)
      {
        value = (int)(dig_value * signalcomp->sensitivity[0] * printsize_y_factor) * signalcomp->polarity;
      }
      else
      {
        value = (int)(dig_value * signalcomp->sensitivity[0]) * signalcomp->polarity;

        signalcomp->stat_cnt++;
        signalcomp->stat_sum += dig_value;
        signalcomp->stat_sum_sqr += ((dig_value) * (dig_value));
        if(dig_value < 0)
        {
          signalcomp->stat_sum_rectified += (dig_value * -1.0);
        }
        else
        {
          signalcomp->stat_sum_rectified += dig_value;
        }

        if(s==signalcomp->sample_start)
        {
          if(dig_value < 0.0)
          {
            stat_zero_crossing = 0;
          }
          else
          {
            stat_zero_crossing = 1;
          }
        }
        else
        {
          if(dig_value < 0.0)
          {
            if(stat_zero_crossing)
            {
              stat_zero_crossing = 0;

              signalcomp->stat_zero_crossing_cnt++;
            }
          }
          else
          {
            if(!stat_zero_crossing)
            {
              stat_zero_crossing = 1;

              signalcomp->stat_zero_crossing_cnt++;
            }
          }
        }
      }

      if(dig_value > signalcomp->max_dig_value)  signalcomp->max_dig_value = dig_value;
      if(dig_value < signalcomp->min_dig_value)  signalcomp->min_dig_value = dig_value;

      if(printing)
      {
        value = baseline - value + (int)(signalcomp->screen_offset * printsize_y_factor);
      }
      else
      {
        value = baseline - value + signalcomp->screen_offset;
      }

      if(s >= signalcomp->sample_start)
      {
        x1 = (int)((double)s / signalcomp->sample_pixel_ratio);
        y1 = signalcomp->oldvalue;
        x2 = (int)(((double)s + 1.0) / signalcomp->sample_pixel_ratio);
        y2 = value;

        if(signalcomp->samples_on_screen < (w / 2))
        {
          graphicBuf[*screensamples].graphicLine[i].x1 = x1 - signalcomp->pixels_shift;
          graphicBuf[*screensamples].graphicLine[i].y1 = y2;
          graphicBuf[*screensamples].graphicLine[i].x2 = x2 - signalcomp->pixels_shift;
          graphicBuf[*screensamples].graphicLine[i].y2 = y2;

          if(*screensamples)
          {
            (*screensamples)++;

            graphicBuf[*screensamples].graphicLine[i].x1 = x1 - signalcomp->pixels_shift;
            graphicBuf[*screensamples].graphicLine[i].y1 = y1;
            graphicBuf[*screensamples].graphicLine[i].x2 = x1 - signalcomp->pixels_shift;
            graphicBuf[*screensamples].graphicLine[i].y2 = y2;
          }

          (*screensamples)++;
        }
        else
        {
          if(!(*screensamples))
          {
            graphicBuf[*screensamples].graphicLine[i].x1 = x1;
            graphicBuf[*screensamples].graphicLine[i].y1 = y1;
            graphicBuf[*screensamples].graphicLine[i].x2 = x2;
            graphicBuf[*screensamples].graphicLine[i].y2 = y2;

            (*screensamples)++;
          }
          else
          {
            if((x1==x2)&&(x1==graphicBuf[(*screensamples)-1].graphicLine[i].x1)&&
                (graphicBuf[(*screensamples)-1].graphicLine[i].x1==graphicBuf[(*screensamples)-1].graphicLine[i].x2))
            {
              maximum = y1;
              if(y2>maximum)  maximum = y2;
              if(graphicBuf[(*screensamples)-1].graphicLine[i].y1>maximum)  maximum = graphicBuf[(*screensamples)-1].graphicLine[i].y1;
              if(graphicBuf[(*screensamples)-1].graphicLine[i].y2>maximum)  maximum = graphicBuf[(*screensamples)-1].graphicLine[i].y2;

              minimum = y1;
              if(y2<minimum)  minimum = y2;
              if(graphicBuf[(*screensamples)-1].graphicLine[i].y1<minimum)  minimum = graphicBuf[(*screensamples)-1].graphicLine[i].y1;
              if(graphicBuf[(*screensamples)-1].graphicLine[i].y2<minimum)  minimum = graphicBuf[(*screensamples)-1].graphicLine[i].y2;

              graphicBuf[(*screensamples)-1].graphicLine[i].y1 = maximum;
              graphicBuf[(*screensamples)-1].graphicLine[i].y2 = minimum;
            }
            else
            {
              graphicBuf[*screensamples].graphicLine[i].x1 = x1;
              graphicBuf[*screensamples].graphicLine[i].y1 = y1;
              graphicBuf[*screensamples].graphicLine[i].x2 = x2;
              graphicBuf[*screensamples].graphicLine[i].y2 = y2;

              if((*screensamples)<(w * 2))  (*screensamples)++;
            }
          }
        }
      }

      signalcomp->oldvalue = value;

      if(signalcomp->hascursor1)
      {
        if(printing)
        {
          if(s==((int)((double)(crosshair_1->x_position * printsize_x_factor + (double)signalcomp->pixels_shift) * signalcomp->sample_pixel_ratio)))
          {
            crosshair_1->y_value = value;
            crosshair_1->value = dig_value * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue;
            crosshair_1->time = mainwindow->edfheaderlist[signalcomp->filenum]->l_starttime + mainwindow->edfheaderlist[signalcomp->filenum]->viewtime + signalcomp->edfhdr->starttime_offset + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_1->x_position);
            crosshair_1->time_relative = mainwindow->edfheaderlist[signalcomp->filenum]->viewtime + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_1->x_position);
          }
        }
        else
        {
          if(s==((int)((double)(crosshair_1->x_position + signalcomp->pixels_shift) * signalcomp->sample_pixel_ratio)))
          {
            crosshair_1->y_value = value;
            crosshair_1->value = dig_value * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue;
            crosshair_1->time = mainwindow->edfheaderlist[signalcomp->filenum]->l_starttime + mainwindow->edfheaderlist[signalcomp->filenum]->viewtime + signalcomp->edfhdr->starttime_offset + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_1->x_position);
            crosshair_1->time_relative = mainwindow->edfheaderlist[signalcomp->filenum]->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_1->x_position);
          }
        }
      }

      if(signalcomp->hascursor2)
      {
        if(printing)
        {
          if(s==((int)((double)(crosshair_2->x_position * printsize_x_factor + (double)signalcomp->pixels_shift) * signalcomp->sample_pixel_ratio)))
          {
            crosshair_2->y_value = value;
            crosshair_2->value = dig_value * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue;
            crosshair_2->time = mainwindow->edfheaderlist[signalcomp->filenum]->l_starttime + mainwindow->edfheaderlist[signalcomp->filenum]->viewtime + signalcomp->edfhdr->starttime_offset + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_2->x_position);
            crosshair_2->time_relative = mainwindow->edfheaderlist[signalcomp->filenum]->viewtime + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_2->x_position);
          }
        }
        else
        {
          if(s==((int)((double)(crosshair_2->x_position + signalcomp->pixels_shift) * signalcomp->sample_pixel_ratio)))
          {
            crosshair_2->y_value = value;
            crosshair_2->value = dig_value * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue;
            crosshair_2->time = mainwindow->edfheaderlist[signalcomp->filenum]->l_starttime + mainwindow->edfheaderlist[signalcomp->filenum]->viewtime + signalcomp->edfhdr->starttime_offset + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_2->x_position);
            crosshair_2->time_relative = mainwindow->edfheaderlist[signalcomp->filenum]->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_2->x_position);
          }
        }
      }
    }
  }
}


void ViewCurve::exec_sidemenu(int signal_nr_intern)
{
  int i;

  char str[32]= {0,0,0,0,0,0};

  signal_nr = signal_nr_intern;

  sidemenu = new QDialog(this);

  sidemenu->setMinimumSize(QSize(190, 460));
  sidemenu->setMaximumSize(QSize(190, 460));
  sidemenu->setWindowTitle("Signal");
  sidemenu->setModal(true);
  sidemenu->setAttribute(Qt::WA_DeleteOnClose, true);

  SidemenuLabel = new QLabel(sidemenu);
  SidemenuLabel->setGeometry(QRect(45, 5, 100, 20));
  SidemenuLabel->setText(mainwindow->signalcomp[signal_nr]->signallabel);

  AliasLabel = new QLabel(sidemenu);
  AliasLabel->setGeometry(5, 30, 35, 20);
  AliasLabel->setText("Alias");

  AliasLineEdit = new QLineEdit(sidemenu);
  AliasLineEdit->setGeometry(45, 30, 140, 20);
  AliasLineEdit->setText(mainwindow->signalcomp[signal_nr]->alias);
  AliasLineEdit->setMaxLength(16);

  AmpLabel = new QLabel(sidemenu);
  AmpLabel->setGeometry(QRect(5, 55, 35, 20));
  AmpLabel->setText("Ampl.");

  OffsetLabel = new QLabel(sidemenu);
  OffsetLabel->setGeometry(QRect(5, 80, 35, 20));
  OffsetLabel->setText("Offset");

  ScaleBox = new QDoubleSpinBox(sidemenu);
  ScaleBox->setGeometry(QRect(45, 55, 140, 20));
  ScaleBox->setDecimals(8);
  ScaleBox->setMaximum(1000000.0);
  ScaleBox->setMinimum(0.0000001);
  if(mainwindow->signalcomp[signal_nr]->edfhdr->edfparam[mainwindow->signalcomp[signal_nr]->edfsignal[0]].bitvalue < 0.0)
  {
    ScaleBox->setValue(mainwindow->signalcomp[signal_nr]->voltpercm * -1.0);
  }
  else
  {
    ScaleBox->setValue(mainwindow->signalcomp[signal_nr]->voltpercm);
  }
  strcpy(str, mainwindow->signalcomp[signal_nr]->physdimension);
  for(i=1; i<4; i++)
  {
    if(str[i]==' ')  break;
  }
  strcpy(str + i, "/cm");
  ScaleBox->setSuffix(str);

  ScaleBox2 = new QDoubleSpinBox(sidemenu);
  ScaleBox2->setGeometry(QRect(45, 80, 140, 20));
  ScaleBox2->setDecimals(8);
  ScaleBox2->setMaximum(1000000.0);
  ScaleBox2->setMinimum(-1000000.0);
  ScaleBox2->setValue(-mainwindow->signalcomp[signal_nr]->screen_offset * mainwindow->pixelsizefactor * mainwindow->signalcomp[signal_nr]->voltpercm);
  ScaleBox2->setSuffix(mainwindow->signalcomp[signal_nr]->physdimension);

  sidemenuButton1 = new QPushButton(sidemenu);
  sidemenuButton1->setGeometry(QRect(45, 105, 100, 20));
  sidemenuButton1->setText("Ruler");

  sidemenuButton2 = new QPushButton(sidemenu);
  sidemenuButton2->setGeometry(QRect(45, 130, 100, 20));
  sidemenuButton2->setText("Crosshair");

  sidemenuButton3 = new QPushButton(sidemenu);
  sidemenuButton3->setGeometry(QRect(45, 155, 100, 20));
  sidemenuButton3->setText("Fit to pane");

  sidemenuButton4 = new QPushButton(sidemenu);
  sidemenuButton4->setGeometry(QRect(45, 180, 100, 20));
  sidemenuButton4->setText("Color");

  sidemenuButton5 = new QPushButton(sidemenu);
  sidemenuButton5->setGeometry(QRect(45, 205, 100, 20));
  sidemenuButton5->setText("Invert");

  sidemenuButton6 = new QPushButton(sidemenu);
  sidemenuButton6->setGeometry(QRect(45, 230, 100, 20));
  sidemenuButton6->setText("Spectrum");

  sidemenuButton7 = new QPushButton(sidemenu);
  sidemenuButton7->setGeometry(QRect(45, 255, 100, 20));
  sidemenuButton7->setText("Z-EEG");

  sidemenuButton8 = new QPushButton(sidemenu);
  sidemenuButton8->setGeometry(QRect(45, 280, 100, 20));
  sidemenuButton8->setText("Remove filter");

  sidemenuButton9 = new QPushButton(sidemenu);
  sidemenuButton9->setGeometry(QRect(45, 305, 100, 20));
  sidemenuButton9->setText("Remove signal");

  sidemenuButton10 = new QPushButton(sidemenu);
  sidemenuButton10->setGeometry(QRect(45, 330, 100, 20));
  sidemenuButton10->setText("Adjust filter");

  sidemenuButton11 = new QPushButton(sidemenu);
  sidemenuButton11->setGeometry(QRect(45, 355, 100, 20));
  sidemenuButton11->setText("Statistics");

  sidemenuButton12 = new QPushButton(sidemenu);
  sidemenuButton12->setGeometry(QRect(45, 380, 100, 20));
  sidemenuButton12->setText("Heart Rate");

  sidemenuButton13 = new QPushButton(sidemenu);
  sidemenuButton13->setGeometry(QRect(45, 405, 100, 20));
  sidemenuButton13->setText("Close");

  QObject::connect(ScaleBox,          SIGNAL(valueChanged(double)),     this,     SLOT(ScaleBoxChanged(double)));
  QObject::connect(ScaleBox2,         SIGNAL(valueChanged(double)),     this,     SLOT(ScaleBox2Changed(double)));
  QObject::connect(sidemenuButton1,   SIGNAL(clicked()),                this,     SLOT(RulerButton()));
  QObject::connect(sidemenuButton2,   SIGNAL(clicked()),                this,     SLOT(CrosshairButton()));
  QObject::connect(sidemenuButton3,   SIGNAL(clicked()),                this,     SLOT(FittopaneButton()));
  QObject::connect(sidemenuButton4,   SIGNAL(clicked()),                this,     SLOT(ColorButton()));
  QObject::connect(sidemenuButton5,   SIGNAL(clicked()),                this,     SLOT(signalInvert()));
  QObject::connect(sidemenuButton6,   SIGNAL(clicked()),                this,     SLOT(FreqSpecButton()));
  QObject::connect(sidemenuButton7,   SIGNAL(clicked()),                this,     SLOT(Z_scoringButton()));
  QObject::connect(sidemenuButton8,   SIGNAL(clicked()),                this,     SLOT(RemovefilterButton()));
  QObject::connect(sidemenuButton9,   SIGNAL(clicked()),                this,     SLOT(RemovesignalButton()));
  QObject::connect(sidemenuButton10,  SIGNAL(clicked()),                this,     SLOT(AdjustFilterButton()));
  QObject::connect(sidemenuButton11,  SIGNAL(clicked()),                this,     SLOT(StatisticsButton()));
  QObject::connect(sidemenuButton12,  SIGNAL(clicked()),                this,     SLOT(ECGdetectButton()));
  QObject::connect(sidemenuButton13,  SIGNAL(clicked()),                this,     SLOT(sidemenu_close()));
  QObject::connect(AliasLineEdit,     SIGNAL(returnPressed()),          this,     SLOT(sidemenu_close()));

  sidemenu->exec();
}


void ViewCurve::sidemenu_close()
{
  if(AliasLineEdit->isModified() == true)
  {
    strncpy(mainwindow->signalcomp[signal_nr]->alias, AliasLineEdit->text().toLatin1().data(), 16);

    mainwindow->signalcomp[signal_nr]->alias[16] = 0;

    latin1_to_ascii(mainwindow->signalcomp[signal_nr]->alias, 16);

    remove_trailing_spaces(mainwindow->signalcomp[signal_nr]->alias);

    remove_leading_spaces(mainwindow->signalcomp[signal_nr]->alias);
  }

  sidemenu->close();
}


void ViewCurve::signalInvert()
{
  sidemenu->close();

  if(!mainwindow->signalcomps)
  {
    return;
  }

  if(signal_nr>(mainwindow->signalcomps - 1))
  {
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->polarity != 1)
  {
    mainwindow->signalcomp[signal_nr]->polarity = 1;
  }
  else
  {
    mainwindow->signalcomp[signal_nr]->polarity = -1;
  }

  mainwindow->setup_viewbuf();
}


void ViewCurve::ECGdetectButton()
{
  int i;

  struct signalcompblock *newsignalcomp;

  sidemenu->close();

  if(!mainwindow->signalcomps)
  {
    return;
  }

  if(signal_nr>(mainwindow->signalcomps - 1))
  {
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->zratio_filter != NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Z-ratio is active for this signal!");
    messagewindow.exec();

    return;
  }

  if(mainwindow->signalcomp[signal_nr]->ecg_filter != NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Heartrate detection is already active for this signal!");
    messagewindow.exec();

    return;
  }

  newsignalcomp = mainwindow->create_signalcomp_copy(mainwindow->signalcomp[signal_nr]);
  if(newsignalcomp == NULL)
  {
    return;
  }

  newsignalcomp->ecg_filter =
    create_ecg_filter(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].smp_per_record /
                      newsignalcomp->edfhdr->data_record_duration,
                      newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].bitvalue,
                      mainwindow->powerlinefreq);

  strcpy(newsignalcomp->signallabel_bu, newsignalcomp->signallabel);
  newsignalcomp->signallabellen_bu = newsignalcomp->signallabellen;
  strcpy(newsignalcomp->signallabel, "HR");
  newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);
  strcpy(newsignalcomp->physdimension_bu, newsignalcomp->physdimension);
  strcpy(newsignalcomp->physdimension, "bpm");
  newsignalcomp->alias[0] = 0;

  if(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].bitvalue < 0.0)
  {
    for(i=0; i<newsignalcomp->num_of_signals; i++)
    {
      newsignalcomp->sensitivity[i] = newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[i]].bitvalue / -5.0 / mainwindow->pixelsizefactor;
    }

    newsignalcomp->voltpercm = -5.0;
  }
  else
  {
    for(i=0; i<newsignalcomp->num_of_signals; i++)
    {
      newsignalcomp->sensitivity[i] = newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[i]].bitvalue / 5.0 / mainwindow->pixelsizefactor;
    }

    newsignalcomp->voltpercm = 5.0;
  }

  newsignalcomp->screen_offset = 55.0 / (mainwindow->pixelsizefactor * newsignalcomp->voltpercm);

  mainwindow->setup_viewbuf();
}


void ViewCurve::AdjustFilterButton()
{
  sidemenu->close();

  if(!mainwindow->signalcomps)
  {
    return;
  }

  if(signal_nr>(mainwindow->signalcomps - 1))
  {
    return;
  }

  AdjustFilterSettings filtersettings(mainwindow->signalcomp[signal_nr], this);
}


void ViewCurve::StatisticsButton()
{
  if(!mainwindow->signalcomps)
  {
    sidemenu->close();

    return;
  }

  if(signal_nr>(mainwindow->signalcomps - 1))
  {
    sidemenu->close();

    return;
  }

  sidemenu->close();

  UI_StatisticWindow show_stats_window(mainwindow->signalcomp[signal_nr], mainwindow->pagetime);
}


void ViewCurve::FreqSpecButton()
{
  int i, j;

  for(i=0; i<MAXSPECTRUMDIALOGS; i++)
  {
    if(mainwindow->spectrumdialog[i] == NULL)
    {
      mainwindow->spectrumdialog[i] = new UI_FreqSpectrumWindow(mainwindow->signalcomp[signal_nr], mainwindow->viewbuf, mainwindow->spectrumdialog, i, mainwindow);

      break;
    }
  }

  sidemenu->close();

  if(i<MAXSPECTRUMDIALOGS)
  {
    for(j=0; j<MAXSPECTRUMDIALOGS; j++)
    {
      if(mainwindow->signalcomp[signal_nr]->spectr_dialog[j] == 0)
      {
        mainwindow->signalcomp[signal_nr]->spectr_dialog[j] = i + 1;

        break;
      }
    }

    mainwindow->spectrumdialog[i]->SpectrumDialog->move(((i % 15) * 30) + 200, ((i % 15) * 30) + 200);

    for(j=0; j<MAXSPECTRUMDIALOGS; j++)
    {
      if(mainwindow->spectrumdialog[j] != NULL)
      {
        mainwindow->spectrumdialog[j]->SpectrumDialog->raise();
      }
    }
    mainwindow->spectrumdialog[i]->SpectrumDialog->activateWindow();
    mainwindow->spectrumdialog[i]->SpectrumDialog->raise();
  }
}


void ViewCurve::Z_scoringButton()
{
  int i, j;

  if(mainwindow->signalcomp[signal_nr]->ecg_filter != NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Heartrate detection is active for this signal!");
    messagewindow.exec();

    return;
  }

  if(mainwindow->signalcomp[signal_nr]->zratio_filter != NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Z-ratio is already active for this signal!");
    messagewindow.exec();

    return;
  }

  if(mainwindow->annot_editor_active || mainwindow->epoch_editor_active)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Close the annotation editor and try again.");
    messagewindow.exec();

    return;
  }

  for(i=0; i<MAXZSCOREDIALOGS; i++)
  {
    if(mainwindow->zscoredialog[i] == NULL)
    {
      mainwindow->zscoredialog[i] = new UI_ZScoreWindow(mainwindow, mainwindow->zscoredialog, i, signal_nr);

      break;
    }
  }

  sidemenu->close();

  if(i<MAXZSCOREDIALOGS)
  {
    for(j=0; j<MAXSPECTRUMDIALOGS; j++)
    {
      if(mainwindow->signalcomp[signal_nr]->zscoredialog[j] == 0)
      {
        mainwindow->signalcomp[signal_nr]->zscoredialog[j] = i + 1;

        break;
      }
    }

    mainwindow->zscoredialog[i]->zscore_dialog->move(((i % 15) * 30) + 200, ((i % 15) * 30) + 200);

    for(j=0; j<MAXZSCOREDIALOGS; j++)
    {
      if(mainwindow->zscoredialog[j] != NULL)
      {
        mainwindow->zscoredialog[j]->zscore_dialog->raise();
      }
    }
    mainwindow->zscoredialog[i]->zscore_dialog->activateWindow();
    mainwindow->zscoredialog[i]->zscore_dialog->raise();
  }
}


void ViewCurve::FittopaneButton()
{
  int j,
      pane_size,
      signalcomps;

  struct signalcompblock **signalcomp;


  signalcomps = mainwindow->signalcomps;
  signalcomp = mainwindow->signalcomp;

  if(!signalcomps)
  {
    sidemenu->close();

    return;
  }

  if(signal_nr>(signalcomps - 1))
  {
    sidemenu->close();

    return;
  }

  pane_size = (int)(((double)height() * 0.95) / signalcomps);

  for(j=0; j<signalcomp[signal_nr]->num_of_signals; j++)
  {
    if(signalcomp[signal_nr]->max_dig_value!=signalcomp[signal_nr]->min_dig_value)
    {
      signalcomp[signal_nr]->sensitivity[j] = (double)pane_size / (double)(signalcomp[signal_nr]->max_dig_value - signalcomp[signal_nr]->min_dig_value);
    }
    else
    {
      signalcomp[signal_nr]->sensitivity[j] = pane_size;
    }

    signalcomp[signal_nr]->voltpercm =
      signalcomp[signal_nr]->edfhdr->edfparam[signalcomp[signal_nr]->edfsignal[0]].bitvalue
      / (signalcomp[signal_nr]->sensitivity[0] * mainwindow->pixelsizefactor);

    signalcomp[signal_nr]->screen_offset = ((signalcomp[signal_nr]->max_dig_value + signalcomp[signal_nr]->min_dig_value) / 2.0) * signalcomp[signal_nr]->sensitivity[0];
  }

  drawCurve_stage_1();

  sidemenu->close();
}



void ViewCurve::ColorButton()
{
  int color;

  sidemenu->hide();

  UI_ColorMenuDialog colormenudialog(&color, this);

  if(color < 0)
  {
    sidemenu->close();

    return;
  }

  mainwindow->signalcomp[signal_nr]->color = color;

  update();

  sidemenu->close();
}



void ViewCurve::ScaleBox2Changed(double value)
{
  mainwindow->signalcomp[signal_nr]->screen_offset = -(value / (mainwindow->pixelsizefactor * mainwindow->signalcomp[signal_nr]->voltpercm));

  drawCurve_stage_1();
}



void ViewCurve::ScaleBoxChanged(double value)
{
  int i;

  double original_value;

  if(mainwindow->signalcomp[signal_nr]->edfhdr->edfparam[mainwindow->signalcomp[signal_nr]->edfsignal[0]].bitvalue < 0.0)
  {
    value *= -1.0;
  }

  for(i=0; i<mainwindow->signalcomp[signal_nr]->num_of_signals; i++)
  {
    mainwindow->signalcomp[signal_nr]->sensitivity[i] = mainwindow->signalcomp[signal_nr]->edfhdr->edfparam[mainwindow->signalcomp[signal_nr]->edfsignal[i]].bitvalue / value / mainwindow->pixelsizefactor;
  }

  original_value = mainwindow->signalcomp[signal_nr]->voltpercm;

  mainwindow->signalcomp[signal_nr]->voltpercm = value;

  mainwindow->signalcomp[signal_nr]->screen_offset *= (original_value / value);

  drawCurve_stage_1();
}



void ViewCurve::RemovefilterButton()
{
  int j;

  if(!mainwindow->signalcomps)
  {
    sidemenu->close();

    return;
  }

  if(signal_nr>(mainwindow->signalcomps - 1))
  {
    sidemenu->close();

    return;
  }

  for(j=0; j<mainwindow->signalcomp[signal_nr]->filter_cnt; j++)
  {
    free(mainwindow->signalcomp[signal_nr]->filter[j]);
  }

  mainwindow->signalcomp[signal_nr]->filter_cnt = 0;

  for(j=0; j<mainwindow->signalcomp[signal_nr]->ravg_filter_cnt; j++)
  {
    free_ravg_filter(mainwindow->signalcomp[signal_nr]->ravg_filter[j]);
  }

  mainwindow->signalcomp[signal_nr]->ravg_filter_cnt = 0;

  if(mainwindow->signalcomp[signal_nr]->ecg_filter != NULL)
  {
    free_ecg_filter(mainwindow->signalcomp[signal_nr]->ecg_filter);

    mainwindow->signalcomp[signal_nr]->ecg_filter = NULL;

    strcpy(mainwindow->signalcomp[signal_nr]->signallabel, mainwindow->signalcomp[signal_nr]->signallabel_bu);
    mainwindow->signalcomp[signal_nr]->signallabellen = mainwindow->signalcomp[signal_nr]->signallabellen_bu;
    strcpy(mainwindow->signalcomp[signal_nr]->physdimension, mainwindow->signalcomp[signal_nr]->physdimension_bu);
  }

  if(mainwindow->signalcomp[signal_nr]->zratio_filter != NULL)
  {
    free_zratio_filter(mainwindow->signalcomp[signal_nr]->zratio_filter);

    mainwindow->signalcomp[signal_nr]->zratio_filter = NULL;

    strcpy(mainwindow->signalcomp[signal_nr]->signallabel, mainwindow->signalcomp[signal_nr]->signallabel_bu);
    mainwindow->signalcomp[signal_nr]->signallabellen = mainwindow->signalcomp[signal_nr]->signallabellen_bu;
    strcpy(mainwindow->signalcomp[signal_nr]->physdimension, mainwindow->signalcomp[signal_nr]->physdimension_bu);
  }

  for(j=0; j<mainwindow->signalcomp[signal_nr]->fidfilter_cnt; j++)
  {
    free(mainwindow->signalcomp[signal_nr]->fidfilter[j]);
    fid_run_free(mainwindow->signalcomp[signal_nr]->fid_run[j]);
    fid_run_freebuf(mainwindow->signalcomp[signal_nr]->fidbuf[j]);
    fid_run_freebuf(mainwindow->signalcomp[signal_nr]->fidbuf2[j]);
  }

  mainwindow->signalcomp[signal_nr]->fidfilter_cnt = 0;

  mainwindow->signalcomp[signal_nr]->signallabel[mainwindow->signalcomp[signal_nr]->signallabellen] = 0;

  drawCurve_stage_1();

  sidemenu->close();
}



void ViewCurve::RemovesignalButton()
{
  int i, j, p;

  if(!mainwindow->signalcomps)
  {
    sidemenu->close();

    return;
  }

  if(signal_nr>(mainwindow->signalcomps - 1))
  {
    sidemenu->close();

    return;
  }

  if(mainwindow->spectrumdock->signalcomp == mainwindow->signalcomp[signal_nr])
  {
    mainwindow->spectrumdock->clear();
    mainwindow->spectrumdock->dock->hide();
  }

  for(i=0; i<MAXSPECTRUMDIALOGS; i++)
  {
    p = mainwindow->signalcomp[signal_nr]->spectr_dialog[i];

    if(p != 0)
    {
      delete mainwindow->spectrumdialog[p - 1];

      mainwindow->spectrumdialog[p - 1] = NULL;
    }
  }

  for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
  {
    p = mainwindow->signalcomp[signal_nr]->avg_dialog[i];

    if(p != 0)
    {
      delete mainwindow->averagecurvedialog[p - 1];

      mainwindow->averagecurvedialog[p - 1] = NULL;
    }
  }

  for(i=0; i<MAXZSCOREDIALOGS; i++)
  {
    p = mainwindow->signalcomp[signal_nr]->zscoredialog[i];

    if(p != 0)
    {
      delete mainwindow->zscoredialog[p - 1];

      mainwindow->zscoredialog[p - 1] = NULL;
    }
  }

  if(mainwindow->signalcomp[signal_nr]->hascursor2)
  {
    crosshair_2.active = 0;
    crosshair_2.moving = 0;
  }

  if(mainwindow->signalcomp[signal_nr]->hascursor1)
  {
    crosshair_1.active = 0;
    crosshair_2.active = 0;
    crosshair_1.moving = 0;
    crosshair_2.moving = 0;

    for(i=0; i<mainwindow->signalcomps; i++)
    {
      mainwindow->signalcomp[i]->hascursor2 = 0;
    }
  }

  if(mainwindow->signalcomp[signal_nr]->hasruler)
  {
    ruler_active = 0;
    ruler_moving = 0;
  }

  for(j=0; j<mainwindow->signalcomp[signal_nr]->filter_cnt; j++)
  {
    free(mainwindow->signalcomp[signal_nr]->filter[j]);
  }

  mainwindow->signalcomp[signal_nr]->filter_cnt = 0;

  for(j=0; j<mainwindow->signalcomp[signal_nr]->ravg_filter_cnt; j++)
  {
    free_ravg_filter(mainwindow->signalcomp[signal_nr]->ravg_filter[j]);
  }

  mainwindow->signalcomp[signal_nr]->ravg_filter_cnt = 0;

  if(mainwindow->signalcomp[signal_nr]->ecg_filter != NULL)
  {
    free_ecg_filter(mainwindow->signalcomp[signal_nr]->ecg_filter);

    mainwindow->signalcomp[signal_nr]->ecg_filter = NULL;

    strcpy(mainwindow->signalcomp[signal_nr]->signallabel, mainwindow->signalcomp[signal_nr]->signallabel_bu);
    mainwindow->signalcomp[signal_nr]->signallabellen = mainwindow->signalcomp[signal_nr]->signallabellen_bu;
    strcpy(mainwindow->signalcomp[signal_nr]->physdimension, mainwindow->signalcomp[signal_nr]->physdimension_bu);
  }

  if(mainwindow->signalcomp[signal_nr]->zratio_filter != NULL)
  {
    free_zratio_filter(mainwindow->signalcomp[signal_nr]->zratio_filter);

    mainwindow->signalcomp[signal_nr]->zratio_filter = NULL;

    strcpy(mainwindow->signalcomp[signal_nr]->signallabel, mainwindow->signalcomp[signal_nr]->signallabel_bu);
    mainwindow->signalcomp[signal_nr]->signallabellen = mainwindow->signalcomp[signal_nr]->signallabellen_bu;
    strcpy(mainwindow->signalcomp[signal_nr]->physdimension, mainwindow->signalcomp[signal_nr]->physdimension_bu);
  }

  for(j=0; j<mainwindow->signalcomp[signal_nr]->fidfilter_cnt; j++)
  {
    free(mainwindow->signalcomp[signal_nr]->fidfilter[j]);
    fid_run_free(mainwindow->signalcomp[signal_nr]->fid_run[j]);
    fid_run_freebuf(mainwindow->signalcomp[signal_nr]->fidbuf[j]);
    fid_run_freebuf(mainwindow->signalcomp[signal_nr]->fidbuf2[j]);
  }

  mainwindow->signalcomp[signal_nr]->fidfilter_cnt = 0;

  free(mainwindow->signalcomp[signal_nr]);

  for(i=signal_nr; i<mainwindow->signalcomps - 1; i++)
  {
    mainwindow->signalcomp[i] = mainwindow->signalcomp[i + 1];
  }

  mainwindow->signalcomps--;

  drawCurve_stage_1();

  sidemenu->close();
}


void ViewCurve::RulerButton()
{
  int i;


  crosshair_1.moving = 0;
  crosshair_2.moving = 0;
  ruler_active = 0;
  ruler_moving = 0;
  use_move_events = 0;
  setMouseTracking(false);

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    mainwindow->signalcomp[i]->hasruler = 0;
  }

  if(width() < 300)
  {
    goto END_OF_FUNC;
  }

  if(height() < 300)
  {
    goto END_OF_FUNC;
  }

  if((mainwindow->pagetime / TIME_DIMENSION) < 2LL)
  {
    QMessageBox messagewindow(QMessageBox::Warning, "Ruler", "The ruler can not be used when the Timescale\n is set to a value less than 2 seconds.");
    messagewindow.exec();

    goto END_OF_FUNC;
  }

  if((mainwindow->pagetime / TIME_DIMENSION) > 120LL)
  {
    QMessageBox messagewindow(QMessageBox::Warning, "Ruler", "The ruler can not be used when the Timescale\n is set to a value more than 120 seconds.");
    messagewindow.exec();

    goto END_OF_FUNC;
  }

  if((mainwindow->signalcomp[signal_nr]->voltpercm < 1.0) && (mainwindow->signalcomp[signal_nr]->voltpercm > -1.0))
  {
    QMessageBox messagewindow(QMessageBox::Warning, "Ruler", "The ruler can not be used when the Amplitude\n is set to a value less than 1.");
    messagewindow.exec();

    goto END_OF_FUNC;
  }

  if((mainwindow->signalcomp[signal_nr]->voltpercm > 2000.0) || (mainwindow->signalcomp[signal_nr]->voltpercm < -2000.0))
  {
    QMessageBox messagewindow(QMessageBox::Warning, "Ruler", "The ruler can not be used when the Amplitude\n is set to a value more than 2000.");
    messagewindow.exec();

    goto END_OF_FUNC;
  }

  ruler_x_position = 200;
  ruler_y_position = 200;
  mainwindow->signalcomp[signal_nr]->hasruler = 1;
  ruler_active = 1;

  update();

END_OF_FUNC:

  sidemenu->close();
}


void ViewCurve::CrosshairButton()
{
  if(!crosshair_1.active)
  {
    for(int i=0; i<mainwindow->signalcomps; i++)
    {
      mainwindow->signalcomp[i]->hascursor1 = 0;
      mainwindow->signalcomp[i]->hascursor2 = 0;
    }
    crosshair_1.value = 0.0;
    crosshair_2.value = 0.0;
    mainwindow->signalcomp[signal_nr]->hascursor1 = 1;
    use_move_events = 0;
    setMouseTracking(true);
    crosshair_1.active = 1;
    crosshair_2.active = 0;
    crosshair_1.moving = 0;
    crosshair_2.moving = 0;
    crosshair_1.file_num = mainwindow->signalcomp[signal_nr]->filenum;

    crosshair_1.x_position = w * 0.3; // early third part of the window
    crosshair_1.y_position = h * 0.7; // annotation of crosshair at lower half of the window

    drawCurve_stage_1();
  }
  else
  {
    if(!crosshair_2.active)
    {
      for(int i=0; i<mainwindow->signalcomps; i++)
      {
        mainwindow->signalcomp[i]->hascursor2 = 0;
      }
      crosshair_2.value = 0.0;
      mainwindow->signalcomp[signal_nr]->hascursor2 = 1;
      use_move_events = 0;
      setMouseTracking(true);
      crosshair_2.active = 1;
      crosshair_1.moving = 0;
      crosshair_2.moving = 0;
      crosshair_2.file_num = mainwindow->signalcomp[signal_nr]->filenum;

      crosshair_2.x_position = w * 0.7;   // late third part of the window
      crosshair_2.y_position = h * 0.7;   // annotation of crosshair at lower half of the window

      drawCurve_stage_1();
    }
  }

  sidemenu->close();
}


void ViewCurve::resizeEvent(QResizeEvent *rs_event)
{
  drawCurve_stage_1();

  QWidget::resizeEvent(rs_event);
}


inline void ViewCurve::floating_ruler(QPainter *painter, int x_pos, int y_pos, struct signalcompblock *signalcomp, int print_linewidth)
{
  int i, j;

  double d_tmp,
         d_tmp2,
         h_size,
         w_size,
         pixels_per_second;

  char str_hz[7][16]= {" 9", "10", "11", "12", "13", "14", "15"};

  char str_uv[5][16]= {"200", "150", "100", "50", "0"};

  char str_s[4][16]= {"1.00", "0.75", "0.50", "0.25"};


  if(w < 400)
  {
    return;
  }

  if((mainwindow->pagetime / TIME_DIMENSION) < 2LL)
  {
    return;
  }

  if((mainwindow->pagetime / TIME_DIMENSION) > 60LL)
  {
    return;
  }

  if((signalcomp->voltpercm < 1.0) && (signalcomp->voltpercm > -1.0))
  {
    return;
  }

  if((signalcomp->voltpercm > 2000.0) || (signalcomp->voltpercm < -2000.0))
  {
    return;
  }

  x_pos *= printsize_x_factor;
  y_pos *= printsize_y_factor;

  pixels_per_second = ((double)w) / (((double)mainwindow->pagetime) / ((double)TIME_DIMENSION));

  h_size = 4.0 * painter_pixelsizefactor;

  w_size = (double)w / 10.0;

  d_tmp = h_size / 7.0;

  d_tmp2 = h_size / 14.0;

  if(floating_ruler_value)
  {
    for(i=0; i<7; i++)
    {
      snprintf(str_hz[i], 15, "%.1f",  (pixels_per_second / w_size) * (2.0 + i));

      str_hz[i][15] = 0;
    }

    if(((int)signalcomp->voltpercm) < 20)
    {
      for(i=0; i<5; i++)
      {
        snprintf(str_uv[i], 15, "%.1f", signalcomp->voltpercm * i * (double)(signalcomp->polarity * -1));

        str_uv[i][15] = 0;
      }
    }
    else
    {
      for(i=0; i<5; i++)
      {
        snprintf(str_uv[i], 15, "%i", (int)(signalcomp->voltpercm * i * (double)(signalcomp->polarity * -1)));

        str_uv[i][15] = 0;
      }
    }
  }
  else
  {
    for(i=0; i<7; i++)
    {
      snprintf(str_hz[i], 15, "%.1f",  (w_size / pixels_per_second) * (9.0 + i));

      str_hz[i][15] = 0;
    }

    if(((int)signalcomp->voltpercm) < 20)
    {
      for(i=0; i<5; i++)
      {
        snprintf(str_uv[i], 15, "%.1f", signalcomp->voltpercm * (4 - i) * (double)signalcomp->polarity);

        str_uv[i][15] = 0;
      }
    }
    else
    {
      for(i=0; i<5; i++)
      {
        snprintf(str_uv[i], 15, "%i", (int)(signalcomp->voltpercm * (4 - i)) * signalcomp->polarity);

        str_uv[i][15] = 0;
      }
    }
  }

  for(i=0; i<4; i++)
  {
    snprintf(str_s[i], 15, "%.2f",  ((w / 40.0) / pixels_per_second) * (4 - i));

    str_s[i][15] = 0;
  }

  painter->setPen((Qt::GlobalColor)floating_ruler_color);

  painter->drawText(x_pos, y_pos - ((double)h / 80.0), "Hz");

  for(i=0; i<7; i++)
  {
    painter->drawText(x_pos - ((double)w / 30.0), y_pos + ((double)h / 204.8) + d_tmp2 + (d_tmp * i), str_hz[i]);
  }

  for(i=1; i<7; i++)
  {
    painter->drawLine(x_pos, y_pos + (d_tmp * i), x_pos + w_size, y_pos + (d_tmp * i));
  }

  painter->drawRect(x_pos, y_pos, w_size, h_size);

  if(floating_ruler_value)
  {
    for(j=2; j<9; j++)
    {
      d_tmp2 = w_size / j;

      for(i=1; i<j; i++)
      {
        painter->drawLine(x_pos + (d_tmp2 * i), y_pos + (d_tmp * (j - 2)), x_pos + (d_tmp2 * i), y_pos + (d_tmp * (j - 1)));
      }
    }
  }
  else
  {
    for(j=9; j<16; j++)
    {
      d_tmp2 = w_size / j;

      for(i=1; i<j; i++)
      {
        painter->drawLine(x_pos + (d_tmp2 * i), y_pos + (d_tmp * (j - 9)), x_pos + (d_tmp2 * i), y_pos + (d_tmp * (j - 8)));
      }
    }
  }

  painter->drawText(x_pos + (3.0 * printsize_x_factor), y_pos + h_size + (25.0 * printsize_y_factor), "more...");

  x_pos += (w_size + ((double)w / 18.0));

  d_tmp = h_size / 4.0;

  painter->drawText(x_pos - ((double)w / 45.0), y_pos - ((double)h / 80.0), signalcomp->physdimension);

  for(i=0; i<5; i++)
  {
    painter->drawText(x_pos - ((double)w / 45.0), y_pos + ((double)h / 204.8) + (d_tmp * i), str_uv[i]);

    painter->drawLine(x_pos + ((double)w / 160.0), y_pos + (d_tmp * i), x_pos + ((double)w / 45.7), y_pos + (d_tmp * i));
  }

  painter->drawLine(x_pos + ((double)w / 45.7), y_pos, x_pos + ((double)w / 45.7), y_pos + h_size);

  d_tmp = h_size / 20.0;

  for(i=1; i<20; i++)
  {
    painter->drawLine(x_pos + ((double)w / 71.1), y_pos + (d_tmp * i), x_pos + ((double)w / 45.7), y_pos + (d_tmp * i));
  }

  x_pos += ((double)w / 10.0);

  painter->drawLine(x_pos, y_pos, x_pos + (w_size / 2.0), y_pos + h_size);
  painter->drawLine(x_pos + w_size, y_pos, x_pos + (w_size / 2.0), y_pos + h_size);

  d_tmp = h_size / 4.0;
  d_tmp2 = w_size / 8.0;

  for(i=0; i<4; i++)
  {
    painter->drawLine(x_pos + (d_tmp2 * i), y_pos + (d_tmp * i), x_pos + w_size - (d_tmp2 * i), y_pos + (d_tmp * i));
  }

  for(i=0; i<4; i++)
  {
    painter->drawText(x_pos - ((double)w / 25.0) + (d_tmp2 * i), y_pos + ((double)h / 204.8) + (d_tmp * i), str_s[i]);
  }

  painter->drawText(x_pos + ((double)w / 125.0) + w_size, y_pos + ((double)h / 204.8), "Sec");

  special_pen->setColor((Qt::GlobalColor)floating_ruler_color);

  special_pen->setWidth(print_linewidth);

  painter->setPen(*special_pen);

  d_tmp = h_size / 20.0;
  d_tmp2 = w_size / 40.0;

  for(i=1; i<20; i++)
  {
    painter->drawLine(x_pos + (d_tmp2 * i), y_pos + (d_tmp * i), x_pos + w_size - (d_tmp2 * i), y_pos + (d_tmp * i));
  }
}


void ViewCurve::backup_colors_for_printing()
{
  int i;

  backup_color_1 = backgroundcolor;
  backgroundcolor = Qt::white;
  backup_color_2 = small_ruler_color;
  small_ruler_color = Qt::black;
  backup_color_3 = big_ruler_color;
  big_ruler_color = Qt::black;
  backup_color_4 = mouse_rect_color;
  mouse_rect_color = Qt::black;
  backup_color_5 = text_color;
  text_color = Qt::black;
  backup_color_14 = floating_ruler_color;
  floating_ruler_color = Qt::black;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    backup_color_10[i] = mainwindow->signalcomp[i]->color;
    mainwindow->signalcomp[i]->color = Qt::black;
  }
  backup_color_11 = crosshair_1.color;
  crosshair_1.color = Qt::black;
  backup_color_12 = crosshair_2.color;
  crosshair_2.color = Qt::black;
  backup_color_13 = baseline_color;
  baseline_color = Qt::black;
  backup_color_15 = annot_marker_color;
  annot_marker_color = Qt::black;
  epoch_marker_color = Qt::black;
}


void ViewCurve::restore_colors_after_printing()
{
  int i;

  backgroundcolor = backup_color_1;
  small_ruler_color = backup_color_2;
  big_ruler_color = backup_color_3;
  mouse_rect_color = backup_color_4;
  text_color = backup_color_5;
  floating_ruler_color = backup_color_14;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    mainwindow->signalcomp[i]->color = backup_color_10[i];
  }
  crosshair_1.color = backup_color_11;
  crosshair_2.color = backup_color_12;
  baseline_color = backup_color_13;
  annot_marker_color = backup_color_15;
  epoch_marker_color = backup_color_15;
}



void ViewCurve::set_crosshairs(double fraction_1, double fraction_2)
{
  if(crosshair_1.active) crosshair_1.x_position = fraction_1 * width();
  if(crosshair_2.active) crosshair_2.x_position = fraction_2 * width();
}



void ViewCurve::strip_types_from_label(char *label)
{
  int i,
      len;


  len = strlen(label);
  if(len<16)
  {
    return;
  }

  if( (not strncmp(label, "EEG ", 4)) // Electrencephalogram
      or (not strncmp(label, "ECG ", 4)) // Electrocardiogram
      or (not strncmp(label, "EOG ", 4)) // Electrookulogram
      or (not strncmp(label, "ERG ", 4)) // Electroretinogramm
      or (not strncmp(label, "EMG ", 4)) // Electromuogram
      or (not strncmp(label, "MEG ", 4)) // Magnetencephalogram
      or (not strncmp(label, "MCG ", 4)) ) // Magnetokardiogram??
  {
    if(label[4] != ' ')
    {
      for(i=0; i<(len-4); i++)
      {
        label[i] = label[i+4];
      }

      for(; i<len; i++)
      {
        label[i] = ' ';
      }
    }
  }
}


















