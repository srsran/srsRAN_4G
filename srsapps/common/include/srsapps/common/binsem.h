
#include <pthread.h>

#ifndef BINSEM_H
#define BINSEM_H

/** Implementation of a binary semaphore using POSIX condition variable and mutex
 */

namespace srslte {

  class binsem
  {
  public: 
    binsem() {
      pthread_mutex_init(&mutex, NULL);
      pthread_cond_init(&cv, NULL);
      state = true; 
    }
    ~binsem() {
      pthread_mutex_destroy(&mutex);
      pthread_cond_destroy(&cv); 
    }
    void take() {
      pthread_mutex_lock(&mutex);
      while(!state) {
        pthread_cond_wait(&cv, &mutex);        
      }
      state = false;
      pthread_mutex_unlock(&mutex);
    }
    void give() {
      pthread_mutex_lock(&mutex);
      pthread_cond_signal(&cv);
      state = true; 
      pthread_mutex_unlock(&mutex);      
    }
  private:
    pthread_cond_t cv; 
    pthread_mutex_t mutex; 
    bool state; 
  };
}

#endif
