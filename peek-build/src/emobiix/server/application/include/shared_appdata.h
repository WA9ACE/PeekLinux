#ifndef __EMOBIIX_SHARED_APPDATA_H__
#define __EMOBIIX_SHARED_APPDATA_H__

#include <string>
#include <map>
#include <boost/thread/mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

namespace emobiix {

class connection;

struct appdata
{
	connection *device;
};

class shared_appdata : private boost::noncopyable
{
  shared_appdata();

public:
	static shared_appdata& instance();

  bool get(const std::string& key, appdata& appdata);
  void put(const std::string &key, const appdata& appdata);
  void remove(const std::string& key);

private:
	std::map<std::string, appdata> m_map_appdata;
	boost::mutex m_mutex;
};

} 

#endif // __EMOBIIX_SHARED_APPDATA_H__
