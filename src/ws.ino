#include "Printable.h"
#include <ESPAsyncWebServer.h>

#include "peripherals.hpp"
#include "serial.hpp"
#include "ws.hpp"

namespace ws {

namespace messages {
    // Message
    // =======

    Message *Message::fromBuffer(const uint8_t *data, size_t len,
                                 FromBufferFailReason *failReason) {
        if (len < sizeof(Header)) {
            *failReason = FromBufferFailReason::MissingHeader;
            return nullptr;
        }

        const auto header = static_cast<Header>(*data);
        data += sizeof(Header);
        len -= sizeof(Header);

        switch (header) {
        case Header::SetRGB:
            return SetRGB::fromBuffer(data, len, failReason);
        case Header::SetButtonRGB:
            return SetButtonRGB::fromBuffer(data, len, failReason);
        default:
            *failReason = FromBufferFailReason::UnknownHeader;
            return nullptr;
        }
    }

    // SetRGB
    // ======

    SetRGB *SetRGB::fromBuffer(const uint8_t *data, size_t len,
                               FromBufferFailReason *failReason) {
        if (len != sizeof(Payload)) {
            *failReason = FromBufferFailReason::IncorrectPayloadSize;
            return nullptr;
        }
        auto *msg = new SetRGB();
        memcpy(&msg->payload, data, sizeof(Payload));
        return msg;
    }

    size_t SetRGB::printTo(Print &p) const {
        return p.printf("SetRGB(r=0x%02hhx, g=0x%02hhx, b=0x%02hhx)",
                        this->payload.r, this->payload.g, this->payload.b);
    }

    // SetButtonRGB
    // ============

    SetButtonRGB *SetButtonRGB::fromBuffer(const uint8_t *data, size_t len,
                                           FromBufferFailReason *failReason) {
        if (len != sizeof(Payload)) {
            *failReason = FromBufferFailReason::IncorrectPayloadSize;
            return nullptr;
        }
        auto *msg = new SetButtonRGB();
        memcpy(&msg->payload, data, sizeof(Payload));
        return msg;
    }

    size_t SetButtonRGB::printTo(Print &p) const {
        return p.printf(
            "SetButtonRGB(button=%hhu, r=0x%02hhx, g=0x%02hhx, b=0x%02hhx)",
            payload.button, payload.r, payload.g, payload.b);
    }
} // namespace messages

void handleEvent(AsyncWebSocket *ws, AsyncWebSocketClient *client,
                 AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
    case WS_EVT_CONNECT:
        ::ws::handleEventConnect(ws, client, arg, data, len);
        break;
    case WS_EVT_DISCONNECT:
        ::ws::handleEventDisconnect(ws, client, arg, data, len);
        break;
    case WS_EVT_DATA:
        ::ws::handleEventData(ws, client, arg, data, len);
        break;
    default:
        break;
    }
}

// Handlers for each type of event
// ===============================

void handleEventConnect(AsyncWebSocket *ws, AsyncWebSocketClient *client,
                        void *arg, uint8_t *data, size_t len) {
    SERIAL_PRINTF("[I] [WS %d] Conectado\n", client->id());
}

void handleEventDisconnect(AsyncWebSocket *ws, AsyncWebSocketClient *client,
                           void *arg, uint8_t *data, size_t len) {
    SERIAL_PRINTF("[I] [WS %d] Desconectado\n", client->id());
}

void handleEventData(AsyncWebSocket *ws, AsyncWebSocketClient *client,
                     void *arg, uint8_t *data, size_t len) {
    const auto *info = static_cast<AwsFrameInfo *>(arg);
    if (info->opcode != WS_BINARY)
        return;

    messages::FromBufferFailReason failReason;
    const auto *msg = messages::Message::fromBuffer(data, len, &failReason);
    if (msg == nullptr) {
        switch (failReason) {
        case messages::FromBufferFailReason::MissingHeader:
            ws->text(client->id(), "Error: message has no header\n");
            return;
        case messages::FromBufferFailReason::UnknownHeader:
            ws->text(client->id(), "Error: message has unexpected header\n");
            return;
        case messages::FromBufferFailReason::IncorrectPayloadSize:
            ws->text(client->id(),
                     "Error: message has unexpected payload size\n");
            return;
        default:
            ws->text(client->id(), "Error: message could not parsed\n");
            return;
        }
    }

    switch (msg->getHeader()) {
    case messages::Header::SetRGB:
        handleMessageSetRGB(ws, client,
                            static_cast<const messages::SetRGB *>(msg));
        break;
    case messages::Header::SetButtonRGB:
        handleMessageSetButtonRGB(
            ws, client, static_cast<const messages::SetButtonRGB *>(msg));
        break;
    default:
        assert(false);
        break;
    }
}

// Handlers for each Message class
// ===============================

void handleMessageSetRGB(AsyncWebSocket *ws, AsyncWebSocketClient *client,
                         const messages::SetRGB *msg) {
    SERIAL_PRINTF("[D] [WS %d] ", client->id());
    SERIAL_PRINTLN(*msg);

    peripherals::RGB(msg->payload.r, msg->payload.g, msg->payload.b).setColor();
}

void handleMessageSetButtonRGB(AsyncWebSocket *ws, AsyncWebSocketClient *client,
                               const messages::SetButtonRGB *msg) {
    SERIAL_PRINTF("[D] [WS %d] ", client->id());
    SERIAL_PRINTLN(*msg);

    peripherals::RGB *rgb;
    switch (msg->payload.button) {
    case 1:
        rgb = &peripherals::rgbButtonIndex;
        break;
    case 2:
        rgb = &peripherals::rgbButtonMiddle;
        break;
    case 3:
        rgb = &peripherals::rgbButtonRing;
        break;
    default:
        ws->text(client->id(),
                 "Error: invalid button number, must be 1, 2 or 3");
        break;
    }

    *rgb = peripherals::RGB(msg->payload.r, msg->payload.g, msg->payload.b);
}

} // namespace ws
