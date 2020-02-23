#pragma once

inline namespace BASE
{
	template <typename T>
	class TSingleton
	{
	public:
		static T& GetInstance()
		{
			static T instance;
			return instance;
		}
	protected:
		TSingleton() {}
		~TSingleton() {}
	public:
		TSingleton(TSingleton const&) = delete;
		TSingleton& operator=(TSingleton const&) = delete;
	};
};