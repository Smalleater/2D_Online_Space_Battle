#ifndef TRA_NETCODE_ENGINE_MESSAGE_HPP
#define TRA_NETCODE_ENGINE_MESSAGE_HPP

#include "TRA/export.hpp"
#include "TRA/debugUtils.hpp"

#include <string>
#include <map>
#include <vector>
#include <variant>
#include <memory>
#include <cstdint>
#include <functional>
#include <algorithm>

namespace tra::netcode::engine
{
	using FieldValue = std::variant<int, float, std::string>;
	using SerializerFunc = std::function<void(const void*, std::vector<uint8_t>&)>;
	using DeserializerFunc = std::function<void(const void*, const std::vector<uint8_t>&, size_t&)>;

	struct TRA_API Message
	{
	public:
		virtual ~Message() = default;
		virtual std::string getType() const = 0;
		virtual std::vector<uint8_t> serialize() const = 0;

		static std::map<uint32_t, std::vector<std::pair<std::string, std::pair<size_t, SerializerFunc>>>>& getSerializers();
		static std::map<uint32_t, std::vector<std::pair<std::string, std::pair<size_t, DeserializerFunc>>>>& getDeserializers();
	};

	namespace internal
	{
        TRA_API uint32_t hashTypeName(const char* _str);

        TRA_API void registerMessageType(const uint32_t _id,
			std::shared_ptr<Message>(*_creator)(const std::vector<uint8_t>&));

        TRA_API void serializeField(std::vector<uint8_t>& _data, int _value);
        TRA_API void serializeField(std::vector<uint8_t>& _data, float _value);
        TRA_API void serializeField(std::vector<uint8_t>& _data, const std::string& _value);

		template<typename T>
		void registerSerializer(const uint32_t _messageId, const std::string& _fieldName, size_t _fieldOffset)
		{
			auto& serializers = Message::getSerializers();
			serializers[_messageId].emplace_back(_fieldName, std::make_pair(_fieldOffset, [_fieldOffset](const void* base, std::vector<uint8_t>& data) {
				const T* field = reinterpret_cast<const T*>(static_cast<const char*>(base) + _fieldOffset);
				serializeField(data, *field);
				}));
		}

        TRA_API void deserializeField(const std::vector<uint8_t>& _data, size_t& _offset, int& _value);
        TRA_API void deserializeField(const std::vector<uint8_t>& _data, size_t& _offset, float& _value);
        TRA_API void deserializeField(const std::vector<uint8_t>& _data, size_t& _offset, std::string& _value);

		template<typename T>
		void registerDeserializer(const uint32_t _messageId, const std::string& _fieldName, size_t _fieldOffset)
		{
			auto& deserializers = Message::getDeserializers();
			deserializers[_messageId].emplace_back(_fieldName, std::make_pair(_fieldOffset,
				[_fieldOffset](const void* base, const std::vector<uint8_t>& data, size_t& offset) {
					T* field = reinterpret_cast<T*>(static_cast<char*>(const_cast<void*>(base)) + _fieldOffset);
					deserializeField(data, offset, *field);
				}));
		}
	}
}

#define DECLARE_MESSAGE_BEGIN(MessageType) \
namespace tra::message { \
    struct MessageType : public Message \
    { \
    public: \
        static constexpr const char* MESSAGE_TYPE_NAME = #MessageType; \
        inline static uint32_t MESSAGE_TYPE_ID = internal::hashTypeName(MESSAGE_TYPE_NAME); \
        using CurrentMessageType = MessageType;

#define FIELD(type, name) \
    type name; \
    private: \
        struct name##_Registrar \
        { \
            name##_Registrar() \
            { \
                const auto offset = reinterpret_cast<size_t>(&(static_cast<CurrentMessageType*>(nullptr)->name)); \
                internal::registerSerializer<type>(MESSAGE_TYPE_ID, #name, offset); \
                internal::registerDeserializer<type>(MESSAGE_TYPE_ID, #name, offset); \
            } \
        }; \
        inline static name##_Registrar name##_reg; \
    public: \

#define DECLARE_MESSAGE_END() \
        std::string getType() const override { return MESSAGE_TYPE_NAME; } \
        std::vector<uint8_t> serialize() const override \
        { \
            std::vector<uint8_t> data; \
            uint32_t typeId = MESSAGE_TYPE_ID; \
            data.insert(data.end(), \
                reinterpret_cast<const uint8_t*>(&typeId), \
                reinterpret_cast<const uint8_t*>(&typeId) + sizeof(typeId)); \
            auto& serializers = getSerializers(); \
            auto it = serializers.find(MESSAGE_TYPE_ID); \
            if (it != serializers.end()) \
            { \
                for (const auto& [fieldName, fieldData] : it->second) \
                { \
                    fieldData.second(this, data); \
                } \
            } \
            return data; \
        } \
        static std::shared_ptr<Message> createFromBytes(const std::vector<uint8_t>& _payload) \
        { \
            std::shared_ptr<CurrentMessageType> message = std::make_shared<CurrentMessageType>(); \
            size_t offset = sizeof(uint32_t); \
            auto& deserializers = getDeserializers(); \
            auto it = deserializers.find(MESSAGE_TYPE_ID); \
            if (it != deserializers.end()) \
            { \
                for (const auto& [fieldName, fieldData] : it->second) \
                { \
                    fieldData.second(message.get(), _payload, offset); \
                } \
            } \
            return message; \
        } \
    private: \
        struct Register \
        { \
            Register() { internal::registerMessageType(MESSAGE_TYPE_ID, CurrentMessageType::createFromBytes); } \
        }; \
        inline static Register _register{}; \
    }; \
}

#endif