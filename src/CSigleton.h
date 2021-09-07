#pragma once
#include <cassert>
#include <mutex>
#include <memory>

/************************************************************************/
/* 名称：万能单例类                                                     */
/* 说明：可把任何类包装成线程安全的全局单例类，出口默认智能指针			*/
/************************************************************************/

template<typename T>
class CSingleton
{
public:

	//获取全局单例对象
	template<typename ...Args>
	static std::shared_ptr<T> GetInstance(Args&&... args)
	{
		if (!m_pSington)
		{
			std::lock_guard<std::mutex> gLock(m_Mutex);
			if (nullptr == m_pSington)
			{
				m_pSington = std::make_shared<T>(std::forward<Args>(args)...);
			}
		}
		return m_pSington;
	}

	//主动析构单例对象（一般不需要主动析构，除非特殊需求）
	static void DesInstance()
	{
		if (m_pSington)
		{
			m_pSington.reset();
			m_pSington = nullptr;
		}
	}

private:
	explicit CSingleton();
	CSingleton(const CSingleton&) = delete;
	CSingleton& operator=(const CSingleton&) = delete;

private:
	static std::shared_ptr<T> m_pSington;
	static std::mutex m_Mutex;
};

template<typename T>
std::shared_ptr<T> CSingleton<T>::m_pSington = nullptr;

template<typename T>
std::mutex CSingleton<T>::m_Mutex;