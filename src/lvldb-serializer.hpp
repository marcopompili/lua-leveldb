/*
 * buffer.hpp
 *
 *  Created on: 05/lug/2014
 *      Author: Marco Pompili
 */

#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <cstring>
#include <limits>

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

	int set_lua_value(int idx);
	void print_buffer();
	size_t buffer_len();
	void buffer_data(unsigned char *out);

private:
	lua_State *L;
	size_t size;
	size_t seek;
	size_t head;
	size_t data_len;
	unsigned char *data;

	int write(unsigned char *str, size_t len);

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
