#include <iostream>
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
#include <forward_list>
#include "p2p_client.h"
class session {
	private:
		p2p_client cln;
		void start_download(unsigned short _port) {
			cln.init();
			cln.start(_port);
		}
		void start_new_seed() {
			cln.init();
			cln.new_seed();
		}
	public:
		/*filedata& get_filedata() {
			return f;
		}*/
		auto port() {
			return cln.get_tracker().port();
		}
		session() {
		}
		auto init_filename(const Glib::ustring& str) {
			cln.init_filename(str);
		}
		void set_row(Gtk::TreeIter& tr) {
			cln.set_row(tr);
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
		std::forward_list<std::thread> sessions;
		Gtk::Window *w;
		Glib::RefPtr<Gtk::ListStore> down_list;
		Gtk::ToolButton *new_file, *download;
		Gtk::FileChooserDialog *fc;
		Gtk::Button *fc_ok, *fc_close, *close_port, *ok_port;
		Gtk::SpinButton *get_port;
		Gtk::Dialog *port_dialog;
		void addseed() {
			Glib::ustring filename;
			filename = fc->get_filename();
			if (!filename.empty()) {
				session s;
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
				session s;
				s.set_row(down_list->append());
				sessions.push_front(std::move(s.start_download_async(port)));
			}
		}
	public:
		p2p_main(int argc, char** argv)
			: Gtk::Main(argc, argv) {
			{
				init_winsock();
				Glib::RefPtr<Gtk::Builder> builder;
				{
					char buff[13000];
					memset(buff, 0, sizeof(buff));
					auto f = fopen("p2p.glade", "rb");
					fread(buff, 1, sizeof(buff), f);
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
				down_list = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(builder->get_object("downloadList"));
			}
			fc_ok->signal_clicked().connect(sigc::mem_fun(*this, &p2p_main::addseed));
			new_file->signal_clicked().connect(sigc::mem_fun(*fc, &Gtk::FileChooserDialog::show));
			fc_close->signal_clicked().connect(sigc::mem_fun(*fc, &Gtk::FileChooserDialog::hide));
			download->signal_clicked().connect(sigc::mem_fun(*port_dialog, &Gtk::Dialog::show));
			close_port->signal_clicked().connect(sigc::mem_fun(*port_dialog, &Gtk::Dialog::hide));
			ok_port->signal_clicked().connect(sigc::mem_fun(*this, &p2p_main::add_download));
			w->show();
			run();
		}
};
int main(int argc, char** argv) {
	auto self = p2p_main(argc, argv);
	return 0;
}