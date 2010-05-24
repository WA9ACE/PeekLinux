#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* semaphore support */
struct Semaphore_t;
typedef struct Semaphore_t Semaphore;

Semaphore *semaphore_create(int initialValue);
void semaphoreP(Semaphore *s);
void semaphoreV(Semaphore *s);

/* thread support */
struct Thread_t;
typedef struct Thread_t Thread;

Thread *thread_run(void (*)(void *), void *d);

/* Cache folder */
extern const char *cacheBaseDir;

/* Platform Initialization */
void PlatformInit(void);

#ifdef __cplusplus
}
#endif

#endif /* _PLATFORM_H_ */
