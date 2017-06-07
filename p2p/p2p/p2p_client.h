#pragma once
#include "client_util.h"
#include <vector.h>
using namespace util;
using namespace socket;
/**
 * \class p2p_client
 * \brief A client that allows P2P downloads using a central tracker.
*/
class p2p_client : protected p2p_socket {
	private:
		etl::vector<util::sockaddr, CLN_NUM> clients;
		util::sockaddr tracker;
		filedata f;
		Gtk::TreeIter row_ref;
		Glib::ustring full_filename;
		/**
		 * Sends the specified command to the tracker.
		 * \param com	the command that should be sent to the tracker
		*/
		void send_command_tracker(trk_com_enum com) {
			trk_com msg{ com };
			socket::sendto(sock, (char*)&msg, sizeof(msg), 0, &tracker, sizeof(tracker));
		}
		/**
		 * Sends the specified command to the tracker and returns the tracker's status.
		 * \param com	the command that should be sent to the tracker
		*/
		unsigned short command_tracker(trk_com_enum com) {
			send_command_tracker(com);
			unsigned short status;
			recvfrom(sock, (char*)&status, sizeof(status), 0, 0, 0);
			std::cout << "command_tracker: " << WSAGetLastError() << ' ' << status << '\n';
			return status;
		}
		/**
		 * \brief	Uploads data on demand.
		 * Waits for a client to request a file piece or for an internal signal to stop doing so.
		*/
		void _seed() {
			static const char *const enum_rep[]{ "FILEDATA", "PKT" };
			puts("seed running");
			settimeout(0, 0);
			cln_com recvd_com;
			util::sockaddr client;
			const char *const open_name = full_filename.empty() ? f.name : full_filename.c_str();
			for (;;) {
				int fromlen = sizeof(client);
				auto got = recvfrom(sock, (char*)&recvd_com, sizeof(recvd_com), 0, &client, &fromlen);
				if (got > 0) {
					std::cout << recvd_com.param << ',';
					if (recvd_com.command == PKT) {
						char buff[BUFFSIZE];
						auto fhandle = fopen(open_name, "rb");
						if (fhandle) {
							fseek(fhandle, recvd_com.param*BUFFSIZE, SEEK_SET);
							auto red = fread(buff, 1, sizeof(buff), fhandle);
							socket::sendto(sock, buff, red, 0, &client, sizeof(client));
							fclose(fhandle);
						} else
							puts("seed file shite");
					} else if (recvd_com.command == FILEDATA)
						socket::sendto(sock, (char*)&f, sizeof(f.size) + strlen(f.name) + 1, 0, &client, sizeof(client));
					/*if (recvd_com.command > 1)
						std::cout << "seed command shite " << recvd_com.command;
					else
						std::cout << enum_rep[recvd_com.command];
					std::cout << '\n';*/
				}
			}
			//puts("_seed stopped");
		}
		/**
		 * \brief	Receives a file.
		 * Iterates over the seeders list until all of the file's pieces have been received.
		 * Checks for the seeders' reliablity in order to maintain download speed.
		 * \returns	0 on success, 1 on access violation, 2 when all seeders aren't reliable.
		*/
		char _recv() {
			auto fhandle = fopen(f.name, "wb");
			std::cout << "recv-fhandle: " << fhandle;
			if (fhandle) {
				auto psize = f.size / BUFFSIZE + (f.size % BUFFSIZE ? 1 : 0);
				cln_com pkt_msg{ PKT };
				char buff[BUFFSIZE];
				etl::array<char, CLN_NUM> relib;
				relib.fill(3);
				for (BYTE i = 0; pkt_msg.param < psize; ) {
					if (clients.empty())
						return 2;
					sendto(sock, (char*)&pkt_msg, sizeof(pkt_msg), 0, clients.data() + i, sizeof(clients[i]));
					auto check = recvfrom(sock, buff, sizeof(buff), 0, 0, 0);
					if (check == -1) {
						std::cout << "recv_file: " << WSAGetLastError() << ' ' << check << '\n';
						if (relib[i])
							relib[i]--;
						else {
							clients.erase(clients.data() + i);
							if (clients.empty())
								return 2;
							relib.erase_at(i);
							continue;
						}
					} else {
						++pkt_msg.param;
						relib[i] = 3;
						fwrite(buff, 1, check, fhandle);
						row_ref->set_value(3, guint(pkt_msg.param * 100 / psize));
					}
					i = (i + 1) % clients.size();
				}
				fclose(fhandle);
				return 0;
			}
			puts("_recv shit filename");
			return 1;
		}
		/**
		 * \brief	requests file data (name and size)
		 * Iterates over the seeders list until the file's data has been received.
		 * Checks for the seeders' reliablity in order to maintain download speed in the future.
		 * \returns	true on success, false on failure.
		*/
		bool request_filedata() {
			std::cout << "request_filedata:" << clients.size() << '\n';
			if (clients.size()) {
				etl::array<char, CLN_NUM> relib;
				relib.fill(3);
				cln_com filedata_msg{ FILEDATA };
				for (unsigned char i = 0;;) {
					sendto(sock, (char*)&filedata_msg, sizeof(filedata_msg), 0, clients.data() + i, sizeof(clients[i]));
					auto check = recvfrom(sock, (char*)&f, sizeof(f), 0, 0, 0);
					std::cout << "request_filedata: " << WSAGetLastError() << ' ' << check << '\n'
						<< f.name << ' ' << f.size << '\n';
					if (check != -1)
						break;
					else if (relib[i]) {
						relib[i]--;
						i = (i + 1) % clients.size();
					} else {
						clients.erase(clients.data() + i);
						if (clients.empty())
							return false;
						relib.erase_at(i);
					}
				}
				row_ref->set_value(0, Glib::ustring(f.name));
				row_ref->set_value(1, gulong(f.size));
				row_ref->set_value(2, guint((unsigned int)tracker.port()));
				puts(f.name);
				return true;
			}
			puts("shit request_filedata");
			return false;
		}
		/**
		 * Gets a list of the current seeders from the tracker.
		*/
		void request_list() {
			if (tracker.port() != 16673) {
				clients.resize(clients.capacity());
				int size, count = 4;
				do {
					send_command_tracker(LIST);
					size = recvfrom(sock, (char*)clients.data(), clients.capacity() * sizeof(clients[0]), 0, 0, 0);
					--count;
				} while (size < 0 && count);
				if (count)
					clients.resize(size / sizeof(clients[0]));
				std::cout << clients.size();
				std::cout << "request_list: " << WSAGetLastError() << '\n';
			} else
				puts("request_list why");
		}
	public:
		/**
		 * \brief	Starts a new download from a given port.
		 * Requests the seeders list, the file's data, and finally the file itself.
		 * \param	_port	a tracker port from which to get the seeders list.
		*/
		void start_download(unsigned short _port) {
			init();
			tracker.port(_port);
			bool reqf = false;
			for (int count = 4; !reqf && count; --count) {
				request_list();
				reqf = request_filedata();
			}
			if (reqf) {
				char recv_stat = _recv();
				for (int count = 4; recv_stat == 2 && count; --count) {
					request_list();
					recv_stat = _recv();
				}
				if (!recv_stat) {
					command_tracker(ADD);
					_seed();
				}
			}
		}
		/**
		* \brief	Starts a completely new seed.
		* Gets a port from a newly created tracker which will be used for future P2P communication.
		* Sets the corresponding row in the GUI accordingly (name, size, port and progress (to a 100%))
		*/
		void start_new_seed() {
			init();
			row_ref->set_value(0, Glib::ustring(f.name));
			row_ref->set_value(1, gulong(f.size));
			row_ref->set_value(3, guint(100));
			int check;
			int tsize = sizeof(tracker);
			for (unsigned int count = 4; ; count--) {
				sendto(sock, 0, 0, 0, &tracker, sizeof(tracker));
				check = recvfrom(sock, 0, 0, 0, &tracker, &tsize);
				if (!count)
					return;
				if (check >= 0)
					break;
			}
			sendto(sock, 0, 0, 0, &tracker, sizeof(tracker));
			auto newport = tracker.port();
			std::cout << "new_seed: " << WSAGetLastError() << ' ' << newport << '\n';
			row_ref->set_value(2, guint((unsigned int)newport));
			_seed();
		}
		/**
		 * Creates a new p2p_client object.
		 * \param	taddr	the default tracker's address.
		 * \param	tr	the row in the GUI associated with the current client.
		*/
		p2p_client(ip4addr taddr, Gtk::TreeIter&& tr)
			: tracker(taddr, 16673),
			row_ref(tr) {
		}
		/**
		 * \brief	Sets the file's name.
		 * Sets the file's name and the file's size according to it.
		 * \param	_name	the file's name.
		*/
		p2p_client& set_filename(const Glib::ustring& _name) {
			auto off = _name.find_last_of('\\') + 1;
			full_filename = _name;
			puts(full_filename.c_str());
			_name.copy(f.name, _name.length() - off, off);
			f.name[_name.length() - off] = 0;
			struct stat stat_buf;
			int rc = stat(_name.c_str(), &stat_buf);
			f.size = rc == 0 ? stat_buf.st_size : 0;
			return *this;
		}
		~p2p_client() {
			puts("client destructed");
		}
};