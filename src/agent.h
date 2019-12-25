#include "ssh_pack.h"
#include "win32_utils.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace cngeant {

struct bcrypt_algos
{
	bcrypt_algo_handle md5;
	bcrypt_algo_handle sha1;
	bcrypt_algo_handle sha256;
	bcrypt_algo_handle sha384;
	bcrypt_algo_handle sha512;

	bcrypt_algos();
};

using sign_fn = void(ssh_writer & wr, bcrypt_algos const & algos, NCRYPT_KEY_HANDLE key, std::string_view tbs, uint32_t flags);

struct key_info
{
	ncrypt_handle key;
	std::string comment;
	std::string algo_id;
	std::string public_blob;
	bool is_hw;
	sign_fn * sign;

	std::string get_public_key() const;
};

struct new_key_type
{
	std::string name;
	double score;
	std::function<void(key_info & ki)> create_fn;
};

struct agent
{
	agent();

	std::vector<std::string> new_key_types();
	void new_key(size_t algo_idx, std::string comment);

	bool process_message(ssh_writer & wr, std::string_view msg);

	std::vector<key_info> const & keys() const;
	void delete_key(size_t index);

private:
	void _enum_algos(std::shared_ptr<ncrypt_handle> provider, bool is_hw);
	void _enum_keys(NCRYPT_PROV_HANDLE provider, bool is_hw);

	bcrypt_algos _algos;
	std::vector<key_info> _keys;
	std::vector<new_key_type> _new_key_types;
};

}

#pragma once
