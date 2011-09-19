/* jni_audio.c
 * JNIRecorder library for Android
 *
 * Copyright (c) 2010 Ethan Chen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
/*****************************************************************************/

#include <time.h>
#include "jni_audio.h"

static long get_timestamp()
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return now.tv_sec * 1000000 + now.tv_nsec / 1000;
}

static int is_running(jni_play *play)
{
  int ret = 0;
  pthread_mutex_lock(play->lock);
  ret = play->running;
  pthread_mutex_unlock(play->lock);
  return ret;
}

static int is_running(jni_record *record)
{
  int ret = 0;
  pthread_mutex_lock(record->lock);
  ret = record->running;
  pthread_mutex_unlock(record->lock);
  return ret;
}

static void record_function(void *ptr)
{
  jni_record *record = (jni_record *) ptr;
  JNIEnv *jni_env = NULL;
  jbyteArray j_in_buf;
  jbyte *in_buf;
  jmethodID read_method, record_method;
  int bytes_read;
  long now, last_frame;
  int elapsed_ms, to_wait_ms;
  // TODO(echen): figure out values for these
  int size;
  int nframes;
  int samples_per_sec;
  // for frame time calculation
  int frame_time = nframes * 1000 / samples_per_sec;
  int missed_time = frame_time

  ATTACH_JVM(jni_env);

  // TODO(echen): lots of error checking
  read_method = (*jni_env)->GetMethodID(record->r_class, "read", "([BII)I");
  record_method = (*jni_env)->GetMethodID(record->r_class,
                                          "startRecording", "()V");
  j_in_buf = (*jni_env)->NewByteArray(size);
  in_buf = (*jni_env)->GetByteArrayElements(j_in_buf, 0);

  // TODO(echen): set thread priority to ANDROID_PRIORITY_AUDIO
  (*jni_env)->CallVoidMethod(record->r_obj, record_method);
  last_frame = get_timestamp();
  while (is_running(record)) {
    now = get_timestamp();
    elapsed_ms = now - last_frame;
    last_frame = get_timestamp();
    // adjust time if we are filling faster than time
    missed_time = missed_time / 2 + elapsed_ms - frame_time;
    if (missed_time <= 0) {
      to_wait_ms = (-1 * missed_time) - 2;
      if (to_wait_ms > 0) {
        // sleep(to_wait_ms)
      }
    }
    bytes_read = (*jni_env)->CallIntMethod(record->r_obj,
                                           read_method,
                                           j_in_buf,
                                           0, size);
    if (bytes_read <= 0) {
      // error
    }
    if (bytes_read != size) {
      // overrun?
    }

    // in_buf is aliased to j_in_buf
    (*record->r_callback)(in_buf);
  }

  (*jni_env)->ReleaseByteArrayElements(j_in_buf, in_buf, 0);
  (*jni_env)->DeleteLocalRef(j_in_buf);
  DETACH_JVM(jni_env);
  return 0;
}

static void play_function(void *ptr)
{
  jni_play *play = (jni_play *) ptr;
  JNIEnv *jni_env = NULL;
  ATTACH_JVM(jni_env);
  jbyteArray j_out_buf;
  jbyte *out_buf;
  jmethodID write_method, play_method;
  // TODO(echen): figure out values for these
  int size;
  int nframes;

  write_method = (*jni_env)->GetMethodID(play->p_class, "write", "([BII)I");
  play_method = (*jni_env)->GetMethodID(play->p_class, "play", "()V");

  j_out_buf = (*jni_env)->NewByteArray(size);
  out_buf = (*jni_env)->GetByteArrayElements(j_out_buf, 0);

  // TODO(echen): set thread priority to ANDROID_PRIORITY_AUDIO
  (*jni_env)->CallVoidMethod(play->p_obj, play_method);

  while (is_running(play)) {
    count = play->p_callback(out_buf);
    status = (*jni_env)->CallIntMethod(play->p_obj,
                                       write_method,
                                       j_out_buf,
                                       0, size);
    // TODO(echen): error checking
  }

  (*jni_env)->ReleaseByteArrayElements(j_out_buf, out_buf, 0);
  (*jni_env)->DeleteLocalRef(j_out_buf);
  DETACH_JVM(jni_env);
  return 0;
}

jni_audio *init_jni_audio(int sample_rate, jobject audio_record,
    jobject audio_track)
{
  jni_audio *audio = NULL;
  JNIEnv *jni_env = NULL;
  ATTACH_JVM(jni_env);

  audio = (jni_audio *) malloc(sizeof(jni_audio));
  record = (jni_record *) malloc(sizeof(jni_record))
  play = (jni_play *) malloc(sizeof(jni_play))

  if (audio) {
    if (record) {
      // init record object
      record->r_thread = (pthread_t *) malloc(sizeof(pthread_t));
      record->lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
      pthread_mutex_init(record->lock, NULL);
      record->running = 0;
      record->r_obj = (*jni_env)->NewGlobalRef(audio_record);
      record->r_class = (jclass) (*jni_env)->NewGlobalRef(
          jni_env->FindClass("android/media/AudioRecord"));
      record->r_callback = NULL;
      audio->record = record;
    }

    if (play) {
      // init play object
      play->p_thread = (pthread_t *) malloc(sizeof(pthread_t));
      play->lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
      pthread_mutex_init(play->lock, NULL);
      play->running = 0;
      play->p_obj = (*jni_env)->NewGlobalRef(audio_track);
      play->p_class = (jclass) (*jni_env)->NewGlobalRef(
          jni_env->FindClass("android/media/AudioTrack"));
      play->p_callback = NULL;
      audio->play = play;
    }
  }

  DETACH_JVM(jni_env);
  return audio;
}

void set_record_callback(jni_audio *audio, void (*callback)(jbyte *))
{
  jni_record *record = audio->record;
  if (record) {
    record->r_callback = callback;
  }
}

void set_play_callback(jni_audio *audio, void (*callback)(jbyte *))
{
  jni_play *play = audio->play;
  if (play) {
    play->p_callback = callback;
  }
}

void start_record(jni_audio *audio)
{
  int res;
  jni_record *record = audio->record;

  record->running = 1;
  res = pthread_create(record->record_thread,
                       NULL,
                       record_function,
                       (void *) record);
  if (res) {
    // error occurred
    record->running = 0;
    return;
  }
}

void start_play(jni_audio *audio)
{
  int res;
  jni_play *play = audio->play;

  play->running = 1;
  res = pthread_create(play->play_thread,
                       NULL,
                       play_function,
                       (void *) play);
  if (res) {
    // error occurred
    play->running = 0;
    return;
  }
}

void stop_record(jni_audio *audio)
{
  int res;
  jni_record *record = audio->record;

  pthread_mutex_lock(record->lock);
  record->running = 0;
  pthread_mutex_unlock(record->lock);
  res = pthread_join(*(record->r_thread), NULL);
  if (res) {
    // error occurred
    return;
  }
}

void stop_play(jni_audio *audio)
{
  int res;
  jni_play *play = audio->play;

  pthread_mutex_lock(play->lock);
  play->running = 0;
  pthread_mutex_unlock(play->lock);
  res = pthread_join(*(play->p_thread), NULL);
  if (res) {
    // error occurred
    return;
  }
}

void cleanup_jni_audio(jni_audio *audio)
{
  JNIEnv *jni_env = NULL;
  jmethodID release_method = NULL;
  int running = 0;
  ATTACH_JVM(jni_env);

  jni_record *record = audio->record;
  jni_play *play = audio->play;

  if (is_running(record)) {
    stop_record(audio);
  }

  if (is_running(play)) {
    stop_play(audio);
  }

  if (record) {
    free(record->r_thread);
    pthread_mutex_destroy(&record->lock);

    release_method = (*jni_env)->GetMethodID(record->r_class, "release", "()V");
    (*jni_env)->CallVoidMethod(record->r_obj, release_method);
    (*jni_env)->DeleteGlobalRef(record->r_obj);
    (*jni_env)->DeleteGlobalRef(record->r_class);

    record->r_obj = NULL;
    record->r_class = NULL;

    free(record);
  }
  if (play) {
    free(play->p_thread);
    pthread_mutex_destroy(&play->lock);

    release_method = (*jni_env)->GetMethodID(play->p_class,
                                             "release",
                                             "()V");
    (*jni_env)->CallVoidMethod(play->p_obj, release_method);
    (*jni_env)->DeleteGlobalRef(play->p_obj);
    (*jni_env)->DeleteGlobalRef(play->p_class);

    audio->play = NULL;
    audio->play_class = NULL;

    free(play);
  }
  if (audio) {
    free(audio);
  }
  DETACH_JVM(jni_env);
}
