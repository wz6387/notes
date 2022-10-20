#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_
#include <functional>
#include <memory>
#include <string>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class WebSocketConnection;
typedef std::shared_ptr<WebSocketConnection> WebSocketConnectionPtr;
typedef std::function<void (const WebSocketConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const WebSocketConnectionPtr&)> CloseCallback;
typedef std::function<void (const WebSocketConnectionPtr&, const std::string&)> MessageCallback;
typedef std::function<void()> TimerCallback;

void defaultConnectionCallback(const WebSocketConnectionPtr& conn);
void defaultCloseCallback(const WebSocketConnectionPtr& conn);
void defaultMessageCallback(const WebSocketConnectionPtr& conn, const std::string& msg);

#endif
