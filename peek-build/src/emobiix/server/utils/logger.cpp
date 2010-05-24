#include "logger.h"
#include <time.h>
#include <sys/time.h>

using namespace std;

namespace emobiix
{

logger::logger()
	: m_nLogLevel(LOG_VERBOSE),
		m_stream(&(std::cerr))
{
}

void logger::SetStream(ostream &stream)
{
	m_stream = &stream;
}

void logger::SetLevel(eLogLevel nLogLevel)
{
	m_nLogLevel = nLogLevel;
}

emobiix::eLogLevel logger::GetLevel() const 
{ 
	return m_nLogLevel; 
}

ostream &logger::Log(eLogLevel level, const char *file, int line, const char *function) 
{
	static char *msgLabels[LOG_VERBOSE + 1] = { "ERROR", "INFO", "DEBUG", "TRACE" };
	(*m_stream) << msgLabels[level] << " [" << GetCurrentLocal() << "] " << file << ":" << line << " " << function << "(): ";
	return (*m_stream);
}

string logger::GetCurrentLocal()
{
    static char szBuffer[64] = "";

    struct timeval tv;
    gettimeofday(&tv, NULL);

    strftime(szBuffer, sizeof(szBuffer), "%T", localtime(&(tv.tv_sec)));
    sprintf(szBuffer + 8, ".%06d", (int)tv.tv_usec);

    return szBuffer;
}

const char* logger::FormatFunction(char *szFunction)
{
	const char *function = szFunction;
	if (char *pszParen = strchr(szFunction, '('))
	{
		*pszParen = 0;
		if (const char *pszSpace = strrchr(szFunction, ' '))
			function = pszSpace + 1;
	}

	return function;
}

}
