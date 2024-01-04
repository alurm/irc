Channel::Channel(const std::string& name, const std::string& key, Client* admin)
    : _name(name), _admin(admin), _k(key), _l(0), _n(false)
{

}

Channel::~Channel() {}