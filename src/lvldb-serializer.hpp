/*
 * buffer.hpp
 *
 *  Created on: 05/lug/2014
 *      Author: marco
 */

#ifndef BUFFER_HPP_
#define BUFFER_HPP_

extern "C" {
#include <stdlib.h>
#include <stdint.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

#define MAR_CHR 1
#define MAR_I32 4
#define MAR_I64 8


class LvLDBSerializer {
public:
	LvLDBSerializer(lua_State *L): size(128), seek(0), head(0), data_len(0) {
		this->L = L;

		if (!(data = (unsigned char*)malloc(size)))
			luaL_error(L, "Out of memory!");
	}
	~LvLDBSerializer() {
		free(data);
	}

	int set_lua_value(int idx) {
		int lua_value_type = lua_type(L, idx);
		//lua_pushvalue(L, idx);

		write((unsigned char*)&lua_value_type, sizeof(int));

		switch(lua_value_type)
		{
			case LUA_TBOOLEAN: {
				int int_val = lua_toboolean(L, -1);
				write((unsigned char*)&int_val, MAR_CHR);
				break;
			}
			case LUA_TSTRING: {
				size_t l;
				const char *str_val = lua_tolstring(L, idx, &l);
				write((unsigned char*)str_val, l);
				break;
			}
			case LUA_TNUMBER: {
				lua_Number num_val = lua_tonumber(L, idx);
				write((unsigned char*)&num_val, MAR_I64);
				break;
			}
			default: {
				luaL_argerror(L, idx, "Expecting number, string or boolean");
				return 1; // not executed
			}
		}

		return 0;
	}

	void print_buffer() {
		print_bytes(data, size);
	}

	size_t buffer_len() {
		return data_len;
	}

	void buffer_data(unsigned char *out) {
		size_t i;

		for(i = 0; i < data_len; i++)
			out[i] = data[i];

		//std::cout << "buffer_data\n";
		//print_bytes(out, data_len);
	}

private:
	lua_State *L;
	size_t size;
	size_t seek;
	size_t head;
	size_t data_len;
	unsigned char *data;

	int write(unsigned char *str, size_t len) {
		if (len > UINT32_MAX)
			luaL_error(L, "buffer too long");

		if (size - head < len) {
			size_t new_size = size << 1;
			size_t cur_head = head;
			while (new_size - cur_head <= len) {
				new_size = new_size << 1;
			}
			if (!(data = (unsigned char*)realloc(data, new_size)))
				luaL_error(L, "Out of memory!");

			size = new_size;
		}

		memcpy(&data[head], str, len);
		head += len;
		data_len += len;

		return 0;
	}

	void insert_int_big_endian(int n, unsigned char *byte) {
		byte[0] = (n >> 24) & 0xFF;
		byte[1] = (n >> 16) & 0xFF;
		byte[2] = (n >> 8) & 0xFF;
		byte[3] = n & 0xFF;

		data_len += 4;
	}

	void print_bytes(unsigned char *b, size_t len) {
		size_t i;

		for(i = 0; i < len; i++)
			printf("%p\t0x%.2x\n", b+1, *(b+i));
		printf("\n");
	}
};


#endif /* BUFFER_HPP_ */
