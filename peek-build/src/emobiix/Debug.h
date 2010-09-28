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

#define emo_abort abort()

#define EMO_ASSERT(__ast, __msg) \
if (!(__ast)) { \
	emo_printf("ASSERT:" __FILE__ ":%d: " __msg NL, __LINE__); \
	emo_abort; \
	return; \
}

#define EMO_ASSERT_NULL(__ast, __msg) \
if (!(__ast)) { \
	emo_printf("ASSERT:" __FILE__ ":%d: "__msg NL, __LINE__); \
	emo_abort; \
	return NULL; \
}

#define EMO_ASSERT_INT(__ast, __ival, __msg) \
if (!(__ast)) { \
	emo_printf("ASSERT:" __FILE__ ":%d: "__msg NL, __LINE__); \
	emo_abort; \
	return (__ival); \
}

#ifdef __cplusplus
}
#endif
#endif /* _DEBUG_H_ */
