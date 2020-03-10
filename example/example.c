/*
MIT License

Copyright (c) 2019 Marcin Borowicz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <assert.h>

#include "../src/cat.h"

static int32_t speed;
static uint8_t x;
static uint8_t y;
static char message[16];

static int x_write(const struct cat_variable *var)
{
        printf("x variable updated internally to: %d\n", x);
        return 0;
}

static int y_write(const struct cat_variable *var)
{
        printf("y variable updated internally to: %d\n", y);
        return 0;
}

static int speed_write(const struct cat_variable *var)
{
        printf("speed variable updated internally to: %d\n", speed);
        return 0;
}

static int go_write(const struct cat_command *cmd, const uint8_t *data, const size_t data_size, size_t args_num)
{
        printf("<%s>: x=%d y=%d msg=%s @ speed=%d\n",
                cmd->name,
                *(uint8_t*)(cmd->var[0].data),
                *(uint8_t*)(cmd->var[1].data),
                message,
                speed
        );
        return 0;
}

static int set_write(const struct cat_command *cmd, const uint8_t *data, const size_t data_size, size_t args_num)
{
        printf("<%s>: SET SPEED TO = %d\n",
                cmd->name,
                speed
        );
        return 0;
}

static int test_run(const struct cat_command *cmd)
{
        printf("TEST: <%s>", cmd->name);
        return 0;
}


static struct cat_variable go_vars[] = {
        {
                .type = CAT_VAR_UINT_DEC,
                .data = &x,
                .data_size = sizeof(x),
                .write = x_write
        },
        {
                .type = CAT_VAR_UINT_DEC,
                .data = &y,
                .data_size = sizeof(y),
                .write = y_write
        }
};

static struct cat_variable set_vars[] = {
        {
                .type = CAT_VAR_INT_DEC,
                .data = &speed,
                .data_size = sizeof(speed),
                .write = speed_write
        }
};

static struct cat_command cmds[] = {
        {
                .name = "+GO",
                .write = go_write,
                .var = go_vars,
                .var_num = sizeof(go_vars) / sizeof(go_vars[0]),
                .need_all_vars = true
        },
        {
                .name = "+SET",
                .write = set_write,
                .var = set_vars,
                .var_num = sizeof(set_vars) / sizeof(set_vars[0]),
                .need_all_vars = true
        },
        {
                .name = "#TEST",
                .run = test_run
        }
};

static char buf[128];
static uint8_t state_buf[8];

static struct cat_descriptor desc = {
        .cmd = cmds,
        .cmd_num = sizeof(cmds) / sizeof(cmds[0]),

        .buf = buf,
        .buf_size = sizeof(buf),
        .state_buf = state_buf,
        .state_buf_size = sizeof(state_buf)
};

static int write_char(char ch)
{
        putc(ch, stdout);
        return 1;
}

static int read_char(char *ch)
{
        *ch = getc(stdin);
        return 1;
}

static struct cat_io_interface iface = {
        .read = read_char,
        .write = write_char
};

int main(int argc, char **argv)
{
	struct cat_object at;

	cat_init(&at, &desc, &iface);

        while (cat_service(&at) != 0) {};

	return 0;
}