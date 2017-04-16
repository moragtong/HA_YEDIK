#include <iostream>
#include <forward_list>
#include <thread>
#undef max
#undef min
#include <gtkmm\main.h>
#include <gtkmm\window.h>
#include <gtkmm\toolbutton.h>
#include <gtkmm\button.h>
#include <gtkmm\builder.h>
#include <gtkmm\liststore.h>
#include <gtkmm\filechooserdialog.h>
#include <gtkmm\spinbutton.h>
//#include <gtkmm\box.h>
#include "p2p_client.h"
class session : private p2p_client {
	private:
		void start_download(unsigned short _port) {
			init();
			start(_port);
		}
		void start_new_seed() {
			init();
			new_seed();
		}
	public:
		/*filedata& get_filedata() {
			return f;
		}*/
		using p2p_client::p2p_client;
		using p2p_client::init_filename;
		using p2p_client::set_row;
		auto port() {
			return get_tracker().port();
		}
		auto start_download_async(unsigned short _port) {
			return std::thread(&session::start_download, *this, _port);
		}
		auto start_new_seed_async() {
			return std::thread(&session::start_new_seed, *this);
		}
		/*~session() {
			puts("session destructed");
		}*/
};
class p2p_main : public Gtk::Main {
	private:
		ip4addr tracker_addr;
		std::forward_list<std::thread> sessions;
		Gtk::Window *w;
		Glib::RefPtr<Gtk::ListStore> down_list;
		Gtk::ToolButton *new_file, *download;
		Gtk::FileChooserDialog *fc;
		Gtk::Button *fc_ok, *fc_close, *close_port, *ok_port, *taddr_next;
		Gtk::SpinButton *get_port, *ips[4];
		Gtk::Dialog *port_dialog, *taddr_dialog;
		//Gtk::Box *taddr_vbox;
		void addseed() {
			Glib::ustring filename;
			filename = fc->get_filename();
			if (!filename.empty()) {
				session s(tracker_addr);
				fc->hide();
				//std::cout << sessions. << '\n';
				s.init_filename(filename);
				s.set_row(down_list->append());
				sessions.push_front(std::move(s.start_new_seed_async()));
			}
		}
		void add_download() {
			auto port = get_port->get_value_as_int();
			if (port >= 2000) {
				port_dialog->hide();
				//std::cout << sessions.size() << '\n';
				session s(tracker_addr);
				s.set_row(down_list->append());
				sessions.push_front(std::move(s.start_download_async(port)));
			}
		}
		void startmain() {
			taddr_dialog->hide();
			auto i = ips;
			auto addri = tracker_addr._addr;
			char count = sizeof(ips) / sizeof(ips[0]);
			do {
				*addri++ = (char&&)(*i++)->get_value_as_int();
				count--;
			} while (count);
			w->show();
		}
	public:
		p2p_main(int argc, char** argv)
			: Gtk::Main(argc, argv) {
			{
				init_winsock();
				Glib::RefPtr<Gtk::Builder> builder;
				{
					Glib::ustring buff(22000, '\0');
					auto f = fopen("p2p.glade", "rb");
					fread((void*)buff.c_str(), 1, buff.bytes(), f);
					builder = Gtk::Builder::create_from_string(buff);
					fclose(f);
				}
				builder->get_widget("mainWindow", w);
				builder->get_widget("new", new_file);
				builder->get_widget("fc", fc);
				builder->get_widget("file_ok", fc_ok);
				builder->get_widget("file_close", fc_close);
				builder->get_widget("download", download);
				builder->get_widget("close_port", close_port);
				builder->get_widget("ok_port", ok_port);
				builder->get_widget("port_dialog", port_dialog);
				builder->get_widget("port_spin", get_port);
				builder->get_widget("continue", taddr_next);
				//builder->get_widget("taddr_vbox", taddr_vbox);
				builder->get_widget("taddr_dialog", taddr_dialog); 
				{
					char ip_names[] = "ip1";
					auto i = ips;
					char counter = sizeof(ips) / sizeof(ips[0]);
					do {
						builder->get_widget(ip_names, *i++);
						ip_names[2]++;
						counter--;
					} while (counter);
				}
				down_list = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(builder->get_object("downloadList"));
			}
			fc_ok->signal_clicked().connect(sigc::mem_fun(this, &p2p_main::addseed));
			new_file->signal_clicked().connect(sigc::mem_fun(fc, &Gtk::FileChooserDialog::show));
			fc_close->signal_clicked().connect(sigc::mem_fun(fc, &Gtk::FileChooserDialog::hide));
			download->signal_clicked().connect(sigc::mem_fun(port_dialog, &Gtk::Dialog::show));
			close_port->signal_clicked().connect(sigc::mem_fun(port_dialog, &Gtk::Dialog::hide));
			ok_port->signal_clicked().connect(sigc::mem_fun(this, &p2p_main::add_download));
			taddr_next->signal_clicked().connect(sigc::mem_fun(this, &p2p_main::startmain));
			taddr_dialog->show();
			run();
		}
};
int main(int argc, char** argv) {
	p2p_main self(argc, argv);
	return 0;
}