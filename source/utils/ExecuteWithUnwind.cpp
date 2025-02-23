/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExecuteWithUnwind.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 14:42:29 by mhuszar           #+#    #+#             */
/*   Updated: 2025/02/23 16:22:57 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExecuteWithUnwind.hpp"
#include <unistd.h>

ExecuteWithUnwind::ExecuteWithUnwind(const char *path, char **args, char **env)
{
    _path = path;
    _args = args;
    _env = env;
}

ExecuteWithUnwind::~ExecuteWithUnwind() throw()
{
    if (_path)
        delete _path;
    if (_args)
        delete _args;
    if (_env)
        delete _env;
}

ExecuteWithUnwind::ExecuteWithUnwind(const ExecuteWithUnwind& other) : _path(other._path), _args(other._args), _env(other._env)
{
}

int ExecuteWithUnwind::execute()
{
    int status = execve(_path, _args, _env);
    return (status);
}
