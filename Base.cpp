#include "Base.hpp"

Base::Base(Server *srv, bool auth) : _srv(srv), _auth(auth) {}

Base::~Base() {}

bool Base::auth_required() const { return _auth; }

void Base::dispatch(Server *s, Client *c, message m) {
	// To-do: this is incorrect.
	// - ... It is possible to send multiple PASS commands before
	// registering ... Decision: if password is not correct, set client
	// state to client::to_be_closed.
	std::vector<std::string> args;
	for (int i = 0; m.params[i] != nullptr; ++i) {
		args.push_back(std::string(m.params[i]));
	}

	if (strcmp(m.command, "PASS") == 0) {
		std::cout << "in pass\n";
		this->execute(c, args);
	}

}
 