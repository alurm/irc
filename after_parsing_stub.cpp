
typedef bool (*pfunc)(const std::string &);

bool validateKick(const std::string &command) { return (command == "KICK"); }

bool validateInvite(const std::string &command) {
	return (command == "INVITE");
}

bool validateTopic(const std::string &command) { return (command == "TOPIC"); }
/*

using pfunc = void (*)(string);

map<string, pfunc> funcMap;
funcMap["f1"] = f1; //and so forth
And then call:

pfunc f = funcMap[commandType];
(*f)(commandParam);
*/

bool isCommandValid(const std::string &command) {
	std::unordered_map<std::string, pfunc> commandMap;
	commandMap.insert(std::make_pair("KICK", &validateKick));
	commandMap.insert(std::make_pair("INVITE", &validateInvite));
	commandMap.insert(std::make_pair("TOPIC", &validateTopic));

	std::unordered_map<std::string, pfunc>::iterator it =
	    commandMap.find(command);
	if (it != commandMap.end()) {
		pfunc func = it->second;
		bool result = func(command);
		return result;
	}

	return false;
}
// is_valid_params

parseme parse(lexeme l, parse_state *p) {
	switch (p->state) {
	case parse_state::out_of_word:

		if (l.tag == lex_state::carriage_return_found) {
			p->state = parse_state::carriage_return_found;

			return (parseme){
			    .tag = parseme::nothing,
			};
		}
		p->state = parse_state::in_word;

		if (l.value.word[0] == ':') {
			/*
					char *prefix;
					bool is_prefixed;
					char *command;
					char **params;
					int params_count;
			*/
			message msg = {nullptr, false, nullptr, nullptr, 0};
			msg.prefix = strdup(l.value.word);
			std::cout << ">>>>>>>>>>>>" << l.value.word
				  << "<<<<<<<<<<<<<" << std::endl;
			p->is_prefix = true;
			return (parseme){
			    .tag = parseme::message,
			    .value.message.prefix = strdup(l.value.word),
			};
			// std::cout << "1111\n";
			// exit(1);
			p->is_prefix = true;
		} else if (isCommandValid(l.value.word) && !p->is_command) {
			return (parseme){
			    .tag = parseme::message,
			    .value.message.command = strdup(l.value.word),
			    // .value.message.command = nullptr,
			};
			p->is_command = true;
		} else if (p->is_command) {
			return (parseme){
			    .tag = parseme::message,
			    // .value.error = "Error\n"
			};
		}
		break;
	case parse_state::in_word:
		// if (isCommandValid(l.value.word) && !p->is_command)
		// {
		return (parseme){
		    .tag = parseme::message,
		    .value.message.command = l.value.word,
		};
		// }
		// else if (p->is_command)
		// {
		// 	// state valid_params
		// }
		break;
	case parse_state::carriage_return_found:
		break;
	default:
		break;
	}

	parseme result;
	result.tag = parseme::nothing;
	return result;
}
