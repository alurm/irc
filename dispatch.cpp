#include <map>

#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREGISTRED "462"

typedef int client_descriptor;

struct client {
	bool password_verified;
	bool must_be_closed;
};

struct server {
	char *password;
	std::map<client_descriptor, client> clients;
};

// Fix me.
optional<message> dispatch(server *s, client *c, message m) {
	assert(!c->must_be_closed);

	// To-do: this is incorrect.
	// - ... It is possible to send multiple PASS commands before registering ...
	// Decision: if password is not correct, set client state to client::to_be_closed.
	if (strcmp(m.command, "PASS") == 0) {
		if (c->state == client::registered) {
			return (optional<message>){
				.has_value = true,
					.value = {
						.command = const_cast<char *>(ERR_ALREADYREGISTRED),
						.params = const_cast<char **>((const char *[]){
							"You may not reregister",
						}),
						.params_count = 1,
					},
			};
		}

		if (m.params_count == 0) {
			return (optional<message>){
				.has_value = true,
				.value = {
					.command = const_cast<char *>(ERR_NEEDMOREPARAMS),
					.params = const_cast<char **>((const char *[]){
						"PASS",
						"Not enough parameters",
					}),
					.params_count = 2,
				},
			};
		}

		char *password = m.params[0];

		if (strcmp(s->password, password) != 0) {
			c->state = client::to_be_closed;
			return (optional<message>){ .has_value = false };
		}

		return (optional<message>){ .has_value = false };
	}

	if (!c->password_verified) {
		c->state = client::to_be_closed;
		return (message){ .has_value = false };
	}


	else if (strcmp(m.command, "NICK") == 0) {

	} else {
		assert(0);
	}
}
