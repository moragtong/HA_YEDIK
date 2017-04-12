#pragma once
#include "client_util.h"
#include <vector.h>
enum { CLN_NUM = 32, TIMEOUT = 800 };
using namespace util;
using namespace socket;
namespace p2p_client {
	class p2p_client : protected p2p_socket {
		private:
			bool seeding;
			etl::vector<util::sockaddr, CLN_NUM> clients;
			util::sockaddr tracker;
			void send_command_tracker(trk_com_enum com) {
				trk_com msg{ com };
				socket::sendto(sock, (char*)&msg, sizeof(msg), 0, &tracker, sizeof(tracker));
			}
			unsigned short command_tracker(trk_com_enum com) {
				send_command_tracker(com);
				unsigned short status;
				recvfrom(sock, (char*)&status, sizeof(status), 0, 0, 0);
				std::cout << "command_tracker: " << WSAGetLastError() << ' ' << status << '\n';
				return status;
			}
			void _seed() {
				const char * enum_rep[]{ "FILEDATA", "PKT" };
				puts("seed running");
				cln_com recvd_com;
				util::sockaddr client;
				while (seeding) {
					int fromlen = sizeof(client);
					auto got = recvfrom(sock, (char*)&recvd_com, sizeof(recvd_com), 0, &client, &fromlen);
					if (got > 0) {
						std::cout << socket::WSAGetLastError() << '\n';
						if (recvd_com.command == PKT) {
							char buff[buffsize];
							auto fhandle = fopen(f.name, "rb");
							std::cout << "seed-fhandle: " << recvd_com.param;
							fseek(fhandle, recvd_com.param*buffsize, SEEK_SET);
							fread(buff, sizeof(char), sizeof(buff) / sizeof(char), fhandle);
							socket::sendto(sock, buff, sizeof(buff), 0, &client, sizeof(client));
							fclose(fhandle);
						} else if (recvd_com.command == FILEDATA)
							socket::sendto(sock, (char*)&f, sizeof(f), 0, &client, sizeof(client));
						if (recvd_com.command > 1)
							std::cout << recvd_com.command;
						else
							std::cout << enum_rep[recvd_com.command];
						std::cout << '\n';
					}
				}
				
			}
			void _recv() {
				auto fhandle = fopen(f.name, "wb");
				std::cout << "recv-fhandle: " << fhandle;
				auto psize = f.size / buffsize + (f.size % buffsize ? 1 : 0);
				cln_com pkt_msg{ PKT };
				char buff[buffsize];
				for (BYTE i = 0; pkt_msg.param < psize; i = (i + 1) % clients.size()) {
					sendto(sock, (char*)&pkt_msg, sizeof(pkt_msg), 0, clients.data() + i, sizeof(clients[i]));
					auto check = recvfrom(sock, buff, sizeof(buff), 0, 0, 0);
					std::cout << "recv_file: " << WSAGetLastError() << ' ' << check << '\n';
					if (check != -1) {
						pkt_msg.param++;
						fwrite(buff, sizeof(char), sizeof(buff) / sizeof(char), fhandle);
					}
				}
				if (fhandle)
					fclose(fhandle);
			}
			void request_filedata() {
				std::cout << "request_filedata:" << clients.size() << '\n';
				if (clients.size()) {
					cln_com filedata_msg{ FILEDATA };
					for (unsigned char i = 0;; i = (i + 1) % clients.size()) {
						sendto(sock, (char*)&filedata_msg, sizeof(filedata_msg), 0, clients.data() + i, sizeof(clients[i]));
						auto check = recvfrom(sock, (char*)&f, sizeof(f), 0, 0, 0);
						std::cout << "request_filedata: " << WSAGetLastError() << ' ' << check << '\n'
							<< f.name << ' ' << f.size << '\n';
						if (check != -1)
							break;
					}
					puts(f.name);
				} else
					puts("shit");
			}
		public:
			filedata f;
			p2p_client()
				: tracker({ 127,0,0,1 }, 16673),
				seeding(false) {
			}
			void init() {
				sock = socket::socket(AF_INET, SOCK_DGRAM, 0);
				util::sockaddr sname({ 127,0,0,1 }, 0);
				std::cout << bind(sock, &sname, sizeof(sname));
				int p = sizeof(sname);
				getsockname(sock, &sname, &p);
				std::cout << "client sock port: " << sname.port() << '\n';
				settimeout(0, TIMEOUT);
			}
			void seed() {
				if (!seeding) {
					command_tracker(ADD);
					seeding = true;
					_seed();
				}
			}
			void stop_seeding() {
				if (seeding) {
					seeding = false;
					command_tracker(REMOVE);
				}
			}
			void new_seed() {
				int check;
				int tsize = sizeof(tracker);
				do {
					sendto(sock, 0, 0, 0, &tracker, sizeof(tracker));
					check = recvfrom(sock, 0, 0, 0, &tracker, &tsize);
				} while (check < 0);
				sendto(sock, 0, 0, 0, &tracker, sizeof(tracker));
				std::cout << "new_seed: " << WSAGetLastError() << ' ' << tracker.port() << '\n';
				seeding = true;
				_seed();
			}
			void request_list() {
				if (tracker.port() != 16673) {
					clients.resize(clients.capacity());
					int size;
					do {
						send_command_tracker(LIST);
						size = recvfrom(sock, (char*)clients.data(), clients.capacity() * sizeof(clients[0]), 0, 0, 0);
					} while (size < 0);
					clients.resize(size / sizeof(clients[0]));
					std::cout << clients.size();
					std::cout << "request_list: " << WSAGetLastError() << '\n';
				} else
					puts("request_list why");
			}
			void start(unsigned short _port) {
				tracker.port(_port);
				request_list();
				request_filedata();
				_recv();
				seed();
			}
	};
}