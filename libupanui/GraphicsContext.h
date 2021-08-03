/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/
 */

#pragma once

#include <timer_thread.h>

namespace upanui {
  class FrameManager;

  class GraphicsContext {
  private:
    static GraphicsContext* _instance;
    GraphicsContext();
    ~GraphicsContext();
  public:
    static void Init();
    static void Destroy();
    static GraphicsContext& Instance();

    uint32_t pitch() const {
      return _frameBufferInfo._pitch;
    }

    uint32_t bytesPerPixel() const {
      return _frameBufferInfo._bpp / 8;
    }

  private:
    class RefreshThread : public upan::timer_thread {
    public:
      RefreshThread(GraphicsContext& gc);
      void on_timer_trigger() override;
    private:
      GraphicsContext& _gc;
    };

  private:
    FrameBufferInfo _frameBufferInfo;
    FrameManager* _frame;
    RefreshThread* _refreshThread;
    friend class RefreshThread;
  };
}