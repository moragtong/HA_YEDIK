//#include <iostream>
//#include <gtkmm.h>
/*#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>*/
#include "p2p_client.h"
/*class HelloWorld : public Gtk::Window {

public:
	HelloWorld();
	virtual ~HelloWorld();

protected:
	//Signal handlers:
	void on_button_clicked();

	//Member widgets:
	Gtk::Button m_button;
};


HelloWorld::HelloWorld()
	: m_button("Hello World")   // creates a new button with label "Hello World".
{
	// Sets the border width of the window.
	set_border_width(10);

	// When the button receives the "clicked" signal, it will call the
	// on_button_clicked() method defined below.
	m_button.signal_clicked().connect(sigc::mem_fun(*this,
		&HelloWorld::on_button_clicked));

	// This packs the button into the Window (a container).
	add(m_button);

	// The final step is to display this newly created widget...
	m_button.show();
}

HelloWorld::~HelloWorld() {
}

void HelloWorld::on_button_clicked() {
	std::cout << "Hello World" << std::endl;
}*/
int main(int argc, char** argv) {
	init_winsock();
	p2p_client::p2p_client client;
	client.init();
	char command[16];
	//for (;;) {
		std::memset(command, 0, sizeof(command));
		std::cin >> command;
		/*if (!strcmp(command, "remove"))
			client.command_tracker(p2p_client::REMOVE);
		else*/ if (!strcmp(command, "recv")) {
			unsigned short i;
			std::cin >> i;
			client.start(i);
		} else if (!strcmp(command, "add")) {
			std::cin >> client.f;
			std::cout << command << ' ' << client.f.name << '\n';
			client.new_seed();
		} else
			puts("invalid input");
	//}
	puts("done");
	/*Gtk::Main kit(argc, argv);

	HelloWorld helloworld;
	//Shows the window and returns when it is closed.
	Gtk::Main::run(helloworld);*/

	return 0;
}