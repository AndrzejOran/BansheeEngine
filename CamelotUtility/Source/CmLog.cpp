#include "CmLog.h"
#include "CmException.h"
#include "boost/signal.hpp"

namespace BansheeEngine
{
	LogEntry::LogEntry(const String& msg, const String& level)
		:mMsg(msg), mChannel(level)
	{ }

	Log::Log()
	{
	}

	Log::~Log()
	{
		CM_LOCK_RECURSIVE_MUTEX(mMutex);

		for(auto iter = mEntries.begin(); iter != mEntries.end(); ++iter)
			cm_delete<PoolAlloc>(*iter);
	}

	void Log::logMsg(const String& message, const String& level)
	{
		CM_LOCK_RECURSIVE_MUTEX(mMutex);

		LogEntry* newEntry = cm_new<LogEntry, PoolAlloc>(message, level);
		mEntries.push_back(newEntry);

		doOnEntryAdded(*newEntry);
	}

	void Log::clear()
	{
		CM_LOCK_RECURSIVE_MUTEX(mMutex);

		for(auto iter = mEntries.begin(); iter != mEntries.end(); ++iter)
			cm_delete<PoolAlloc>(*iter);

		mEntries.clear();
	}

	void Log::saveToFile(const WString& path)
	{
		// TODO - Save the log as HTML
		CM_EXCEPT(NotImplementedException, "Log save to file not yet implemented.");
	}

	void Log::doOnEntryAdded(const LogEntry& entry)
	{
		onEntryAdded(entry);
	}
}