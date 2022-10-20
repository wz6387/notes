#include "WebSocketEventLoop.h"
#include "WebSocketEventLoopThread.h"
#include "WebSocketClient.h"
#include <iostream>
#ifdef WIN32
#include <conio.h>
#endif

void connectCB(const WebSocketConnectionPtr&)
{
	std::cout << "onConnect" << std::endl;
}

void closeCB(const WebSocketConnectionPtr&)
{
	std::cout << "onClose" << std::endl;
}

void MessageCB(const WebSocketConnectionPtr& conn, const std::string& msg)
{
	std::cout << "recv:" << msg << std::endl;
}


int main(void)
{

/*
	WebSocketEventLoop base;
	std::thread thread([&]() {
		base.loop();
	});

	WebSocketClient client(&base);
	client.setConnectionCallback(connectCB);
	client.setCloseCallback(closeCB);
	client.setMessageCallback(MessageCB);
	client.connect("community.tvtsecu.com", 8890, "/ws");

	char ch;
	while (ch = _getch()) {
		if (ch == 'q') {
			break;
		}
	}
	client.disconnect();
	base.quit();
	thread.join();
*/

	WebSocketEventLoopThread loopthread;
	WebSocketEventLoop* loop = loopthread.startLoop();
	//void* tv = loop->runAfter([]() {std::cout << "timeout callback" << std::endl;}, 2);

	WebSocketClient client(loop);
	client.enableRetry();
	client.setConnectionCallback(connectCB);
	client.setCloseCallback(closeCB);
	client.setMessageCallback(MessageCB);
	//client.connect("community.tvtsecu.com", 8890, "BFEBFBFF000906E90000W9A9R4NZ", "/ws");
	client.connect("community.tvtsecu.com", 8020, "cid", "/ws");
	
	char ch;
#ifdef WIN32
	while (ch = _getch()) {
#else
    while (ch = getchar()) {
#endif
		
		if (ch == 'q') {
			client.disconnect();
			std::cout << "quit" << std::endl;
		} else if (ch == 'x') {
			break;
		} else {
			std::cout << "input 'x' to exit;" << std::endl;
			std::cout << "input 'q' to disconnect;" << std::endl;
		}
	}
	
	return 0;
}
