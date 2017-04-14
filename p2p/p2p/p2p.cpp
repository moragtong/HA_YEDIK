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
#include "p2p_client.h"
class session : private p2p_client {
	private:
		std::thread thread;
		void start_download(unsigned short _port) {
			init();
			start(_port);
		}
		void start_new_seed() {
			init();
			new_seed();
		}
	public:
		filedata& get_filedata() {
			return f;
		}
		auto port() {
			return get_tracker().port();
		}
		session() {
		}
		void start_download_async(unsigned short _port) {
			thread = std::thread(&session::start_download, this, _port);
		}
		void start_new_seed_async(Gtk::TreeIter& tr) {
			row_ref = tr;
			thread = std::thread(&session::start_new_seed, this);
		}
};
class p2p_main : public Gtk::Main {
	private:
		std::vector<session> sessions;
		Gtk::Window* w;
		Glib::RefPtr<Gtk::ListStore> down_list;
		Gtk::ToolButton* new_file;
		Gtk::FileChooserDialog* fc;
		Gtk::Button* fc_ok;
		void addseed() {
			Glib::ustring filename;
			filename = fc->get_filename();
			if (!filename.empty()) {
				fc->hide();
				sessions.emplace_back();
				auto& cur = sessions.back();
				auto& fdata = cur.get_filedata();
				fdata.init(filename);
				auto cur_row = down_list->append();
				cur_row->set_value(0, Glib::ustring(fdata.name));
				cur_row->set_value(1, gulong(fdata.size));
				cur_row->set_value(3, guint(100));
				cur.start_new_seed_async(cur_row);
			}
		}
		void add_download(unsigned short port) {
			sessions.emplace_back();
			sessions.back().start_download_async(port);
		}
		void fc_run() {
			fc->run();
		}
	public:
		p2p_main(int argc, char** argv)
			: Gtk::Main(argc, argv), w(0), new_file(0), fc(0) {
			{
				//Gtk::Widget* progress_bars;
				Glib::RefPtr<Gtk::Builder> builder;
				{
					char buff[10000];
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
				down_list = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(builder->get_object("downloadList"));
			}
			fc_ok->signal_clicked().connect(sigc::mem_fun(*this, &p2p_main::addseed));
			new_file->signal_clicked().connect(sigc::mem_fun(*this, &p2p_main::fc_run));
			w->show_all();
			run();
		}
};
int main(int argc, char** argv) {
	init_winsock();
	auto self = p2p_main(argc, argv);
	
	return 0;
}