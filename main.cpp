#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};

	// ソケット作成
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		std::cerr << "Socket failed" << std::endl;
		return EXIT_FAILURE;
	}

	// ソケットオプション設定
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)))
	{
		std::cerr << "setsockopt failed" << std::endl;
		close(server_fd);
		return EXIT_FAILURE;
	}

	// アドレスの設定
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// バインド
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cerr << "Bind failed" << std::endl;
		close(server_fd);
		return EXIT_FAILURE;
	}

	// リッスン
	if (listen(server_fd, 3) < 0)
	{
		std::cerr << "Listen failed" << std::endl;
		close(server_fd);
		return EXIT_FAILURE;
	}

	std::cout << "Waiting for connections..." << std::endl;

	// クライアント接続の受け入れ
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
	{
		std::cerr << "Accept failed" << std::endl;
		close(server_fd);
		return EXIT_FAILURE;
	}

	std::cout << "Connection accepted" << std::endl;

	// データの受信・送信（エコー）
	int valread;
	while ((valread = read(new_socket, buffer, BUFFER_SIZE)) > 0)
	{
		std::cout << "Received: " << buffer << std::endl;
		send(new_socket, buffer, valread, 0);
        if (strncmp(buffer, "exit", 4) == 0)
        {
            break;
        }
		memset(buffer, 0, BUFFER_SIZE); // バッファのクリア
	}

	// 接続の終了
	close(new_socket);
	close(server_fd);

	std::cout << "Connection closed" << std::endl;

	return EXIT_SUCCESS;
}
