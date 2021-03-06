/*
 * libjingle
 * Copyright 2004--2008, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "event.h"

#if defined(WIN32)
#include <windows.h>
#elif defined(POSIX)
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#else
#error "Must define either WIN32 or POSIX."
#endif

namespace txmpp {

#if defined(WIN32)

Event::Event(bool manual_reset, bool initially_signaled)
    : is_manual_reset_(manual_reset),
      is_initially_signaled_(initially_signaled),
      event_handle_(NULL) {
}

bool Event::EnsureInitialized() {
  if (event_handle_ == NULL) {
    event_handle_ = ::CreateEvent(NULL,                 // Security attributes.
                                  is_manual_reset_,
                                  is_initially_signaled_,
                                  NULL);                // Name.
  }

  return (event_handle_ != NULL);
}

Event::~Event() {
  if (event_handle_ != NULL) {
    CloseHandle(event_handle_);
    event_handle_ = NULL;
  }
}

bool Event::Set() {
  if (!EnsureInitialized())
    return false;

  SetEvent(event_handle_);
  return true;
}

bool Event::Reset() {
  if (!EnsureInitialized())
    return false;

  ResetEvent(event_handle_);
  return true;
}

bool Event::Wait(int cms) {
  DWORD ms = (cms == kForever)? INFINITE : cms;

  if (!EnsureInitialized())
    return false;
  else
    return (WaitForSingleObject(event_handle_, ms) == WAIT_OBJECT_0);
}

#elif defined(POSIX)

Event::Event(bool manual_reset, bool initially_signaled)
    : is_manual_reset_(manual_reset),
      event_status_(initially_signaled),
      event_mutex_initialized_(false),
      event_cond_initialized_(false) {
}

bool Event::EnsureInitialized() {
  if (!event_mutex_initialized_) {
    if (pthread_mutex_init(&event_mutex_, NULL) == 0)
      event_mutex_initialized_ = true;
  }

  if (!event_cond_initialized_) {
    if (pthread_cond_init(&event_cond_, NULL) == 0)
      event_cond_initialized_ = true;
  }

  return (event_mutex_initialized_ && event_cond_initialized_);
}

Event::~Event() {
  if (event_mutex_initialized_) {
    pthread_mutex_destroy(&event_mutex_);
    event_mutex_initialized_ = false;
  }

  if (event_cond_initialized_) {
    pthread_cond_destroy(&event_cond_);
    event_cond_initialized_ = false;
  }
}

bool Event::Set() {
  if (!EnsureInitialized())
    return false;

  pthread_mutex_lock(&event_mutex_);
  event_status_ = true;
  pthread_cond_broadcast(&event_cond_);
  pthread_mutex_unlock(&event_mutex_);
  return true;
}

bool Event::Reset() {
  if (!EnsureInitialized())
    return false;

  pthread_mutex_lock(&event_mutex_);
  event_status_ = false;
  pthread_mutex_unlock(&event_mutex_);
  return true;
}

bool Event::Wait(int cms) {
  if (!EnsureInitialized())
    return false;

  pthread_mutex_lock(&event_mutex_);
  int error = 0;

  if (cms != kForever) {
    // Converting from seconds and microseconds (1e-6) plus
    // milliseconds (1e-3) to seconds and nanoseconds (1e-9).

    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct timespec ts;
    ts.tv_sec = tv.tv_sec + (cms / 1000);
    ts.tv_nsec = tv.tv_usec * 1000 + (cms % 1000) * 1000000;

    // Handle overflow.
    if (ts.tv_nsec >= 1000000000) {
      ts.tv_sec++;
      ts.tv_nsec -= 1000000000;
    }

    while (!event_status_ && error == 0)
      error = pthread_cond_timedwait(&event_cond_, &event_mutex_, &ts);
  } else {
    while (!event_status_ && error == 0)
      error = pthread_cond_wait(&event_cond_, &event_mutex_);
  }

  // NOTE(liulk): Exactly one thread will auto-reset this event. All
  // the other threads will think it's unsignaled.  This seems to be
  // consistent with auto-reset events in WIN32.
  if (error == 0 && !is_manual_reset_)
    event_status_ = false;

  pthread_mutex_unlock(&event_mutex_);

  return (error == 0);
}

#endif  // defined WIN32/POSIX

}  // namespace txmpp
