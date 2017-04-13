#include <iostream>
#include <thread>
#include "p2p_client.h"
#undef max
#undef min
#include <gtkmm\main.h>
#include <gtkmm\window.h>
#include <gtkmm\button.h>
#include <gtkmm\builder.h>
#include <gtkmm\liststore.h>
#include <gtkmm\filechooser.h>
class session : private p2p_client {
	private:
		std::thread thread;
		unsigned char index;
		void start_download(unsigned short _port) {
			init();
			start(_port);
		}
		void start_new_seed() {
			init();
			new_seed();
		}
	public:
		session(unsigned char _index) 
			: index(_index) {
		}
		void start_download_async(unsigned short _port) {
			thread = std::thread(&session::start_download, this, _port);
		}
		void start_new_seed_async() {
			thread = std::thread(&session::start_new_seed, this);
		}
};
class p2p_main : public Gtk::Main {
	private:
		std::vector<session> sessions;
		Gtk::Window* w;
		Glib::RefPtr<Gtk::ListStore> down_list;
		Gtk::Button* NewFile;
		void addseed() {
			sessions.emplace_back(sessions.size());
			sessions.back().start_new_seed_async();
		}
		void add_download(unsigned short port) {
			sessions.emplace_back(sessions.size());
			sessions.back().start_download_async(port);
		}
	public:
		p2p_main(int argc, char** argv)
			: Gtk::Main(argc, argv) {
			Glib::RefPtr<Gtk::Builder> builder;
			{
				char buff[10000];
				memset(buff, 0, sizeof(buff));
				auto f = fopen("p2p.glade", "rb");
				fread(buff, 1, sizeof(buff), f);
				builder = Gtk::Builder::create_from_string(buff);
				fclose(f);
			}
			Gtk::Window* w = nullptr;
			builder->get_widget("mainWindow", w);
			down_list = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(builder->get_object("downloadList"));
			w->show_all();
			/*Gtk::Widget* fc = nullptr;
			builder->get_widget("fc", fc);
			fc->show();*/
			run();
	}

};
int main(int argc, char** argv) {
	init_winsock();
	p2p_main(argc, argv);
	
	return 0;
}