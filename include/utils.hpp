#pragma once

#include "defines.hpp"
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

namespace utils
{

bool safe_close(int& fd);
bool safe_close_pipe(int (&pipe_fd)[2]);
bool setup_nonblocking(int fd);

} // namespace utils
