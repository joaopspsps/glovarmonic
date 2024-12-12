#pragma once

#include "Printable.h"
#include <ESPAsyncWebServer.h>

namespace ws {

AsyncWebSocket server("/ws");

namespace messages {
    enum class Header : uint8_t {
        SetRGB = 0x00,
        SetButtonRGB = 0x01,
    };

    enum class FromBufferFailReason {
        MissingHeader,
        UnknownHeader,
        IncorrectPayloadSize,
    };

    class Message {
      public:
        virtual ~Message() = default;
        virtual Header getHeader() const = 0;
        static Message *fromBuffer(const uint8_t *data, size_t len,
                                   FromBufferFailReason * = nullptr);
    };

    class SetRGB : public Message, public Printable {
      public:
#pragma pack(push, 1)
        struct Payload {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        } payload;
#pragma pack(pop)

        SetRGB() = default;
        SetRGB(uint8_t r, uint8_t g, uint8_t b)
            : payload{.r = r, .g = g, .b = b} {}

        Header getHeader() const override { return Header::SetRGB; }
        static SetRGB *fromBuffer(const uint8_t *data, size_t len,
                                  FromBufferFailReason * = nullptr);

        size_t printTo(Print &) const override;
    };

    class SetButtonRGB : public Message, public Printable {
      public:
#pragma pack(push, 1)
        struct Payload {
            uint8_t button;
            uint8_t r;
            uint8_t g;
            uint8_t b;
        } payload;
#pragma pack(pop)

        SetButtonRGB() = default;
        SetButtonRGB(uint8_t button, uint8_t r, uint8_t g, uint8_t b)
            : payload{.button = button, .r = r, .g = g, .b = b} {}

        Header getHeader() const override { return Header::SetButtonRGB; }
        static SetButtonRGB *fromBuffer(const uint8_t *data, size_t len,
                                        FromBufferFailReason * = nullptr);

        size_t printTo(Print &) const override;
    };
} // namespace messages

void handleEvent(AsyncWebSocket *ws, AsyncWebSocketClient *client,
                 AwsEventType type, void *arg, uint8_t *data, size_t len);

// Handlers for each type of event
// ===============================

void handleEventConnect(AsyncWebSocket *ws, AsyncWebSocketClient *client,
                        void *arg, uint8_t *data, size_t len);
void handleEventDisconnect(AsyncWebSocket *ws, AsyncWebSocketClient *client,
                           void *arg, uint8_t *data, size_t len);
void handleEventData(AsyncWebSocket *ws, AsyncWebSocketClient *client,
                     void *arg, uint8_t *data, size_t len);

// Handlers for each Message class
// ===============================

void handleMessageSetRGB(AsyncWebSocket *ws, AsyncWebSocketClient *client,
                         const messages::SetRGB *msg);
void handleMessageSetButtonRGB(AsyncWebSocket *ws, AsyncWebSocketClient *client,
                               const messages::SetButtonRGB *msg);

} // namespace ws
