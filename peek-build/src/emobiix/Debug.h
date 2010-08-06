#ifndef _DEBUG_H_
#define _DEBUG_H_

#define EMO_DEBUG_UI  (0)
#define EMO_DLVL_WARN (1)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SIMULATOR
void script_emo_printf(const char *fmt, ...);
#ifdef WIN32
#define NL "\r\n"
#else
#define NL "\n"
#endif
#else
#define NL
#endif

void emo_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif /* _DEBUG_H_ */
