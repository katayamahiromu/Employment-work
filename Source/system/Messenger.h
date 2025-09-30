#pragma once

#include <functional>
#include <string>
#include <map>

class Messenger
{
public:
	//ƒƒbƒZ[ƒWóMŠÖ”
	using Function = void(void*);
	using Receiver = std::function<Function>;

	static constexpr const uint64_t InvalidKey = 0xFFFFFFFFFFFFFFFF;
private:
	Messenger() {};
	~Messenger() { clear(); };
public:
	static Messenger& instance()
	{
		static Messenger instance;
		return instance;
	}
	//óMŠÖ”“o˜^‰ğœ
	void clear();

	//ƒf[ƒ^‘—M
	void sendData(const std::string& identifier, void* data);

	//ŠÖ”“o˜^(“¯ˆêŠÖ”‚ğ•¡”“o˜^‚Å‚«‚é‚Ì‚Å’ˆÓ)
	uint64_t addReceiver(const std::string& identifer, Receiver receiver);

	//ŠÖ”“o˜^‰ğœ(“o˜^‚ÌƒL[‚ª•K—v)
	void removeReceiver(uint64_t key);

	template<typename ty>
	inline ty *getMessageData(void*data)
	{
		return static_cast<ty*>(data);
	}
private:
	struct Data
	{
		uint64_t key;
		Receiver func;
		Data(uint64_t key, Receiver func) :key(key),func(func){}
		bool operator==(Data& r)
		{
			return key == r.key;
		}
	};
	std::multimap<std::string, Data> receivers;
	uint64_t incrementKey = 0;
};