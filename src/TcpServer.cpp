#include "Log.h"
#include "Scheduler.h"
#include "TcpServer.h"

#include <assert.h>


namespace melon {

TcpServer::TcpServer(const IpAddress& listen_addr, Scheduler* scheduler)
	:listen_addr_(listen_addr),
	listen_socket_(Socket::CreateTcp()),
	scheduler_(scheduler) {

	listen_socket_->SetNonBlockAndCloseOnExec();
	listen_socket_->bind(listen_addr_);
}

void TcpServer::start() {
	listen_socket_->listen();

	scheduler_->addTask(std::bind(&TcpServer::startAccept, this), "Accept");
}

void TcpServer::startAccept() {
	while (true) {
		IpAddress peer_addr;
		int connfd = listen_socket_->accept(peer_addr);

		Socket::Ptr socket = std::make_shared<Socket>(connfd);
		socket->SetNonBlockAndCloseOnExec();
		scheduler_->addTask(std::bind(&TcpServer::handleClient, this, std::make_shared<TcpConnection>(socket, peer_addr)), "Connect");
	}
}

void TcpServer::handleClient(TcpConnection::Ptr connection) {
	LOG_INFO << "new connection, peer addr:" << connection->peerAddr().toString();
}

}
