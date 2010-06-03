#include <boost/interprocess/sync/scoped_lock.hpp>

#include "shared_appdata.h"
#include "logger.h"

using namespace std;

namespace emobiix
{

shared_appdata& shared_appdata::instance()
{
	static shared_appdata inst;
	return inst;
}

shared_appdata::shared_appdata()
{
}

void shared_appdata::remove(const std::string& key)
{
	boost::mutex::scoped_lock removelock(m_mutex);

	map<string, appdata>::iterator it = m_map_appdata.find(key);
	if (it != m_map_appdata.end())
		m_map_appdata.erase(it);
}

bool shared_appdata::get(const std::string& key, appdata& data)
{
	boost::mutex::scoped_lock getlock(m_mutex);

	map<string, appdata>::iterator it = m_map_appdata.find(key);
	if (it != m_map_appdata.end())
	{
		data = it->second;
		return true;
	}

	return false;
}

void shared_appdata::put(const std::string &key, const appdata& data)
{
	boost::mutex::scoped_lock putlock(m_mutex);

	m_map_appdata[key] = data;
}

}
