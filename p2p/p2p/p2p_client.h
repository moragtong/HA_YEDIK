#pragma once
#include "client_util.h"
#include <vector.h>
//#include "md5.h"
using namespace util;
using namespace socket;
namespace p2p_client {
	class p2p_client {
		private:
			std::thread seed_thread;
			SOCKET recv_sock;
			SOCKET seed_sock;
			util::sockaddr tracker;
			etl::vector<util::sockaddr, 128> clients;
			parser parser;
			int settimeout(long sec, long usec) {
				const timeval& tv{ sec, usec };
				return setsockopt(recv_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
			}
			int send_command_tracker(trk_com_enum com) {
				trk_com msg{ com };
				return sendto(recv_sock, (char*)&msg, sizeof(msg), 0, &tracker, sizeof(tracker));
			}
			/*int send_command_client(cln_com_enum com, const util::sockaddr& c) {
				cln_com msg{ com };
				return sendto(recv_sock, (char*)&msg, sizeof(msg), 0, &c, sizeof(c));
			}*/
		public:
			p2p_client() {
				tracker = { { 127, 0, 0, 1 }, 16673 };
				init_winsock();
				recv_sock = socket::socket(AF_INET, SOCK_DGRAM, 0);
				seed_sock = socket::socket(AF_INET, SOCK_DGRAM, 0);
				{
					util::sockaddr sname({ 127, 0, 0, 1 }, 0);
					std::cout << bind(recv_sock, &sname, sizeof(sname));
					int p = sizeof(sname);
					getsockname(recv_sock, &sname, &p);
					std::cout << "recv sock port: " << sname.port() << '\n';
					sname.port(sname.port() + 1);
					std::cout << "seed sock port: " << sname.port() << '\n';
					std::cout << bind(seed_sock, &sname, sizeof(sname)) << '\n';
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
					recvfrom(seed_sock, (char*)&recvd_com, sizeof(recvd_com), 0, &client, &fromlen);
					std::cout << WSAGetLastError() << '\n';
					if (recvd_com.command == PKT) {
						char buff[buffsize];
						auto fhandle = fopen(f.name, "rb");
						std::cout << "seed-fhandle: " << recvd_com.param;
						fseek(fhandle, recvd_com.param*buffsize, SEEK_SET);
						fread(buff, sizeof(char), sizeof(buff) / sizeof(char), fhandle);
						sendto(seed_sock, buff, sizeof(buff), 0, &client, sizeof(client));
						fclose(fhandle);
					} else if (recvd_com.command == FILEDATA)
						sendto(seed_sock, (char*)&f, sizeof(f), 0, &client, sizeof(client));
					if (recvd_com.command > 1)
						std::cout << recvd_com.command;
					else
						std::cout << enum_rep[recvd_com.command];
					std::cout << '\n';
				}
			}	
			int command_tracker(trk_com_enum com) {
				send_command_tracker(com);
				recvfrom(recv_sock, (char*)&com, sizeof(com), 0, 0, 0);
				std::cout << "command_tracker: " << WSAGetLastError() << ' ' << com << '\n';
				return com;
			}
			void request_list() {
				send_command_tracker(LIST);
				clients.resize(clients.capacity());
				auto size = socket::recvfrom(recv_sock, (char*)clients.begin(), clients.capacity() * sizeof(decltype(clients)::value_type), 0, 0, 0);
				clients.resize(size / sizeof(decltype(clients)::value_type));
				std::cout << clients.size();
				std::cout << "request_list: " << WSAGetLastError() << '\n';
			}
			size_t request_filedata_size() {
				cln_com size_msg{ FILEDATA_SIZE };
				size_t ret;
				settimeout(0, 200);
				for (auto& client : clients) {
					socket::sendto(recv_sock, (char*)&size_msg, sizeof(size_msg), 0, &client, sizeof(client));
					auto check = recvfrom(recv_sock, (char*)&ret, sizeof(ret), 0, 0, 0);
					std::cout << "request_filedata: " << WSAGetLastError() << ' ' << check << '\n';
					std::cout << ret << '\n';
					if (check != -1)
						break;
				}
				settimeout(0, 0);
				return ret;
			}
			void request_filedata(BYTE* output) {
				std::cout << "request_filedata:" << clients.size() << '\n';
				cln_com filedata_msg{ FILEDATA };
				settimeout(0, 200);
				for (auto& client : clients) {
					socket::sendto(recv_sock, (char*)&filedata_msg, sizeof(filedata_msg), 0, &client, sizeof(client));
					auto check = recvfrom(recv_sock, (char*)output, sizeof(output), 0, 0, 0);
					if (!check)
						parser.init(output);
					std::cout << "request_filedata: " << WSAGetLastError() << ' ' << check << '\n'
						<< parser.fname() << ' ' << parser.fname() << '\n';
					if (check != -1)
						break;
				}
				settimeout(0, 0);
			}
			void recv_file() {
				auto fhandle = fopen(f.name, "wb");
				std::cout << "recv-fhandle: " << fhandle;
				if (fhandle) {
					auto psize = f.size / buffsize + (f.size % buffsize ? 1 : 0);
					cln_com pkt_msg{ PKT };
					char buff[buffsize];
					settimeout(0, 200);
					for (size_t i = 0; pkt_msg.param < psize; i++) {
						socket::sendto(recv_sock, (char*)&pkt_msg, sizeof(pkt_msg), 0, clients._buff + i % clients.size(), sizeof(util::sockaddr));
						auto check = recvfrom(recv_sock, buff, sizeof(buff), 0, 0, 0);
						std::cout << "recv_file: " << WSAGetLastError() << ' ' << check << '\n';
						if (check != -1) {
							pkt_msg.param++;
							fwrite(buff, sizeof(char), sizeof(buff) / sizeof(char), fhandle);
						}
					}
					fclose(fhandle);
					settimeout(0, 0);
				}
			}
			~p2p_client() {
				closesocket(recv_sock);
				seed_thread.join();
			}
	};
}