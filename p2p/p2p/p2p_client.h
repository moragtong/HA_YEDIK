#pragma once
#include "client_util.h"
#include <vector.h>
using namespace util;
using namespace socket;
namespace p2p_client {
	class p2p_client {
		private:
			std::thread seed_thread;
			socket::SOCKET recv_sock;
			socket::SOCKET seed_sock;
			util::sockaddr tracker;
			etl::vector<util::sockaddr, 128> clients;
			void send_command_tracker(trk_com_enum com) {
				trk_com msg{ com };
				socket::sendto(recv_sock, (char*)&msg, sizeof(msg), 0, &tracker, sizeof(tracker));
			}
			int settimeout(long sec, long usec) {
				const timeval& tv{ sec, usec };
				auto v = setsockopt(recv_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
				std::cout << "setsockopt(" << tv.tv_sec << ", " << tv.tv_usec << ") -> " << v << '\n';
				return v;
			}
		public:
			filedata f;
			p2p_client() :
				tracker("127.0.0.1", 16673) {
				socket::init_winsock();
				recv_sock = util::udp_sock();
				seed_sock = util::udp_sock();
				{
					util::sockaddr sname("127.0.0.1", 0);
					std::cout << socket::bind(recv_sock, &sname, sizeof(sname));
					int p = sizeof(sname);
					socket::getsockname(recv_sock, &sname, &p);
					std::cout << "recv sock port: " << sname.port() << '\n';
					sname.port(sname.port() + 1);
					std::cout << "seed sock port: " << sname.port() << '\n';
					std::cout << socket::bind(seed_sock, &sname, sizeof(sname)) << '\n';
				}
				seed_thread = std::thread(&p2p_client::seed, this);
			}
			void seed() {
				const char * enum_rep[]{ "FILEDATA", "PKT" };
				puts("seed running");
				cln_com recvd_com;
				util::sockaddr client;
				for (;;) {
					int fromlen = sizeof(client);
					socket::recvfrom(seed_sock, (char*)&recvd_com, sizeof(recvd_com), 0, &client, &fromlen);
					std::cout << socket::WSAGetLastError() << '\n';
					if (recvd_com.command == PKT) {
						char buff[buffsize];
						auto fhandle = fopen(f.name, "rb");
						std::cout << "seed-fhandle: " << recvd_com.param;
						fseek(fhandle, recvd_com.param*buffsize, SEEK_SET);
						fread(buff, sizeof(char), sizeof(buff) / sizeof(char), fhandle);
						socket::sendto(seed_sock, buff, sizeof(buff), 0, &client, sizeof(client));
						fclose(fhandle);
					} else if (recvd_com.command == FILEDATA)
						socket::sendto(seed_sock, (char*)&f, sizeof(f), 0, &client, sizeof(client));
					if (recvd_com.command > 1)
						std::cout << recvd_com.command;
					else
						std::cout << enum_rep[recvd_com.command];
					std::cout << '\n';
				}
			}
			void command_tracker(trk_com_enum com) {
				send_command_tracker(com);
				trk_com_enum status;
				socket::recvfrom(recv_sock, (char*)&status, sizeof(status), 0, 0, 0);
				std::cout << "command_tracker: " << socket::WSAGetLastError() << ' ' << status << '\n';
			}
			void request_list() {
				send_command_tracker(LIST);
				clients.resize(clients.capacity());
				auto size = socket::recvfrom(recv_sock, (char*)clients.data(), clients.capacity() * sizeof(clients[0]), 0, 0, 0);
				clients.resize(size / sizeof(decltype(clients)::value_type));
				std::cout << clients.size();
				std::cout << "request_list: " << WSAGetLastError() << '\n';
			}
			void request_filedata() {
				std::cout << "request_filedata:" << clients.size() << '\n';
				cln_com filedata_msg{ FILEDATA };
				settimeout(0, 200);
				for (unsigned char i = 0;; i = (i + 1) % clients.size()) {
					socket::sendto(recv_sock, (char*)&filedata_msg, sizeof(filedata_msg), 0, clients.data() + i, sizeof(clients[i]));
					auto check = socket::recvfrom(recv_sock, (char*)&f, sizeof(f), 0, 0, 0);
					std::cout << "request_filedata: " << socket::WSAGetLastError() << ' ' << check << '\n'
						<< f.name << ' ' << f.size << '\n';
					if (check != -1)
						break;
				}
				settimeout(0, 0);
				puts(f.name);
			}
			void recv_file() {
				auto fhandle = fopen(f.name, "wb");
				std::cout << "recv-fhandle: " << fhandle;
				auto psize = f.size / buffsize + (f.size % buffsize ? 1 : 0);
				cln_com pkt_msg{ PKT };
				char buff[buffsize];
				settimeout(0, 200);
				for (size_t i = 0; pkt_msg.param < psize; i = (i + 1) % clients.size()) {
					socket::sendto(recv_sock, (char*)&pkt_msg, sizeof(pkt_msg), 0, clients.data() + i, sizeof(clients[i]));
					auto check = socket::recvfrom(recv_sock, buff, sizeof(buff), 0, 0, 0);
					std::cout << "recv_file: " << socket::WSAGetLastError() << ' ' << check << '\n';
					if (check != -1) {
						pkt_msg.param++;
						fwrite(buff, sizeof(char), sizeof(buff) / sizeof(char), fhandle);
					}
				}
				settimeout(0, 0);
				if (fhandle)
					fclose(fhandle);
			}
			~p2p_client() {
				socket::closesocket(recv_sock);
				seed_thread.join();
			}
	};
}